--
-- This file initializes the therms database
--
-- For example:
--  mysql -u therm_writer -p < db-schema.sql
--
-- Micah Dowty
--

USE therm;


-- This table includes metadata for this database. Currently
-- this just consists of the schema version it uses.
CREATE TABLE IF NOT EXISTS meta
(
    name         VARCHAR(32),
    value        VARCHAR(255)
);

INSERT IGNORE INTO meta VALUES( 'version', 1 );


-- Information about data sources. Each source is
-- identified elsewhere only by a unique ID. This table
-- maps those IDs to a particular communications medium,
-- the IDs used within that medium, a short name, and
-- a long description.
--
CREATE TABLE IF NOT EXISTS sources
(
    id            INTEGER PRIMARY KEY AUTO_INCREMENT,

    medium        VARCHAR(32) NOT NULL,
    protocol      INTEGER,
    station_id    INTEGER,

    name          VARCHAR(64) NOT NULL,
    description   TEXT,

    micro_type    VARCHAR(64),
    sensor_type   VARCHAR(64),

    INDEX(name),
    INDEX(medium, protocol, station_id)
) TYPE=INNODB;


-- Every packet received is assigned a unique ID.
-- This table links that ID with the packet's source
-- and timestamp. Content decoded from the packet
-- is stored in other tables, referencing this packet ID.
--
CREATE TABLE IF NOT EXISTS packets
(
    id            BIGINT PRIMARY KEY AUTO_INCREMENT,
    time          TIMESTAMP NOT NULL,
    source        INTEGER NOT NULL,

    num_copies      INTEGER NOT NULL DEFAULT 1,
    sequence        INTEGER,
    signal_strength FLOAT,

    FOREIGN KEY (source) REFERENCES sources(id) ON DELETE CASCADE,
    INDEX (source),
    INDEX (time)
) TYPE=INNODB;


CREATE TABLE IF NOT EXISTS battery_voltage
(
    packet        BIGINT PRIMARY KEY,
    voltage       FLOAT NOT NULL,

    FOREIGN KEY (packet) REFERENCES packets(id) ON DELETE CASCADE
) TYPE=INNODB;


CREATE TABLE IF NOT EXISTS temperatures
(
    packet        BIGINT PRIMARY KEY,
    average       FLOAT NOT NULL,
    num_samples   INTEGER NOT NULL DEFAULT 1,

    FOREIGN KEY (packet) REFERENCES packets(id) ON DELETE CASCADE
) TYPE=INNODB;

--- The End ---
