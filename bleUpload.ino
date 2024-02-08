/*
 * BLE Beacon Pet Indoor location
 * Roni Bandini @ronibandini Febr 2024
 * bandini.medium.com
 * MIT License
 * Arduino IDE: Select Firebeetle 2 ESP32 with Partition Scheme HUGE 
 * For Fermion BLE Beacon Pack
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <BLEDevice.h>
#include <BLEScan.h>

bool device_found;
int scanTime=5; 
int rssi=0;
String deviceMac="";
int httpResponseCode=0;

BLEScan* pBLEScan;
HTTPClient http;

// Edit WiFi Credentials
const char* ssid = "";
const char* password = "";

// Edit location map PHP server URL
String serverName = "http://YourServerHere.com/updateBle.php";
String serverPath="";

// Edit Beacons Mac
String location1="01:00:00:00:00:00";
String location2="02:00:00:00:00:00";
String location3="03:00:00:00:00:00";

// RSSI signal limit
int myThreshold=-65;

String lastLocation="";
int updateLocation=0;

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

void blink(byte n){
  for(; n > 0; n--){
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(500);
    digitalWrite(LED_BUILTIN, LOW); 
    delay(500);
  } 
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
           
      rssi = advertisedDevice.getRSSI();
      deviceMac=advertisedDevice.getAddress().toString().c_str();
      String locationName=advertisedDevice.getName().c_str();

      if (locationName!=""){
          Serial.print("Name : ");
          Serial.println(locationName);
          Serial.print("Beacon MAC: ");
          Serial.println(deviceMac);
          Serial.print("RSSI : ");
          Serial.println(rssi);
          Serial.print("");
      }

      if (deviceMac==location1 and lastLocation!=location1 and rssi>myThreshold){        
                Serial.println(" ** Change 1");                           
                Serial.println(" Pet is at "+locationName);                           
                lastLocation=location1;     
                updateLocation=1;   
      
        } // location 1

      else if (deviceMac==location2 and lastLocation!=location2 and rssi>myThreshold){        
                Serial.println(" ** Change 2"); 
                Serial.println(" Pet is at "+locationName);                           
                lastLocation=location2;     
                updateLocation=1;   
      
        } // location 2

      else if (deviceMac==location3 and lastLocation!=location3 and rssi>myThreshold){        
                Serial.println(" ** Change 3");   
                Serial.println(" Pet is at "+locationName);                          
                lastLocation=location3;     
                updateLocation=1;   
      
        } // location 3

      if (updateLocation==1){
                  
                  blink(2);

                  Serial.print("Updating server location...");
                  
                  if (lastLocation==location1) {serverPath = serverName + "?location=1";}
                  if (lastLocation==location2) {serverPath = serverName + "?location=2";}
                  if (lastLocation==location3) {serverPath = serverName + "?location=3";}
                  
                  http.begin(serverPath.c_str());                          
                  httpResponseCode = http.GET();
                
                if (httpResponseCode>0) {
                  Serial.print("HTTP Response code: ");
                  Serial.println(httpResponseCode);
                }
                else {
                  Serial.print("Error code: ");
                  Serial.println(httpResponseCode);
                }
                // Free resources
                http.end();
                
                // pause to scan again
                delay(10000);

        updateLocation=0;
        }

    }
};

void setup() {
  
  Serial.begin(115200); 

  pinMode(LED_BUILTIN, OUTPUT);
  
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("BLE Indoor location started");
  Serial.println("Roni Bandini, February 2024");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); // Callback Function
  pBLEScan->setActiveScan(true);  
  pBLEScan->setInterval(100); // Scan interval
  pBLEScan->setWindow(99);  // less or equal setInterval value
}

void loop() {
  
  // Check beacons every 10 seconds
  if ((millis() - lastTime) > timerDelay) {

    
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
        digitalWrite(LED_BUILTIN, HIGH); 
        BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
        pBLEScan->clearResults();  
        digitalWrite(LED_BUILTIN, LOW); 
           
    }
    else {
      Serial.println("WiFi Disconnected :(");
    }
    lastTime = millis();
  }
}
