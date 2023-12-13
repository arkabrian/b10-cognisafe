// #include <Wire.h>
// #include <MPU6050.h>
// #include <HTTPClient.h>
// #include "time.h"
// #include <WiFi.h>
// #include <PubSubClient.h>

// MPU6050 mpu;

// #define BUZZER_PIN 14
// #define GAS_PIN 19
// #define BUZZER_CHANNEL 0
// #define BEEP_DURATION 5000 // 1 second (in milliseconds)

// const int PIN_LED = 2;
// int tempGas;

// const int FALL_THRESHOLD = 650000; // Adjust this value to suit your needs (in m/s^3)
// const int SAMPLE_INTERVAL = 10;    // Interval in milliseconds between readings

// float prevAccX = 0.0, prevAccY = 0.0, prevAccZ = 0.0;

// // MQTT Configuration
// const char* ssid = "CogniSafe";
// const char* password = "12345678";
// const char* mqtt_server = "172.173.157.174";
// const int mqtt_port = 1883; // Port for MQTT over WebSockets

// WiFiClient espClient;
// PubSubClient client(espClient);

// const char* topic = "greeting";

// // Initialize MPU6050
// void initMPU6050() {
//   Wire.begin(22, 23); // SDA to GPIO 22, SCL to GPIO 23
//   mpu.initialize();
// }

// // Function to detect a fall
// bool detectFall() {
//   int16_t ax, ay, az;
//   mpu.getAcceleration(&ax, &ay, &az);

//   // Convert accelerometer readings from m/s^2 to mg (1 g = 9.8 m/s^2)
//   float acceleration_mg_x = ax / 9.8;
//   float acceleration_mg_y = ay / 9.8;
//   float acceleration_mg_z = az / 9.8;

//   // Print acceleration values
//   Serial.println("Acceleration (mg):");
//   Serial.print("X: ");
//   Serial.println(acceleration_mg_x);
//   Serial.print("Y: ");
//   Serial.println(acceleration_mg_y);
//   Serial.print("Z: ");
//   Serial.println(acceleration_mg_z);

//   // Calculate jerk by taking the derivative of acceleration with respect to time
//   float jerkX = (acceleration_mg_x - prevAccX) / (SAMPLE_INTERVAL / 1000.0);
//   float jerkY = (acceleration_mg_y - prevAccY) / (SAMPLE_INTERVAL / 1000.0);
//   float jerkZ = (acceleration_mg_z - prevAccZ) / (SAMPLE_INTERVAL / 1000.0);

//   // Update previous acceleration values for the next iteration
//   prevAccX = acceleration_mg_x;
//   prevAccY = acceleration_mg_y;
//   prevAccZ = acceleration_mg_z;

//   // Calculate the magnitude of jerk
//   float jerkMagnitude = sqrt(jerkX * jerkX + jerkY * jerkY + jerkZ * jerkZ);
//   Serial.println(jerkMagnitude);

//   // Check for a fall
//   if (jerkMagnitude > FALL_THRESHOLD) {
//     return true; // Fall detected
//   }

//   return false; // No fall detected
// }

// int readGasSensor() {
//   int sensorValue = analogRead(19); // Assuming the MQ2 sensor is connected to analog pin A0
//   return sensorValue;
// }

// void setup_wifi() {
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.println("Connecting to WiFi...");
//   }
//   Serial.print("\n");
// }

// void reconnect() {
//   while (!client.connected()) {
//     Serial.println("Attempting MQTT connection...");
//     if (client.connect("ESP32Client1")) {
//       Serial.println("Connected to MQTT broker");
//       client.subscribe(topic);
//       client.publish(topic, "Hello CogniSafe");
//     } else {
//       Serial.print("MQTT connection failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" Retrying in 5 seconds...");
//       delay(5000);
//     }
//   }
// }

// void setup() {
//   Serial.begin(115200);
//   initMPU6050();
//   pinMode(BUZZER_PIN, OUTPUT);
//   pinMode(PIN_LED, OUTPUT);
//   // setup_wifi();
//   // client.setServer(mqtt_server, mqtt_port);
// }

// int counter = 0;

// void loop() {
//   // if (!client.connected()) {
//   //   reconnect();
//   // }
//   // client.loop();

//   // Detect fall and gas
//   bool isFallDetected = detectFall();
//   int gasSensorValue = readGasSensor();

//   // Publish the status of fall detection and gas sensor value to the topic
//   char message[50];
//   sprintf(message, "Fall: %s, Gas: %d", isFallDetected ? "Detected" : "Not Detected", gasSensorValue);
//   client.publish(topic, message);

//   // Print information to Serial monitor
//   Serial.print("Fall: ");
//   Serial.print(isFallDetected ? "Detected" : "Not Detected");
//   Serial.print(", Gas: ");
//   Serial.println(gasSensorValue);

//   delay(1000); // Publish every second
//   counter += 1;
// }


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Wire.h>
#include <MPU6050.h>
#include <HTTPClient.h>
#include "time.h"
#include <WiFi.h>
#include <PubSubClient.h>

#define AO_PIN 14  // ESP32's pin GPIO14 connected to AO pin of the MQ2 sensor
// #define BUZZER_PIN 4

MPU6050 mpu;

