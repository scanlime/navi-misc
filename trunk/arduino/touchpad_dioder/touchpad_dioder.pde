
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
class Color
{
public:
  float r, g, b;   

  /*
   * HSL to RGB conversion, from Wikipedia.
   * All of H, S, and L are in the range [0, 1].
   */
  void fromHSL(float h, float s, float l)
  {
    float q = l < 0.5 ? l * (1+s) : l + s - (l*s);
    float p = 2*l - q;
    float tR = h + (1.0 / 3.0);
    float tG = h;
    float tB = h - (1.0 / 3.0);

    if (tR < 0) tR += 1.0;
    if (tG < 0) tG += 1.0;
    if (tB < 0) tB += 1.0;
    if (tR > 1) tR -= 1.0;
    if (tG > 1) tG -= 1.0;
    if (tB > 1) tB -= 1.0;

    if (tR < (1.0 / 6.0))
      r = p + ((q - p) * 6.0 * tR);
    else if (tR < 0.5)
      r = q;
    else if  (tR < (2.0 / 3.0))
      r = p + ((q - p) * 6.0 * (2.0 / 3.0 - tR));
    else
      r = p;

    if (tG < (1.0 / 6.0))
      g = p + ((q - p) * 6.0 * tG);
    else if (tG < 0.5)
      g = q;
    else if  (tG < (2.0 / 3.0))
      g = p + ((q - p) * 6.0 * (2.0 / 3.0 - tG));
    else
      g = p;

    if (tB < (1.0 / 6.0))
      b = p + ((q - p) * 6.0 * tB);
    else if (tB < 0.5)
      b = q;
    else if  (tB < (2.0 / 3.0))
      b = p + ((q - p) * 6.0 * (2.0 / 3.0 - tB));
    else
      b = p;
  }
  
  bool operator==(const Color &other) const
  {
      return r == other.r && g == other.g && b == other.b;
  }

  bool operator!=(const Color &other) const
  {
      return !(*this == other);
  }
};


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

  typedef struct {
    int x, y;
    byte button;
  } MousePacket;

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

  void waitForPacket(MousePacket &packet)
  {
    char status;
    char rx, ry;

    do {
      port.write(0xeb);  // Poll the mouse
      port.read();       // ack byte
      status = port.read();
      rx = port.read();
      ry = port.read();
    } while (!(status & 0x08));
    
    packet.x = rx;
    packet.y = ry;
    packet.button = status & 1;
  } 
  
private:
  PS2 port;
};

/*
 * Hardware defs
 */
Touchpad touchpad(7, 8);
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
  static Color c;
  Color prevColor = c;

  Touchpad::MousePacket packet;
  touchpad.waitForPacket(packet);

  x += packet.x * 0.0005;
  y += packet.y * 0.002;
  
  if (x > 1.0) x -= 1.0;
  if (x < 0.0) x += 1.0;
  if (y > 1.0) y = 1.0;
  if (y < 0.0) y = 0.0;
  
  c.fromHSL(x, 1.0, y);
  
  if (c != prevColor) {
    click();
  }

  setColor(c);

  delay(10);  
} 

