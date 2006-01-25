""" LibCIA.Security

Implements CIA's simple security model. CIA uses a simple
capabilities-like system, where a particular capability is represented
on the wire as an unguessable random key, and internally by nearly any
python object.

Generally the 'universe' key will be saved somewhere only the server's
owner can access it on startup. The 'universe' key can be used to grant
other keys, which can then be distributed to other people or machines.

Note that this system has many of the same qualities as traditional
capabilities, but is not implemented in the same way. In traditional
capabilities, the unguessable keys map directly to objects that provide
whatever interface that key grants permissions to. This is simple and
effective for some systems, however in CIA the meaning of a key must be
preserved over a long period of time regardless of code changes- simply
making keys map to pickled callable objects would be too fragile.
"""
#
# CIA open source notification system
# Copyright (C) 2003-2005 Micah Dowty <micah@navi.cx>
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

from twisted.web import xmlrpc
from twisted.internet import defer
from twisted.python import failure, log
import string, os, md5, time, cPickle
from cStringIO import StringIO
import Database, RpcServer

# PyCAPTCHA is optional. We use it for certain operations
# that should be easy for humans to perform but should not
# be scriptable, like creating new users and granting capabilities to them.
try:
    import Captcha
    import Captcha.Visual.Tests
    # This trick keeps our factory from being overwritten on rebuild()
    if 'captchaFactory' not in globals():
        captchaFactory = Captcha.Factory()
except ImportError:
    captchaFactory = None


class SecurityException(Exception):
    pass

class NoSuchUser(SecurityException):
    pass

class InsufficientCapabilities(SecurityException):
    pass


class SecurityInterface(RpcServer.Interface):
    """An XML-RPC interface to the global capabilities database"""
    def protected_createUser(self, fullName, email, loginName=None):
        """Create a new user with the given identity, returning
           a (uid, key) tuple for the new user.
           """
        u = User(full_name=fullName, email=email,
                 login_name=loginName, create=True)
        result = defer.Deferred()
        u.getInfo().addCallback(self._createUser, result).addErrback(result.errback)
        return result

    def _createUser(self, info, result):
        """This gets the user info dict from User.getInfo()
           and reformats it into a (uid, key) tuple for createUser()
           """
        result.callback((info['uid'], info['secret_key']))

    def protected_grant(self, capability, uid):
        """Grant the given capability to the given user. Note that this means
           that the capability to use this function is effectively equivalent
           to the 'universe' key
           """
        # XML-RPC will munge tuples into lists, when we really want a tuple
        if type(capability) == list:
            capability = tuple(capability)

        return User(int(uid)).grant(capability)

    def xmlrpc_test(self, key, *capabilities):
        """Test the given key against one or more capabilities, returning True if it
           matches any of them, False otherwise.
           """
        return caps.test(key, *capabilities)

    def getCaptchaFactory(self):
        """Return our current CAPTCHA factory"""
        global captchaFactory
        if not captchaFactory:
            raise SecurityException("PyCAPTCHA is not installed")
        return captchaFactory

    def getCaptchaFromID(self, id):
        """Get a CAPTCHA test given its ID"""
        test = self.getCaptchaFactory().get(id)
        if not test:
            raise SecurityException("No test with the given ID was found, it might have expired")
        return test

    def xmlrpc_newCaptcha(self):
        """Creates a new CAPTCHA test that the caller can solve to perform
           certain operations that humans should be able to do easily but
           shouldn't be scriptable. Returns an ID that uniquely identifies
           this CAPTCHA test.
           """
        return self.getCaptchaFactory().new(Captcha.Visual.Tests.PseudoGimpy).id

    def xmlrpc_renderCaptcha(self, id):
        """Renders a (value, type) tuple containing a rendered version of
           the CAPTCHA test with the given ID. Currently the type will
           always be image/jpeg.
           """
        io = StringIO()
        self.getCaptchaFromID(id).render().save(io, "JPEG")
        return (xmlrpc.Binary(io.getvalue()), "image/jpeg")

    def requireCaptcha(self, id, solutions):
        """A utility for XML-RPC interfaces that require that they were
           originally invoked by a human. The caller must have previously
           requested and rendered a test using newCaptcha() and renderCaptcha(),
           then presented it to the user. They must then present the test ID and
           solution(s) to gain access to this function.
           A particular CAPTCHA test can only be used once.
           """
        if not self.getCaptchaFromID(id).testSolutions(solutions):
            raise SecurityException("Incorrect or expired CAPTCHA solution(s)")

    def xmlrpc_selfCreateUser(self, captchaId, captchaSolutions, fullName, email, loginName=None):
        """Whereas createUser() above requires special permissions,
           anyone can call this function if they can prove they're human
           by solving a CAPTCHA test. This lets people create new accounts
           for themselves on their own. Unlike createUser(), this can not be
           used to retrieve information about an existing user.
           """
        self.requireCaptcha(captchaId, captchaSolutions)
        u = User(full_name=fullName, email=email,
                 login_name=loginName, create=True)
        result = defer.Deferred()
        u.getInfo().addCallback(self._selfCreateUser, u, result).addErrback(result.errback)
        return result

    def _selfCreateUser(self, info, user, result):
        if not user.newUser:
            raise SecurityException("That user already exists")
        result.callback((info['uid'], info['secret_key']))

    def xmlrpc_selfGrant(self, captchaId, captchaSolutions, userKey, capability):
        """Any user can use this function, and a CAPTCHA solution (to prevent
           scripted aquisition of many capabilities) to grant themselves certain
           capabilities. Only a limited number of capabilities can be granted
           with this function.
           """
        self.requireCaptcha(captchaId, captchaSolutions)
        # XML-RPC will munge tuples into lists, when we really want a tuple
        if type(capability) == list:
            capability = tuple(capability)

        # Make sure the capability is one we allow
        if not self.isSelfGrantable(capability):
            raise SecurityException("This capability can not be self-granted")

        return User(key=userKey).grant(capability)

    def isSelfGrantable(self, capability):
        """Defines policy on which capabilities can be self-granted by a user"""
        if type(capability) == tuple and len(capability) == 2:
            # Two-element tuples...

            if capability[0] == "stats.path":
                # Granting a capability to a stats path is allowed
                return True

        return False


