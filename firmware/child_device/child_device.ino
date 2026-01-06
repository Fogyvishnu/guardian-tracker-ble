#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WebSocketsServer.h>

// Define WiFi and WebSocket
WiFiMulti WiFiMulti;
WebSocketsServer webSocket = WebSocketsServer(81); // WebSocket server on port 81

BLEScan* pBLEScan;
String targetMAC = "d8:3b:da:a4:e5:39"; // Replace with your beacon's MAC address
int liveRSSI;
float estimatedDirection;
int angles[36];

int rssiAngles[36] = {  // Pre-measured RSSI data for angles
  -40, -42, -43, -45, -48, -50, -52, -55, -57, -58, -60, -61, -63, -64, -65, -66,
  -67, -68, -69, -70, -71, -72, -73, -74, -75, -76, -77, -78, -79, -80, -81, -82,
  -83, -84, -85, -86
};

const int ledPins[8] = {8, 18, 17, 16, 15, 9, 46, 3}; // Define the LED pins

void setup() {
  // Start serial for debugging
  Serial.begin(115200);
 pinMode(7, OUTPUT);
  // Set up WiFi connection
  WiFiMulti.addAP("vishnu", "88888888");  // Replace with your WiFi credentials
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  // Start WebSocket server
  webSocket.begin();
  webSocket.onEvent(webSocketEvent); // Set up WebSocket event handling

  // Initialize BLE
  BLEDevice::init("ESP32-SCANNER");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true);  // Active scan for more detailed data
  Serial.println("BLE Scanner Started");

  // Set up LED pins
  for (int i = 0; i < 8; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW); // Start with all LEDs off
  }
}

// WebSocket event handling
void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("Client connected from %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_DISCONNECTED:
      Serial.printf("Client %u disconnected\n", num);
      break;
    case WStype_TEXT:
      // Handle text messages (if needed)
      break;
    case WStype_BIN:
      // Handle binary messages (if needed)
      break;
    default:
      break;
  }
}

void loop() {
  // Start scanning for BLE devices
  BLEScanResults* foundDevices = pBLEScan->start(2); // Scan for 2 seconds

  for (int i = 0; i < foundDevices->getCount(); i++) {
    BLEAdvertisedDevice device = foundDevices->getDevice(i);
    String devAddress = device.getAddress().toString().c_str();

    if (devAddress.equals(targetMAC)) { // Match the specific device MAC address
      int rssi = device.getRSSI();
      Serial.printf("Target Device Found! RSSI: %d\n", rssi);

      // Estimate distance using RSSI
      float distance = pow(10, (-(rssi + 54)) / (10 * 2.75));  // 2.4 GHz frequency assumed
      Serial.printf("Estimated Distance: %.2f meters\n", distance);

      // Estimate direction using the RSSI value
      estimatedDirection = estimateAngle(rssi);
      Serial.printf("Direction: %.2f degrees\n", estimatedDirection);

      // Calculate LED index based on the estimated direction
      int ledIndex = angleToLED(estimatedDirection);

      // Send calculated data to WebSocket client
      String message = "LED: " + String(ledIndex) + ", Distance: " + String(distance);
      webSocket.broadcastTXT(message);

      // If distance is greater than or equal to 6 meters, turn on a warning LED
      if (distance >= 6.0) {
        digitalWrite(7, HIGH);  // Turn on warning LED
      } else {
        digitalWrite(7, LOW);  // Turn off warning LED
      }

      // Turn on the LED corresponding to the calculated index and turn off others
      for (int i = 0; i < 8; i++) {
        digitalWrite(ledPins[i], (i == ledIndex) ? HIGH : LOW);
      }
    }
  }

  delay(1000);
}

// Function to convert angle to LED index (0 to 7)
int angleToLED(int angle) {
  int segment = angle / 45; // Divide angle by 45 to get segment index (0 to 7)
  return segment % 8; // Ensure index wraps around at 360 degrees
}

// Function to estimate angle based on RSSI value
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
