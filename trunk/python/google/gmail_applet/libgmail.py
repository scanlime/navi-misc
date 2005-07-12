#!/usr/bin/python2.3
#
# libgmail -- Gmail access via Python
#
# Version: 0.0.9 (XX October 2004)
#
# Author: follower@myrealbox.com
#
# License: GPL 2.0
#
# Thanks:
#   * Live HTTP Headers <http://livehttpheaders.mozdev.org/>
#   * Gmail <http://gmail.google.com/>
#   * Google Blogoscoped <http://blog.outer-court.com/>
#   * ClientCookie <http://wwwsearch.sourceforge.net/ClientCookie/>
#     (There when I needed it...)
#   * The *first* big G. :-)
#
# NOTE:
#   You should ensure you are permitted to use this script before using it
#   to access Google's Gmail servers.
#
#
# Gmail Implementation Notes
# ==========================
#
# * Folders contain message threads, not individual messages. At present I
#   do not know any way to list all messages without processing thread list.
#

from constants import *

import os
import re
import urllib
import urllib2
import logging
import mimetypes

from email.MIMEBase import MIMEBase
from email.MIMEText import MIMEText
from email.MIMEMultipart import MIMEMultipart

#logging.getLogger().setLevel(logging.DEBUG)

URL_LOGIN = "https://www.google.com/accounts/ServiceLoginBoxAuth"
URL_GMAIL = "https://gmail.google.com/gmail"

# TODO: Get these on the fly?
STANDARD_FOLDERS = [U_INBOX_SEARCH, U_STARRED_SEARCH,
                    U_ALL_SEARCH, U_DRAFTS_SEARCH,
                    U_SENT_SEARCH, U_SPAM_SEARCH]

# Constants with names not from the Gmail Javascript:
# TODO: Move to `constants.py`?
U_SAVEDRAFT_VIEW = "sd"

D_DRAFTINFO = "di"
# NOTE: All other DI_* field offsets seem to match the MI_* field offsets
DI_BODY = 19

versionWarned = False # If the Javascript version is different have we
                      # warned about it?


RE_SPLIT_PAGE_CONTENT = re.compile("D\((.*?)\);", re.DOTALL)
def _parsePage(pageContent):
    """
    Parse the supplied HTML page and extract useful information from
    the embedded Javascript.
    
    """
    # Note: We use the easiest thing that works here and no longer
    #       extract the Javascript code we want from the page first.
    items = (re.findall(RE_SPLIT_PAGE_CONTENT, pageContent)) 

    # TODO: Check we find something?
    
    itemsDict = {}

    namesFoundTwice = []

    for item in items:
        item = item.strip()[1:-1]
        name, value = (item.split(",", 1) + [""])[:2]

        name = name[1:-1] # Strip leading and trailing single or double quotes.
        
        try:
            # By happy coincidence Gmail's data is stored in a form
            # we can turn into Python data types by simply evaluating it.
            # TODO: Parse this better/safer?
            # TODO: Handle "mb" mail bodies better as they can be anything.
            if value != "": # Empty strings aren't parsed successfully.
                parsedValue = eval(value.replace("\n","").replace(",,",",None,").replace(",,",",None,")) # Yuck! Need two ",," replaces to handle ",,," overlap. TODO: Tidy this up... TODO: It appears there may have been a change in the number & order of at least the CS_* values, investigate.
            else:
                parsedValue = value
        except SyntaxError:
            logging.warning("Could not parse item `%s` as it was `%s`." %
                            (name, value))
        else:
            if itemsDict.has_key(name):
                # This handles the case where a name key is used more than
                # once (e.g. mail items, mail body) and automatically
                # places the values into list.
                # TODO: Check this actually works properly, it's early... :-)
                if (name in namesFoundTwice):
                    itemsDict[name].append(parsedValue)
                else:
                    itemsDict[name] = [itemsDict[name], parsedValue]
                    namesFoundTwice.append(name)
            else:
                itemsDict[name] = parsedValue

    global versionWarned
    if itemsDict[D_VERSION] != js_version and not versionWarned:
        logging.debug("Live Javascript and constants file versions differ.")
        versionWarned = True

    return itemsDict