const int FALL_THRESHOLD = 650000; // Adjust this value to suit your needs (in m/s^3)
const int SAMPLE_INTERVAL = 10;    // Interval in milliseconds between readings

float prevAccX = 0.0, prevAccY = 0.0, prevAccZ = 0.0;
int gasValue;
bool isFallDetected;

// MQTT Configuration
const char* ssid = "CogniSafe";
const char* password = "12345678";
const char* mqtt_server = "172.173.157.174";
const int mqtt_port = 1883; // Port for MQTT over WebSockets
int counter = 0;            // message counter

WiFiClient espClient;
PubSubClient client(espClient);

const char* FALL_TOPIC = "Fall";
const char* GAS_TOPIC = "Gas";

void initMPU6050() {
  Wire.begin(22, 23); // SDA to GPIO 22, SCL to GPIO 23
  mpu.initialize();
}

// Function to detect a fall
void detectFall(void* parameter) {
  while(1) {
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);

    // Convert accelerometer readings from m/s^2 to mg (1 g = 9.8 m/s^2)
    float acceleration_mg_x = ax / 9.8;
    float acceleration_mg_y = ay / 9.8;
    float acceleration_mg_z = az / 9.8;

    // Print acceleration values
    Serial.println("Acceleration (mg):");
    Serial.print("X: ");
    Serial.println(acceleration_mg_x);
    Serial.print("Y: ");
    Serial.println(acceleration_mg_y);
    Serial.print("Z: ");
    Serial.println(acceleration_mg_z);

    // Calculate jerk by taking the derivative of acceleration with respect to time
    float jerkX = (acceleration_mg_x - prevAccX) / (SAMPLE_INTERVAL / 1000.0);
    float jerkY = (acceleration_mg_y - prevAccY) / (SAMPLE_INTERVAL / 1000.0);
    float jerkZ = (acceleration_mg_z - prevAccZ) / (SAMPLE_INTERVAL / 1000.0);

    // Update previous acceleration values for the next iteration
    prevAccX = acceleration_mg_x;
    prevAccY = acceleration_mg_y;
    prevAccZ = acceleration_mg_z;

    // Calculate the magnitude of jerk
    float jerkMagnitude = sqrt(jerkX * jerkX + jerkY * jerkY + jerkZ * jerkZ);
    Serial.println(jerkMagnitude);

    // Check for a fall
    if (jerkMagnitude > FALL_THRESHOLD) {
      isFallDetected = true; // Fall detected
    } else{
      isFallDetected = false; // No fall detected
    }
    Serial.print("Fall: ");
    Serial.println(isFallDetected ? "Detected" : "Not Detected");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void readMQ2Sensor(void *parameter) {
  while(1) {
    gasValue = analogRead(AO_PIN);
      if (gasValue > 1550){
        Serial.println("Gas: Detected!");
        Serial.print("MQ2 sensor AO value: ");
        Serial.println(gasValue);
      }
      else {
        Serial.println("Gas: Not Detected");
      }
 
    vTaskDelay(500 / portTICK_PERIOD_MS); // wait for the MQ2 to warm up
  }
}

// void buzzerTask(void* parameter) {
//   pinMode(BUZZER_PIN, OUTPUT);

//   while (true) {
//     if(isFallDetected != true || gasValue < 1550){
//       digitalWrite(BUZZER_PIN, LOW);
//       vTaskDelay(500 / portTICK_PERIOD_MS);
//       continue;
//     }

//     digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
//     vTaskDelay(2000 / portTICK_PERIOD_MS);
//   }
// }

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  Serial.print("\n");
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client1")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(FALL_TOPIC);
      client.subscribe(GAS_TOPIC);
      // client.publish(topic, "Connecte");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  // initialize serial communication
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  initMPU6050();

  Serial.println("=============================");
  Serial.println("===COGNISAFE SAFETY MODULE===");
  Serial.println("=============================");
  Serial.println("Warming up the MQ2 sensor");
  //delay(10000);  // wait for the MQ2 to warm up

  // Create a task to read analog values from the MQ2 sensor
  xTaskCreatePinnedToCore(
      readMQ2Sensor,   /* Task function. */
      "ReadGasStatus", /* Name of task. */
      10000,           /* Stack size of task */
      NULL,            /* parameter of the task */
      1,               /* priority of the task */
      NULL,            /* Task handle to keep track of created task */
      1);              /* Core (0 or 1) to run the task on */
  
  xTaskCreatePinnedToCore(
      detectFall,
      "FallDetection",
      10000,
      NULL,
      1,
      NULL,
      1);

  // xTaskCreatePinnedToCore(
  //     buzzerTask, 
  //     "Buzzer", 
  //     2048, 
  //     NULL, 
  //     1, 
  //     NULL, 
  //     1);

}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // Publish the status of fall detection and gas sensor value to the topic
  char FALL_MESSAGE[15];
  char GAS_MESSAGE[10];
  sprintf(FALL_MESSAGE, "%d", isFallDetected);
  sprintf(GAS_MESSAGE, "%d", gasValue);
  client.publish(GAS_TOPIC, GAS_MESSAGE);
  client.publish(FALL_TOPIC, FALL_MESSAGE);

  delay(500); // Publish every second
  counter++;
}