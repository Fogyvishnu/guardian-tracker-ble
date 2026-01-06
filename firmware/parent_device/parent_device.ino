#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>

BLEScan* pBLEScan;
String targetMAC = "d8:3b:da:a4:e5:39"; // Replace with your beacon's MAC address
int liveRSSI;
float estimatedDirection;
int angles[36];
/*int rssiData[4][36] = {
  { -40, -42, -45, -47, -50, -52, -54, -55, -57, -58, -59, -60, -61, -62, -63, -64, -65, -66, -67, -68, -69, -70, -71, -72, -73, -74, -75, -76, -77, -78, -79, -80, -81, -82, -83, -84 }, // 1m
  { -50, -52, -55, -57, -60, -62, -64, -65, -67, -68, -69, -70, -71, -72, -73, -74, -75, -76, -77, -78, -79, -80, -81, -82, -83, -84, -85, -86, -87, -88, -89, -90, -91, -92, -93, -94 }, // 2m
  { -60, -62, -65, -67, -70, -72, -74, -75, -77, -78, -79, -80, -81, -82, -83, -84, -85, -86, -87, -88, -89, -90, -91, -92, -93, -94, -95, -96, -97, -98, -99, -100, -101, -102, -103, -104 }, // 3m
  { -70, -72, -75, -77, -80, -82, -84, -85, -87, -88, -89, -90, -91, -92, -93, -94, -95, -96, -97, -98, -99, -100, -101, -102, -103, -104, -105, -106, -107, -108, -109, -110, -111, -112, -113, -114 }  // 5m
};*/
int rssiAngles[36] = {  // Pre-measured RSSI data for angles
  -40, -42, -43, -45, -48, -50, -52, -55, -57, -58, -60, -61, -63, -64, -65, -66,
  -67, -68, -69, -70, -71, -72, -73, -74, -75, -76, -77, -78, -79, -80, -81, -82,
  -83, -84, -85, -86
};

const int ledPins[8] = {8,18,17,16,15,9,46,3}; 

void setup() {
  Serial.begin(115200);
  pinMode(7, OUTPUT);
  BLEDevice::init("ESP32-SCANNER");
  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Start with all LEDs off
  }
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true); // Active scan for more detailed data
  Serial.println("BLE Scanner Started");
}

void loop() {
  BLEScanResults* foundDevices = pBLEScan->start(2); // Scan for 2 seconds
  for (int i = 0; i < foundDevices->getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices->getDevice(i);
    String devAddress = device.getAddress().toString().c_str();

    if (devAddress.equals(targetMAC)) { // Match specific device MAC
      int rssi = device.getRSSI();
      Serial.printf("Target Device Found! RSSI: %d\n", rssi);

      // Estimate distance using RSSI
      float distance = pow(10, (-(rssi + 54)) / (10*2.75));  // 2.4 GHz frequency assumed
      Serial.printf("Estimated Distance: %.2f meters\n", distance);
      estimatedDirection = estimateAngle(rssi);
      Serial.println("Direction : " + String(estimatedDirection)+" degrees");
      if (estimatedDirection >= 0 && estimatedDirection <= 360) {
      int ledIndex = angleToLED(estimatedDirection);

      if(distance >= 6.0){
        digitalWrite(7, HIGH);
      }
      else {
        digitalWrite(7, LOW);
      }
      
      // Turn on the LED corresponding to the calculated index and turn off the others
      for (int i = 0; i < 8; i++) {
        digitalWrite(ledPins[i], (i == ledIndex) ? HIGH : LOW);
      }
    }
    }
  }
  
  delay(1000);
}
int angleToLED(int angle) {
  int segment = angle / 45; // Divide angle by 45 to get segment index (0 to 7)
  return segment % 8; // Ensure index wraps around at 360 degrees
}
/*int getLiveAngle(int liveRSSI) {
  int bestAngle = 0;
  int minDifference = 1000; // Large initial value

  for (int angleStep = 0; angleStep < 36; angleStep++) {
    int avgRSSI = (rssiData[0][angleStep] + rssiData[1][angleStep] + 
                   rssiData[2][angleStep] + rssiData[3][angleStep]) / 4;

    int difference = abs(liveRSSI - avgRSSI);

    if (difference < minDifference) {
      minDifference = difference;
      bestAngle = angleStep * 10;
    }
  }

  return bestAngle;
}*/
int estimateAngle(int liveRSSI) {
  int minDiff = 1000;  // Large initial value
  int bestAngle = 0;

  for (int i = 0; i < 36; i++) {
    int diff = abs(liveRSSI - rssiAngles[i]);
    if (diff < minDiff) {
      minDiff = diff;
      bestAngle = i * 10;  // Angle step (0°, 10°, ..., 350°)
    }
  }

  return bestAngle;
}