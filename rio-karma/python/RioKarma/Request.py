""" RioKarma.Request

This module holds all implementations of the actual requests used
by the Pearl protocol.

"""
#
# Copyright (C) 2005 Micah Dowty <micah@navi.cx>
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

import struct, md5
from RioKarma import Pearl, Property


class GetProtocolVersion(Pearl.StructRequest):
    """Return the major and minor version of this device's protocol"""
    id = 0
    parameters = (0,0)
    requestFormat = '<II'
    responseFormat = '<II'


class GetAuthenticationSalt(Pearl.StructRequest):
    """Returns a 16-byte random string that is to be prepended to the password when hashed"""
    id = 3
    responseFormat = '16s'


class Authenticate(Pearl.StructRequest):
    """The MD5 hash of the salt+password is sent. On success, returns a list of access rights
       as a tuple of strings containing 'read' and/or 'write'. Raises an AuthenticationError
       if the password was incorrect.
       """
    id = 4
    requestFormat = '16s'
    responseFormat = '<II'

    def __init__(self, salt, password):
        Pearl.StructRequest.__init__(self)
        self.parameters = (md5.md5(salt + password).digest(),)

    def receivedResponse(self, source, status, accessRights):
        self.decodeStatus(status)
        if accessRights == 0:
            self.result.callback(('read',))
        elif accessRights == 1:
            self.result.callback(('read', 'write'))
        else:
            raise ProtocolError("Unexpected access rights code: %d" % accessRights)


class GetDeviceDetails(Pearl.StructRequest):
    """Returns the device's name, version, and number of storage devices"""
    id = 5
    responseFormat = '<32s32sI'

    def receivedResponse(self, source, name, version, numStorageDevices):
        """Trim NULs from the received strings"""
        self.result.callback(( name.replace("\x00", ""),
                               version.replace("\x00", ""),
                               numStorageDevices ))


class GetStorageDetails(Pearl.StructRequest):
    """For any storage device, returns a dictionary with information on its status.
       Note that nothing except totalSpace and freeSpace is implemented on current firmware.
       """
    id = 6
    requestFormat = '<I'
    responseFormat = '<IIQQI'

    def __init__(self, storageId=0):
        self.parameters = (storageId,)
        Pearl.StructRequest.__init__(self)

    def receivedResponse(self, source, status, *args):
        self.decodeStatus(status)
        argNames = ('numFiles', 'totalSpace', 'freeSpace', 'highestFileID')
        results = {}
        for i in xrange(len(argNames)):
            results[argNames[i]] = args[i]
        self.result.callback(results)


class GetDeviceSettings(Pearl.StructRequest):
    """Return a dictionary with the device's settings"""
    id = 7
    responseFormat = '<I'

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.properties = Property.PropertyFileWriter({})

        self.reader = Pearl.AlignedStringReader()
        self.stateTransition(self.state_readSettings, source)

    def state_readSettings(self, fileObj):
        if not self.reader.next(fileObj, self.properties):
            self.result.callback(self.properties.db)


class RequestIOLock(Pearl.StructRequest):
    """Request an I/O lock. The lock type must be 'read' or 'write'.
       You must have at least read lock to retrieve information from the audio database,
       and at least a write lock to upload new data.
       """
    id = 9
    requestFormat = '<I'
    responseFormat = '<I'
    lockTypes = {
        'read': 0,
        'write': 1,
        }

    def __init__(self, lockType):
        try:
            self.parameters = (self.lockTypes[lockType],)
        except KeyError:
            raise ProtocolError("Unknown lock type %r" % lockType)
        Pearl.StructRequest.__init__(self)

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.result.callback(None)


class ReleaseIOLock(Pearl.StructRequest):
    """Release any I/O locks that are currently held"""
    id = 10
    responseFormat = '<I'

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.result.callback(None)


class WriteFileChunk(Pearl.StructRequest):
    """Write a block of data to a file, identified by its number.
       The offset and size are 64-bit.
       """
    id = 12
    requestFormat = '<QQII'
    responseFormat = '<I'

    def __init__(self, fileID, offset, size, dataSource, storageID=0):
        self.dataSource = dataSource
        self.size = size
        self.parameters = (offset, size, fileID, storageID)
        Pearl.StructRequest.__init__(self)

    def __repr__(self):
        return "<WriteFileChunk: 0x%02X bytes at 0x%02X>" % (
            self.parameters[1], self.parameters[0])

    def sendTo(self, fileObj):
        Pearl.StructRequest.sendTo(self, fileObj)
        Pearl.AlignedBlockWriter(self.size).next(self.dataSource, fileObj)

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.result.callback(None)