class CookieJar:
    """
    A rough cookie handler, intended to only refer to one domain.

    Does no expiry or anything like that.

    (The only reason this is here is so I don't have to require
    the `ClientCookie` package.)
    
    """

    def __init__(self):
        """
        """
        self._cookies = {}


    def extractCookies(self, response, nameFilter = None):
        """
        """
        # TODO: Do this all more nicely?
        for cookie in response.headers.getheaders('Set-Cookie'):
            name, value = (cookie.split("=", 1) + [""])[:2]
            logging.debug("Extracted cookie `%s`" % (name))
            if not nameFilter or name in nameFilter:
                self._cookies[name] = value.split(";")[0]
                logging.debug("Stored cookie `%s` value `%s`" %
                              (name, self._cookies[name]))


    def addCookie(self, name, value):
        """
        """
        self._cookies[name] = value


    def setCookies(self, request):
        """
        """
        request.add_header('Cookie',
                           ";".join(["%s=%s" % (k,v)
                                     for k,v in self._cookies.items()]))

        
    
def _buildURL(**kwargs):
    """
    """
    return "%s?%s" % (URL_GMAIL, urllib.urlencode(kwargs))



def _paramsToMime(params, filenames, files):
    """
    """
    mimeMsg = MIMEMultipart("form-data")

    for name, value in params.iteritems():
        mimeItem = MIMEText(value)
        mimeItem.add_header("Content-Disposition", "form-data", name=name)

        # TODO: Handle this better...?
        for hdr in ['Content-Type','MIME-Version','Content-Transfer-Encoding']:
            del mimeItem[hdr]

        mimeMsg.attach(mimeItem)

    if filenames or files:
        filenames = filenames or []
        files = files or []
        for idx, item in enumerate(filenames + files):
            # TODO: This is messy, tidy it...
            if isinstance(item, str):
                # We assume it's a file path...
                filename = item
                contentType = mimetypes.guess_type(filename)[0]
                payload = open(filename, "rb").read()
            else:
                # We assume it's an `email.Message.Message` instance...
                # TODO: Make more use of the pre-encoded information?
                filename = item.get_filename()
                contentType = item.get_content_type()
                payload = item.get_payload(decode=True)
                
            if not contentType:
                contentType = "application/octet-stream"
                
            mimeItem = MIMEBase(*contentType.split("/"))
            mimeItem.add_header("Content-Disposition", "form-data",
                                name="file%s" % idx, filename=filename)
            # TODO: Encode the payload?
            mimeItem.set_payload(payload)

            # TODO: Handle this better...?
            for hdr in ['MIME-Version','Content-Transfer-Encoding']:
                del mimeItem[hdr]

            mimeMsg.attach(mimeItem)

    del mimeMsg['MIME-Version']

    return mimeMsg


class GmailLoginFailure(Exception):
    """
    Raised whenever the login process fails--could be wrong username/password,
    or Gmail service error, for example.
    """
    

