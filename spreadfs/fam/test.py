#!/usr/bin/env python

#
# $Id: test.py,v 1.7 2003/01/29 21:39:08 gurubert Exp $
#

import os, sys

if os.name != 'posix':
    print 'I am sorry, but this script will run only on POSIX platforms.'
    sys.exit(1)

import _fam, time, signal, getopt, errno, select

# hash of filename => TestRequest
requests = {}

showreqid = None
suspend = None
cont = None
intr = None
usr1 = None
usr2 = None

class TestRequest:
    def __init__(self, userData, isDir):
        self.fr = None
        self.userData = userData
        self.isDir = isDir

def handle_stop(signum, frame):
    global suspend
    
    print 'Suspended!'
    suspend = 1

def handle_cont(signum, frame):
    global cont
    
    print 'Resumed!'
    signal.signal(signal.SIGCONT, handle_cont)
    cont = 1

def handle_int(signum, frame):
    global intr
    
    print 'Interupted!'
    signal.signal(signal.SIGINT, handle_int)
    intr = 1

def handle_usr1(signum, frame):
    global usr1
    
    print 'Got USR1!'
    signal.signal(signal.SIGUSR1, handle_usr1)
    usr1 = 1

def handle_usr2(signum, frame):
    global usr2
    
    print 'Got USR2!'
    signal.signal(signal.SIGUSR2, handle_usr2)
    usr2 = 1

def sendRequests():
    for path in requests.keys():
        if requests[path].isDir:
            requests[path].fr = fc.monitorDirectory(path, requests[path].userData)
        else:
            requests[path].fr = fc.monitorFile(path, requests[path].userData)
        if showreqid:
            print 'sent req %d' % requests[path].fr.requestID(),
        if requests[path].isDir:
            print 'FAMMonitorDirectory("%s")' % path
        else:
            print 'FAMMonitorFile("%s")' % path

def processDirEvents(fe):
    if fe.userData:
        print fe.userData,
    if showreqid:
        print 'req %d' % fe.requestID,
    print fe.filename, fe.code2str()

if len(sys.argv) < 2:
    print 'usage: %s [-r] [-f <filename>] [-d <dirname>]' % sys.argv[0]
    sys.exit(1)

fc = _fam.open()

optlist, args = getopt.getopt(sys.argv[1:], 'f:d:r')

for arg, value in optlist:
    if arg == '-r':
        showreqid = 1

for arg, filename in optlist:
    if arg == '-f':
        requests[filename] = TestRequest('FILE %s: ' % filename, None)
    if arg == '-d':
        requests[filename] = TestRequest('DIR %s: ' % filename, 1)
        
signal.signal(signal.SIGTSTP, handle_stop)
signal.signal(signal.SIGCONT, handle_cont)
signal.signal(signal.SIGINT, handle_int)
signal.signal(signal.SIGUSR1, handle_usr1)
signal.signal(signal.SIGUSR2, handle_usr2)

sendRequests()

while 1:
    if suspend:
        for request in requests.values():
            request.fr.suspendMonitor()
            print 'Suspended monitoring of request %i' % request.fr.requestID()
        suspend = None
        signal.signal(signal.SIGTSTP, handle_stop)
    if cont:
        for request in requests.values():
            request.fr.resumeMonitor()
            print 'Resumed monitoring of request %i' % request.fr.requestID()
        cont = None
    if intr:
        # The dealloc methods of all objects should handle
        # FAMCancelMonitor and FAMClose
        sys.exit(0)
    if usr1:
        # Cancel all requests, close the connection, and reopen it.
        # This makes sure long-lived clients can connect, monitor, and
        # disconnect repeatedly without leaking.
        usr1 = None
        sleeptime = 1
        for request in requests.values():
            print 'Cancelling monitoring of request %i' % request.fr.requestID()
            request.fr.cancelMonitor()
        fc.close()
        print 'Closed connection, sleeping %d...' % sleeptime
        time.sleep(sleeptime)
        fc = _fam.open()
        sendRequests()
    if usr2:
        # Clean things up like a well-behaved client and exit.
        for request in requests.values():
            print 'Cancelling monitoring of request %i' % request.fr.requestID()
            request.fr.cancelMonitor()
        fc.close()
        print 'Closed connection'
        sys.exit(0)
    try:
        ri, ro, re = select.select([fc], [], [])
    except select.error, er:
        errnumber, strerr = er
        if errnumber == errno.EINTR:
            continue
        else:
            print strerr
            sys.exit(1)
    while fc.pending():
        fe = fc.nextEvent()
        processDirEvents(fe)
