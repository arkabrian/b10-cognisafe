-- name: GetAccount :one
SELECT * FROM account
WHERE lab_id = $1 LIMIT 1;

-- name: CreateAccount :one
INSERT INTO account (
  labname, email, password_hash
) VALUES (
  $1, $2, $3
)
RETURNING *;

-- name: DeleteAccount :one
DELETE FROM account
WHERE lab_id = $1
RETURNING *;

-- name: GetAccountbyEmail :one
SELECT * FROM account
WHERE email = $1 LIMIT 1;