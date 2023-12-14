package utils

import (
	"database/sql"
	"fmt"
	"time"

	"golang.org/x/crypto/bcrypt"
)

func HashPassword(password string) (string, error) {
	hashedPassword, err := bcrypt.GenerateFromPassword([]byte(password), bcrypt.DefaultCost)
	if err != nil {
		return "", err
	}
	return string(hashedPassword), nil
}

func NullStringToString(ns sql.NullString) string {
	if ns.Valid {
		return ns.String
	}
	return "" // Handle the case where the value is NULL
}

func StringToNullString(s string) sql.NullString {
	if s != "" {
		return sql.NullString{String: s, Valid: true}
	}
	return sql.NullString{Valid: false}
}

func StringToNullTime(s string) sql.NullTime {
	if s != "" {
		// Set the location to the local time zone
		loc, err := time.LoadLocation("Asia/Jakarta")
		if err != nil {
			// handle the error, for example, return an sql.NullTime with Valid=false
			fmt.Println("Error loading location:", err)
			return sql.NullTime{Valid: false}
		}

		t, err := time.ParseInLocation(time.RFC3339, s, loc)
		if err == nil {
			fmt.Println(t)
			return sql.NullTime{Time: t, Valid: true}
		}
		// handle the error, for example, return an sql.NullTime with Valid=false
		fmt.Println("Error parsing time:", err)
	}
	return sql.NullTime{Valid: false}
}
