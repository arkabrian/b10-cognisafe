-- name: CreateLabSession :one
INSERT INTO LabSession (
    lab_id,
    pic,
    module_topic,
    start_time,
    end_time,
    location
) VALUES ( $1, $2, $3, $4, $5, $6 ) RETURNING *;

-- name: GetLabSession :one
SELECT * FROM LabSession
WHERE lab_session_id = $1 LIMIT 1;

-- name: AddAttendance :one
UPDATE LabSession
SET attendance = attendance + 1
WHERE lab_session_id = $1
RETURNING *;

-- name: EndLabSession :one
UPDATE LabSession
SET attendance = attendance + 1
WHERE lab_session_id = $1
RETURNING *;

-- name: UpdateGasTrue :one
UPDATE FallGas SET gas = 1 RETURNING *;

-- name: UpdateGasFalse :one
UPDATE FallGas SET gas = 0 RETURNING *;

-- name: UpdateFallTrue :one
UPDATE FallGas SET fall = 1 RETURNING *;

-- name: UpdateFallFalse :one
UPDATE FallGas SET fall = 0 RETURNING *;

-- name: GetFallGasData :one
SELECT * FROM FallGas;