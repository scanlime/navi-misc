--
-- Upgrades a CIA database from version 2 to version 3
--
-- Micah Dowty
--

------ Set version

UPDATE meta SET value = 3 WHERE name = 'version';

------ Create completely new tables

CREATE TABLE IF NOT EXISTS stats_relations
(
    target_a_path  VARCHAR(128),
    target_b_path  VARCHAR(128),
    strength       INT NOT NULL DEFAULT 0,
    freshness      BIGINT,

    PRIMARY KEY(target_a_path, target_b_path),
    INDEX (target_a_path),
    INDEX (target_b_path),
    FOREIGN KEY (target_a_path) REFERENCES stats_catalog(target_path) ON DELETE CASCADE,
    FOREIGN KEY (target_b_path) REFERENCES stats_catalog(target_path) ON DELETE CASCADE
) TYPE=INNODB;

--- The End ---