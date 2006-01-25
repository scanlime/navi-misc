--
-- Upgrades a CIA database from version 5 to version 6
--
-- Micah Dowty
--

------ Set version

UPDATE meta SET value = 6 WHERE name = 'version';

------ Modify existing tables

ALTER TABLE stats_messages
    MODIFY target_path VARCHAR(128) NOT NULL,
    MODIFY id INT UNSIGNED AUTO_INCREMENT,
    MODIFY timestamp INT UNSIGNED,
    ADD INDEX (id),
    ADD INDEX (target_path, id);

ALTER TABLE stats_metadata
    MODIFY mtime INT UNSIGNED;


ALTER TABLE stats_counters
    MODIFY event_count INT UNSIGNED NOT NULL DEFAULT 0,
    MODIFY first_time INT UNSIGNED,
    MODIFY last_time INT UNSIGNED;

ALTER TABLE stats_subscriptions
    MODIFY id INT UNSIGNED AUTO_INCREMENT,
    MODIFY expiration INT UNSIGNED NOT NULL;

ALTER TABLE stats_relations
    MODIFY strength INT UNSIGNED NOT NULL DEFAULT 0,
    MODIFY freshness INT UNSIGNED;

-- These really should be run, but can't easily because of foreign key constraints.

-- ALTER TABLE users
--     ADD INDEX (uid),
--     MODIFY uid INT UNSIGNED AUTO_INCREMENT,
--     MODIFY creation_time INT UNSIGNED NOT NULL,
--     MODIFY key_atime INT UNSIGNED,
--     MODIFY login_atime INT UNSIGNED,
--     MODIFY login_mtime INT UNSIGNED;

-- ALTER TABLE capabilities
--     MODIFY uid INT UNSIGNED NOT NULL;

-- ALTER TABLE audit_trail
--     MODIFY id INT UNSIGNED AUTO_INCREMENT,
--     MODIFY timestamp INT UNSIGNED NOT NULL,
--     MODIFY uid INT UNSIGNED NOT NULL;

--- The End ---