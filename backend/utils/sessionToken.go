package utils

import (
	"encoding/base64"
	"encoding/json"
	"errors"
	"io/ioutil"
	"os"
	"path/filepath"
	"time"
)

const (
	tokenFile = "token.json"
	tokenDir  = ".labSessionToken"
)

type LabSessTokenParams struct {
	LabSessionID string    `json:"lab_session_id"`
	StartTime    time.Time `json:"start_time"`
	EndTime      time.Time `json:"end_time"`
}

func EncodeLabSessTokenParams(params LabSessTokenParams) (string, error) {
	// Convert the payload to JSON
	jsonPayload, err := json.Marshal(params)
	if err != nil {
		return "", err
	}

	// Encode the JSON payload using Base64
	encodedPayload := base64.StdEncoding.EncodeToString(jsonPayload)
	return encodedPayload, nil
}

func DecodeLabSessTokenParams(encoded string) (LabSessTokenParams, error) {
	// Decode the Base64-encoded string
	decodedBytes, err := base64.StdEncoding.DecodeString(encoded)
	if err != nil {
		return LabSessTokenParams{}, err
	}

	// Unmarshal the JSON payload
	var params LabSessTokenParams
	err = json.Unmarshal(decodedBytes, &params)
	if err != nil {
		return LabSessTokenParams{}, err
	}

	return params, nil
}

func getTokenFilePath() string {
	homeDir, _ := os.UserHomeDir()
	return filepath.Join(homeDir, tokenDir, tokenFile)
}

func ReadTokenFromFile() (string, error) {
	tokenPath := getTokenFilePath()
	token, err := ioutil.ReadFile(tokenPath)
	if err != nil {
		return "", errors.New("no token found")
	}

	return string(token), nil
}

func SaveTokenToFile(token string) error {
	tokenPath := getTokenFilePath()

	err := os.MkdirAll(filepath.Dir(tokenPath), 0700)
	if err != nil {
		return err
	}

	return ioutil.WriteFile(tokenPath, []byte(token), 0600)
}
