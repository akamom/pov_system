#include <SoftwareSerial.h> 
SoftwareSerial MyBlue(2, 3); // RX | TX 

int led=8;
 
void setup() {
  pinMode(8,OUTPUT);
  Serial.begin(38400);
  MyBlue.begin(9400);
  }

void loop() {
  if (MyBlue.available()>0) {
    String data=MyBlue.readString(); //reading the data from the bluetooth module
    if(data == "a")
    {
      digitalWrite(8,HIGH); // Pressing 'A' will power the led
    }
    else if (data != -1){
      digitalWrite(8,LOW); // pressing 'B' will turn off the led
    }
    Serial.println(data); 
    delay(1000);
  } 
}
