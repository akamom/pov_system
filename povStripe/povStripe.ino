//********Bluetooth****************************************************************************************
#include <SoftwareSerial.h> 
SoftwareSerial MyBlue(2, 3); // RX | TX 

//********LED STRIP**********************************************************************************
#include <Adafruit_NeoPixel.h>
#define PIN            6 
#define NUMPIXELS      4
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
int quads [4][6/*NUMPIXELS*/][6/*NUMPIXELS*/] = {
                        //Quad 1
                        {
                          {1,0,0,0,0,0},
                          {1,0,0,0,0,0},
                          {1,0,0,0,0,0},
                          {1,0,0,0,0,0},
                          {1,0,0,0,0,0},
                          {1,0,0,0,0,0},
                        },
                        //Quad 2
                        {
                          {0,1,0,0,0,0},
                          {0,1,0,0,0,0},
                          {0,1,0,0,0,0},
                          {0,1,0,0,0,0},
                          {0,1,0,0,0,0},
                          {0,1,0,0,0,0},
                          
                        },
                        //Quad 3
                        {
                          {0,0,1,0,0,0},
                          {0,0,1,0,0,0},
                          {0,0,1,0,0,0},
                          {0,0,1,0,0,0},
                          {0,0,1,0,0,0},
                          {0,0,1,0,0,0},
                        },
                        //Quad 4
                        {
                          {0,0,0,1,0,0},
                          {0,0,0,1,0,0},
                          {0,0,0,1,0,0},
                          {0,0,0,1,0,0},
                          {0,0,0,1,0,0},
                          {0,0,0,1,0,0},
                        }
                      };
    int yawModQuad; //index of current quadrant
    int yawModDeg; //index of line in quadrant

void setup() {
  Serial.begin(115200);
  
  //********Bluetooth**********************************************************************************
  MyBlue.begin(9600);

  //********LED****************************************************************************************
  pixels.begin();
  pixels.show();
  pixels.setBrightness(50); //max = 255

  
  //********GYRO***************************************************************************************
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
    String data=MyBlue.readString(); //reading the data from the bluetooth module
    Serial.println("Received Data: " + data); 
  }  
    
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { // Get the Latest packet    
    // get current angle
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    yaw = ypr[0] * 180/M_PI;
    
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
    yawModQuad = (int)yaw / 90;
    if(yaw < 0){
      if(yawModQuad == -1) yawModQuad = 2;
      if(yawModQuad == 0) yawModQuad = 3;
    }

    Serial.print("modQuad\t");
    Serial.println(yawModQuad);
    
    yawModDeg = (int)yaw / stepDeg;
    
    for(int i = 0; i< NUMPIXELS ; i++){
      if(quads[yawModQuad][i][yawModDeg] > 0){
          pixels.setPixelColor(i, 255,255,255);
        }
      }
    }
    pixels.show();
}