class GmailAccount:
    """
    """

    def __init__(self, name = "", pw = "", state = None):
        """
        """
        # TODO: Change how all this is handled?
        if name and pw:
            self.name = name
            self._pw = pw
            self._cookieJar = CookieJar()
        elif state:
            # TODO: Check for stale state cookies?
            self.name, self._cookieJar = state.state
        else:
            raise ValueError("GmailAccount must be instantiated with " \
                             "either GmailSessionState object or name " \
                             "and password.")

        self._cachedQuotaInfo = None
        self._cachedLabelNames = None
        

    def login(self):
        """
        """
        # TODO: Throw exception if we were instantiated with state?
        data = urllib.urlencode({'continue': URL_GMAIL,
                                 'service': 'mail',
                                 'Email': self.name,
                                 'Passwd': self._pw,
                                 'null': 'Sign+in'})
    
        headers = {'Host': 'www.google.com',
                   'User-Agent': 'User-Agent: Mozilla/5.0 (compatible;)'}

        req = urllib2.Request(URL_LOGIN, data=data, headers=headers)
        pageData = self._retrievePage(req)

        # TODO: Tidy this up?
        # This requests the page that provides the required "GV" cookie.
        RE_PAGE_REDIRECT = 'top\.location\W=\W"CheckCookie\?continue=([^"]+)'
        # TODO: Catch more failure exceptions here...?
        try:
            redirectURL = urllib.unquote(re.search(RE_PAGE_REDIRECT,
                                                   pageData).group(1))
        except AttributeError:
            raise GmailLoginFailure
        # We aren't concerned with the actual content of this page,
        # just the cookie that is returned with it.
        pageData = self._retrievePage(redirectURL)
        

    def _retrievePage(self, urlOrRequest):
        """
        """
        if not isinstance(urlOrRequest, urllib2.Request):
            req = urllib2.Request(urlOrRequest)
        else:
            req = urlOrRequest
            
        self._cookieJar.setCookies(req)
        resp = urllib2.urlopen(req)

        pageData = resp.read()

        # Extract cookies here
        self._cookieJar.extractCookies(resp)

        # TODO: Enable logging of page data for debugging purposes?

        return pageData


    def _parsePage(self, urlOrRequest):
        """
        Retrieve & then parse the requested page content.
        
        """
        items = _parsePage(self._retrievePage(urlOrRequest))
        
        # Automatically cache some things like quota usage.
        # TODO: Cache more?
        # TODO: Expire cached values?
        # TODO: Do this better.
        try:
            self._cachedQuotaInfo = items[D_QUOTA]
        except KeyError:
            pass

        try:
            self._cachedLabelNames = [category[CT_NAME]
                                         for category in items[D_CATEGORIES]]
        except KeyError:
            pass
        
        return items


    def _parseSearchResult(self, searchType, start = 0, **kwargs):
        """
        """
        params = {U_SEARCH: searchType,
                  U_START: start,
                  U_VIEW: U_THREADLIST_VIEW,
                  }
        params.update(kwargs)
        
        return self._parsePage(_buildURL(**params))


    def _parseThreadSearch(self, searchType, allPages = False, **kwargs):
        """

        Only works for thread-based results at present. # TODO: Change this?
        """
        start = 0
        threadsInfo = []

        # Option to get *all* threads if multiple pages are used.
        while (start == 0) or (allPages and
                               len(threadsInfo) < threadListSummary[TS_TOTAL]):
            
                items = self._parseSearchResult(searchType, start, **kwargs)

                #TODO: Handle single & zero result case better? Does this work?
                try:
                    threads = items[D_THREAD]
                except KeyError:
                    break
                else:
                    if type(threads[0]) not in [tuple, list]:#TODO:Urgh,change!
                        threadsInfo.append(threads)
                    else:
                        # Note: This also handles when more than one "t"
                        # "DataPack" is on a page.
                        threadsInfo.extend(_splitBunches(threads))

                    # TODO: Check if the total or per-page values have changed?
                    threadListSummary = items[D_THREADLIST_SUMMARY]
                    threadsPerPage = threadListSummary[TS_NUM]

                    start += threadsPerPage

        # TODO: Record whether or not we retrieved all pages..?
        return GmailSearchResult(self, (searchType, kwargs), threadsInfo)


    def _retrieveJavascript(self, version = ""):
        """

        Note: `version` seems to be ignored.
        """
        return self._retrievePage(_buildURL(view = U_PAGE_VIEW,
                                            name = "js",
                                            ver = version))
        
        
    def getMessagesByFolder(self, folderName, allPages = False):
        """

        Folders contain conversation/message threads.

          `folderName` -- As set in Gmail interface.

        Returns a `GmailSearchResult` instance.

        *** TODO: Change all "getMessagesByX" to "getThreadsByX"? ***
        """
        return self._parseThreadSearch(folderName, allPages = allPages)


    def getMessagesByQuery(self, query,  allPages = False):
        """

        Returns a `GmailSearchResult` instance.
        """
        return self._parseThreadSearch(U_QUERY_SEARCH, q = query,
                                       allPages = allPages)

    
    def getQuotaInfo(self, refresh = False):
        """

        Return MB used, Total MB and percentage used.
        """
        # TODO: Change this to a property.
        if not self._cachedQuotaInfo or refresh:
            # TODO: Handle this better...
            self.getMessagesByFolder(U_INBOX_SEARCH)

        return self._cachedQuotaInfo[:3]


    def getLabelNames(self, refresh = False):
        """
        """
        # TODO: Change this to a property?
        if not self._cachedLabelNames or refresh:
            # TODO: Handle this better...
            self.getMessagesByFolder(U_INBOX_SEARCH)

        return self._cachedLabelNames


    def getMessagesByLabel(self, label, allPages = False):
        """
        
        """
        return self._parseThreadSearch(U_CATEGORY_SEARCH,
                                       cat=label, allPages = allPages)


    def getRawMessage(self, msgId):
        """
        """
        return self._retrievePage(
            _buildURL(view=U_ORIGINAL_MESSAGE_VIEW, th=msgId))


    def getUnreadMsgCount(self):
        """
        """
        # TODO: Clean up queries a bit..?
        items = self._parseSearchResult(U_QUERY_SEARCH,
                                        q = "is:" + U_AS_SUBSET_UNREAD)

        return items[D_THREADLIST_SUMMARY][TS_TOTAL_MSGS]


    def _getActionToken(self):
        """
        """
        try:
            at = self._cookieJar._cookies[ACTION_TOKEN_COOKIE]
        except KeyError:
            self.getLabelNames(True) 
            at = self._cookieJar._cookies[ACTION_TOKEN_COOKIE]           

        return at


    def sendMessage(self, msg, asDraft = False, _extraParams = None):
        """

          `msg` -- `GmailComposedMessage` instance.

          `_extraParams` -- Dictionary containing additional parameters
                            to put into POST message. (Not officially
                            for external use, more to make feature
                            additional a little easier to play with.)
        
        Note: Now returns `GmailMessageStub` instance with populated
              `id` (and `_account`) fields on success or None on failure.

        """
        # TODO: Handle drafts separately?
        params = {U_VIEW: [U_SENDMAIL_VIEW, U_SAVEDRAFT_VIEW][asDraft],
                  U_REFERENCED_MSG: "",
                  U_THREAD: "",
                  U_DRAFT_MSG: "",
                  U_COMPOSEID: "1",
                  U_ACTION_TOKEN: self._getActionToken(),
                  U_COMPOSE_TO: msg.to,
                  U_COMPOSE_CC: msg.cc,
                  U_COMPOSE_BCC: msg.bcc,
                  "subject": msg.subject,
                  "msgbody": msg.body,
                  }

        if _extraParams:
            params.update(_extraParams)

        # Amongst other things, I used the following post to work out this:
        # <http://groups.google.com/groups?
        #  selm=mailman.1047080233.20095.python-list%40python.org>
        mimeMessage = _paramsToMime(params, msg.filenames, msg.files)

        #### TODO: Ughh, tidy all this up & do it better...
        ## This horrible mess is here for two main reasons:
        ##  1. The `Content-Type` header (which also contains the boundary
        ##     marker) needs to be extracted from the MIME message so
        ##     we can send it as the request `Content-Type` header instead.
        ##  2. It seems the form submission needs to use "\r\n" for new
        ##     lines instead of the "\n" returned by `as_string()`.
        ##     I tried changing the value of `NL` used by the `Generator` class
        ##     but it didn't work so I'm doing it this way until I figure
        ##     out how to do it properly. Of course, first try, if the payloads
        ##     contained "\n" sequences they got replaced too, which corrupted
        ##     the attachments. I could probably encode the submission,
        ##     which would probably be nicer, but in the meantime I'm kludging
        ##     this workaround that replaces all non-text payloads with a
        ##     marker, changes all "\n" to "\r\n" and finally replaces the
        ##     markers with the original payloads.
        ## Yeah, I know, it's horrible, but hey it works doesn't it? If you've
        ## got a problem with it, fix it yourself & give me the patch!
        ##
        origPayloads = {}
        FMT_MARKER = "&&&&&&%s&&&&&&"

        for i, m in enumerate(mimeMessage.get_payload()):
            if not isinstance(m, MIMEText): #Do we care if we change text ones?
                origPayloads[i] = m.get_payload()
                m.set_payload(FMT_MARKER % i)

        mimeMessage.epilogue = ""
        msgStr = mimeMessage.as_string()
        contentTypeHeader, data = msgStr.split("\n\n", 1)
        contentTypeHeader = contentTypeHeader.split(":", 1)
        data = data.replace("\n", "\r\n")
        for k,v in origPayloads.iteritems():
            data = data.replace(FMT_MARKER % k, v)
        ####
        
        req = urllib2.Request(_buildURL(search = "undefined"), data = data)
        req.add_header(*contentTypeHeader)

        items = self._parsePage(req)

        # TODO: Check composeid?
        result = None
        resultInfo = items[D_SENDMAIL_RESULT]
        
        if resultInfo[SM_SUCCESS]:
            result = GmailMessageStub(id = resultInfo[SM_NEWTHREADID],
                                      _account = self)
            
        return result


    def trashMessage(self, msg):
        """
        """
        # TODO: Decide if we should make this a method of `GmailMessage`.
        # TODO: Should we check we have been given a `GmailMessage` instance?
        params = {
            U_ACTION: U_DELETEMESSAGE_ACTION,
            U_ACTION_MESSAGE: msg.id,
            U_ACTION_TOKEN: self._getActionToken(),
            }

        items = self._parsePage(_buildURL(**params))

        # TODO: Mark as trashed on success?
        return (items[D_ACTION_RESULT][AR_SUCCESS] == 1)


    def _doThreadAction(self, actionId, thread):
        """
        """
        # TODO: Decide if we should make this a method of `GmailThread`.
        # TODO: Should we check we have been given a `GmailThread` instance?
        params = {
            U_SEARCH: U_ALL_SEARCH, #TODO:Check this search value always works.
            U_VIEW: U_UPDATE_VIEW,
            U_ACTION: actionId,
            U_ACTION_THREAD: thread.id,
            U_ACTION_TOKEN: self._getActionToken(),
            }

        items = self._parsePage(_buildURL(**params))

        return (items[D_ACTION_RESULT][AR_SUCCESS] == 1)
        
        
    def trashThread(self, thread):
        """
        """
        # TODO: Decide if we should make this a method of `GmailThread`.
        # TODO: Should we check we have been given a `GmailThread` instance?

        result = self._doThreadAction(U_MARKTRASH_ACTION, thread)
        
        # TODO: Mark as trashed on success?
        return result


    def _createUpdateRequest(self, actionId): #extraData):
        """
        Helper method to create a Request instance for an update (view)
        action.

        Returns populated `Request` instance.
        """
        params = {
            U_VIEW: U_UPDATE_VIEW,
            }

        data = {
            U_ACTION: actionId,
            U_ACTION_TOKEN: self._getActionToken(),
            }

        #data.update(extraData)

        req = urllib2.Request(_buildURL(**params),
                              data = urllib.urlencode(data))

        return req


    # TODO: Extract additional common code from handling of labels?
    def createLabel(self, labelName):
        """
        """
        req = self._createUpdateRequest(U_CREATECATEGORY_ACTION + labelName)

        # Note: Label name cache is updated by this call as well. (Handy!)
        items = self._parsePage(req)

        return (items[D_ACTION_RESULT][AR_SUCCESS] == 1)


    def deleteLabel(self, labelName):
        """
        """
        # TODO: Check labelName exits?
        req = self._createUpdateRequest(U_DELETECATEGORY_ACTION + labelName)

        # Note: Label name cache is updated by this call as well. (Handy!)
        items = self._parsePage(req)

        return (items[D_ACTION_RESULT][AR_SUCCESS] == 1)


    def renameLabel(self, oldLabelName, newLabelName):
        """
        """
        # TODO: Check oldLabelName exits?
        req = self._createUpdateRequest("%s%s^%s" % (U_RENAMECATEGORY_ACTION,
                                                   oldLabelName, newLabelName))

        # Note: Label name cache is updated by this call as well. (Handy!)
        items = self._parsePage(req)

        return (items[D_ACTION_RESULT][AR_SUCCESS] == 1)


    def storeFile(self, filename, label = None):
        """
        """
        # TODO: Handle files larger than single attachment size.
        # TODO: Allow file data objects to be supplied?
        FILE_STORE_VERSION = "FSV_01"
        FILE_STORE_SUBJECT_TEMPLATE = "%s %s" % (FILE_STORE_VERSION, "%s")

        subject = FILE_STORE_SUBJECT_TEMPLATE % os.path.basename(filename)

        msg = GmailComposedMessage(to="", subject=subject, body="",
                                   filenames=[filename])

        draftMsg = self.sendMessage(msg, asDraft = True)

        if draftMsg and label:
            draftMsg.addLabel(label)

        return draftMsg

        
        
