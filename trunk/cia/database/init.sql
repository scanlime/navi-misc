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
    id           BIGINT NOT NULL AUTO_INCREMENT UNIQUE PRIMARY KEY,
    target_path  TEXT NOT NULL,
    xml          TEXT NOT NULL,
);

