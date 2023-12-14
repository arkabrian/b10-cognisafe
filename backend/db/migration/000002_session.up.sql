CREATE TABLE IF NOT EXISTS "sessions" (
  "id" uuid PRIMARY KEY,
  "lab_id" varchar NOT NULL, 
  "labname" varchar NOT NULL,
  "refresh_token" varchar NOT NULL,
  "is_blocked" boolean NOT NULL DEFAULT false,
  "expires_at" timestamptz NOT NULL,
  "created_at" timestamptz NOT NULL DEFAULT (now())
);

ALTER TABLE "sessions" ADD FOREIGN KEY ("lab_id") REFERENCES "account" ("lab_id");