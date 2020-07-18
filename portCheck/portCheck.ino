void setup() {
  // put your setup code here, to run once:
   DDRD |= B11111100;      //2 to 7 as output  
  DDRB |= B00011111;      //8 to 12 as output
  //DDRE |= B00111111;
  DDRB &= B11011111;      //13 input
  PORTD &= B00000011;     //2 to 7 LOW
  PORTB &= B00000000;     //8 to 13 LOW
}

void loop() {
  // put your main code here, to run repeatedly:
    //PORTE |= B00000001; //0???
    //PORTE |= B00000010; //1 ???
    PORTE |= B00010000; //2
    PORTE |= B00100000; //3
    PORTG |= B00100000; //4
    PORTE |= 000001000; //5
    PORTH |= 000010000; //6
    //PORTH |= B00010000; //7
    //PORTH |= B00100000; //8
    //PORTH |= B01000000; //9
    //PORTB |= B00010000; //10
    //PORTB |= B00100000; //11
    //PORTB |= B01000000; //12



    //PORTH |= (1 << PORTH5); //8
    //PORTH |= (1 << PORTH6); //9
    //PORTB |= (1 <<PORTB4); //10
    //PORTB |= (1 <<PORTB5); //11
    //PORTB |= (1 <<PORTB6); //12
    //PORTB |= B10000000;
}
