#include <Firebase_ESP_Client.h>
#include <WiFi.h>

// Replace these with your WiFi network settings
const char* ssid = "MDX welcomes you";
const char* password = "MdxL0vesyou";


// Firebase Data object for sending and receiving data
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Firebase setup details
const char* FIREBASE_HOST = "book-wand-default-rtdb.europe-west1.firebasedatabase.app";
const char* API_KEY = "AIzaSyD38zvdXz-YdVceZyk5PFNl-_ibYSN_Omk";
const char* USER_EMAIL = "esp@email.com";
const char* USER_PASSWORD = "123456";

const int outputPin = 4;

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  connectWifi();

  pinMode(outputPin, OUTPUT);
  digitalWrite(outputPin, LOW); // Set GPIO low

  // Setup Firebase configuration
  config.host = FIREBASE_HOST;
  config.api_key = API_KEY;

  // Auth details for accessing Firebase
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
 
  // Read test/int data from Firebase
  if (Firebase.RTDB.getDouble(&firebaseData, "/test/int")) {
    if (firebaseData.intData() == 1) {
     Serial.println("1 detected");
      digitalWrite(outputPin, HIGH); // Set GPIO high
      delay(3000); // Wait for one second
      digitalWrite(outputPin, LOW); // Set GPIO low
    }

  } else {
    Serial.println("Failed to read data: " + firebaseData.errorReason());
  }

  // Delay before next loop iteration
  delay(10000);
}
