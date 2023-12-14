CREATE SEQUENCE IF NOT EXISTS lab_id_seq;
CREATE TABLE IF NOT EXISTS account (
    lab_id VARCHAR(32) DEFAULT 'LID' || nextval('lab_id_seq') || to_char(current_timestamp, 'YYYYMMDDHH24MISS'),
    labname VARCHAR(20) NOT NULL,
    email VARCHAR(20) UNIQUE NOT NULL,
    password_hash VARCHAR(100) NOT NULL,
    created_at TIMESTAMP DEFAULT NOW(),
    PRIMARY KEY (lab_id)
);

CREATE SEQUENCE IF NOT EXISTS lab_session_id_seq;
CREATE TABLE IF NOT EXISTS LabSession (
    lab_session_id VARCHAR(32) DEFAULT 'LSID' || nextval('lab_session_id_seq') || to_char(current_timestamp, 'YYYYMMDDHH24MISS'),
    lab_id VARCHAR(32),
    pic VARCHAR(48),
    module_topic VARCHAR(255),
    start_time TIMESTAMP,
    end_time TIMESTAMP,
    location VARCHAR(32),
    attendance INT,
       -- valid boolean DEFAULT TRUE,
    indicator VARCHAR(255),
    PRIMARY KEY (lab_session_id),
    FOREIGN KEY (lab_id) REFERENCES account(lab_id)
);

DROP TABLE IF EXISTS Attendance;
CREATE TABLE IF NOT EXISTS Attendance (
    lab_session_id VARCHAR(32),
    ip_address VARCHAR(32),
    mac_address VARCHAR(32),
    fall INT DEfAULT 0,
    gas INT DEFAULT 0,
   -- FOREIGN KEY (valid) REFERENCES LabSession(valid)
    PRIMARY KEY (lab_session_id, ip_address, mac_address),
    FOREIGN KEY (lab_session_id) REFERENCES LabSession(lab_session_id)
);

CREATE TABLE IF NOT EXISTS Logging (
    lab_session_id VARCHAR(32),
    time_log TIMESTAMP,
    ip_address VARCHAR(15),
    report TEXT,
    FOREIGN KEY (lab_session_id) REFERENCES LabSession(lab_session_id)
);

CREATE TABLE IF NOT EXISTS FallGas (
    fall INT DEfAULT 0,
    gas INT DEFAULT 0
);
