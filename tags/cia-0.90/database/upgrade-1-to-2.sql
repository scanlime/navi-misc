--
-- Upgrades a CIA database from version 1 to version 2
--
-- Micah Dowty
--

------ Create completely new tables

CREATE TABLE IF NOT EXISTS meta
(
    name         VARCHAR(32),
    value        VARCHAR(255),
);

INSERT IGNORE INTO meta VALUES( 'version', 2 );

CREATE TABLE IF NOT EXISTS cache
(
    id          VARCHAR(32) PRIMARY KEY,
    value       LONGBLOB NOT NULL,
    atime       BIGINT,
    expiration  BIGINT,
);

-- stats_subscriptions isn't new, but the addition of an 'id'
-- column is quite significant. Subscriptions aren't that important
-- to preserve, so we'll just redo the table instead of trying to
-- retroactively assign ids to each row.
DROP TABLE stats_subscriptions;
CREATE TABLE stats_subscriptions
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

------ Add 'NOT NULL' qualifiers

ALTER TABLE stats_metadata MODIFY mime_type VARCHAR(32) NOT NULL;
ALTER TABLE stats_metadata MODIFY value LONGBLOB NOT NULL;

------ Add new columns

ALTER TABLE stats_metadata ADD mtime BIGINT AFTER value;

--- The End ---