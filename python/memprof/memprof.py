#!/usr/bin/env python
import gc, time


def twistedInstall(interval=5):
    from twisted.internet import reactor
    s = Sampler()
    def sample():
        s.run()
        reactor.callLater(interval, sample)
    sample()

class Sampler:
    def __init__(self, log="pymemprof.log"):
        self.log = open(log, "a")

    def run(self):
        timestamp = time.time()

        typeMap = {}
        for obj in gc.get_objects():
            if hasattr(obj, '__class__'):
                t = obj.__class__
            else:
                t = type(obj)
            t = str(getattr(t, '__name__', t))

            typeMap[t] = typeMap.get(t, 0) + 1

        tokens = [str(timestamp)]
        for name, count in typeMap.iteritems():
            tokens.append("%s=%s" % (name, count))
        self.log.write(" ".join(tokens) + "\n")


class Analyzer:
    def __init__(self, log="pymemprof.log"):
        self.typeMap = {}

        for line in open(log):
            try:
                tokens = line.strip().split()
                timestep = float(tokens[0])
            except:
                continue

            for token in tokens[1:]:
                try:
                    type, count = token.split('=')
                    count = int(count)
                except:
                    continue

                self.typeMap.setdefault(type, []).append((timestep, count))

    def dump(self):
        for type, points in self.typeMap.iteritems():
            f = open("types/%s" % type, "w")
            for point in points:
                f.write("%s %s\n" % point)


if __name__ == "__main__":
    #import psyco
    #psyco.full()

    Analyzer().dump()
