-- name: Attend :one
INSERT INTO Attendance (
    lab_session_id,
    ip_address,
    mac_address
) VALUES ( $1, $2, $3 ) RETURNING *;

-- name: GetValidAttendance :many
SELECT lab_session_id,
    ip_address,
    mac_address FROM Attendance; 