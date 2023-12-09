package db

import (
	"database/sql"
	"log"

	"cognisafe.com/b/db/sqlc"
	_ "github.com/lib/pq"
)

const (
	DB_DRIVER            = "postgres"
	DB_CONNECTION_STRING = "postgresql://bernanda:bernanda@localhost:5432/sr-db?sslmode=disable"
)

func Instantiate(l *log.Logger) (*sql.DB, *sqlc.Queries) {
	db, err1 := sql.Open(DB_DRIVER, DB_CONNECTION_STRING)
	if err1 != nil {
		l.Println("Error creating DB connection", err1)
		return nil, nil
	}

	err2 := db.Ping()
	if err2 != nil {
		l.Println("Error connecting to DB ", err2)
		return nil, nil
	}

	l.Println("🛢️  DB Connected")
	return db, sqlc.New(db)
}