def createRandomKey(bytes = 24,
                    allowedChars = string.ascii_letters + string.digits):
    """Create a somewhat-secure random string of the given length.
       This implementation probably only works on Linux and similar systems.
       Also note that since we're using /dev/urandom instead of /dev/random,
       the system might be out of entropy. Using /dev/random however could
       block, and would make everything else here more complex.
       The result will be base64-encoded.
       """
    s = ''
    f = open("/dev/urandom")
    for i in xrange(bytes):
        s += allowedChars[ ord(f.read(1)) % len(allowedChars) ]
    f.close()
    return s


def logProtectedCall(result, path, args, user, allowed=True):
    """This should be called when a protected call was attempted,
       successful or not. It logs the attempt and its results in the
       audit_trail database. This audit trail can be used for several things-
       listing recently updated metadata (perhaps for a 'whats new?' page)
       or detecting and recovering from malicious use of keys.
       """
    # Store the first argument separately so we can relatively efficiently search for it
    if args:
        main_param = str(args[0])
    else:
        main_param = None

    # Get the user's UID. If it hasn't even been looked up successfully,
    # this is just a failed operation on a nonexistent user and it's not worth logging.
    uid = user.getCachedUid()
    if uid is None:
        return

    Database.pool.runOperation(
        "INSERT INTO audit_trail (timestamp, uid, action_domain, action_name,"
        " main_param, params, allowed, results)"
        " VALUES(%d, %d, 'protected_call', %s, %s, '%s', %d, '%s')" % (
        time.time(),
        uid,
        Database.quote(".".join(path), 'text'),
        Database.quote(main_param, 'text'),
        Database.quoteBlob(cPickle.dumps(args)),
        allowed,
        Database.quoteBlob(cPickle.dumps(result))))
    return result