def _splitBunches(infoItems):
    """

    Utility to help make it easy to iterate over each item separately,
    even if they were bunched on the page.
    """
    result= []

    # TODO: Decide if this is the best approach.
    for group in infoItems:
        if type(group) == tuple:
            result.extend(group)
        else:
            result.append(group)

    return result
            
        

class GmailSearchResult:
    """
    """

    def __init__(self, account, search, threadsInfo):
        """

        `threadsInfo` -- As returned from Gmail but unbunched.
        """
        self._account = account
        self.search = search # TODO: Turn into object + format nicely.

        self._threads = [GmailThread(self, thread)
                         for thread in threadsInfo]


    def __iter__(self):
        """
        """
        return iter(self._threads)


    def __len__(self):
        """
        """
        return len(self._threads)


from cPickle import load, dump

class GmailSessionState:
    """
    """

    def __init__(self, account = None, filename = ""):
        """
        """
        if account:
            self.state = (account.name, account._cookieJar)
        elif filename:
            self.state = load(open(filename, "rb"))
        else:
            raise ValueError("GmailSessionState must be instantiated with " \
                             "either GmailAccount object or filename.")


    def save(self, filename):
        """
        """
        dump(self.state, open(filename, "wb"), -1)


class _LabelHandlerMixin(object):
    """

    Note: Because a message id can be used as a thread id this works for
          messages as well as threads.
    """

    def addLabel(self, labelName):
        """
        """
        # Note: It appears this also automatically creates new labels.
        result = self._account._doThreadAction(U_ADDCATEGORY_ACTION+labelName,
                                               self)
        # TODO: Update list of attached labels?
        return result


    def removeLabel(self, labelName):
        """
        """
        # TODO: Check label is already attached?
        # Note: An error is not generated if the label is not already attached.
        result = \
               self._account._doThreadAction(U_REMOVECATEGORY_ACTION+labelName,
                                             self)
        # TODO: Update list of attached labels?
        return result
    


