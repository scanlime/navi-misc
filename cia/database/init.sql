--
-- Schema for CIA's database: run this to create tables needed by CIA
-- in a blank database, or repair deleted tables. This will -not- upgrade
-- existing old tables to a new schema.
--
-- In postgresql, for example:
--   dbcreate cia
--   psql -f init.sql cia
--

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

CREATE TABLE stats_messages
(
    target_path  VARCHAR(128) NOT NULL,
    id           INT NOT NULL,
    xml          TEXT NOT NULL,
    PRIMARY KEY(target_path, id)
);

CREATE TABLE stats_catalog
(
    target_path  VARCHAR(128) NOT NULL,
    parent_path  VARCHAR(128) NOT NULL,
    PRIMARY KEY(target_path, parent_path)
);

CREATE TABLE stats_metadata
(
    target_path  VARCHAR(128) NOT NULL,
    name         VARCHAR(32) NOT NULL,
    mime_type    VARCHAR(32) NOT NULL,
    value        BYTEA NOT NULL,
    PRIMARY KEY(target_path, name)
);

CREATE TABLE stats_counters
(
    target_path  VARCHAR(128) NOT NULL,
    name         VARCHAR(32) NOT NULL,
    event_count  INT NOT NULL DEFAULT 0,
    first_time   BIGINT,
    last_time    BIGINT,
    PRIMARY KEY(target_path, name)
);

--- The End ---