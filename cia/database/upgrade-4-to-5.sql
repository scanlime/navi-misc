--
-- Upgrades a CIA database from version 4 to version 5
--
-- Micah Dowty
--

------ Set version

UPDATE meta SET value = 5 WHERE name = 'version';

------ Delete unused tables

DROP TABLE cache;

--- The End ---