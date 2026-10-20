-- Phase 2 authentication migration for databases created before auth_sessions.
BEGIN;

ALTER TABLE users
    DROP COLUMN IF EXISTS google_subject;

ALTER TABLE users
    ADD COLUMN IF NOT EXISTS username VARCHAR(50);

UPDATE users
SET username = split_part(email, '@', 1) || '_' || user_id
WHERE username IS NULL;

ALTER TABLE users
    ALTER COLUMN username SET NOT NULL;

CREATE UNIQUE INDEX IF NOT EXISTS users_username_unique
    ON users(username);

CREATE TABLE IF NOT EXISTS auth_sessions (
    session_id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    user_id BIGINT NOT NULL,
    token_hash CHAR(64) NOT NULL UNIQUE,
    expires_at TIMESTAMPTZ NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT CURRENT_TIMESTAMP,

    CONSTRAINT fk_auth_sessions_user
        FOREIGN KEY (user_id)
        REFERENCES users(user_id)
        ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS auth_sessions_token_lookup
    ON auth_sessions(token_hash);

COMMIT;
