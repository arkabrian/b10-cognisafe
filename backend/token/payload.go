package token

import (
	"time"

	"github.com/google/uuid"
)

// struct of payload
type Payload struct {
	ID        uuid.UUID `json:"id"`
	LabID     string    `json:"lab_id"`
	Labname   string    `json:"labname"`
	IssuedAt  time.Time `json:"issued_at"`
	ExpiredAt time.Time `json:"expired_at"`
}

func NewPayload(lab_id string, labname string, duration time.Duration) *Payload {
	token_id, err := uuid.NewRandom()
	if err != nil {
		return nil
	}

	return &Payload{
		ID:        token_id,
		LabID:     lab_id,
		Labname:   labname,
		IssuedAt:  time.Now(),
		ExpiredAt: time.Now().Add(duration),
	}
}

func (p *Payload) TimeValid() error {
	if time.Now().After(p.ExpiredAt) {
		return ErrExpiredToken
	}
	return nil
}
