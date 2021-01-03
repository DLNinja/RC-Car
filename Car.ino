#include <SPI.h>
#include <printf.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

Servo myservo;  // create servo object to control the servo
int pos = 105;  // variable to store the servo position
// 105 is the angle at which the car goes straight


int enA = 7;
int in1 = 3;
int in2 = 4;

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

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

  // Begin transmission
  radio.begin();
  printf_begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.printDetails();
  radio.startListening();

  myservo.attach(2);  // attaches the servo on pin 9 to the servo object
  myservo.write(pos);
  
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  resetData();
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
    lastReceiveTime = millis(); // The moment we received the data
  }
  
  currentTime = millis();
  
  if ( currentTime - lastReceiveTime > 1000 ) {
    // If the time since last received data is above 1 sec, we have probasbly lost connection
    resetData(); // If connection is lost, reset the data. 
  }
  
  // We'll only use data.X1 for front/back and data.Y2 for left/right movement

  // Mapping the Y-axis from our left joystick as the angle for the servo motor
  int x = map(data.Y2, 0, 1023, -30, 25); 
  
  // Here we'll decide if the car goes front, back or it stays in place
  if(data.X1 > 520){
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    analogWrite(enA, (data.X1 - 520)/2);
  }else if(data.X1 < 500){
    digitalWrite(in2, HIGH);
    digitalWrite(in1, LOW);
    analogWrite(enA, (500 - data.X1)/2);
  }else{
    analogWrite(enA, 0);
  }

  /// Writing the direction to the servo
  myservo.write(pos-x);
  
  delay(30);
  
  
  /*Serial.print(data.X1); Serial.print(" ");
  Serial.print(data.Y1); Serial.print(" ");
  Serial.print(data.X2); Serial.print(" ");
  Serial.print(data.Y2); Serial.print(" ");
  Serial.print(data.b1); Serial.print(" ");
  Serial.print(data.b2); Serial.println();
  */
}

void resetData(){
  data.X1 = 504;
  data.Y1 = 504;
  data.X2 = 504;
  data.Y2 = 504;
  data.b1 = 0;
  data.b2 = 0;
}
