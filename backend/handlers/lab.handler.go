package handlers

import (
	"errors"
	"fmt"
	"log"
	"net"
	"net/http"
	"time"

	"cognisafe.com/b/db/sqlc"
	"cognisafe.com/b/token"
	"cognisafe.com/b/utils"
)

func NewLabHandler(l *log.Logger, q *sqlc.Queries, u *AuthedUser, t *token.Maker) *LabHandler {
	var c uint = 0
	return &LabHandler{&Handler{l, q, &c, u}}
}

func (lh *LabHandler) CreateLabSessionH(w http.ResponseWriter, r *http.Request) {
	hp := HandlerParam{w, r, http.MethodPost, lh.createLabSession}
	lh.h.handleRequest(hp, lh.h.u)
}

func (lh *LabHandler) AttendenceSessionH(w http.ResponseWriter, r *http.Request) {
	hp := HandlerParam{w, r, http.MethodPost, lh.attendance}
	lh.h.handleRequest(hp, lh.h.u)
}

func (lh *LabHandler) GetPerson(w http.ResponseWriter, r *http.Request) {
	hp := HandlerParam{w, r, http.MethodGet, lh.getPerson}
	lh.h.handleRequest(hp, lh.h.u)
}

func (lh *LabHandler) getPerson(w http.ResponseWriter, r *http.Request) error {
	valid_attendance, err := lh.h.q.GetValidAttendance(r.Context())
	if err != nil {
		http.Error(w, "Error getting data", http.StatusBadRequest)
		return err
	}

	toJSON(w, valid_attendance)
	return nil
}

func (lh *LabHandler) createLabSession(w http.ResponseWriter, r *http.Request) error {
	// Parse form data
	if err := r.ParseForm(); err != nil {
		http.Error(w, "Error parsing form data", http.StatusBadRequest)
		return err
	}

	// Retrieve form values
	lab_id := r.FormValue("lab_id")
	pic := r.FormValue("pic")
	module_topic := r.FormValue("module_topic")
	start_time := r.FormValue("start_time")
	end_time := r.FormValue("end_time")
	location := r.FormValue("location")

	labSessParam := sqlc.CreateLabSessionParams{
		LabID:       utils.StringToNullString(lab_id),
		Pic:         utils.StringToNullString(pic),
		ModuleTopic: utils.StringToNullString(module_topic),
		StartTime:   utils.StringToNullTime(start_time),
		EndTime:     utils.StringToNullTime(end_time),
		Location:    utils.StringToNullString(location),
	}

	labSess, err := lh.h.q.CreateLabSession(r.Context(), labSessParam)
	if err != nil {
		http.Error(w, "Cannot create lab session", http.StatusInternalServerError)
		return errors.New("cannot create lab session")
	}

	sessionTokenParams := utils.LabSessTokenParams{
		LabSessionID: labSess.LabSessionID,
		StartTime:    labSess.StartTime.Time,
		EndTime:      labSess.EndTime.Time,
	}

	token, err := utils.EncodeLabSessTokenParams(sessionTokenParams)
	if err != nil {
		lh.h.l.Println("Cannot encode the payload")
	}

	if err := utils.SaveTokenToFile(token); err != nil {
		lh.h.l.Println("Cannot store it in local storge ", err)
	}

	w.WriteHeader(http.StatusCreated)
	toJSON(w, labSess)
	return nil
}

func (lh *LabHandler) attendance(w http.ResponseWriter, r *http.Request) error {
	if err := r.ParseForm(); err != nil {
		http.Error(w, "Error parsing form data", http.StatusBadRequest)
		return err
	}

	// Retrieve form values
	mac_addr := r.FormValue("mac_address")
	lh.h.l.Println(mac_addr)

	token, err := utils.ReadTokenFromFile()
	if err != nil {
		lh.h.l.Println("Lab Session doesn't exists")
	}

	payload, err := utils.DecodeLabSessTokenParams(token)
	if err != nil {
		lh.h.l.Println(err)
	}

	// subnetStr := "0.0.0.0"

	// _, subnet, err := net.ParseCIDR(subnetStr)
	// if err != nil {
	// 	http.Error(w, "Error "+err.Error(), http.StatusInternalServerError)
	// 	return fmt.Errorf("error parsing subnet: %v", err)
	// }

	// // Check if the request is coming from the same subnet
	clientIP, _, err := net.SplitHostPort(r.RemoteAddr)
	if err != nil {
		http.Error(w, "Error "+err.Error(), http.StatusInternalServerError)
		return fmt.Errorf("error splitting host and port: %v", err)
	}

	// requestIP := net.ParseIP(clientIP)
	// if requestIP == nil {
	// 	return fmt.Errorf("error parsing request IP: %v", err)
	// }

	// lh.h.l.Println(clientIP, requestIP)
	// if !subnet.Contains(requestIP) {
	// 	return fmt.Errorf("request is not coming from the same subnet")
	// }

	currentTime := time.Now()

	lh.h.l.Println(currentTime)
	lh.h.l.Println(payload.StartTime)
	lh.h.l.Println(payload.EndTime)

	if currentTime.Before(payload.StartTime) || currentTime.After(payload.EndTime) {
		http.Error(w, "Error request is not within the payload's time range", http.StatusInternalServerError)
		return fmt.Errorf("request is not within the payload's time range")
	}

	_, err = lh.h.q.AddAttendance(r.Context(), payload.LabSessionID)
	if err != nil {
		http.Error(w, "Cannot add attendance", http.StatusInternalServerError)
		return errors.New("cannot add attendance")
	}
	attendParam := sqlc.AttendParams{
		LabSessionID: payload.LabSessionID,
		IpAddress:    clientIP,
		MacAddress:   mac_addr,
	}
	_, err = lh.h.q.Attend(r.Context(), attendParam)
	if err != nil {
		http.Error(w, "Failed to attend", http.StatusInternalServerError)
		return errors.New(err.Error())
	}

	toJSON(w, token)
	return nil
}
