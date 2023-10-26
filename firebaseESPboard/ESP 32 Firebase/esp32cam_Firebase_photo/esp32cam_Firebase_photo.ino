/*********
  Rui Santos
  Complete instructions at: https://RandomNerdTutorials.com/esp32-cam-save-picture-firebase-storage/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  Based on the example provided by the ESP Firebase Client Library
*********/



#include "WiFi.h"
#include "esp_camera.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <SPIFFS.h>
#include <FS.h>
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include <addons/TokenHelper.h>

//Replace with your network credentials
const char* ssid = "Max 20A Unfused";
const char* password = "bonaparte";

// Insert Firebase project API Key
// API Idrus
#define API_KEY "AIzaSyCD_FJF0WqKDVrmAkxiY410ActHjTwpBAI"
// API Danen
//#define API_KEY "AIzaSyANfnHFl8Up3hPDnwjSx0qBuvmHLqujKyE"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "mohamadidrus240@gmail.com"
#define USER_PASSWORD "firebaseidrus24"
//#define USER_EMAIL "danendradhiaulichsan@gmail.com"
//#define USER_PASSWORD "123456"

// Insert Firebase storage bucket ID e.g bucket-name.appspot.com
#define STORAGE_BUCKET_ID "iot-project-1c879.appspot.com"
//#define STORAGE_BUCKET_ID "tugas-akhir-59391.appspot.com"

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "/data/photo.jpeg"
String path = "/Data_Kebisingan/";
String photoFile = "test";
String photoExtention = ".jpeg";
int count = 1;
unsigned long myTimes = 0;

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

boolean takeNewPhoto = true;

//Define Firebase Data objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig configF;

bool taskCompleted = false;

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);
  initWiFi();
  initSPIFFS();
  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  initCamera();

  //Firebase
  // Assign the api key
  configF.api_key = API_KEY;
  //Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  //Assign the callback function for the long running token generation task
  configF.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&configF, &auth);
  Firebase.reconnectWiFi(true);
  myTimes = millis();
}

void loop() {
  if (millis() - myTimes >= 10000UL){
    myTimes = millis();
    takeNewPhoto = true;
    taskCompleted = false;
  }
  if (takeNewPhoto) {
    Serial.println("capturePhotoSaveSpiffs Function");
    capturePhotoSaveSpiffs();
    takeNewPhoto = false;
  }
  delay(1);
  if (Firebase.ready() && !taskCompleted) {
    taskCompleted = true;
    Serial.print("Uploading picture to : ");
    Serial.println(STORAGE_BUCKET_ID);
    Serial.print("With path : ");
    Serial.println(path);

    //MIME type should be valid to avoid the download problem.
    //The file systems for flash and SD/SDMMC can be changed in FirebaseFS.h.
    //    if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, FILE_PHOTO /* path to local file */, mem_storage_type_flash /* memory storage type, mem_storage_type_flash and mem_storage_type_sd */, FILE_PHOTO /* path of remote file stored in the bucket */, "image/jpeg" /* mime type */)) {
    //      Serial.printf("\nDownload URL: %s\n", fbdo.downloadURL().c_str());
    //    }
    //    if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID, FILE_PHOTO, mem_storage_type_flash, FILE_PHOTO, "image/jpeg")) {
    //      Serial.printf("\nDownload URL: %s\n", fbdo.downloadURL().c_str());
    //    }
    count++;
    photoFile = "test";
    photoFile += String(count);
    String sendToBucket = path;
    sendToBucket += photoFile;
    sendToBucket += photoExtention;
    Serial.print("Image name : ");
    Serial.println(sendToBucket);
    if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID, FILE_PHOTO, mem_storage_type_flash, sendToBucket, "image/jpeg")) {
      Serial.printf("\nDownload URL: %s\n", fbdo.downloadURL().c_str());
    }
    else {
      Serial.println(fbdo.errorReason());
    }
  }
  //Serial.println("Loop Here");
}
