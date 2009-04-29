#
# Check the original Robot Odyssey binaries in 'original', and copy
# them to the proper place in 'build'.
#
# This script supports plain files, zip files, or any combination
# thereof.  We don't care about file naming, since we're just
# searching for files with the proper SHA-1 hash. This helps us avoid
# having to care about the directory structure or filename case of the
# original archive.
#
# Micah Dowty <micah@navi.cx>
#

import os
import sha
import sys
import zipfile

class OriginalFileCollector:
    hashes = {
        # Binaries to translate
        'e4a1e59665595ef84fe7ff45474bcb62c382b68d': 'build/tut.exe',
        '756a92e6647a105695ac61e374fd2e9edbe8d935': 'build/game.exe',
        '692a9bb5caca7827eb933cc3e88efef4812b30c5': 'build/lab.exe',
        'a6293df401a3d4b8b516aa6a832b9dd07f782a39': 'build/menu.exe',
        '12df28e9c3998714feaa81b99542687fc36f792f': 'build/play.exe',

        # Game data files
        '9a2656381e063e43bf7d1534b5724362e11236d3': 'build/fs/4bitcntr.csv',
        '751343a72a371ecfafa1c35224bb3bda2831eeb9': 'build/fs/adder.csv',
        '04e596d9f418fa71f6ddb44c3c12310f207bbb95': 'build/fs/bus.csv',
        '4d00a4911643ccb5ff58e44d96dc9261ea705a14': 'build/fs/clock.csv',
        'e412bb69e310cc9305d344331d49ee37e3c7d997': 'build/fs/countton.csv',
        'f4a35b329b5d2888a9b341ca56f61d732e34e646': 'build/fs/delay.csv',
        '5cec1833e2ec79cb89d8db7bc476922653f28387': 'build/fs/oneshot.csv',
        '092d64ee500ac0a5924c6b9cf10ec1ea71605392': 'build/fs/comp.wld',
        'dabd64458ba5ff6e8ce08810d5a6f2435115edf8': 'build/fs/countton.chp',
        '191753097a89ae326dbbdb6725ab51b9282c985f': 'build/fs/countton.pin',
        'cd000439bb2d137bd6b9ecfd86d432708f23aa3a': 'build/fs/font.fon',
        '0df0e301260e06ded7c64996aa99b04d8030f18b': 'build/fs/joyfile.joy',
        '82563b7b1b44e4784afd1221c894e71f92725643': 'build/fs/lab.wor',
        'ece893a440bfcf81ae0c438f6035713a825bc6ba': 'build/fs/pulse.chp',
        'b0aa4b549f325cca9c9dfa6ce1bd6072aeaeac71': 'build/fs/pulse.pin',
        '932e2e777a257271685e4babb3494717e8034756': 'build/fs/rsflop.csv',
        '194267d3de8afed93516db5a78b6559b185db0cc': 'build/fs/sewer.cir',
        '96d0e323e8c065b19303f54a3b87a4bfb9443f2b': 'build/fs/sewer.wor',
        '6a0f6f3b102491e6b6c6a42cc48abbcc17249a95': 'build/fs/show2.shw',
        '40adb729d3e86be25cde6796d87b3d0560e7e239': 'build/fs/show.shw',
        '51b8c31c15dbc779c626c24408c68f5931ca7ff8': 'build/fs/stereo.csv',
        '84d2931d1c3464bf53dae45185dd5f80344b8bfc': 'build/fs/street.wld',
        'caf4b8c04656bffa767caf7f10819d4585fc9ed3': 'build/fs/subway.wld',
        '075d5e2030db9ac03e29b1611275f39c085fe5e9': 'build/fs/town.wld',
        '09a215de45dbb3ea0e83fb162b9f33ad5e03dae5': 'build/fs/tut1.cir',
        '6aa4bac173438903474e9f825b7f7ad4db270e02': 'build/fs/tut1.wor',
        '84647898c5e6e0e89f14d0e0a2f008671e4a9f1a': 'build/fs/tut2.cir',
        '2ecd9ac71d7b08f6a9bd429fcae253c746b08345': 'build/fs/tut2.wor',
        '71c1da6bc2defea903a2cc103bb2adc7af80e39c': 'build/fs/tut3.wor',
        'd4bf5cb9fba323db3dbe164a797325d7243a7fa3': 'build/fs/tut4.cir',
        'b2957a82d9509ac48b779163b39c22d59efb7809': 'build/fs/tut4.wor',
        'c709edebb988291f5e3e4d5557e9003f17fe485c': 'build/fs/tut5.cir',
        'a56604066a5f70da2d1e9eb1cc0a698458342627': 'build/fs/tut5.wor',
        '2a1b2674e77271af92291e574b6acb22e5e60b8f': 'build/fs/tut6.wor',
        'fef1c47b666afc77b9e06ccd137bebec129063f1': 'build/fs/tut7.cir',
        '37c75f56b94ebe0cdc208935e2b287e28201722c': 'build/fs/tut7.wor',
        '3a3e10d3b05bce9a53780d89dad2f23d39f5ff62': 'build/fs/wallhug.chp',
        'dd8c20196b39dd53ade20c6f28e7cf9cb1dc7754': 'build/fs/wallhug.csv',
        'eac709e814a2601c96b2c4056a722e3320daab09': 'build/fs/wallhug.pin',
        }

    def __init__(self):
        self.remaining = self.hashes

    def scan(self, path, data):
        hash = sha.new(data).hexdigest()
        if hash in self.remaining:
            dest = self.remaining[hash]
            del self.remaining[hash]
            print "%s => %s" % (path, dest)
            open(dest, 'wb').write(data)

    def scanFile(self, path):
        if path.lower().endswith('.zip'):
            self.scanZip(path)
        else:
            self.scan(path, open(path, 'rb').read())

    def scanZip(self, zipFile):
        zip = zipfile.ZipFile(zipFile)
        for name in zip.namelist():
            self.scan(os.path.join(zipFile, name), zip.read(name))

    def scanDir(self, dir):
        for root, dirs, files in os.walk(dir):
            for file in files:
                self.scanFile(os.path.join(root, file))

    def finish(self, dir):
        if self.remaining:
            print ("\n"
                   "*** ERROR: Couldn't find the following original data files.\n"
                   "           Your copy of Robot Odyssey is incomplete, "
                   "corrupted, or cracked!\n\n"
                   "Please place a complete and unmodified copy of Robot Odyssey\n"
                   "in the %r directory. You can simply place a zip file in this\n"
                   "directory, no need to extract it yourself.\n" % dir)

            for hash, filename in self.remaining.iteritems():
                print "%s  %s" % (hash, filename)
            print
            sys.exit(1)


def main(srcDir='original', destDir='build/fs'):
    try:
        os.makedirs(destDir)
    except OSError:
        pass

    collector = OriginalFileCollector()
    collector.scanDir(srcDir)
    collector.finish(srcDir)

if __name__ == '__main__':
    main()
