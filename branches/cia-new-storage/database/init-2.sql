--
-- This file initializes CIA's SQL database
-- This version requires MySQL, and you -must-
-- be using a MySQL server with InnoDB enabled.
--
-- For example:
--  mysql -u root -p < init.sql
--
-- Micah Dowty
--

CREATE DATABASE IF NOT EXISTS cia;
USE cia;

------------------------------------------------------------------------
----------------------------------------------------------- Metadata
------------------------------------------------------------------------

-- This table includes metadata for this database. Currently
-- this just consists of the schema version it uses.
CREATE TABLE IF NOT EXISTS meta
(
    name         VARCHAR(32),
    value        VARCHAR(255)
);

INSERT IGNORE INTO meta VALUES( 'version', 2 );


------------------------------------------------------------------------
----------------------------------------------------------- Security
------------------------------------------------------------------------

CREATE TABLE IF NOT EXISTS capabilities
(
    key_data  TEXT NOT NULL,
    id        TEXT NOT NULL,
    owner     TEXT
);

------------------------------------------------------------------------
----------------------------------------------------------- Rulesets
------------------------------------------------------------------------

CREATE TABLE IF NOT EXISTS rulesets
(
    uri TEXT NOT NULL,
    xml TEXT NOT NULL
);

------------------------------------------------------------------------
----------------------------------------------------------- Cache
------------------------------------------------------------------------

-- This is a generic cache system that maps opaque object IDs
-- to the cached data. Each cache entry has an access time that gets
-- updated when there's a cache hit. Occasionally we delete all entries
-- that haven't been accessed in a while.
CREATE TABLE IF NOT EXISTS cache
(
    id          VARCHAR(32) PRIMARY KEY,
    value       LONGBLOB NOT NULL,
    atime       BIGINT,
    expiration  BIGINT
);

------------------------------------------------------------------------
----------------------------------------------------------- Stats
------------------------------------------------------------------------

-- Map the full path of each stats target to its parent.
-- Every stats target we know about is in this table, and
-- deleting a target causes all metadata, messages, subtargets,
-- and counters for that target to be automatically deleted.
CREATE TABLE IF NOT EXISTS stats_catalog
(
    parent_path  VARCHAR(128),
    target_path  VARCHAR(128) PRIMARY KEY,

    FOREIGN KEY (parent_path) REFERENCES stats_catalog (target_path) ON DELETE CASCADE,
    INDEX (parent_path)
) TYPE=INNODB;

-- A place to store all stats messages, with a global continuously increasing ID.
-- This doesn't yet include rules to automatically delete old messages, so the log is infinite.
-- Note the index on (id). This is required to get reasonable performance.
CREATE TABLE IF NOT EXISTS stats_messages
(
    target_path  VARCHAR(128),
    id           BIGINT PRIMARY KEY AUTO_INCREMENT,
    timestamp    BIGINT,
    xml          TEXT NOT NULL,

    INDEX (target_path),
    FOREIGN KEY (target_path) REFERENCES stats_catalog(target_path) ON DELETE CASCADE
) TYPE=INNODB;

-- Store metadata keys, times, and values for each target
CREATE TABLE IF NOT EXISTS stats_metadata
(
    target_path  VARCHAR(128),
    name         VARCHAR(32),
    mime_type    VARCHAR(32) NOT NULL,
    value        LONGBLOB NOT NULL,
    mtime        BIGINT,

    PRIMARY KEY(target_path, name),
    INDEX (target_path),
    INDEX (name),
    FOREIGN KEY (target_path) REFERENCES stats_catalog(target_path) ON DELETE CASCADE
) TYPE=INNODB;

-- Store counters
CREATE TABLE IF NOT EXISTS stats_counters
(
    target_path  VARCHAR(128),
    name         VARCHAR(32),
    event_count  INT NOT NULL DEFAULT 0,
    first_time   BIGINT,
    last_time    BIGINT,

    PRIMARY KEY(target_path, name),
    INDEX (target_path),
    INDEX (name),
    FOREIGN KEY (target_path) REFERENCES stats_catalog(target_path) ON DELETE CASCADE
) TYPE=INNODB;

-- Users can subscribe to changes in stats targets.
-- Each subscription has an expiration timestamp, so it's not necessary
-- to check foreign key constraints.
--
-- Subscriptions have an optional 'scope' that indicates what part of the
-- stats target they refer to, or NULL for any part. The 'trigger' is a pickled
-- (callable, args, kwargs) tuple. callable(*args, **kwargs) is called when the
-- stats target is modified in a way that matches 'scope'.
--
-- 'client' is the IP address of the client who created this subscription. It
-- is not used in calling the trigger (if it's needed it will be included in
-- the pickle) but it is used to administer the subscriptions table.
--
CREATE TABLE IF NOT EXISTS stats_subscriptions
(
    id           BIGINT PRIMARY KEY AUTO_INCREMENT,
    target_path  VARCHAR(128) NOT NULL,
    expiration   BIGINT NOT NULL,
    scope        VARCHAR(32),
    client       VARCHAR(64),
    trigger      BLOB NOT NULL,
    failures     INT NOT NULL DEFAULT 0,

    INDEX (target_path),
    INDEX (expiration),
    INDEX (client)
);

--- The End ---
