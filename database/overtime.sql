BEGIN;

ALTER TABLE parking_assignments
    ADD COLUMN IF NOT EXISTS scheduled_end TIMESTAMPTZ;

UPDATE parking_assignments
SET scheduled_end = parked_at + (
    GREATEST(
        1,
        CEIL(EXTRACT(EPOCH FROM (COALESCE(checkout_at, CURRENT_TIMESTAMP) - parked_at)) / 3600.0)
    ) * INTERVAL '1 hour'
)
WHERE scheduled_end IS NULL;

ALTER TABLE parking_assignments
    ALTER COLUMN scheduled_end SET NOT NULL;

DO $$
BEGIN
    IF NOT EXISTS (
        SELECT 1
        FROM pg_constraint
        WHERE conname = 'parking_assignments_scheduled_end_check'
    ) THEN
        ALTER TABLE parking_assignments
            ADD CONSTRAINT parking_assignments_scheduled_end_check
            CHECK (scheduled_end > parked_at);
    END IF;
END
$$;

COMMIT;
