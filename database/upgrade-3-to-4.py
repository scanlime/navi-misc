#
# Upgrades a CIA database from version 3 to version 4.
#
# This script must migrate security data to the new format.
# Back up your database before running this, as failure to
# finish could cause all security data to be lost!
#
# -- Micah Dowty
#

import sys, os, time, md5
sys.path[0] = os.path.join(sys.path[0], "..")
from LibCIA import Database

Database.init()
cursor = Database.pool.connect().cursor()

# Make sure we're starting with version 3
cursor.execute("SELECT value FROM meta WHERE name = 'version'")
if cursor.fetchone()[0] != "3":
    raise Exception("This script must only be run on version 3 databases")

# Read in all security data from the old capabilities table...
# Each key gets a list of capabilities, but only one owner.
cursor.execute("SELECT * FROM capabilities")
key_owner = {}
key_ownerMail = {}
key_capabilities = {}
while True:
    row = cursor.fetchone()
    if row:
        key, id, owner = row
        ownerMail = None
        key_capabilities.setdefault(key, []).append(id)

        # Separate the owner into name and email address
        if owner:
            ownerParts = owner.split("<")
            if len(ownerParts) > 1:
                owner = ownerParts[0].strip()
                ownerMail = ownerParts[1].split(">")[0].strip()
            else:
                owner = owner.strip()

        key_owner[key] = owner
        key_ownerMail[key] = ownerMail
    else:
        break

# Delete the old capabilities table and create the new ones
cursor.execute("DROP TABLE capabilities")

cursor.execute("""
CREATE TABLE users
(
    uid           BIGINT PRIMARY KEY AUTO_INCREMENT,
    secret_key    VARCHAR(32) NOT NULL,
    active        BOOL NOT NULL DEFAULT 1,

    full_name     TEXT,
    email         TEXT,
    creation_time BIGINT NOT NULL,
    key_atime     BIGINT,

    login_name       VARCHAR(32),
    login_passwd_md5 CHAR(32),
    login_atime      BIGINT,
    login_mtime      BIGINT,

    INDEX(secret_key),
    INDEX(login_name)
) TYPE=INNODB
""")

cursor.execute("""
CREATE TABLE capabilities
(
    uid       BIGINT NOT NULL,
    cap_md5   CHAR(32) NOT NULL,
    cap_repr  TEXT NOT NULL,

    FOREIGN KEY (uid) REFERENCES users(uid) ON DELETE CASCADE,
    INDEX (cap_md5),
    UNIQUE INDEX (uid, cap_md5)
) TYPE=INNODB
""")

cursor.execute("""
CREATE TABLE audit_trail
(
    id               BIGINT PRIMARY KEY AUTO_INCREMENT,
    timestamp        BIGINT NOT NULL,
    uid              BIGINT NOT NULL,

    action_domain    VARCHAR(32) NOT NULL,
    action_name      TEXT NOT NULL,

    main_param       TEXT,
    params           LONGBLOB,

    allowed          BOOL NOT NULL,
    results          LONGBLOB,

    FOREIGN KEY (uid) REFERENCES users(uid) ON DELETE CASCADE,
    INDEX (id),
    INDEX (uid)
) TYPE=INNODB
""")

# Populate the user table, saving the resulting UIDs
key_uids = {}
for key in key_owner.iterkeys():
    cursor.execute("INSERT INTO users (secret_key, full_name, email, creation_time) "
                   "VALUES (%s, %s, %s, %d)" % (
        Database.quote(key, 'varchar'),
        Database.quote(key_owner[key], 'varchar'),
        Database.quote(key_ownerMail[key], 'varchar'),
        time.time()))
    cursor.execute("SELECT LAST_INSERT_ID()")
    key_uids[key] = cursor.fetchone()[0]

# Populate the capabilities table
for key in key_owner.iterkeys():
    for capability in key_capabilities[key]:
        cursor.execute("INSERT INTO capabilities (uid, cap_md5, cap_repr) VALUES(%s, %s, %s)" % (
            key_uids[key],
            Database.quote(md5.new(capability).hexdigest(), 'char'),
            Database.quote(capability, 'text')))

# Seems this was a success, bump the db version
cursor.execute("UPDATE meta SET value = '4' WHERE name = 'version'")

### The End ###
