package handlers

import (
	"log"
	"net/http"
	"time"

	"cognisafe.com/b/db/sqlc"
	"cognisafe.com/b/token"
	mqtt "github.com/eclipse/paho.mqtt.golang"
	"github.com/google/uuid"
)

type Handler struct {
	l *log.Logger
	q *sqlc.Queries
	c *uint
	u *AuthedUser
}

type AuthHandler struct {
	h *Handler
	t token.Maker
}

type AccountHandler struct {
	h *Handler
}

type LabHandler struct {
	h *Handler
}

type MQTTHandler struct {
	h          *Handler
	mqttClient mqtt.Client
}

type HandlerParam struct {
	w           http.ResponseWriter
	r           *http.Request
	method      string
	handlerFunc func(http.ResponseWriter, *http.Request) error
}

type AuthedUser struct {
	LabID   string `json:"lab_id"`
	Labname string `json:"labname"`
}

type LoginUserResponse struct {
	SessionID      uuid.UUID `json:"session_id"`
	AccessToken    string    `json:"access_token"`
	AccessTokenEx  time.Time `json:"access_token_expire"`
	RefreshToken   string    `json:"refresh_token"`
	RefreshTokenEx time.Time `json:"refresh_token_expire"`
	LabID          string    `json:"lab_id"`
	Labname        string    `json:"labname"`
}

type renewAccessTokenResponse struct {
	AccessToken          string    `json:"access_token"`
	AccessTokenExpiresAt time.Time `json:"access_token_expire"`
}
