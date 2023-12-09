// #include <WiFi.h>
// #include <PubSubClient.h>

// const char* ssid = "CogniSafe";
// const char* password = "12345678";
// const char* mqtt_server = "192.168.137.1";
// const int mqtt_port = 1883; // Port for MQTT over WebSockets

// WiFiClient espClient;
// PubSubClient client(espClient);

// const char* topic = "greeting";

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
//   setup_wifi();
//   client.setServer(mqtt_server, mqtt_port);
// }

// int counter = 0;

// void loop() {
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();

//   // Publish the counter value to the topic
//   char message[50];
//   sprintf(message, "Count: %d", counter);
//   client.publish(topic, message);

//   delay(1000); // Publish every second
//   counter+=1;
// }