class GmailThread(_LabelHandlerMixin):
    """



    Note: As far as I can tell, the "canonical" thread id is always the same
          as the id of the last message in the thread. But it appears that
          the id of any message in the thread can be used to retrieve
          the thread information.
    
    """

    def __init__(self, parent, threadInfo):
        """
        """
        # TODO Handle this better?
        self._parent = parent
        self._account = self._parent._account
        
        self.id = threadInfo[T_THREADID] # TODO: Change when canonical updated?
        self.subject = threadInfo[T_SUBJECT_HTML]

        self.snippet = threadInfo[T_SNIPPET_HTML]
        #self.extraSummary = threadInfo[T_EXTRA_SNIPPET] #TODO: What is this?

        # TODO: Store other info?
        # Extract number of messages in thread/conversation.

        self._authors = threadInfo[T_AUTHORS_HTML]

        try:
            # TODO: Find out if this information can be found another way...
            #       (Without another page request.)
            self._length = int(re.search("\((\d+?)\)\Z",
                                         self._authors).group(1))
        except AttributeError:
            # If there's no message count then the thread only has one message.
            self._length = 1

        # TODO: Store information known about the last message  (e.g. id)?
        self._messages = []

        
    def __len__(self):
        """
        """
        return self._length


    def __iter__(self):
        """
        """
        if not self._messages:
            self._messages = self._getMessages(self)
            
        return iter(self._messages)


    def _getMessages(self, thread):
        """
        """
        # TODO: Do this better.
        # TODO: Specify the query folder using our specific search?
        items = self._account._parseSearchResult(U_QUERY_SEARCH,
                                                 view = U_CONVERSATION_VIEW,
                                                 th = thread.id,
                                                 q = "in:anywhere")

        result = []
        # TODO: Handle this better?
        # Note: This handles both draft & non-draft messages in a thread...
        for key, isDraft in [(D_MSGINFO, False), (D_DRAFTINFO, True)]:
            try:
                msgsInfo = items[key]
            except KeyError:
                # No messages of this type (e.g. draft or non-draft)
                continue
            else:
                # TODO: Handle special case of only 1 message in thread better?
                if type(msgsInfo) != type([]):
                    msgsInfo = [msgsInfo]

                result += [GmailMessage(thread, msg, isDraft = isDraft)
                           for msg in msgsInfo]

        return result


    # TODO: Add property to retrieve list of labels for this message.
    


