#include "DHT.h"
// Fill-in information from your Blynk Template here
// #define BLYNK_TEMPLATE_ID           "IdrusTest24"
// #define BLYNK_DEVICE_NAME           "Device
#define BLYNK_TEMPLATE_ID "TMPLg-AigSze"
#define BLYNK_DEVICE_NAME "IoT Device"

#define BLYNK_FIRMWARE_VERSION "0.1.0"

#define BLYNK_PRINT Serial
// #define BLYNK_DEBUG

#define APP_DEBUG

// Uncomment your board, or configure a custom board in Settings.h
// #define USE_SPARKFUN_BLYNK_BOARD
#define USE_NODE_MCU_BOARD
// #define USE_WITTY_CLOUD_BOARD
// #define USE_WEMOS_D1_MINI

#include "BlynkEdgent.h"
#define DHTPIN D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

BLYNK_WRITE(V0) // this command is listening when something is written to V1
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable

  if (pinValue == 1)
  {
    // do something when button is pressed;
  }
  else if (pinValue == 0)
  {
    // do something when button is released;
  }

  Serial.print("V1 button value is: "); // printing value to serial monitor
  Serial.println(pinValue);
}

void setup()
{
  Serial.begin(115200);
  delay(100);
  dht.begin();
  BlynkEdgent.begin();
}

void loop()
{
  BlynkEdgent.run();
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  Blynk.virtualWrite(V5, temp);
  Blynk.virtualWrite(V6, hum);
}