class User:
    """Representation of one user. A user always has a secret key
       and a list of capabilities. Most users have a name, though
       this isn't required. Users may also have login information.

       A User instance may be created in several different ways:
         - From a user ID
         - From a full name
         - From a login name
         - From a secret key
         - If nothing else is specified, this returns the default (system)
           user, which has no name.

       Normally if a matching user can't be found, this throws a NoSuchUser
       exception. If create=True and we're searching by full name, this
       can create a new user if necessary.

       Note that any exception won't be generated until the user is
       first queried (such as via getUid) since a database lookup
       must be made.
       """
    def __init__(self, uid=None, full_name=None, login_name=None, key=None,
                 email=None, create=False):
        self._uid = uid
        self._full_name = full_name
        self._login_name = login_name
        self._key = key
        self._create = create
        self._email = email
        self.newUser = False

        self._validatedUid = None

    def getUid(self):
        """Return, via a Deferred, this user's ID"""
        return Database.pool.runInteraction(self._getUid)

    def _getUid(self, cursor):
        """Return the user ID, given a database cursor. Caches the validated UID"""
        if self._validatedUid is None:
            self._validatedUid = self._uncachedGetUid(cursor)
        return self._validatedUid

    def getCachedUid(self):
        """If we've already looked up this UID before, this will immediately
           return the UID. It returns None instead of a deferred if we don't
           know what our UID is yet.
           """
        return self._validatedUid

    def _uncachedGetUid(self, cursor):
        """Internal function to find a valid UID, without caching"""
        if self._uid is not None:
            return self._validateUid(cursor, self._uid)

        if self._key is not None:
            # Look for a user with the given key
            return self._getUidFromKey(cursor, self._key)

        if self._login_name is not None:
            # Get/create a user with the given login name
            try:
                return self._getUidFromLoginName(cursor, self._login_name)
            except NoSuchUser:
                if self._create:
                    return self._createUser(cursor)
                else:
                    raise

        if self._full_name is not None:
            # Get/create a user with the given full name
            try:
                return self._getUidFromFullName(cursor, self._full_name)
            except NoSuchUser:
                if self._create:
                    return self._createUser(cursor)
                else:
                    raise

        if self._email is not None:
            # Look for a user by email address (can't create one unless a name is given too)
            return self._getUidFromEmail(cursor, self._email)

        # Nothing specified, get/create the default user.
        # Since we should always have a default user, ignore the create flag
        # and always create if necessary.
        try:
            return self._getUidFromFullName(cursor, None)
        except NoSuchUser:
            return self._createUser(cursor)

    def getInfo(self):
        """Return, via a Deferred, a dictionary of information about this user.
           Dict keys match database columns. This includes uid, secret_key,
           active, full_name, email, creation_time, key_atime, login_name,
           login_passwd_md5, login_atime, and login_mtime.
           """
        return Database.pool.runInteraction(self._getInfo)

    def _getInfo(self, cursor):
        uid = self._getUid(cursor)
        cursor.execute("SELECT * FROM users WHERE uid = %d" % uid)
        row = cursor.fetchone()
        d = {}
        for i in xrange(len(row)):
            d[cursor.description[i][0]] = row[i]
        return d

    def saveKey(self, file, *grantCapabilities):
        """Save a key for this user to disk, after optionally
           granting them capabilities if necessary. This is here to
           bootstrap the capability system, since normally creating
           users and granting capabilities requires one to already have
           a powerful user's key.
           """
        return Database.pool.runInteraction(self._saveKey, file, *grantCapabilities)

    def _saveKey(self, cursor, file, *grantCapabilities):
        file = os.path.expanduser(file)
        self._grant(cursor, *grantCapabilities)
        key = self._getInfo(cursor)['secret_key']
        f = open(file, "w")
        os.chmod(file, 0600)
        f.write(key)
        f.close()

    def test(self, *capabilities):
        """Test this user for one or more of the given capabilities.
           Returns true if the user has any of the given capabilities,
           false if they have none, or raises a NoSuchUser exception
           if this isn't a valid user. Returns its result in a Deferred.

           If the capabilities list is empty, this will always return True
           for a valid user.
           """
        return Database.pool.runInteraction(self._test, *capabilities)

    def _test(self, cursor, *capabilities):
        # If the user has been disabled, they have no capabilities
        cursor.execute("SELECT active FROM users WHERE uid = %d" % self._getUid(cursor))
        if not int(cursor.fetchone()[0]):
            return False

        if cursor.execute(self._createTestQuery(self._getUid(cursor), capabilities)):
            # We do have permission, update the key's access time and return
            cursor.execute("UPDATE users SET key_atime = %d WHERE uid = %d" %
                            (time.time(), self._getUid(cursor)))
            return True
        else:
            return False

    def grant(self, *capabilities):
        """Grant all capabilities in the given list, ignoring any the user already has"""
        return Database.pool.runInteraction(self._grant, *capabilities)

    def _grant(self, cursor, *capabilities):
        uid = self._getUid(cursor)
        for capability in capabilities:
            rep = repr(capability)
            cursor.execute("INSERT IGNORE INTO capabilities (uid, cap_md5, cap_repr)"
                           " VALUES(%d, %s, %s)" % (
                uid,
                Database.quote(md5.new(rep).hexdigest(), 'char'),
                Database.quote(rep, 'text')))

    def require(self, *capabilities):
        """Like test(), but in case none of the listed capabilities have been
           granted to this user, raises a SecurityException.
           This is guaranteed to either return None or a Failure, where the
           Failure should be a SecurityException.
           """
        return Database.pool.runInteraction(self._require, *capabilities)

    def _require(self, cursor, *capabilities):
        try:
            if not self._test(cursor, *capabilities):
                raise InsufficientCapabilities("One of the following capabilities are required: " +
                                               repr(capabilities)[1:-1])
        except:
            return failure.Failure()

    def _createTestQuery(self, uid, capabilities):
        """Create an SQL query that returns something nonzero if a uid matches any of
           a list of capabilities. If the capabilities list is empty, this creates a
           query that always has a nonzero result.
           """
        if capabilities:
            return "SELECT 1 FROM capabilities WHERE uid = %d AND (%s) LIMIT 1" % (
                uid,
                " OR ".join(["cap_md5 = " + Database.quote(md5.new(repr(c)).hexdigest(),
                                                           'char') for c in capabilities]),
                )
        else:
            return "SELECT 1"

    def _getUidFromKey(self, cursor, key):
        """Find a user by their key"""
        cursor.execute("SELECT uid FROM users WHERE secret_key = %s" %
                       Database.quote(key.strip(), 'varchar'))
        row = cursor.fetchone()
        if row:
            return int(row[0])
        else:
            raise NoSuchUser("No user found matching the given key")

    def _getUidFromLoginName(self, cursor, name):
        """Find a user by login name"""
        cursor.execute("SELECT uid FROM users WHERE login_name = %s" %
                       Database.quote(name, 'text'))
        row = cursor.fetchone()
        if row:
            return int(row[0])
        else:
            raise NoSuchUser("No such login name: %r" % name)

    def _getUidFromFullName(self, cursor, name):
        """Find a user by full name"""
        if name is None:
            cursor.execute("SELECT uid FROM users WHERE full_name is NULL")
        else:
            cursor.execute("SELECT uid FROM users WHERE full_name = %s" %
                           Database.quote(name, 'text'))
        row = cursor.fetchone()
        if row:
            return int(row[0])
        else:
            raise NoSuchUser("No such name: %r" % name)

    def _getUidFromEmail(self, cursor, name):
        """Find a user by email address"""
        cursor.execute("SELECT uid FROM users WHERE email = %s" %
                       Database.quote(name, 'text'))
        row = cursor.fetchone()
        if row:
            return int(row[0])
        else:
            raise NoSuchUser("No such email address: %r" % name)

    def _createUser(self, cursor):
        """Create a new user, optionally setting the given parameters.
           Returns the new user ID.
           """
        log.msg("Creating new user %r" % self._full_name)
        self.newUser = True
        cursor.execute("INSERT INTO users (secret_key, creation_time, full_name, email, login_name) "
                       "VALUES (%s, %d, %s, %s, %s)" % (
            Database.quote(createRandomKey(), 'varchar'),
            time.time(),
            Database.quote(self._full_name, 'text'),
            Database.quote(self._email, 'text'),
            Database.quote(self._login_name, 'varchar')))
        cursor.execute("SELECT LAST_INSERT_ID()")
        return int(cursor.fetchone()[0])

    def _validateUid(self, cursor, uid):
        """We have a UID. Validate it, returning a valid UID or raising NoSuchUser"""
        cursor.execute("SELECT 1 FROM users WHERE uid = %d" % uid)
        if cursor.fetchone():
            return uid
        else:
            raise NoSuchUser("No such user ID: %d" % uid)

    def getCapabilities(self):
        """Return a list of capability names (strings) that this user has, via a Deferred"""
        return Database.pool.runInteraction(self._getCapabilities)

    def _getCapabilities(self, cursor):
        uid = self._getUid(cursor)
        cursor.execute("SELECT cap_repr FROM capabilities WHERE uid = %d" % uid)
        return [row[0] for row in cursor.fetchall()]

### The End ###
