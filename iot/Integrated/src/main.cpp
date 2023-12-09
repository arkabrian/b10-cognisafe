#include <Wire.h>
#include <MPU6050.h>
#include <HTTPClient.h>
#include "time.h"
#include <WiFi.h>
#include <PubSubClient.h>

MPU6050 mpu;

#define BUZZER_PIN 14
#define GAS_PIN 19
#define BUZZER_CHANNEL 0
#define BEEP_DURATION 5000 // 1 second (in milliseconds)

const int PIN_LED = 2;
int tempGas;

const int FALL_THRESHOLD = 650000; // Adjust this value to suit your needs (in m/s^3)
const int SAMPLE_INTERVAL = 10;    // Interval in milliseconds between readings

float prevAccX = 0.0, prevAccY = 0.0, prevAccZ = 0.0;

// MQTT Configuration
const char* ssid = "CogniSafe";
const char* password = "12345678";
const char* mqtt_server = "192.168.137.1";
const int mqtt_port = 1883; // Port for MQTT over WebSockets

WiFiClient espClient;
PubSubClient client(espClient);

const char* topic = "greeting";

// Initialize MPU6050
void initMPU6050() {
  Wire.begin(22, 23); // SDA to GPIO 22, SCL to GPIO 23
  mpu.initialize();
}

// Function to detect a fall
bool detectFall() {
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
    return true; // Fall detected
  }

  return false; // No fall detected
}

int readGasSensor() {
  int sensorValue = analogRead(19); // Assuming the MQ2 sensor is connected to analog pin A0
  return sensorValue;
}

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.print("\n");
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP32Client1")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(topic);
      client.publish(topic, "Hello CogniSafe");
    } else {
      Serial.print("MQTT connection failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  initMPU6050();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
}

int counter = 0;

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Detect fall and gas
  bool isFallDetected = detectFall();
  int gasSensorValue = readGasSensor();

  // Publish the status of fall detection and gas sensor value to the topic
  char message[50];
  sprintf(message, "Fall: %s, Gas: %d", isFallDetected ? "Detected" : "Not Detected", gasSensorValue);
  client.publish(topic, message);

  // Print information to Serial monitor
  Serial.print("Fall: ");
  Serial.print(isFallDetected ? "Detected" : "Not Detected");
  Serial.print(", Gas: ");
  Serial.println(gasSensorValue);

  delay(1000); // Publish every second
  counter += 1;
}
