#!/usr/bin/env python
#
# XMMS visualization and information display on the LED Board and VFD.
# Requires the pyxmms, pyesdm ledboard, and Numeric modules!
#
import ledboard, xmms, time, esd, os, socket, Numeric, FFT, math, random

class ESDmonitor:
    "Monitors EsounD's output stream, returning a NumPy array"
    def __init__(self):
        self.rate = 44100
        fd = esd.esd_monitor_stream(esd.ESD_MONO,self.rate,'localhost','')
        self.socket = socket.fromfd(fd, socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setblocking(0)
        
    def read(self):
        data = ''
        while True:
            try:
                data += self.socket.recv(4096)
            except socket.error:
                break
        return Numeric.fromstring(data, Numeric.Int16)


class AudioAnalyzer:
    def __init__(self):
        self.monitor = ESDmonitor()
        self.vuMeterSampler = NormalizedFreqRange(self, 20, 10000)
        self.spectrumSamplers = []
        self.oldSpectrum = Numeric.zeros(8)
        fundamental = 30
        octavesPerLED = 1
        for i in range(8):
            f1 = fundamental*(1<<(i*octavesPerLED))
            f2 = fundamental*(1<<((i+1)*octavesPerLED))
            self.spectrumSamplers.append(NormalizedFreqRange(self,f1,f2))

    def freqIndex(self, hz):
        return int(hz / (self.monitor.rate/1000.0))

    def freqRange(self, fromHz, toHz):
        idx1 = self.freqIndex(fromHz)
        idx2 = self.freqIndex(toHz)
        numBands = idx2 - idx1 + 1
        return Numeric.add.reduce(self.freqDomain[idx1:idx2])

    def sample(self):
        newData = self.monitor.read()
        if len(newData) == 0:
            return
        self.timeDomain = newData

        # Round our actual sample size down to the nearest power of two for the FFT,
        # and only save the absolute value of the first half of the real part.
        self.numSamples = len(self.timeDomain)
        f = 1
        while f <= self.numSamples:
            self.fftSamples = f
            f *= 2
        self.freqDomain = FFT.real_fft(self.timeDomain, self.fftSamples).real
        self.freqDomain = Numeric.fabs(self.freqDomain[:len(self.freqDomain)/2])

        self.spectrum = []
        for s in self.spectrumSamplers:
            self.spectrum.append(s.get())
        self.spectrum = Numeric.array(self.spectrum)

        self.beat = self.spectrum - self.oldSpectrum
        self.oldSpectrum = self.spectrum

        self.vuMeter = self.vuMeterSampler.get()


class NormalizedFreqRange:
    def __init__(self, signal, fromHz, toHz):
        self.max = 0
        self.signal = signal
        self.fromHz = fromHz
        self.toHz = toHz

    def get(self):
        # Decay our max slowly to account for changes in peak volume between songs
        self.max *= 0.999                                            

        raw = self.signal.freqRange(self.fromHz, self.toHz)
        if raw > self.max:
            self.max = raw
        if self.max > 0:
            return float(raw) / self.max
        else:
            return 0


class VisualizerState:
    def __init__(self, vis):
        self.vis = vis
        self.led = vis.led

    def init(self):
        pass
    def slowUpdate(self):
        pass
    def fastUpdate(self):
        pass

    def transition(self, nextClass):
        self.vis.state = nextClass(self.vis)
        self.vis.state.init()


class IdleState(VisualizerState):
    def init(self):
        self.frame = 0

    def fastUpdate(self):
        self.frame += 1
        if self.frame == 2:
            self.led.fade()
            self.led.blit()
            self.frame = 0

    def slowUpdate(self):
        if xmms.is_paused():
            self.transition(PausedState)
        elif xmms.is_playing():
            self.transition(BeginPlayingState)


class StoppedState(VisualizerState):
    def init(self):
        self.led.clear()
        ledboard.GridPattern([
            '     ',
            '     ',
            'ooooo',
            'o   o',
            'o   o',
            'o   o',
            'ooooo',
            '     ',
            ]).set(self.led)
        self.led.blit()
        self.timeout = 10

    def slowUpdate(self):
        self.timeout -= 1
        if self.timeout < 0:
            self.transition(IdleState)
        if xmms.is_paused():
            self.transition(PausedState)
        elif xmms.is_playing():
            self.transition(BeginPlayingState)


class PausedState(VisualizerState):
    def init(self):
        self.theta = 0
        self.led.clear()
        ledboard.GridPattern([
            '     ',
            '     ',
            ' o o ',
            ' o o ',
            ' o o ',
            ' o o ',
            '     ',
            '     ',
            ]).set(self.led)
        self.led.blit()

    def wavy(self, x):
        return math.sin(self.theta + x*0.8)*7+8

    def upperWavyFunc(self, led):
        return self.wavy(led['x'])

    def lowerWavyFunc(self, led):
        return self.wavy(7-led['x'])

    def fastUpdate(self):
        self.theta += 0.15
        self.theta %= 2*3.1415926
        led.set(led.select({'group': 'bars', 'color': 'yellow'}), self.upperWavyFunc)
        led.set(led.select({'group': 'bars', 'color': 'green' }), self.lowerWavyFunc)
        led.blit()

    def slowUpdate(self):
        if not xmms.is_paused():
            if xmms.is_playing():
                self.transition(BeginPlayingState)
            else:
                self.transition(StoppedState)


class BeginPlayingState(VisualizerState):
    def init(self):
        self.led.clear()
        self.pattern = ledboard.GridPattern([
            '      ',
            'o     ',
            'oo    ',
            'o.o   ',
            'o..o  ',
            'o.o   ',
            'oo    ',
            'o     ',
            ])
        self.pattern.set(self.led)
        self.led.blit()
        self.timeout = 5

    def slowUpdate(self):
        self.pattern.scrollRight()
        self.pattern.set(self.led)
        self.led.blit()
        self.timeout -= 1
        if xmms.is_paused():
            self.transition(PausedState)
        elif not xmms.is_playing():
            self.transition(StoppedState)
        if self.timeout < 0:
            self.transition(BeginPlayingFaderState)

                            
class BeginPlayingFaderState(VisualizerState):
    def init(self):
        self.frame = 0
        self.timeout = 2

    def fastUpdate(self):
        self.frame += 1
        if self.frame == 2:
            self.led.fade()
            self.led.blit()
            self.frame = 0

    def slowUpdate(self):
        self.timeout -= 1
        if self.timeout < 0:
            self.transition(PlayingState)


class VisualizerEffect:
    def __init__(self, vis):
        self.led = vis.led
        self.vis = vis

    def init(self):
        pass

    def fastUpdate(self):
        pass

    def slowUpdate(self):
        pass
        

class SpectrogramEffect(VisualizerEffect):
    def __init__(self,vis):
        VisualizerEffect.__init__(self,vis)
        self.grid = self.led.select({'group': 'grid'})
        self.spectrogram = [[0]*8]*5

    def spectrogramFunc(self, led):
        return pow(self.spectrogram[led['x']][led['y']],1.8)*15.99

    def slowUpdate(self):
        self.spectrogram = [Numeric.fabs(self.vis.signal.spectrum)] + self.spectrogram[:-1]
        self.led.set(self.grid, self.spectrogramFunc)


class BeatMapEffect(VisualizerEffect):
    def __init__(self, vis, criteria, axis='x', axisScale=1, axisBias=0, threshold=0.5, fadeSpeed=0.3):
        VisualizerEffect.__init__(self,vis)
        self.group = self.led.select(criteria)
        self.axis = axis
        self.axisScale = axisScale
        self.axisBias = axisBias
        self.threshold = threshold
        self.fadeSpeed = fadeSpeed

    def func(self, led):
        if self.vis.signal.beat[led[self.axis]*self.axisScale+self.axisBias] > self.threshold:
            return 15
        
    def fastUpdate(self):
        self.led.fade(self.group, self.fadeSpeed)
        self.led.set(self.group, self.func)
        

class VUMeterEffect(VisualizerEffect):
    def __init__(self, vis, criteria, axis='x'):
        VisualizerEffect.__init__(self,vis)
        self.group = self.led.select(criteria)
        self.axis = axis

    def vuFunc(self, led):
        if led[self.axis] <= self.vis.signal.vuMeter*8:
            return 15

    def fastUpdate(self):
        self.led.fade(self.group, 3)
        self.led.set(self.group, self.vuFunc)


class VUBrightnessEffect(VisualizerEffect):
    def __init__(self, vis, criteria, fadeGroup=None, fadeSpeed=0.2):
        VisualizerEffect.__init__(self,vis)
        self.group = self.led.select(criteria)
        self.fadeGroup = self.led.select(fadeGroup)
        self.fadeSpeed = fadeSpeed
        
    def fastUpdate(self):
        self.led.fade(self.fadeGroup, self.fadeSpeed)
        self.led.set(self.group, self.vis.signal.vuMeter * 15.9)


class DotAnalyzerEffect(VisualizerEffect):
    def __init__(self, vis, fadeSpeed=0.9):
        VisualizerEffect.__init__(self,vis)
        self.grid = self.led.select({'group': 'grid'})
        self.fadeSpeed = fadeSpeed

    def analyzerFunc(self, led):
        if int(self.vis.signal.spectrum[led['y']]*5.9) == led['x']:
            return 15

    def fastUpdate(self):
        self.led.fade(self.grid, self.fadeSpeed)
        self.led.set(self.grid, self.analyzerFunc)

        
class FadeAnalyzerEffect(VisualizerEffect):
    def __init__(self, vis, fadeSpeed=1.2):
        VisualizerEffect.__init__(self,vis)
        self.grid = self.led.select({'group': 'grid'})
        self.fadeSpeed = fadeSpeed

    def analyzerFunc(self, led):
        if self.vis.signal.spectrum[led['y']]*5.9 >= led['x']:
            return 15

    def fastUpdate(self):
        self.led.fade(self.grid, self.fadeSpeed)
        self.led.set(self.grid, self.analyzerFunc)
        

class PlayingState(VisualizerState):
    def init(self):

        # Exactly one effect from each group will be active at once
        self.effectdb = [
            # LED grid effects
            [ [SpectrogramEffect(self.vis)],
              [DotAnalyzerEffect(self.vis)],
              [FadeAnalyzerEffect(self.vis)],
              [FadeAnalyzerEffect(self.vis, 5)],
              ],
            # Horizontal bar effects
            [ [VUMeterEffect(self.vis, {'group': 'bars'})],
              [BeatMapEffect(self.vis, {'group': 'bars'})],
              ],
            # Dot effects
            [ [VUBrightnessEffect(self.vis, {'group': 'dot'})],
              ],
            # Pacman effects
            [ [BeatMapEffect(self.vis, {'group': 'pacman'}, 'x', 1, 0)],
              [VUBrightnessEffect(self.vis, {'group': 'pacman', 'color': 'blue'}, {'group': 'pacman'})],
              [VUBrightnessEffect(self.vis, {'group': 'pacman', 'color': 'yellow'}, {'group': 'pacman'})],
              [VUBrightnessEffect(self.vis, {'group': 'pacman', 'color': 'red'}, {'group': 'pacman'})],
              ],               
            # Corner effects
            [ [BeatMapEffect(self.vis, {'group': 'corners', 'color': 'blue'},  'x', 0, 0, 0.3),
               BeatMapEffect(self.vis, {'group': 'corners', 'color': 'white'}, 'x', 0, 4)],
	      ],
            ]
        
        self.resetEffect()

    def resetEffect(self):
        # Time limit, in slowUpdate units (nominally deciseconds) for one set of effects to run
        self.timeout = 50

        self.effects = []
        for group in self.effectdb:
            self.effects += random.choice(group)

    def fastUpdate(self):
        for e in self.effects:
            e.fastUpdate()
        self.led.blit()

    def slowUpdate(self):
        for e in self.effects:
            e.slowUpdate()
        self.timeout -= 1
        if xmms.is_paused():
            self.transition(PausedState)
        elif not xmms.is_playing():
            self.transition(StoppedState)
        if self.timeout < 0:
            self.resetEffect()


class Visualizer:
    def __init__(self, led):
        self.led = led
        self.signal = AudioAnalyzer()
        self.state = IdleState(self)
        self.state.init()

    def fastUpdate(self):
        self.signal.sample()
        self.state.fastUpdate()

    def slowUpdate(self):
        self.state.slowUpdate()


class VFDupdater:
    "Collects information from XMMS and displays it on the VFD panel"
    scrollerDelay = 6
    scrollerIndex = 0
    scrollerGap = 5
    lastTitle = ''
    spinnerState = '/-\\|'

    def __init__(self, vfd):
        self.vfd = vfd

    def fastUpdate(self):
        pass

    def slowUpdate(self):
        # Get the current song title
        title = str(xmms.get_playlist_title(xmms.get_playlist_pos()))
        if len(title) > self.vfd.width:
            # Scroll it around so we can see the whole thing. At the beginning
            # of the scroll cycle it delays by scrollerDelay scroll cycles,
            # and there is a scrollerGap character gap between copies of the scrolled title.
            if title != self.lastTitle:
                self.scrollerIndex = -self.scrollerDelay
                self.lastTitle = title
            if self.scrollerIndex > 0:
                self.scrollerIndex %= self.scrollerGap + len(title)
                if self.scrollerIndex == 0:
                    self.scrollerIndex = -self.scrollerDelay
                else:
                    title = ("%s%s%s" % (title, " " * self.scrollerGap, title))[self.scrollerIndex:]
            self.scrollerIndex += 1
    
        # get the current time index
        if xmms.is_playing():
            millisec = xmms.get_output_time()
            minutes = millisec / 60000
            seconds = (millisec % 60000) / 1000.0
            playTime = "%3d:%02d" % (minutes, seconds)
        else:
            playTime = " --:--"

        # Status string
        if xmms.is_paused():
            status = 'Paused'
            spinner = '-'
        elif xmms.is_playing():
            status = 'Playing...'
            self.spinnerState = self.spinnerState[1:] + self.spinnerState[0]
            spinner = self.spinnerState[0]
        else:
            status = 'Idle'
            spinner = ' '

        self.vfd.writeScreen("%s\n%-12s%s %s" % (title, status, playTime, spinner))


if __name__ == '__main__':
    led = ledboard.Server()
    vfdupdater = VFDupdater(led.vfd)
    visualizer = Visualizer(led)
    components = [vfdupdater, visualizer]
    then = time.time()

    while True:
        # slowUpdates every 0.1 seconds, fastUpdates every 0.01 seconds nominally
        for c in components:
            c.slowUpdate()
        for i in range(10):
            for c in components:
                c.fastUpdate()
            now = time.time()
            target = then + 0.01
            if now < target:
                time.sleep(target - now)
            then = now
