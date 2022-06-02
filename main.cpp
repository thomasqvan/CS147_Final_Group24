#include <Arduino.h>
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <SparkFunLSM6DSO.h>
#include <Wire.h>
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define LED 32
#include <HttpClient.h>
#include <WiFi.h>

#include <TFT_eSPI.h>
TFT_eSPI tft = TFT_eSPI();
#include <DHT20.h>
DHT20 DHT;
#include <MAX30105.h>
#include "heartRate.h"
MAX30105 particleSensor;

LSM6DSO myIMU;
int step_counter = 0;
float first_acc_x = 0;
float first_acc_y = 0;
float first_acc_z = 0;
float first_gyro_x = 0;
float first_gyro_y = 0;
float first_gyro_z = 0;
BLECharacteristic *pCharacteristic;
int mode = 0;
class MyCallbacks: public BLECharacteristicCallbacks {
   void onWrite(BLECharacteristic *pCharacteristic) {
     std::string value = pCharacteristic->getValue();
 
     if (value.length() > 0) {
       Serial.println("*********");
       Serial.print("New value: ");
       for (int i = 0; i < value.length(); i++)
         Serial.print(value[i]);
 
       Serial.println();
       Serial.println("*********");
     }
     digitalWrite(LED, HIGH);
     if(value == "1") {

       digitalWrite(LED, HIGH);
       mode = 1;
     }
     else if (value == "0") {
       digitalWrite(LED, LOW);
       mode = 0;
     }
     delay(500);
   }
};

// char ssid[] = "UCInet Mobile Access";    // your network SSID (name) 
char ssid[] = "VDCN-Resident";    // your network SSID (name) 
// char pass[] = "";
char pass[] = "AC86fm!6";
const char kHostname[] = "54.67.30.223";
char kPath[] = "/?var=10"; // "/?var=10&var2=12";
const char kPort[] = "5000";
const int kNetworkTimeout = 30*1000;
const int kNetworkDelay = 1000;

void setup() {
 Serial.begin(115200);
 tft.init();
 tft.fillScreen(TFT_BLACK);
 DHT.begin(21, 22);

particleSensor.begin(Wire, I2C_SPEED_FAST);
particleSensor.setup();
particleSensor.setPulseAmplitudeRed(0x0A);
particleSensor.setPulseAmplitudeGreen(0);

 delay(1000);
 
//  BLEDevice::init("CS147_GROUP24_FINAL"); // 
//  BLEServer *pServer = BLEDevice::createServer(); // 
//  BLEService *pService = pServer->createService(SERVICE_UUID); //
//  pCharacteristic = pService->createCharacteristic( //
//                                         CHARACTERISTIC_UUID, //
//                                         BLECharacteristic::PROPERTY_READ | //
//                                         BLECharacteristic::PROPERTY_WRITE //
//                                       ); //
//   pCharacteristic->setCallbacks(new MyCallbacks()); //
//   pService->start(); //
//   BLEAdvertising *pAdvertising = pServer->getAdvertising(); //
//   pAdvertising->start(); //

 pinMode(LED, OUTPUT);
  delay(500);
  Wire.begin();
  delay(10);
  if( myIMU.begin() )
    Serial.println("Ready.");

  if( myIMU.initialize(BASIC_SETTINGS) )
    Serial.println("Loaded Settings.");
    tft.setRotation(1);

  WiFi.begin(ssid, pass); //
  while (WiFi.status() != WL_CONNECTED) { //
      delay(500); //
      Serial.print("."); //
  } // 
}
 
void loop() {
  tft.setTextSize(2);
  tft.setCursor(24, 15);
  tft.print("CS147 Fitness App");
  tft.setCursor(0, 40);
  tft.drawLine(0, 35, 250, 35, TFT_GREEN);
  tft.setCursor(0, 60);

    long irValue = particleSensor.getIR();

    Serial.print("IR=");
    Serial.print(irValue);
    Serial.print(", BPM=");
    if (irValue < 50000) {
      Serial.print(0);
      tft.setCursor(160, 120);
      tft.print("00.00");  
    }
    else {
      Serial.print(irValue/(double)1500);
      tft.setCursor(160, 120);
      tft.print(irValue/(double)1500);
    }

    Serial.println();


  DHT.read();
  float h = DHT.getHumidity();
  float t = DHT.getTemperature();
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("\nTemperature: ");
  Serial.println(t);

  tft.setCursor(10, 45);
  if(mode == 0) {
    tft.print("Humidity:  Temp:(C)");
  }
  else if (mode == 1) {
    tft.print("Humidity:  Temp:(F)");
  }
  tft.setCursor(30, 70);
  tft.print(h);
  tft.setCursor(160, 70);
  if (mode == 0) {
    tft.print(t);
  }
  else if (mode == 1) {
    tft.print(t*1.8 + 32);
  }
  tft.setCursor(10, 98);
  tft.print("Step Cnt:  BPM: ");


  if(first_acc_x == 0) {

    first_acc_x = myIMU.readFloatAccelX();
    first_acc_y = myIMU.readFloatAccelY();
    first_acc_z = myIMU.readFloatAccelZ();

    first_gyro_x = myIMU.readFloatGyroX();
    first_gyro_y = myIMU.readFloatGyroY();
    first_gyro_z = myIMU.readFloatGyroZ();
  }
  else {

    if(abs(myIMU.readFloatAccelX() - first_acc_x) > 0.15 && abs(myIMU.readFloatAccelY() - first_acc_y) > 0.035) {
      step_counter += 1;
    }

    Serial.println("Step Counter: ");
    Serial.print(step_counter);
    tft.setCursor(50, 120);
    tft.print(step_counter);
    Serial.println("\n");
    first_acc_x = myIMU.readFloatAccelX();
    first_acc_y = myIMU.readFloatAccelY();
    first_acc_z = myIMU.readFloatAccelZ();

    first_gyro_x = myIMU.readFloatGyroX();
    first_gyro_y = myIMU.readFloatGyroY();
    first_gyro_z = myIMU.readFloatGyroZ();
  }
  // pCharacteristic->setValue(step_counter); //

  // Connection to AWS Cloud Connectivity
  
  int err = 0;
  WiFiClient c;
  HttpClient http(c);
  sprintf(kPath, "/?var1=%f", irValue/(double)1500);
  err = http.get(kHostname,5000, kPath);
  if (err == 0)
  {
    Serial.println("startedRequest ok");

    err = http.responseStatusCode();
    if (err >= 0)
    {
      Serial.print("Got status code: ");
      Serial.println(err);

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        int bodyLen = http.contentLength();
        Serial.print("Content length is: ");
        Serial.println(bodyLen);
        Serial.println();
        Serial.println("Body returned follows:");
      
        unsigned long timeoutStart = millis();
        char c;
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                Serial.print(c);
               
                bodyLen--;
                timeoutStart = millis();
            }
            else
            {
                delay(kNetworkDelay);
            }
        }
      }
      else
      {
        Serial.print("Failed to skip response headers: ");
        Serial.println(err);
      }
    }
    else
    {    
      Serial.print("Getting response failed: ");
      Serial.println(err);
    }
  }
  else
  {
    Serial.print("Connect failed: ");
    Serial.println(err);
  }
  http.stop();
  delay(3000);
}

