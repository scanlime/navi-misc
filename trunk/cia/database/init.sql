--
-- This file initializes CIA's PostgreSQL database.
--
-- For example:
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
-- This doesn't yet include rules to automatically delete old messages, so the log is infinite :-/
CREATE TABLE stats_messages
(
    target_path  VARCHAR(128) REFERENCES stats_catalog ON DELETE CASCADE,
    id           BIGINT DEFAULT nextval('stats_message_id'),
    xml          TEXT NOT NULL,
    PRIMARY KEY(target_path, id)
);

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
    first_time   timestamp DEFAULT NOW(),
    last_time    timestamp,
    PRIMARY KEY(target_path, name)
);

-- This function returns the parent associated with a given stats path
CREATE FUNCTION statsParent(VARCHAR) RETURNS VARCHAR AS '
    SELECT substring($1 from ''(.+)/[^/]+$'')
' LANGUAGE SQL STRICT;

-- A function that creates a target_path in our stats_catalog if it doesn't
-- exist, including creating parent entries recursively if necessary.
-- Really ugly... hopefully there's a better way to do this.
--
CREATE FUNCTION statsCreateTarget(VARCHAR) RETURNS VARCHAR AS '
    SELECT statsCreateTarget(statsParent($1));
    INSERT INTO stats_catalog (
       parent_path,
       target_path)
    (
       SELECT statsParent($1), $1 WHERE NOT EXISTS (
           SELECT 1 FROM stats_catalog WHERE target_path = $1
       )
    );
    SELECT $1;
' LANGUAGE SQL STRICT;

-- Rules to automatically create stats_catalog entries when
-- we try to insert new keys elsewhere for a stats target that
-- doesn't yet exist.
CREATE RULE stats_messages_autocreate AS ON INSERT TO stats_messages
    DO SELECT statsCreateTarget(NEW.target_path);
CREATE RULE stats_metadata_autocreate AS ON INSERT TO stats_metadata
    DO SELECT statsCreateTarget(NEW.target_path);
CREATE RULE stats_counters_autocreate AS ON INSERT TO stats_counters
    DO SELECT statsCreateTarget(NEW.target_path);

-- A function to increment one stats counter, creating it if necessary
-- usage: statsIncrementCounter(stats_target, counter_name)
--
-- Note the statsCreateTarget call: this should be done automatically
-- on insert by the stats_counters_autocreate rule, but it looks like
-- rules might not work from inside functions?
--
CREATE FUNCTION statsIncrementCounter(VARCHAR, VARCHAR) RETURNS VARCHAR AS '
    SELECT statsCreateTarget($1);
    INSERT INTO stats_counters (
       target_path,
       name)
    (
       SELECT $1, $2 WHERE NOT EXISTS (
           SELECT 1 FROM stats_counters WHERE target_path = $1 AND name = $2
       )
    );
    UPDATE stats_counters SET
       event_count = event_count + 1,
       last_time = NOW()
    WHERE target_path = $1 and name = $2;
    SELECT $1;
' LANGUAGE SQL STRICT;

-- Increment all applicable stats counters for a given path
CREATE FUNCTION statsIncrement(VARCHAR) RETURNS VARCHAR AS '
    SELECT statsIncrementCounter($1, ''today'');
    SELECT statsIncrementCounter($1, ''thisWeek'');
    SELECT statsIncrementCounter($1, ''thisMonth'');
    SELECT statsIncrementCounter($1, ''forever'');
' LANGUAGE SQL STRICT;

--- The End ---