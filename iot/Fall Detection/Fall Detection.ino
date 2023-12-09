#define ESP_DRD_USE_SPIFFS true

#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFiManager.h>
#include <ESP_DoubleResetDetector.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <MPU6050.h>
#define BUZZER_PIN 14
#define BUZZER_CHANNEL 0
#define BEEP_DURATION 5000 // 1 second (in milliseconds)
#include <HTTPClient.h>
#include "time.h"

const int PIN_LED = 2;
#define JSON_CONFIG_FILE "/sample_config.json"

#define DRD_TIMEOUT 10

#define DRD_ADDRESS 0
DoubleResetDetector *drd;

//flag for saving data
bool shouldSaveConfig = false;

char testString[50] = "deafult value";
unsigned long long testNumber = 12345678123ULL;
char testNumberStr[20]; // Allocate a char array to hold the converted number as a string
int apikey = 1234567;

unsigned long button_time = 0;  
unsigned long last_button_time = 0;
const float fallThreshold = 650000; // Adjust this value to suit your needs (in m/s^3)
const int sampleInterval = 10;   // Interval in milliseconds between readings
float prevAccX = 0.0, prevAccY = 0.0, prevAccZ = 0.0;
String serverName = "https://api.callmebot.com/whatsapp.php?";
const char* ntpServer = "in.pool.ntp.org";
const long  gmtOffset_sec = 106200;
const int   daylightOffset_sec = 0;
char dateTimeStr[30];
bool buttonInterrupted = false;
unsigned long delayStartTime = 0;
const int DOUBLE_PRESS_THRESHOLD = 1500; // Time threshold for double press in milliseconds
bool lastFallDetection = false;
unsigned long lastFallTime = 0;
IPAddress staticIP(192, 168, 1, 100); // Replace with the desired static IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8); // Replace with your DNS server IP address


struct Button {
    const uint8_t PIN;
    uint32_t numberKeyPresses;
    bool pressed;
};

Button button1 = {12, 0, false};

MPU6050 mpu;

String urlEncode(const char* str) {
  const char* hex = "0123456789ABCDEF";
  String encodedStr = "";

  while (*str != 0) {
    if (('a' <= *str && *str <= 'z')
        || ('A' <= *str && *str <= 'Z')
        || ('0' <= *str && *str <= '9')) {
      encodedStr += *str;
    } else {
      encodedStr += '%';
      encodedStr += hex[*str >> 4];
      encodedStr += hex[*str & 0xf];
    }
    str++;
  }

  return encodedStr;
}





void saveConfigFile()
{
  Serial.println(F("Saving config"));
  StaticJsonDocument<512> json;
  json["testString"] = testString;
  json["testNumber"] = testNumber;
  json["apikey"] = apikey;

  File configFile = SPIFFS.open(JSON_CONFIG_FILE, "w");
  if (!configFile)
  {
    Serial.println("failed to open config file for writing");
  }

  serializeJsonPretty(json, Serial);
  if (serializeJson(json, configFile) == 0)
  {
    Serial.println(F("Failed to write to file"));
  }
  configFile.close();
}


//callback notifying us of the need to save config
void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

// This gets called when the config mode is launced, might
// be useful to update a display with this info.
void configModeCallback(WiFiManager *myWiFiManager)
{
  Serial.println("Entered Conf Mode");

  Serial.print("Config SSID: ");
  Serial.println(myWiFiManager->getConfigPortalSSID());

  Serial.print("Config IP Address: ");
  Serial.println(WiFi.softAPIP());
}

void setup()
{

  Wire.begin(2, 15); // SDA to GPIO 22, SCL to GPIO 21

  // Initialize MPU6050
  mpu.initialize();

  // Verify the connection
  Serial.println(mpu.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  pinMode(button1.PIN, INPUT_PULLUP);
  attachInterrupt(button1.PIN, isr, FALLING);
  ledcSetup(BUZZER_CHANNEL, 2000, 16); // 2000 Hz, 8-bit resolution
  ledcAttachPin(BUZZER_PIN, BUZZER_CHANNEL);
  pinMode(PIN_LED, OUTPUT);

  bool forceConfig = false;


  {
    if (!wm.startConfigPortal("Fall_detector", "clock123"))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      //reset and try again, or maybe put it to deep sleep
      ESP.restart();
      delay(5000);
    }
  }
  else
  {
    if (!wm.autoConnect("Fall_detector", "clock123"))
    {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      // if we still have not connected restart and try all over again
      ESP.restart();
      delay(5000);
    }
  }

  // If we get here, we are connected to the WiFi
  WiFi.config(staticIP, gateway, subnet, dns);


}

// ... Rest of the code ...

void loop()
{
  drd->loop();
  printLocalTime();

  char encodedDateTimeStr[50];
  String encodedDateTime = urlEncode(dateTimeStr);

  char testNumberStr[20]; // Allocate a char array to hold the converted number as a string
  sprintf(testNumberStr, "%llu", testNumber); // Convert the testNumber to a string

  String serverPath = serverName + "phone=" + String(testNumberStr) + "&apikey=" + String(apikey) + "&text=A+Fall+has+been+detected+for+user:+" + testString + "+on+" + encodedDateTime;

  http.begin(serverPath.c_str());
  static unsigned long lastTime = 0;

  // Read accelerometer data
  if (millis() - lastTime >= sampleInterval)
  {
    lastTime = millis();

    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    // Convert accelerometer readings from m/s^2 to mg (1 g = 9.8 m/s^2)
    float acceleration_mg_x = ax / 9.8;
    float acceleration_mg_y = ay / 9.8;
    float acceleration_mg_z = az / 9.8;

    // Calculate jerk by taking the derivative of acceleration with respect to time
    float jerkX = (acceleration_mg_x - prevAccX) / (sampleInterval / 1000.0);
    float jerkY = (acceleration_mg_y - prevAccY) / (sampleInterval / 1000.0);
    float jerkZ = (acceleration_mg_z - prevAccZ) / (sampleInterval / 1000.0);

    // Update previous acceleration values for the next iteration
    prevAccX = acceleration_mg_x;
    prevAccY = acceleration_mg_y;
    prevAccZ = acceleration_mg_z;

    // Calculate the magnitude of jerk
    float jerkMagnitude = sqrt(jerkX * jerkX + jerkY * jerkY + jerkZ * jerkZ);
    //Serial.println(jerkMagnitude);
    // Check for a fall
    if (jerkMagnitude > fallThreshold)
    {
      // Fall detected
      Serial.println("Fall detected!");
      ledcWriteTone(BUZZER_CHANNEL, 5000); // Play a 1kHz tone on the buzzer pin

      // Reset the buttonInterrupted flag
      buttonInterrupted = false;

      delayStartTime = millis();
      while (delayStartTime > 0 && millis() - delayStartTime < BEEP_DURATION)
      {
        if (button1.pressed)
        {
          button1.pressed = false;
          ledcWrite(BUZZER_CHANNEL, 0); // Stop the tone
          Serial.println("Buzzer stopped by button press.");
          break;
        }
      }

  }
  // Check for double press
  }
}
