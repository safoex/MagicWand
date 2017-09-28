#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // указываем пины rx и tx
void setup()
{
Serial.begin(115200);
mySerial.begin(115200);
}
void loop()
{
if (mySerial.available())
{
int c = mySerial.read(); // читаем из software-порта
Serial.write(c); // пишем в hardware-порт
if (Serial.available())
{
int c = Serial.read(); // читаем из hardware-порта
mySerial.write(c); // пишем в software-порт
}
}
}
