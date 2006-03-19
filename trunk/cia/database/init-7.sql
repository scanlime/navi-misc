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

INSERT IGNORE INTO meta VALUES( 'version', 7 );


------------------------------------------------------------------------
----------------------------------------------------------- Security
------------------------------------------------------------------------

CREATE TABLE IF NOT EXISTS users
(
    uid           INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
    secret_key    VARCHAR(32) NOT NULL,
    active        BOOL NOT NULL DEFAULT 1,

    full_name     TEXT,
    email         TEXT,
    creation_time INT UNSIGNED NOT NULL,
    key_atime     INT UNSIGNED,

    login_name       VARCHAR(32),
    login_passwd_md5 CHAR(32),
    login_atime      INT UNSIGNED,
    login_mtime      INT UNSIGNED,

    INDEX(secret_key),
    INDEX(login_name)
) TYPE=INNODB;

CREATE TABLE IF NOT EXISTS capabilities
(
    uid       INT UNSIGNED NOT NULL,
    cap_md5   CHAR(32) NOT NULL,
    cap_repr  TEXT NOT NULL,

    FOREIGN KEY (uid) REFERENCES users(uid) ON DELETE CASCADE,
    INDEX (cap_md5),
    UNIQUE INDEX (uid, cap_md5)
) TYPE=INNODB;

CREATE TABLE IF NOT EXISTS audit_trail
(
    id               INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
    timestamp        INT UNSIGNED NOT NULL,
    uid              INT UNSIGNED NOT NULL,

    action_domain    VARCHAR(32) NOT NULL,
    action_name      TEXT NOT NULL,

    main_param       TEXT,
    params           LONGBLOB,

    allowed          BOOL NOT NULL,
    results          LONGBLOB,

    FOREIGN KEY (uid) REFERENCES users(uid) ON DELETE CASCADE,
    INDEX (id),
    INDEX (uid)
) TYPE=INNODB;


------------------------------------------------------------------------
----------------------------------------------------------- Rulesets
------------------------------------------------------------------------

CREATE TABLE IF NOT EXISTS rulesets
(
    uri TEXT NOT NULL,
    xml TEXT NOT NULL
);


------------------------------------------------------------------------
----------------------------------------------------------- Stats
------------------------------------------------------------------------

-- Map the full path of each stats target to its parent.
-- Every stats target we know about is in this table, and
-- deleting a target causes all metadata, subtargets,
-- and counters for that target to be automatically deleted.
CREATE TABLE IF NOT EXISTS stats_catalog
(
    parent_path  VARCHAR(128),
    target_path  VARCHAR(128) PRIMARY KEY,

    FOREIGN KEY (parent_path) REFERENCES stats_catalog (target_path) ON DELETE CASCADE,
    INDEX (parent_path)
) TYPE=INNODB;

-- This table represents an undirected graph, the vertices of which are stats
-- targets. Edges have an associated 'strength' and 'freshness'. When a message
-- is delivered to multiple stats targets, all edges between those stats
-- targets are strengthened by incrementing their 'strength' and setting the
-- 'freshness' to the current time.
--
-- As the graph is undirected, edge uniqueness is maintained by requiring
-- that target_a_path <= target_b_path.
--
-- This graph can be used to create nifty reports showing other targets
-- related to any one target, and it could even be used to graphically
-- represent the connections between projects and authors.
CREATE TABLE IF NOT EXISTS stats_relations
(
    target_a_path  VARCHAR(128),
    target_b_path  VARCHAR(128),
    strength       INT UNSIGNED NOT NULL DEFAULT 0,
    freshness      INT UNSIGNED,

    PRIMARY KEY(target_a_path, target_b_path),
    INDEX (target_a_path),
    INDEX (target_b_path),
    FOREIGN KEY (target_a_path) REFERENCES stats_catalog(target_path) ON DELETE CASCADE,
    FOREIGN KEY (target_b_path) REFERENCES stats_catalog(target_path) ON DELETE CASCADE
) TYPE=INNODB;

-- Store metadata keys, times, and values for each target
CREATE TABLE IF NOT EXISTS stats_metadata
(
    target_path  VARCHAR(128),
    name         VARCHAR(32),
    mime_type    VARCHAR(32) NOT NULL,
    value        LONGBLOB NOT NULL,
    mtime        INT UNSIGNED,

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
    event_count  INT UNSIGNED NOT NULL DEFAULT 0,
    first_time   INT UNSIGNED,
    last_time    INT UNSIGNED,

    PRIMARY KEY(target_path, name),
    INDEX (target_path),
    INDEX (name),
    INDEX (event_count),
    INDEX (first_time),
    INDEX (last_time),
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
    id           INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
    target_path  VARCHAR(128) NOT NULL,
    expiration   INT UNSIGNED NOT NULL,
    scope        VARCHAR(32),
    client       VARCHAR(64),
    trigger      BLOB NOT NULL,
    failures     INT NOT NULL DEFAULT 0,

    INDEX (target_path),
    INDEX (expiration),
    INDEX (client)
);

--- The End ---
