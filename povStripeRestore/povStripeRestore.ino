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


//******Yaw**********************************************************************************
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
  if (MyBlue.available()>0) {
    String data=MyBlue.readString(); //reading the data from the bluetooth Divule
    Serial.println("received data: " + data);
    
    pixels.setPixelColor(7, 255,0,0);
    pixels.setPixelColor(8, 255,0,0);
    pixels.setPixelColor(9, 255,0,0);
    pixels.setPixelColor(10, 255,0,0);
    delay(1000);
    pixels.clear();
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

    //Serial.print("DivQuad\t");
    //Serial.println(yawDivQuad);
    
    yawDivDeg = (int)yaw / stepDeg;

    //pov
    //for(int i = 0; i< NUMPIXELS ; i++){
      int r = 250;
      int g = 250;
      int b = 250;
      int j;
      int k;
      if(yaw >= 0 && yaw < 18) {
        j = 0;
        k = 1;
      }else if(yaw >= 18 && yaw < 36) {
        j = 2;
        k = 3;
      }else if(yaw >= 36 && yaw < 54) {
        j = 4;
        k = 5;
      }else if(yaw >= 54 && yaw < 72) {
        j = 6;
        k = 7;
      }else if(yaw >= 72 && yaw < 90) {
        j = 8;
        k = 9;
      }else if(yaw < 0 && yaw >= -18) {
        j = 0;
        k = 1;
      }else if(yaw < -18 && yaw >= -36) {
        j = 2;
        k = 3;
      }else if(yaw < -36 && yaw >= -54) {
        j = 4;
        k = 5;
      }else if(yaw < -54 && yaw >= -72) {
        j = 6;
        k = 7;
      }else if(yaw < -72 && yaw >= -90) {
        j = 8;
        k = 9;
      }
      pixels.setPixelColor(j, r,g,b);
      pixels.setPixelColor(k, r,g,b);
    //}
  }
  pixels.show();
  delayMicroseconds(0);
  pixels.clear();
}
