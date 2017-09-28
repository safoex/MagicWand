
#include <SoftwareSerial.h>
SoftwareSerial ESPport(10, 11); 
////////////////////// RX, TX

// если у вас MEGA, то закомментируйте верхние две строчки и раскомментируйте нижеследующую.
// ESP подключите к Serial1 (18, 19), скорость можно сделать 57600
// HardwareSerial & ESPport = Serial1;

const int ledPin =  13;     
int ledState = HIGH;           
#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE];
String vklotkl;
 
void setup() 
{ 
  pinMode(ledPin, OUTPUT);        
  Serial.begin(115200); // Терминал
  ESPport.begin(115200); // ESP8266  
  clearSerialBuffer(); // очистка буфера Serial
  Serial.println("RESET 3,5 sek");
  Serial.println(GetResponse("AT+RST",700)); // перезагрузка ESP
  Serial.println(GetResponse("AT+CIOBAUD=9600", 300));
  ESPport.begin(9600); // ESP8266 
  Serial.println(GetResponse("AT+GMR",300)); 
  Serial.println(GetResponse("AT+CWMODE=1",300)); // режим клиента   
  connectWiFi("myrouter","parolparol"); // подключаемся к домашнему роутеру (имя точки, пароль) 
  Serial.println(GetResponse("AT+CIPMODE=0",300)); // сквозной режим передачи данных. 
  Serial.println(GetResponse("AT+CIPMUX=1",300)); // multiple connection.
  Serial.print("Start TCP-server: ");
  Serial.println(GetResponse("AT+CIPSERVER=1,88", 300)); // запускаем ТСР-сервер на 88-ом порту
  Serial.println(GetResponse("AT+CIPSTO=2", 300)); // таймаут сервера 2 сек
  Serial.println(GetResponse("AT+CIFSR", 300)); // узнаём адрес
  digitalWrite(ledPin,ledState);  
}
///////////////////основной цикл, принимает запрос от клиента/////////////////// 
void loop() 
{
 int ch_id, packet_len; // объявляем переменные
 char *pb;  
 ESPport.readBytesUntil('\n', buffer, BUFFER_SIZE); // читаем строку из есп в переменную "buffer"
  
 if(strncmp(buffer, "+IPD,", 5)==0) // если в "buffer" есть строка из пяти символов - "+IPD," тогда -
  {                                 // (СИшная функция "strncmp" сравнивает содержимое "buffer" с заданой строкой "+IPD,")
   sscanf(buffer+5, "%d,%d", &ch_id, &packet_len); // - вычитываем из "buffer" и засовываем в переменные (ch_id и packet_len) id клиента и длину пакета 
   if (packet_len > 0)                             // (СИшная функция "sscanf")
    {
      pb = buffer+5;
      while(*pb!=':') pb++;
      pb++;
      if(strncmp(pb, "GET / ", 6) == 0) // если в "pb" есть фраза "GET / ", то 
       {
        Serial.println(buffer); // выводим в консоль содержимое buffer (просто чтоб поглядеть чего прилетело, для работы эти пять строк можно закомментировать)
        Serial.print("get led from ch :");
        Serial.println(ch_id);
        delay(100);
        clearSerialBuffer(); 
        
        if(ledState == LOW) // узнаём состояние светика
          {
            ledState = HIGH;
            vklotkl = "VKL";
          }
        
        else 
          {
            ledState = LOW;
            vklotkl = "OTKL";
          } 
        
        digitalWrite(ledPin, ledState); // инвертируем состояние светика
        otvet_klienty(ch_id);  // отправляемся в функцию формирования ответа клиенту (и передаём ей id клиента)
       } 
    }
  }
  clearBuffer();
}
//////////////////////формирование ответа клиенту////////////////////
void otvet_klienty(int ch_id) 
{  
  // собираем строку для клиента (заголовок пакета)                       
  String Header; 
 
  Header =  "HTTP/1.1 200 OK\r\n";  
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
  
  // содержимое пакета
  String Content;

  Content = "LED 13: " + vklotkl;
  
  Header += "Content-Length: ";
  Header += (int)(Content.length());
  Header += "\r\n\r\n";
  
  ESPport.print("AT+CIPSEND="); // ответ клиенту: ат-команда, id-клиента, , , длина пакета
  ESPport.print(ch_id);
  ESPport.print(",");
  ESPport.println(Header.length()+Content.length());
  delay(20);

  if(ESPport.find(">")) // ожидание от esp приглашения ввода
    {
      ESPport.print(Header); // отправка заголовка
      ESPport.print(Content); // отправка содержимого пакета
      delay(110);
    }
}
/////////////////////отправка АТ-команд/////////////////////
String GetResponse(String AT_Command, int wait)
{
  String tmpData;
  
  ESPport.println(AT_Command);
  delay(wait);
  while (ESPport.available() >0 )  
   {
    char c = ESPport.read();
    tmpData += c;
    
//    if ( tmpData.indexOf(AT_Command) > -1 )         
//      tmpData = "";       
      delay(5);
   }
  return tmpData;
}
//////////////////////очистка ESPport////////////////////
void clearSerialBuffer(void) 
{
       while ( ESPport.available() > 0 ) 
       {
         ESPport.read();
       }
}
////////////////////очистка буфера//////////////////////// 
void clearBuffer(void) {
       for (int i =0;i<BUFFER_SIZE;i++ ) 
       {
         buffer[i]=0;
       }
}
////////////////////подключение к wifi/////////////////////        
boolean connectWiFi(String NetworkSSID,String NetworkPASS) 
{
  String cmd = "AT+CWJAP=\"";
  cmd += NetworkSSID;
  cmd += "\",\"";
  cmd += NetworkPASS;
  cmd += "\"";
  Serial.println(cmd); 
  Serial.println(GetResponse(cmd,6500));
}

