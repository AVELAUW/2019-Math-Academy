//------------COMMUNICATION PROTOCOLS----------//

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
#endif

//----------------------------------------------//

//----------------SENSOR LIBRARIES--------------//

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
//#include "I2Cdev.h"
#include "MPU6050.h"
// Load the SD library, which will give us
// commands to interface to the SD card:
#include <SD.h>
// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high
// uncomment "OUTPUT_READABLE_ACCELGYRO" if you want to see a tab-separated
// list of the accel X/Y/Z and then gyro X/Y/Z values in decimal. Easy to read,
// not so easy to parse, and slow(er) over UART.
#define OUTPUT_READABLE_ACCELGYRO
// uncomment "OUTPUT_BINARY_ACCELGYRO" to send all 6 axes of data as 16-bit
// binary, one right after the other. This is very fast (as fast as possible
// without compression or data loss), and easy to parse, but impossible to read
// for a human.
//#define OUTPUT_BINARY_ACCELGYRO
//---------------------------------------------//


//-------------CONSTANT VARIABLES--------------//

const String camperDataFile = "test.csv"; // name must not exceed 8 characters, not including the ".csv"

int16_t ax, ay, az; // acceleration X, Y, Z
int16_t gx, gy, gz; // gyro X, Y, Z
//---------------------------------------------//


//--------------CONSTANT PINS------------------//

const int pinCS = 10; // Pin 10 on Arduino Uno
//---------------------------------------------//

File sensorData;
unsigned long time;

//==========================================================================================//
void setup() {
  // initialize serial communication
  // (38400 chosen because it works as well at 8MHz as it does at 16MHz, but
  Serial.begin(9600); // default is 9600
    
  // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // initialize device
    Serial.println("Initializing I2C devices...");
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  
  // use the code below to change accel/gyro offset values
  Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
  Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
  Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
  Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
  Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
  Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
  Serial.print("\n");
  accelgyro.setXAccelOffset(220);
  accelgyro.setYAccelOffset(76);
  accelgyro.setZAccelOffset(560);
  accelgyro.setXGyroOffset(220);
  accelgyro.setYGyroOffset(76);
  accelgyro.setZGyroOffset(560);
  
  // SD Card Initialization
  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
  
  // Check to see if the file exists:
  if (SD.exists(camperDataFile)) {
    // delete the file:
    SD.remove(camperDataFile);
  }
  // If the file doesn't exist:
  if (SD.exists(camperDataFile)==0) {
    // Create/Open file // File sensorData;
    sensorData = SD.open(camperDataFile, FILE_WRITE);
    sensorData.print("Time [ms]:");
    sensorData.print(",");
    sensorData.println("Acceleration-z [m/s^2]");
    sensorData.close(); // close the file
  }
  
  //-------------DELAY BEFORE STARTING LOOP--------------//
  // 10 second delay: delay is in ms, meaning every 1000ms = 1s
  delay(10000);
}

void loop() {
   // Create/Open file  
   sensorData = SD.open(camperDataFile, FILE_WRITE);

  // if the file opened okay, write to it:
  if (sensorData) {
    Serial.println("Writing to file...");

      //---------------TIME--------------------------
      Serial.print(" Time: ");
      time = millis();
      Serial.println(time);
      
      //--------------Accelerometer------------------
      // read raw accel/gyro measurements from device
      //accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      // these methods (and a few others) are also available
      Serial.print(" Acceleration (z-axis): ");
      accelgyro.getAcceleration(&ax, &ay, &az);
      Serial.println(az);
      //accelgyro.getRotation(&gx, &gy, &gz);

    // Write to file
    sensorData.print(time);
    sensorData.print(",");
    sensorData.println((String)az);
    sensorData.close(); // close the file
    //Serial.println("Done.");
  } else { // if the file didn't open, print an error:
    Serial.println("error opening text file: test.csv");
  }

  //Serial.println();
  delay(500);
}