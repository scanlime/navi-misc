--
-- Schema for CIA's database: run this to create tables needed by CIA
-- in a blank database, or repair deleted tables. This will -not- upgrade
-- existing old tables to a new schema.
--
-- For example:
--  mysql -u root -p < init.sql
--

CREATE DATABASE IF NOT EXISTS cia;
USE cia;

CREATE TABLE IF NOT EXISTS capabilities
(
    key_data  TEXT NOT NULL,
    id        TEXT NOT NULL,
    owner     TEXT,
);

CREATE TABLE IF NOT EXISTS rulesets
(
    uri TEXT NOT NULL,
    xml TEXT NOT NULL,
);

CREATE TABLE IF NOT EXISTS stats_messages
(
    target_path  VARCHAR(128) NOT NULL,
    id           INT NOT NULL AUTO_INCREMENT,
    xml          TEXT NOT NULL,
    PRIMARY KEY(target_path, id)
);

CREATE TABLE IF NOT EXISTS stats_catalog
(
    target_path  VARCHAR(128) NOT NULL,
    parent_path  VARCHAR(128) NOT NULL,
    PRIMARY KEY(target_path, parent_path)
);

CREATE TABLE IF NOT EXISTS stats_metadata
(
    target_path  VARCHAR(128) NOT NULL,
    name         VARCHAR(32) NOT NULL,
    mime_type    VARCHAR(32) NOT NULL,
    value        BLOB NOT NULL,
    PRIMARY KEY(target_path, name)
);

CREATE TABLE IF NOT EXISTS stats_counters
(
    target_path  VARCHAR(128) NOT NULL,
    name         VARCHAR(32) NOT NULL,
    event_count  INT NOT NULL DEFAULT 0,
    first_time   BIGINT,
    last_time    BIGINT,
    PRIMARY KEY(target_path, name)
);
