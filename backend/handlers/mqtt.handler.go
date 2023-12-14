package handlers

import (
	"context"
	"errors"
	"fmt"
	"log"
	"net/http"

	"cognisafe.com/b/db/sqlc"
	mqtt "github.com/eclipse/paho.mqtt.golang"
)

var q_g *sqlc.Queries

func NewMQTTHandler(l *log.Logger, q *sqlc.Queries, u *AuthedUser, mqttC mqtt.Client) *MQTTHandler {
	var c uint = 0
	q_g = q
	return &MQTTHandler{&Handler{l, q, &c, u}, mqttC}
}

func (mqqt_h *MQTTHandler) GetFallGasDataH(w http.ResponseWriter, r *http.Request) {
	hp := HandlerParam{w, r, http.MethodGet, mqqt_h.getFallGasData}
	mqqt_h.h.handleRequest(hp, nil)
}

func (mqqt_h *MQTTHandler) getFallGasData(w http.ResponseWriter, r *http.Request) error {
	data, err := mqqt_h.h.q.GetFallGasData(r.Context())
	if err != nil {
		http.Error(w, "Cannot get data", http.StatusInternalServerError)
		return errors.New("cannot get data")
	}

	toJSON(w, data)
	return nil
}

func (mqqt_h *MQTTHandler) SubscribeHandler(w http.ResponseWriter, r *http.Request) {
	hp := HandlerParam{w, r, http.MethodGet, mqqt_h.subscribe}
	mqqt_h.h.handleRequest(hp, nil)
}

func (mqqt_h *MQTTHandler) subscribe(w http.ResponseWriter, r *http.Request) error {
	attendances, err := mqqt_h.h.q.GetValidAttendance(r.Context())
	if err != nil {
		http.Error(w, "Cannot get any attendance", http.StatusInternalServerError)
		return errors.New("cannot get any attendance")
	}

	for _, topic := range attendances {
		topicFall := topic.IpAddress + "/" + topic.MacAddress + "/Fall"
		if token := mqqt_h.mqttClient.Subscribe(topicFall, 0, MQTTMessageHandlerFall); token.Wait() && token.Error() != nil {
			mqqt_h.h.l.Println(token.Error())
			return errors.New("cannot Subscribe")
		}
		mqqt_h.h.l.Println("Success subscribed to topic", topicFall)

		topicGas := topic.IpAddress + "/" + topic.MacAddress + "/Gas"
		if token2 := mqqt_h.mqttClient.Subscribe(topicGas, 0, MQTTMessageHandlerGas); token2.Wait() && token2.Error() != nil {
			mqqt_h.h.l.Println(token2.Error())
			return errors.New("cannot Subscribe")
		}
		mqqt_h.h.l.Println("Success subscribed to topic", topicGas)
	}

	toJSON(w, "Success subscribe")
	return nil
}

var fallBefore string
var gasBefore string

func MQTTMessageHandlerFall(client mqtt.Client, msg mqtt.Message) {
	payload := string(msg.Payload())
	if payload == "1" && payload != fallBefore {
		q_g.UpdateFallTrue(context.Background())
		fallBefore = "1"
		fmt.Println("Fall detected")
	} else if payload == "0" && payload != fallBefore {
		fallBefore = "0"
		q_g.UpdateFallFalse(context.Background())
	}
}

func MQTTMessageHandlerGas(client mqtt.Client, msg mqtt.Message) {
	payload := string(msg.Payload())
	if payload == "1" && payload != gasBefore {
		q_g.UpdateGasTrue(context.Background())
		gasBefore = "1"
		fmt.Println("Gas Leak detected")
	} else if payload == "0" && payload != gasBefore {
		gasBefore = "0"
		q_g.UpdateGasFalse(context.Background())
	}
}
