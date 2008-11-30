
/*
 * Work in progress, touchpad-controlled RGB lighting (modded Ikea DIODER)
 *
 * Requires the PS2 library:
 *   http://www.arduino.cc/playground/ComponentLib/Ps2mouse
 *
 * Micah Dowty <micah@navi.cx> 
 */

#include <ps2.h>


/*
 * A generic RGB color, with 8 bits of precision.
 */
typedef struct {
  float r, g, b;   
} Color;

/*
 * Mouse packet: Relative x/y, and one button.
 */
typedef struct {
  int x, y;
  byte button;
} MousePacket;

/*
 * Our touchpad. We use its PS/2 mouse emulation mode. It is a
 * Cirque Glidepoint touchpad. I couldn't find any info about its
 * protocol or any open source drivers, and the ALPS protocol
 * doesn't seem to work.
 *
 * The mouse code here is from the PS/2 library's mouse example.
 */
class Touchpad
{
public:
  Touchpad(int clockPin, int dataPin) : port(clockPin, dataPin) {}

  enum Mode {
    IDLE,
    SPIN,
    HORIZONTAL,
    VERTICAL,
    TAP,
  };

  void init()
  {
    port.write(0xff);  // reset
    port.read();       // ack byte
    port.read();       // blank
    port.read();       // blank
    
    port.write(0xf0);  // remote mode
    port.read();       // ack
    delayMicroseconds(100);
  }    

  void poll(Mode &mode, float &amount)
  {
    char status;
    char rx, ry;
    
    port.write(0xeb);  // Poll the mouse
    port.read();       // ack byte
    status = port.read();
    rx = port.read();
    ry = port.read();
    
    if (!(status & 0x08)) {
      /* Bogus mouse packet */
      return;
    }
    
    MousePacket packet = {rx, ry, status & 1};

    /* Button pressed? This was a tap. */
    if (packet.button && !history[hIndex].button) {
      mode = TAP;
      lastMode = IDLE;
      amount = 1.0f;
      return;
    }
    
    /*
     * Update the history ring buffer.
     * This gives us a low-pass filtered version of the
     * relative mouse motion, for computing a more accurate
     * angle of motion.
     */
    const int historySize = (sizeof(history) / sizeof(history[0]));
    hIndex = (hIndex + 1) % historySize;
    total.x -= history[hIndex].x;
    total.y -= history[hIndex].y;
    history[hIndex] = packet;
    total.x += history[hIndex].x;
    total.y += history[hIndex].y;

    /*
     * Compute the angle and delta angle.
     */
    float magnitude = sq(total.x) + sq(total.y);
    float angle = atan2(total.y, total.x);
    float angleDiff;
    if (magnitude > sq(4.0f * historySize)) {
      angleDiff = fmod(angle - lastAngle, 2 * M_PI);
      if (angleDiff > M_PI) {
        angleDiff -= 2 * M_PI;
      }
      if (angleDiff < -M_PI) {
        angleDiff += 2 * M_PI;
      }
    } else {
      /* Too slow to calculate an accurate angle */
      angleDiff = 0.0f;
    }
    lastAngle = angle;

    /*
     * Are we idle? This resets the state machine for detecting gestures.
     */
    if (total.x == 0 && total.y == 0) {
      mode = lastMode = IDLE;
      amount = 0.0f;
      return;
    }

    /*
     * If we just came out of idle, detect a gesture now.
     */
    if (lastMode == IDLE && fabs(angleDiff) > 0.001) {
      
      if (fabs(angleDiff) > 0.02) {
        mode = lastMode = SPIN;
      }

      if (fabs(total.x) > 5 * fabs(total.y)) {
        mode = lastMode = HORIZONTAL;
      }

      if (fabs(total.y) > 5 * fabs(total.x)) {
        mode = lastMode = VERTICAL;
      }
      
      /* No gesture yet. */
    }

    /*
     * Report results for the current gesture.
     */
    mode = lastMode;
    switch (mode) {

    case SPIN:
      amount = angleDiff;
      break;
      
    case HORIZONTAL:
      amount = rx;
      break;

    case VERTICAL:
      amount = ry;
      break;
          
    default:
      amount = 0.0f;
    }
  }
  
private:
  PS2 port;
  MousePacket history[4];
  MousePacket total;
  float lastAngle;
  byte hIndex;
  Mode lastMode;
};

/*
 * Hardware defs
 */
Touchpad touchpad(7, 8);
//Touchpad touchpad(5, 6);
const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;
const int speakerPin = 12;

void click()
{
  digitalWrite(speakerPin, HIGH);
  delay(1);
  digitalWrite(speakerPin, LOW);
}

void setup()
{
  Serial.begin(9600);
  Serial.print("Hello World!");
  Serial.println();
  touchpad.init();
} 

int setColor(Color &c)
{
  int rByte = min(255, max(0, c.r * 255));
  int gByte = min(255, max(0, c.g * 255));
  int bByte = min(255, max(0, c.b * 255));
  
  analogWrite(redPin, rByte);
  analogWrite(greenPin, gByte);
  analogWrite(bluePin, bByte);
}
 
void loop() 
{ 
  static float x = 0.0f;
  static float y = 1.0f;
  float mx, my;
  float lx = x, ly = y;

  Touchpad::Mode mode;
  float value;
  touchpad.poll(mode, value);

  switch (mode) {

  case Touchpad::VERTICAL:
    y = min(1.0, max(0, y + value * 0.01));
    break;

  case Touchpad::SPIN:
    x += 0.05 * value;
    break;
   
  case Touchpad::TAP:
    if (y > 0.01) {
      y = 0;
    } else {
      y = 1.0f;
    }
    break;
    
  }

  const float third = 2 * M_PI / 3;
  Color c = {sin(x) * y, sin(x + third) * y, sin(x + 2*third) * y};
  
  if ((int)(x*10) != (int)(lx*10) ||
      (int)(y*10) != (int)(ly*10)) {
    click();
  }

  setColor(c);

  delay(10);  
} 

