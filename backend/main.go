package main

import (
	"fmt"
	"log"
	"net/http"
	"os"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

var client mqtt.Client

func PublishHandler(client mqtt.Client) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		topic := r.URL.Query().Get("topic")
		message := r.URL.Query().Get("message")

		if token := client.Publish(topic, 0, false, message); token.Wait() && token.Error() != nil {
			http.Error(w, fmt.Sprintf("Error publishing message: %v", token.Error()), http.StatusInternalServerError)
			return
		}

		fmt.Fprintf(w, "Published message '%s' to topic '%s'", message, topic)
	}
}

func SubscribeHandler(client mqtt.Client) http.HandlerFunc {
	return func(w http.ResponseWriter, r *http.Request) {
		topic := r.URL.Query().Get("topic")

		if token := client.Subscribe(topic, 0, nil); token.Wait() && token.Error() != nil {
			http.Error(w, fmt.Sprintf("Error subscribing to topic: %v", token.Error()), http.StatusInternalServerError)
			return
		}

		fmt.Fprintf(w, "Subscribed to topic '%s'", topic)
	}
}

func MQTTMessageHandlerFall(client mqtt.Client, msg mqtt.Message) {
	fmt.Println(msg.Topic(), string(msg.Payload()))
}

func MQTTMessageHandlerGas(client mqtt.Client, msg mqtt.Message) {
	fmt.Println(msg.Topic(), string(msg.Payload()))
}

func main() {
	opts := mqtt.NewClientOptions()
	opts.AddBroker("tcp://172.173.157.174:1883") // MQTT broker URL
	opts.SetClientID("cognisafe")

	client = mqtt.NewClient(opts)
	if token := client.Connect(); token.Wait() && token.Error() != nil {
		log.Fatalf("Error connecting to MQTT broker: %v", token.Error())
	}

	// Subscribe to a specific topic to listen to MQTT messages
	client.Subscribe("Fall", 0, MQTTMessageHandlerFall)
	client.Subscribe("Gas", 0, MQTTMessageHandlerGas)

	http.HandleFunc("/publish", PublishHandler(client))
	http.HandleFunc("/subscribe", SubscribeHandler(client))

	port := os.Getenv("PORT")
	if port == "" {
		port = "8080"
	}

	log.Printf("Starting server on port %s", port)
	log.Fatal(http.ListenAndServe(":"+port, nil))
}
