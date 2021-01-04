#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define jB1 7  // Joystick button 1
#define jB2 8  // Joystick button 2

RF24 radio(9, 10);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001";

struct Data_Package {
  int X1;
  int Y1;
  int X2;
  int Y2;
  bool b1;
  bool b2;
} data;

void setup() {
  Serial.begin(9600);

  // Begin the transmission between the 2 arduinos
  radio.begin();
  radio.openWritingPipe(address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  
  // Activate the Arduino internal pull-up resistors
  pinMode(jB1, INPUT_PULLUP);
  pinMode(jB2, INPUT_PULLUP);
  
  // Set initial default values
  data.X1 = 504;
  data.Y1 = 504;
  data.X2 = 504;
  data.Y2 = 504;
  data.b1 = 0;
  data.b2 = 0;
}

void loop() {
  data.X1 = analogRead(A0); // X-axis from right joystick
  data.Y1 = analogRead(A1); // Y-axis from right joystick
  data.X2 = analogRead(A2); // X-axis from left joystick
  data.Y2 = analogRead(A3); // Y-axis from left joystick

  data.b1 = digitalRead(jB1); // Right joystick button
  data.b2 = digitalRead(jB2); // Left joystick button
  
  // Send the whole data from the structure to the receiver
  radio.write(&data, sizeof(Data_Package));
}
