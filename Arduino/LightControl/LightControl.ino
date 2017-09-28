/*
  Serial Event example

  When new serial data arrives, this sketch adds it to a String.
  When a newline is received, the loop prints the string and clears it.

  A good test for this is to try it with a GPS receiver that sends out
  NMEA 0183 sentences.

  NOTE: The serialEvent() feature is not available on the Leonardo, Micro, or
  other ATmega32U4 based boards.

  created 9 May 2011
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/SerialEvent
*/
int green_1 = 8, green_2 = 10, blue_1 = 9, blue_2 = 11;
String inputString = "";         // a String to hold incoming data
boolean stringComplete = false;  // whether the string is complete

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  pinMode(green_1, OUTPUT);
  pinMode(green_2, OUTPUT);
  pinMode(blue_1, OUTPUT);
  pinMode(blue_2, OUTPUT);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    Serial.println(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
    Serial.println((int)inChar);
    if (inChar == '1') {
      digitalWrite(green_1, HIGH);
      digitalWrite(green_2, HIGH);
    }
    if (inChar == '2') {
      digitalWrite(blue_1, HIGH);
      digitalWrite(blue_2, HIGH);
    }
    if (inChar == '3') {
      digitalWrite(blue_1, LOW);
      digitalWrite(blue_2, LOW);
      digitalWrite(green_1, LOW);
      digitalWrite(green_2, LOW);
    }
  }
}
