/*
 * This file initializes CIA's PostgreSQL database.
 * Requires PostgreSQL and the PL/pgSQL procedural language.
 *
 * For example:
 *   dbcreate cia
 *   psql -f init.sql cia
 *
 * -- Micah Dowty
 */

----------------------------------------------------------- Security

CREATE TABLE capabilities
(
    key_data  TEXT NOT NULL,
    id        TEXT NOT NULL,
    owner     TEXT
);

----------------------------------------------------------- Rulesets

CREATE TABLE rulesets
(
    uri TEXT NOT NULL,
    xml TEXT NOT NULL
);

----------------------------------------------------------- Stats

-- Generates unique IDs for all messages we store
CREATE SEQUENCE stats_message_id;

-- Map the full path of each stats target to its parent.
-- Every stats target we know about is in this table, and
-- deleting a target causes all metadata, messages, subtargets,
-- and counters for that target to be automatically deleted.
CREATE TABLE stats_catalog
(
    parent_path  VARCHAR(128) REFERENCES stats_catalog (target_path) ON DELETE CASCADE,
    target_path  VARCHAR(128) PRIMARY KEY
);

-- A place to store all stats messages, with a global continuously increasing ID.
-- This doesn't yet include rules to automatically delete old messages, so the log is infinite.
-- Note the index on (id). This is required to get reasonable performance.
CREATE TABLE stats_messages
(
    target_path  VARCHAR(128) REFERENCES stats_catalog ON DELETE CASCADE,
    id           BIGINT DEFAULT nextval('stats_message_id'),
    timestamp    BIGINT,
    xml          TEXT NOT NULL,
    PRIMARY KEY(target_path, id)
);
CREATE INDEX stats_message_id_index ON stats_messages (id);

-- Store metadata keys, times, and values for each target
CREATE TABLE stats_metadata
(
    target_path  VARCHAR(128) REFERENCES stats_catalog ON DELETE CASCADE,
    name         VARCHAR(32),
    mime_type    VARCHAR(32) NOT NULL,
    value        BYTEA NOT NULL,
    PRIMARY KEY(target_path, name)
);

-- Store counters
CREATE TABLE stats_counters
(
    target_path  VARCHAR(128) REFERENCES stats_catalog ON DELETE CASCADE,
    name         VARCHAR(32),
    event_count  INT NOT NULL DEFAULT 0,
    first_time   BIGINT,
    last_time    BIGINT,
    PRIMARY KEY(target_path, name)
);

--- The End ---