class GmailMessageStub(_LabelHandlerMixin):
    """

    Intended to be used where not all message information is known/required.

    NOTE: This may go away.
    """

    # TODO: Provide way to convert this to a full `GmailMessage` instance
    #       or allow `GmailMessage` to be created without all info?

    def __init__(self, id = None, _account = None):
        """
        """
        self.id = id
        self._account = _account
    

        
class GmailMessage(object):
    """
    """
    
    def __init__(self, parent, msgData, isDraft = False):
        """

        Note: `msgData` can be from either D_MSGINFO or D_DRAFTINFO.
        """
        # TODO: Automatically detect if it's a draft or not?
        # TODO Handle this better?
        self._parent = parent
        self._account = self._parent._account
        
        self.id = msgData[MI_MSGID]
        self.number = msgData[MI_NUM]
        self.subject = msgData[MI_SUBJECT]

        self.attachments = [GmailAttachment(self, attachmentInfo)
                            for attachmentInfo in msgData[MI_ATTACHINFO]]

        # TODO: Populate additional fields & cache...(?)

        # TODO: Handle body differently if it's from a draft?
        self.isDraft = isDraft
        
        self._source = None


    def _getSource(self):
        """
        """
        if not self._source:
            # TODO: Do this more nicely...?
            # TODO: Strip initial white space & fix up last line ending
            #       to make it legal as per RFC?
            self._source = self._account.getRawMessage(self.id)

        return self._source

    source = property(_getSource, doc = "")
        


