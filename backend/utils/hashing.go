package utils

import (
	"database/sql"
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
		t, err := time.Parse(time.RFC3339, s)
		if err == nil {
			return sql.NullTime{Time: t, Valid: true}
		}
	}
	return sql.NullTime{Valid: false}
}
