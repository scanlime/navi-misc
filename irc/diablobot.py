#! /usr/bin/env python

# Configgy-thingies
pickleFile = "diablo.pickle"
nickname = "Diablo-Bot"
port = 6667
logFile = "diablo.log"

server = "irc.freenode.net"
channels = ("#commits",)

import string, random, pickle, getopt, sys, re, time
from ircbot import SingleServerIRCBot
from irclib import nm_to_n, irc_lower


class ArtificialStupidity:
    def __init__(self):
        self.wordPairs = {}

    def learn(self,sentence):
        # Tokenize the sentence into words, and add each adjacent pair to the database
        words = sentence.split()
        if len(words) < 1:
            return
        self.addPair(None, words[0])
        for i in range(0, len(words)-1):
            self.addPair(words[i], words[i+1])
        self.addPair(words[len(words)-1], None)

    def addPair(self, one, two):
        # Increase the probability of a pair of words being emitted
        if not self.wordPairs.has_key(one):
            self.wordPairs[one] = {}
        if not self.wordPairs[one].has_key(two):
            self.wordPairs[one][two] = 0
        self.wordPairs[one][two] += 1

    def response(self, word=None, sentence=''):
        # Walk the graph starting with the given word, forming a sentence
        if word:
            sentence += word + ' '
        try:
            while True:
                # Form a weighted list of next words
                # FIXME: this is terribly inefficient
                weightedList = []
                hash = self.wordPairs[word]
                for word in hash.keys():
                    for i in range(0, hash[word]):
                        weightedList.append(word)
                word = random.choice(weightedList)
                if not word:
                    break
                sentence += word + ' '
        except KeyError:
            pass
        return sentence


class Bot(SingleServerIRCBot):
    def __init__(self, as):
        # Connect to IRC server
        self.as = as
        self.logfile = open(logFile, "a")
        print "Connecting to %s as %s..." % (server, nickname)
        SingleServerIRCBot.__init__(self, [(server, port)],
                                    nickname, nickname)
        self.start()

    def on_welcome(self, c, e):
        # Join all our channels
        print "Joining channels..."
        for channel in channels:
            print "   " + channel
            c.join(channel)
        print "Running."

    def on_pubmsg(self, c, e):
        try:
            self.on_pubmsg_protected(c,e)
        except:
	    import sys
            self.log("--- EXCEPTION: %s %s" % (sys.exc_info()[0], sys.exc_info()[1]))

    def on_pubmsg_protected(self, c, e):
        sourceNick = re.sub("!.*","",e._source)
        self.log("<%s> %s" % (sourceNick, e._arguments[0]))
        
        # Respond to a public message in any of the channels we've joined
        line = string.lower(e._arguments[0])
        words = line.split()

        # Determine whether we should say anything just yet
        triggerFlag = False
        if re.search("Diablo-D3", sourceNick) and random.random() > 0.2:
            triggerFlag = True

        # Was this line addressed to us? If so, remove that part.
        addressed = False
        if re.search(nickname.lower(), words[0]):
            addressed = True
            words = words[1:]

        # If the first word looks like a nick addressment, remove it
        if re.search(":", words[0]):
            words = words[1:]

        # If this line is long enough, learn from it
        if len(words) > 2 and sourceNick != "CIA":
            line = string.join(words)
            as.learn(line)
            pickle.dump(as, open(pickleFile,"w"))

        # Should we respond?
        if triggerFlag or addressed:
            responseWord = None
            responsePrefix = ''
            
            # If the line was long enough, form a response starting with the last word in it
            if len(words) > 1:
                responseWord = words[-1]
                responseWord = re.sub("!","",responseWord,0)                
                responseWord = re.sub("\?","",responseWord,0)                
                responseWord = re.sub("\.","",responseWord,0)
                self.log("--- Starting response with '%s'" % responseWord)

            # If we were being addressed, address them back
            if addressed:
                responsePrefix = sourceNick + ": "
                self.log("--- Being addressed by '%s'" % sourceNick)

            # Try repeatedly to form a longish sentence using our starting word
            minLength = 4
            for i in range(0,50):
                response = as.response(responseWord,responsePrefix)
                if len(string.split(response)) >= minLength:
                    break

            # If that fails, just use any word
            if len(string.split(response)) < minLength:
                responseWord = None
                for i in range(0,50):
                    response = as.response(responseWord,responsePrefix)
                    if len(string.split(response)) >= minLength:
                        break

            # Log the response
            self.log("--- Responding: '%s'" % response)
        
            # Time for 'typing'
            time.sleep(len(response)/9.0)

            # Send it
            c.privmsg(e._target, response)
            self.log("<%s> %s" % (nickname, response))

    def log(self, str):
        self.logfile.write(str + "\n")
        print str

        
def Train(as, corpus):
    corpus = corpus.lower()
    corpus = re.sub("[^a-zA-Z0-9,\?\.!' ]","",corpus,0)
    corpus = re.sub("u\. *n\. *","UN ",corpus,0)
    corpus = re.sub("u\. *s\. *","US ",corpus,0)
    corpus = re.sub("mr\.","mr",corpus,0)
    corpus = re.sub("\.",". ",corpus,0)
    corpus = re.sub("\?","? ",corpus,0)
    corpus = re.sub("!.","! ",corpus,0)
    wordPat = re.compile("(\S+)")
    sentencePat = re.compile("[!\.\?]")
    matches = wordPat.findall(corpus)
    sentence = ''
    for word in matches:
        sentence += word + ' '
        if sentencePat.search(sentence):
            if len(sentence) > 10 and re.search("[a-zA-Z0-9]",sentence[0]):
                sentence = re.sub("\.","",sentence)
                print sentence
                as.learn(sentence)
            sentence = ''


if __name__ == "__main__":

    def usage():
        print """
dubbyabot, with Artificial Stupidity (tm) technology!
Micah Dowty <micah@navi.picogui.org>

usage: %s [-ch] [-t file]

  c: clear database
  h: this help text
  t: training mode
""" % (sys.argv[0])

    try:
        opts, args = getopt.getopt(sys.argv[1:], "t:ch")
    except getopt.GetoptError:
        usage()
        sys.exit(2)

    trainingMode = False
    clearDB = False
    for o, a in opts:
        if o == "-h":
            usage()
            sys.exit()
        if o == "-t":
            trainingMode = True
            trainingFile = a
        if o == "-c":
            clearDB = True

    if clearDB:
        as = ArtificialStupidity()
    else:
        as = pickle.load(open(pickleFile))

    try:
        if trainingMode:
            print "Training mode"
            Train(as, open(trainingFile).read())
        else:
            print "Bot mode"
            Bot(as)
    finally:
        pickle.dump(as, open(pickleFile,"w"))
