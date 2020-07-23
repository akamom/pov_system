#include "ctype.h"

//********Bluetooth****************************************************************************************
#include <SoftwareSerial.h> 
SoftwareSerial MyBlue(2,3); // RX | TX 

//********LED STRIP**********************************************************************************
#include <Adafruit_NeoPixel.h>
#define PIN            6 
#define NUMPIXELS      10
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN);

//********Speed*******************************************************************************************
int finishTime = 0;
int startTime = 0;
int delayTime = 0;

//********Gyro****************************************************************************************
// Source: https://github.com/jrowberg/i2cdevlib.git

#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"

#include "Wire.h"

MPU6050 mpu;

bool blinkState = false;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
float yaw;

//********STEPS***************************************************************************************
int stepDeg = 90 / NUMPIXELS ;
int quads [4][NUMPIXELS][NUMPIXELS][3];
int yawDivQuad; //index of current quadrant
int yawDivDeg; //index of line in quadrant


//********DEBUG***************************************************************************************
bool firstRun = true;
String message = "0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;255,255,255;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;0,0,0;";
//String message = "1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;1,1,1;2,2,2;3,3,3;4,4,4;5,5,5;6,6,6;7,7,7;8,8,8;9,9,9;10,10,10;";


void setup() {
  Serial.begin(115200);
  
  //********Bluetooth**********************************************************************************
  MyBlue.begin(9600);

  //********LED****************************************************************************************
  pixels.begin();
  pixels.show();
  pixels.setBrightness(50); //max = 255

  
  //********GYRO***************************************************************************************
  // Source: https://github.com/jrowberg/i2cdevlib.git
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
      Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
  
  while (!Serial); // wait for Leonardo enumeration, others continue immediately

  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();

  // verify connection
  Serial.println(F("Testing device connections..."));
  Serial.println(mpu.testConnection() ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  // load and configure the DMP
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();

  // set gyro offsets 
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // returns 0 if connection established
  if (devStatus == 0) {
      mpu.CalibrateAccel(6);
      mpu.CalibrateGyro(6);
      mpu.PrintActiveOffsets();
      // turn on the DMP, now that it's ready
      Serial.println(F("Enabling DMP..."));
      mpu.setDMPEnabled(true);

      Serial.println(F("DMP ready! Waiting for first interrupt..."));
      dmpReady = true;

      packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
      Serial.print(F("DMP Initialization failed (code "));
      Serial.print(devStatus);
      Serial.println(F(")"));
  }
}


void loop() {  
  if(firstRun){
    String result = textParse(message);
    Serial.println("Parse Reuslt: " + result); 
    firstRun = false;
    for(int i = 0; i < 4; i++){
      Serial.println("Quad " + String(i+1));
      for(int j = 0; j < NUMPIXELS; j++){
        for(int k = 0; k < NUMPIXELS; k++){
          String r = String(quads[i][j][k][0]);
          String g = String(quads[i][j][k][1]);
          String b = String(quads[i][j][k][2]);
          Serial.print("{" + r + "," + g + "," + b + "} ");
        }
        Serial.println("");
      }
    }
    Serial.println("Press the Any-Key to continue.");
    while(!Serial.available()){}
    Serial.read();
    
  Serial.println("Starting POV System");
  }
  else if (MyBlue.available()>0) {
    String data=MyBlue.readString(); //reading the data from the bluetooth Divule
    String result = textParse(data);
    Serial.println("Received Data: " + data);
    Serial.println("Parse Reuslt: " + result); 
  }  
  if(CalcYaw()); 
  {    
    Serial.print("ypr\t");
    Serial.println(yaw);

    //when new rotation starts
    if(yaw >= 0 && yaw < 0,1){
      startTime = millis();
    }
    //when full rotation is done
    if(yaw >= -0,1 && yaw < 0){
      finishTime = millis();
      delayTime = (finishTime - startTime) / 360 * stepDeg;
    }

    /* Quadrants:
        Q1 = 0 | Q2 = 1
        -------|-------
        Q3 = 2 | Q4 = 4 
     */
    yawDivQuad = (int)yaw / 90;
    if(yaw >= 0 && yawDivQuad == 0) yawDivQuad = 1;
    else if(yaw >= 0 && yawDivQuad == 1) yawDivQuad = 3;
    else if(yaw < 0 && yawDivQuad == 0) yawDivQuad = 0;
    else if(yaw < 0 && yawDivQuad == -1) yawDivQuad = 2;
    
    yawDivDeg = (int)yaw / stepDeg;

    //pov
    for(int i = 0; i< NUMPIXELS ; i++){
      int r = quads[yawDivQuad][i][yawDivDeg][0];
      int g = quads[yawDivQuad][i][yawDivDeg][1];
      int b = quads[yawDivQuad][i][yawDivDeg][2];
      pixels.setPixelColor(i, r,g,b);
    }
  }
  pixels.show();
  delayMicroseconds(delayTime);
  pixels.clear();
}
//*****************************************************************************************************
//Functions

//in: string "r,g,b;.....;"
//fills quads[][][][] with {r,g,b}
String textParse(String message){
  String pixelTempStr = ""; //Substring containg current nummber
  int colorIndex = 0; //index which color is currently dected r=0, g=1, b=2
  int savedLedCount = 0; //counts how many LEDs were already retrieved from string
  int quadIndex = 0; //index of current quadrant 0,1,2,3
  int rowIndex = 0;  //index of current row 0 -> nbr of LEDs - 1
  int colIndex = 0;  //index of current column 0 -> nbr of LEDs - 1
  
  int pixelTempInt[3]; //temporarly saves color of a pixels
  
  Serial.println(message);
  for(int i = 0; i < message.length(); i++){
    //if char is digit
    if(isDigit(message[i])){
      pixelTempStr += message[i];
    }
    //if number is completed
    else if(message[i] == ','){
      pixelTempInt[colorIndex] = pixelTempStr.toInt();
      colorIndex++;
      pixelTempStr = "";
    }
    //if coordinate is completed color gets saved in array structure depending on the current count of saved LEDs
    else if(message[i] == ';'){
      pixelTempInt[colorIndex] = pixelTempStr.toInt();
      pixelTempStr = "";
      colorIndex = 0;
      
      //get quadrant
      if(savedLedCount < 2*NUMPIXELS * NUMPIXELS){
        quadIndex = (savedLedCount / NUMPIXELS) % 2;
      }
      else{
        quadIndex = (savedLedCount / NUMPIXELS) % 2;
        quadIndex += 2;
      }
      
      //get row
      int temp = 0;
      if(quadIndex == 2){
        temp = 2* NUMPIXELS * NUMPIXELS;      //buffer for 3rd quadrant
      }   
      //starting at 2nd quadrant rowIndex will be resetted
      if(savedLedCount == 2*NUMPIXELS * NUMPIXELS) rowIndex = 0;
      else if((rowIndex + 1) * NUMPIXELS * 2 + temp == savedLedCount) rowIndex++; 
      
      //get column
      if(savedLedCount < NUMPIXELS){
        colIndex = savedLedCount;
      }
      else{
        colIndex = savedLedCount % NUMPIXELS;
      }   
      
      // set color for current Pixel
      for(int j = 0; j < 3; j++){
        quads[quadIndex][rowIndex][colIndex][j] = pixelTempInt[j];
      }        
      savedLedCount ++;
    }
    else {
      Serial.println("ERROR -- unnknown character detected" );
      return "ERROR -- unnknown character detected";
    }
  }
  return "test";
}

//Calculates the current angle of the rotation 0->180°, -180°<-0
bool CalcYaw(){
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { // Get the Latest packet    
    // get current angle
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    yaw = ypr[0] * 180/M_PI;
    
    return true;
  }
  return false;
}
