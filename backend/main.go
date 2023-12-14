package main

import (
	"context"
	"log"
	"net/http"
	"os"
	"os/signal"
	"syscall"
	"time"

	"cognisafe.com/b/db"
	"cognisafe.com/b/db/sqlc"
	"cognisafe.com/b/handlers"
	"cognisafe.com/b/token"
	"github.com/joho/godotenv"
	"github.com/rs/cors"

	mqtt "github.com/eclipse/paho.mqtt.golang"
)

var l = log.New(os.Stdout, "COGNISAFE-SERVER-", log.LstdFlags)
var client mqtt.Client

func main() {
	ctx := context.Background()

	env := os.Getenv("APP_ENV")
	if env == "" {
		env = "remote"
	}
	// load env
	err := godotenv.Load(".env." + env)
	if err != nil {
		l.Fatalf("Error reding the .env %s", err)
	}

	// CRUD
	db, queries := db.Instantiate(l)
	if db == nil || queries == nil {
		l.Println("Exiting due to database connection error")
		return
	}
	defer db.Close()

	opts := mqtt.NewClientOptions()
	opts.AddBroker("tcp://172.173.157.174:1883") // MQTT broker URL
	opts.SetClientID("cognisafe")

	client = mqtt.NewClient(opts)

	server := &http.Server{
		Addr:        "0.0.0.0:" + os.Getenv("PORT"),
		Handler:     defineMultiplexer(l, queries),
		IdleTimeout: 30 * time.Second,
		ReadTimeout: time.Second,
	}

	// now the startServer is run by a routine
	go subscribeTopic(queries, &ctx)
	go startServer(server, l)

	// inorder to block the routine, we might use a channel (we can use wait group also)
	shut := make(chan os.Signal, 1)
	signal.Notify(shut, syscall.SIGINT, syscall.SIGTERM)

	<-shut // Block until a signal is received

	timeout_ctx, cancel := context.WithTimeout(ctx, 30*time.Second)
	defer cancel()

	stopServer(server, l, &timeout_ctx, &cancel)
}

func subscribeTopic(q *sqlc.Queries, ctx *context.Context) {
	if token := client.Connect(); token.Wait() && token.Error() != nil {
		l.Fatalf("Error connecting to MQTT broker: %v", token.Error())
	}

	l.Println("🦟 Connected to MQQT Broker")
}

func MQTTMessageHandlerFall(client mqtt.Client, msg mqtt.Message) {
	if string(msg.Payload()) == "1" {
		l.Println("Fall detected")
	}
}

func MQTTMessageHandlerGas(client mqtt.Client, msg mqtt.Message) {
	if string(msg.Payload()) == "1" {
		l.Println("Gas detected")
	}
}

func startServer(s *http.Server, l *log.Logger) {
	l.Println("🔥 Server is running on", s.Addr)

	err := s.ListenAndServe()
	if err != nil && err != http.ErrServerClosed {
		l.Fatalln("Server is failed due to", err)
	}
}

func stopServer(s *http.Server, l *log.Logger, ctx *context.Context, cancel *context.CancelFunc) {
	l.Println("💅 Shutting down the server")
	s.Shutdown(*ctx)
	c := *cancel
	c()
}

func defineMultiplexer(l *log.Logger, q *sqlc.Queries) http.Handler {
	var u handlers.AuthedUser

	token, err := token.NewPasetoMaker(os.Getenv("PASETO_KEY"))
	if err != nil {
		log.Fatal("Failed creating Paseto token")
	}
	auth_handler := handlers.NewAuthHandler(l, q, &u, &token)
	token_handler := handlers.NewTokenHandler(l, q, &u, &token)
	lab_handler := handlers.NewLabHandler(l, q, &u, &token)
	mqtt_handler := handlers.NewMQTTHandler(l, q, &u, client)

	// handle multiplexer
	mux := http.NewServeMux()

	mux.HandleFunc("/auth/login", auth_handler.Login)
	mux.HandleFunc("/auth/signup", auth_handler.Signup)
	mux.HandleFunc("/auth/renewToken", token_handler.RenewToken)

	// register the lab session
	// this will generate a valid time token (10-12)
	// if this decoded it will have a payload of session id
	// It will be stored inside local pc
	mux.HandleFunc("/lab/startSession", lab_handler.CreateLabSessionH)
	mux.HandleFunc("/lab/attendance", lab_handler.AttendenceSessionH)
	mux.HandleFunc("/lab/getAttendances", lab_handler.GetPerson)

	mux.HandleFunc("/mqtt/subscribe", mqtt_handler.SubscribeHandler)
	mux.HandleFunc("/mqtt/getData", mqtt_handler.GetFallGasDataH)

	// it will return a token based on a certain criteria
	// the user is in the same network/ip
	// decode the token inside cookie
	// it will have a payload of
	// the user requested in the range of lab session

	// mux.HandleFunc("/lab/report", token_handler.RenewToken)
	// // log event
	// mux.HandleFunc("/lab/log", nil)

	corsMiddleware := cors.AllowAll().Handler

	handler := corsMiddleware(mux)

	return handler
}