class GetAllFileDetails(Pearl.StructRequest):
    """Read details for all files, sending each file properties
       dictionary to a provided callback.
       """
    id = 13
    responseFormat = '<I'

    def __init__(self, callback):
        self.callback = callback
        Pearl.StructRequest.__init__(self)

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.fileDatabase = Property.FileDatabaseWriter(self.callback)

        self.reader = Pearl.AlignedStringReader()
        self.stateTransition(self.state_readFiles, source)

    def state_readFiles(self, fileObj):
        if not self.reader.next(fileObj, self.fileDatabase):
            self.result.callback(None)


class GetFileDetails(Pearl.StructRequest):
    """Reads metadata for one file, by ID. Optionally a user-defined
       dictionary-like object may be provided as storage.
       """
    id = 14
    requestFormat = '<I'
    responseFormat = '<I'

    def __init__(self, fileID, storage=None):
        if storage is None:
            storage = {}
        self.storage = storage

        self.parameters = (fileID,)
        Pearl.StructRequest.__init__(self)

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.properties = Property.PropertyFileWriter(self.storage)

        self.reader = Pearl.AlignedStringReader()
        self.stateTransition(self.state_readDetails, source)

    def state_readDetails(self, fileObj):
        if not self.reader.next(fileObj, self.properties):
            self.result.callback(self.storage)


class UpdateFileDetails(Pearl.StructRequest):
    """Update metadata for one file, by ID. The metadata is specified
       as a dictionary-like object.
       """
    id = 15
    requestFormat = '<I'
    responseFormat = '<I'

    def __init__(self, fileId, data):
        self.parameters = (fileId,)
        self.properties = Property.PropertyFileReader(data)
        Pearl.StructRequest.__init__(self)

    def receivedResponse(self, source, status):
        self.decodeStatus(status)
        self.result.callback(None)

    def sendTo(self, fileObj):
        Pearl.StructRequest.sendTo(self, fileObj)
        Pearl.AlignedStringWriter().next(self.properties, fileObj)


class ReadFileChunk(Pearl.StructRequest):
    """Read a block of data from a file, identified by its number.
       This supports 64-bit offsets and lengths. This writes
       all received data to the 'destination' file-like object,
       then returns, via a deferred, the number of bytes
       actually read.
       """
    id = 16
    requestFormat = '<QQI'
    responseFormat = '<QI'

    def __init__(self, fileID, offset, size, destination):
        self.destination = destination
        self.parameters = (offset, size, fileID)
        Pearl.StructRequest.__init__(self)

    def __repr__(self):
        return "<ReadFileChunk: 0x%02X bytes at 0x%02X>" % (
            self.parameters[1], self.parameters[0])

    def receivedResponse(self, source, size, status):
        self.decodeStatus(status)
        self.reader = Pearl.AlignedBlockReader(size)
        self.stateTransition(self.state_readChunk, source)

    def state_readChunk(self, fileObj):
        """Wait for the bulk of the file to transfer"""
        if not self.reader.next(fileObj, self.destination):
            self.stateTransition(self.state_finalStatus, fileObj)

    def state_finalStatus(self, fileObj):
        """Wait for the final status code to arrive, after the file content"""
        if self.fillResponseBuffer(fileObj, 4):
            status = struct.unpack("<I", self.responseBuffer)[0]
            self.responseBuffer = ''
            self.decodeStatus(status)
            self.result.callback(self.reader.transferred)


class DeleteFile(Pearl.StructRequest):
    """Delete a file from storage, given its ID"""
    id = 17
    requestFormat = '<I'

    def __init__(self, fileId):
        self.parameters = (fileId,)
        Pearl.StructRequest.__init__(self)


class Hangup(Pearl.StructRequest):
    """Indicates that the conversation is over"""
    id = 19
    responseFormat = '<I'

    def receivedResponse(self, source, status):
        self.decodeStatus(status)

### The End ###
