### Auto-generated at Wed, 04 Jun 2003 16:12:34 +0000 from idl/rfc1094.x
import rpchelp
NFS_OK = 0
NFSERR_PERM = 1
NFSERR_NOENT = 2
NFSERR_IO = 5
NFSERR_NXIO = 6
NFSERR_ACCES = 13
NFSERR_EXIST = 17
NFSERR_NODEV = 19
NFSERR_NOTDIR = 20
NFSERR_ISDIR = 21
NFSERR_FBIG = 27
NFSERR_NOSPC = 28
NFSERR_ROFS = 30
NFSERR_NAMETOOLONG = 63
NFSERR_NOTEMPTY = 66
NFSERR_DQUOT = 69
NFSERR_STALE = 70
NFSERR_WFLUSH = 99
NFNON = 0
NFREG = 1
NFDIR = 2
NFBLK = 3
NFCHR = 4
NFLNK = 5
MAXDATA = 8192
MAXPATHLEN = 1024
MAXNAMLEN = 255
COOKIESIZE = 4
FHSIZE = 32
nfsdata = rpchelp.opaque (rpchelp.var, MAXDATA)
stat = rpchelp.r_int
ftype = rpchelp.r_int
fhandle = rpchelp.opaque (rpchelp.fixed, FHSIZE)
timeval = rpchelp.struct ('timeval', [('seconds',rpchelp.r_uint),('useconds',rpchelp.r_uint)])
fattr = rpchelp.struct ('fattr', [('type',ftype),('mode',rpchelp.r_uint),('nlink',rpchelp.r_uint),('uid',rpchelp.r_uint),('gid',rpchelp.r_uint),('size',rpchelp.r_uint),('blocksize',rpchelp.r_uint),('rdev',rpchelp.r_uint),('blocks',rpchelp.r_uint),('fsid',rpchelp.r_uint),('fileid',rpchelp.r_uint),('atime',timeval),('mtime',timeval),('ctime',timeval)])
sattr = rpchelp.struct ('sattr', [('mode',rpchelp.r_uint),('uid',rpchelp.r_uint),('gid',rpchelp.r_uint),('size',rpchelp.r_uint),('atime',timeval),('mtime',timeval)])
filename = rpchelp.string (rpchelp.var, MAXNAMLEN)
path = rpchelp.string (rpchelp.var, MAXPATHLEN)
attrstat = rpchelp.union ('attrstat', stat, 'status', {NFS_OK : fattr, None : rpchelp.r_void})
diropargs = rpchelp.struct ('diropargs', [('dir',fhandle),('name',filename)])
diropres = rpchelp.union ('diropres', stat, 'status', {NFS_OK : rpchelp.struct ('diropok', [('file',fhandle),('attributes',fattr)]), None : rpchelp.r_void})
statfsres = rpchelp.union ('statfsres', stat, 'status', {NFS_OK : rpchelp.struct ('info', [('tsize',rpchelp.r_uint),('bsize',rpchelp.r_uint),('blocks',rpchelp.r_uint),('bfree',rpchelp.r_uint),('bavail',rpchelp.r_uint)]), None : rpchelp.r_void})
nfscookie = rpchelp.opaque (rpchelp.fixed, COOKIESIZE)
readdirargs = rpchelp.struct ('readdirargs', [('dir',fhandle),('cookie',nfscookie),('count',rpchelp.r_uint)])
entry = rpchelp.linked_list ('entry', 3, [('fileid',rpchelp.r_uint),('name',filename),('cookie',nfscookie),('nextentry',rpchelp.opt_data (lambda : entry))])
readdirres = rpchelp.union ('readdirres', stat, 'status', {NFS_OK : rpchelp.struct ('readdirok', [('entries',rpchelp.opt_data (lambda : entry)),('eof',rpchelp.r_bool)]), None : rpchelp.r_void})
symlinkargs = rpchelp.struct ('symlinkargs', [('from_',diropargs),('to',path),('attributes',sattr)])
linkargs = rpchelp.struct ('linkargs', [('from_',fhandle),('to',diropargs)])
renameargs = rpchelp.struct ('renameargs', [('from_',diropargs),('to',diropargs)])
createargs = rpchelp.struct ('createargs', [('where',diropargs),('attributes',sattr)])
writeargs = rpchelp.struct ('writeargs', [('file',fhandle),('beginoffset',rpchelp.r_uint),('offset',rpchelp.r_uint),('totalcount',rpchelp.r_uint),('data',nfsdata)])
readargs = rpchelp.struct ('readargs', [('file',fhandle),('offset',rpchelp.r_uint),('count',rpchelp.r_uint),('totalcount',rpchelp.r_uint)])
attrdat = rpchelp.struct ('attrdat', [('attributes',fattr),('data',nfsdata)])
readres = rpchelp.union ('readres', stat, 'status', {NFS_OK : attrdat, None : rpchelp.r_void})
readlinkres = rpchelp.union ('readlinkres', stat, 'status', {NFS_OK : path, None : rpchelp.r_void})
sattrargs = rpchelp.struct ('sattrargs', [('file',fhandle),('attributes',sattr)])
class NFS_PROGRAM_2(rpchelp.Server):
	prog = 100003
	vers = 2
	procs = {0 : rpchelp.Proc ('NFSPROC_NULL', rpchelp.r_void, [rpchelp.r_void]),
	1 : rpchelp.Proc ('NFSPROC_GETATTR', attrstat, [fhandle]),
	2 : rpchelp.Proc ('NFSPROC_SETATTR', attrstat, [sattrargs]),
	3 : rpchelp.Proc ('NFSPROC_ROOT', rpchelp.r_void, [rpchelp.r_void]),
	4 : rpchelp.Proc ('NFSPROC_LOOKUP', diropres, [diropargs]),
	5 : rpchelp.Proc ('NFSPROC_READLINK', readlinkres, [fhandle]),
	6 : rpchelp.Proc ('NFSPROC_READ', readres, [readargs]),
	7 : rpchelp.Proc ('NFSPROC_WRITECACHE', rpchelp.r_void, [rpchelp.r_void]),
	8 : rpchelp.Proc ('NFSPROC_WRITE', attrstat, [writeargs]),
	9 : rpchelp.Proc ('NFSPROC_CREATE', diropres, [createargs]),
	10 : rpchelp.Proc ('NFSPROC_REMOVE', stat, [diropargs]),
	11 : rpchelp.Proc ('NFSPROC_RENAME', stat, [renameargs]),
	12 : rpchelp.Proc ('NFSPROC_LINK', stat, [linkargs]),
	13 : rpchelp.Proc ('NFSPROC_SYMLINK', stat, [symlinkargs]),
	14 : rpchelp.Proc ('NFSPROC_MKDIR', diropres, [createargs]),
	15 : rpchelp.Proc ('NFSPROC_RMDIR', stat, [diropargs]),
	16 : rpchelp.Proc ('NFSPROC_READDIR', readdirres, [readdirargs]),
	17 : rpchelp.Proc ('NFSPROC_STATFS', statfsres, [fhandle])}

MNTPATHLEN = 1024
dirpath = rpchelp.string (rpchelp.var, MNTPATHLEN)
fhstatus = rpchelp.union ('fhstatus', rpchelp.r_uint, 'status', {0 : fhandle, None : rpchelp.r_void})
mountlist = rpchelp.r_int
exportlist = rpchelp.r_int
class MOUNTPROG_1(rpchelp.Server):
	prog = 100005
	vers = 1
	procs = {0 : rpchelp.Proc ('MOUNTPROC_NULL', rpchelp.r_void, [rpchelp.r_void]),
	1 : rpchelp.Proc ('MOUNTPROC_MNT', fhstatus, [dirpath]),
	2 : rpchelp.Proc ('MOUNTPROC_DUMP', mountlist, [rpchelp.r_void]),
	3 : rpchelp.Proc ('MOUNTPROC_UMNT', rpchelp.r_void, [dirpath]),
	4 : rpchelp.Proc ('MOUNTPROC_UMNTALL', rpchelp.r_void, [rpchelp.r_void]),
	5 : rpchelp.Proc ('MOUNTPROC_EXPORT', exportlist, [rpchelp.r_void])}

