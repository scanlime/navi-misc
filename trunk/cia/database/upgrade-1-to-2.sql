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

------ Add 'NOT NULL' qualifiers

ALTER TABLE stats_metadata MODIFY mime_type VARCHAR(32) NOT NULL;
ALTER TABLE stats_metadata MODIFY value LONGBLOB NOT NULL;

------ Add new columns

ALTER TABLE stats_metadata ADD mtime BIGINT AFTER value;
ALTER TABLE stats_subscriptions ADD failures INT NOT NULL DEFAULT 0 AFTER trigger;

--- The End ---