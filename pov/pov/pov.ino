//********Bluetooth****************************************************************************************
#include <SoftwareSerial.h> 
SoftwareSerial MyBlue(2, 3); // RX | TX 

//********Speed*******************************************************************************************
unsigned long currentTime = 0;

unsigned long recentTim = 0;

unsigned long elapsed Time = 0;

int delayTime;


//********Gyro****************************************************************************************
#include "I2Cdev.h"

#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
#endif

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
   int stepDeg = 15;
   int quads [4][6][6] = {
                            //Quad 1
                            {
                              {1,1,1,1,1,1},
                              {1,1,1,1,1,1},
                              {1,1,1,1,1,1},
                              {0,0,0,0,0,0},
                              {0,0,0,0,0,0},
                              {0,0,0,0,0,0}
                            },
                            //Quad 2
                            {
                              {1,0,1,0,1,0},
                              {1,0,1,0,1,0},
                              {1,0,1,0,1,0},
                              {0,1,0,1,0,1},
                              {0,1,0,1,0,1},
                              {0,1,0,1,0,1}
                            },
                            //Quad 3
                            {
                              {1,0,1,0,1,0},
                              {1,0,1,0,1,0},
                              {1,0,1,0,1,0},
                              {0,1,0,1,0,1},
                              {0,1,0,1,0,1},
                              {0,1,0,1,0,1}
                            },
                            //Quad 4
                            {
                              {1,0,1,0,1,0},
                              {1,0,1,0,1,0},
                              {0,1,0,1,0,1},
                              {0,1,0,1,0,1},
                              {0,1,0,1,0,1}
                            }
                          };
    int yawModQuad; //index of current quadrant
    int yawModDeg; //index of line in quadrant


void setup() {
  Serial.begin(115200);
  //********Bluetooth**********************************************************************************
  MyBlue.begin(9600);

  //********LED****************************************************************************************
  DDRD |= B10000000;      //7 as output  
  DDRB |= B00111111;      //8 to 13 as output
  
  PORTD &= B00000011;     //2 to 7 LOW
  PORTB &= B00000000;     //8 to 13 LOW

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

  // supply your own gyro offsets here, scaled for min sensitivity
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  // make sure it worked (returns 0 if so)
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
  recentTime = micros();
  if (MyBlue.available()>0) {
    String data=MyBlue.readString(); //reading the data from the bluetooth module
    Serial.println("Received Data: " + data); 
  }  
    
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { // Get the Latest packet 
    // display Euler angles in degrees
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    yaw = ypr[0] * 180/M_PI;

    //get elapsed time for one rotation
    if(abs(yaw) >= 350 && abs(yaw) < 360){
      currentTime = millis();
      elapsedTime = currentTime - recentTime;
      delayTime = elapsedTime / 360 * stepDeg;
    }
    
    Serial.print("ypr\t");
    Serial.println(yaw);
    yawModQuad = (int)yaw % 90;
    yawModDeg = (int)yaw % 15;
    
    for(int i = 0; i< 6; i++){
      if(quads[yawModQuad][yawModDeg][i] > 0){
        switch(i){
        case 0:
          PORTB |= B00000001;
          break;
        case 1:
          PORTB |= B00000010;
          break;
        case 2:
          PORTB |= B00000100;
          break;
        case 3:
          PORTB |= B00001000;
          break;
        case 4:
          PORTB |= B00010000;
          break;
        case 5:
          PORTB |= B00100000;
          break;
        }
      }
    }
    delayMicroseconds(delaytime);
    PORTB &= B11000000; //
  }
}
