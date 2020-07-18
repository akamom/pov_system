#include <SoftwareSerial.h> 
SoftwareSerial MyBlue(2, 3); // RX | TX 

 
void setup() {
  Serial.begin(9600);
  MyBlue.begin(9600);
    
  DDRD |= B10000000;      //7 as output  
  DDRB |= B00011111;      //8 to 12 as output
  
  PORTD &= B00000011;     //2 to 7 LOW
  PORTB &= B00000000;     //8 to 13 LOW

}

void loop() {
  if (MyBlue.available()>0) {
    String data=MyBlue.readString(); //reading the data from the bluetooth module
    PORTD &= B00000000; //Pins 7,8,9
    PORTB &= B00000000; //Pins 10
    if(data == "7"){
      PORTD |= B10000000; //Pins 7
    }
    else if (data == "8"){
      //PORTD |= B10000000; //Pins 7
      PORTB |= B00000001; //Pins 8
    }
    else if (data == "9"){
      //PORTD |= B10000000; //Pins 7
      PORTB |= B00000010; //Pins 8,9
    }
    else if (data == "10"){
      //PORTD |= B10000000; //Pins 7
      PORTB |= B00000100; //Pins 8,9,10
    }
    else if (data == "11"){
      //PORTD |= B10000000; //Pins 7
      PORTB |= B00001000; //Pins 8,9,10,11
    }
    else if (data == "12"){
      //PORTD |= B10000000; //Pins 7
      PORTB |= B00010000; //Pins 8,9,10,11
    }
    Serial.println(data); 
    delay(1000);
  }
}