class GmailAttachment:
    """
    """

    def __init__(self, parent, attachmentInfo):
        """
        """
        # TODO Handle this better?
        self._parent = parent
        self._account = self._parent._account

        self.id = attachmentInfo[A_ID]
        self.filename = attachmentInfo[A_FILENAME]
        self.mimetype = attachmentInfo[A_MIMETYPE]
        self.filesize = attachmentInfo[A_FILESIZE]

        self._content = None


    def _getContent(self):
        """
        """
        if not self._content:
            # TODO: Do this a more nicely...?
            self._content = self._account._retrievePage(
                _buildURL(view=U_ATTACHMENT_VIEW, disp="attd",
                          attid=self.id, th=self._parent._parent.id))
            
        return self._content

    content = property(_getContent, doc = "")


    def _getFullId(self):
        """

        Returns the "full path"/"full id" of the attachment. (Used
        to refer to the file when forwarding.)

        The id is of the form: "<thread_id>_<msg_id>_<attachment_id>"
        
        """
        return "%s_%s_%s" % (self._parent._parent.id,
                             self._parent.id,
                             self.id)

    _fullId = property(_getFullId, doc = "")



class GmailComposedMessage:
    """
    """

    def __init__(self, to, subject, body, cc = None, bcc = None,
                 filenames = None, files = None):
        """

          `filenames` - list of the file paths of the files to attach.
          `files` - list of objects implementing sub-set of
                    `email.Message.Message` interface (`get_filename`,
                    `get_content_type`, `get_payload`). This is to
                    allow use of payloads from Message instances.
                    TODO: Change this to be simpler class we define ourselves?
        """
        self.to = to
        self.subject = subject
        self.body = body
        self.cc = cc
        self.bcc = bcc
        self.filenames = filenames
        self.files = files



if __name__ == "__main__":
    import sys
    from getpass import getpass

    try:
        name = sys.argv[1]
    except IndexError:
        name = raw_input("Gmail account name: ")
        
    pw = getpass("Password: ")

    ga = GmailAccount(name, pw)

    print "\nPlease wait, logging in..."

    try:
        ga.login()
    except GmailLoginFailure:
        print "\nLogin failed. (Wrong username/password?)"
    else:
        print "Login successful.\n"

        # TODO: Use properties instead?
        quotaInfo = ga.getQuotaInfo()
        quotaMbUsed = quotaInfo[QU_SPACEUSED]
        quotaMbTotal = quotaInfo[QU_QUOTA]
        quotaPercent = quotaInfo[QU_PERCENT]
        print "%s of %s used. (%s)\n" % (quotaMbUsed, quotaMbTotal, quotaPercent)

        searches = STANDARD_FOLDERS + ga.getLabelNames()

        while 1:
            try:
                print "Select folder or label to list: (Ctrl-C to exit)"
                for optionId, optionName in enumerate(searches):
                    print "  %d. %s" % (optionId, optionName)

                name = searches[int(raw_input("Choice: "))]

                if name in STANDARD_FOLDERS:
                    result = ga.getMessagesByFolder(name, True)
                else:
                    result = ga.getMessagesByLabel(name, True)

                print
                if len(result):
                    for thread in result:
                        print
                        print thread.id, len(thread), thread.subject
                        for msg in thread:
                            print "  ", msg.id, msg.number, msg.subject
                            #print msg.source
                else:
                    print "No threads found in `%s`." % name

                print
            except KeyboardInterrupt:
                break
            
    print "\n\nDone."
