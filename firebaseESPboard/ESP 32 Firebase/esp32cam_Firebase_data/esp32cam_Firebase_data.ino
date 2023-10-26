#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include "time.h"

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Max 20A Unfused"
#define WIFI_PASSWORD "bonaparte"

// Insert Firebase project API Key
#define API_KEY "AIzaSyANfnHFl8Up3hPDnwjSx0qBuvmHLqujKyE"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "danendradhiaulichsan@gmail.com"
#define USER_PASSWORD "123456"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://tugas-akhir-59391-default-rtdb.asia-southeast1.firebasedatabase.app"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Database main path (to be updated in setup with the user UID)
String databasePath;
// Database child nodes
String fotoPath = "/Foto";
String jenisPath = "/Jenis_Kendaraan";
String kebisingan1Path = "/Kebisingan1";
String kebisingan2Path = "/Kebisingan2";
String tanggalPath = "/Tanggal";
String waktuPath = "/Waktu";
String timePath = "/timestamp";

String foto;
String jenis = "Roda 2";
int kebisingan1, kebisingan2;

// Parent Node (to be updated in every loop)
String parentPath;

int timestamp;
FirebaseJson json;

const char* ntpServer = "pool.ntp.org";

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 10000;

// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Function that gets current epoch time
unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void setup() {
  Serial.begin(115200);

  // Initialize BME280 sensor
  initWiFi();
  configTime(0, 0, ntpServer);

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  // databasePath = "/UsersData/" + uid + "/readings";
  databasePath = "/data_IoT/Data_IoT_Kebisingan";
}

void loop() {

  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    //Get current timestamp
    timestamp = getTime();
    Serial.print ("time: ");
    Serial.println (timestamp);

    parentPath = databasePath + "/" + String(timestamp);

    json.set(fotoPath.c_str(), String(timestamp) + ".jpeg");
    json.set(jenisPath.c_str(), jenis);
    json.set(kebisingan1Path.c_str(), String(13));
    json.set(kebisingan2Path.c_str(), String(14));
    json.set(tanggalPath.c_str(), String(timestamp));
    json.set(waktuPath.c_str(), String(timestamp));
    //    json.set(timePath, String(timestamp));
    Serial.printf("Set json... %s\n", Firebase.RTDB.setJSON(&fbdo, parentPath.c_str(), &json) ? "ok" : fbdo.errorReason().c_str());
  }
}
