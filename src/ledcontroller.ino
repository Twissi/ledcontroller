/*
 * Project ledcontroller
 * Description: control the led strip over tcp
 * Author: Kristin Fritsch
 * Date: 10/16/18
 */

#include "WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma
#ifdef __AVR_ATtiny85__
#include <avr/power.h>
#endif

// number of pixels of your led strip: 25
Adafruit_WS2801 strip = Adafruit_WS2801(25);

// tcp server listens at port 66
TCPServer server = TCPServer(66);
TCPClient client;

// incoming data
char incoming;

// led strip data
char powerState;
uint8_t r;
uint8_t g;
uint8_t b;
uint8_t wait;

const char off = '6f 66 66';
const char on = '6f 6e';

void setup() {
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
   clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
  #endif

  strip.begin();

  // Update LED contents, to start they are all 'off'
  strip.show();

  // start tcp server
  server.begin();

  Serial.begin(9600);
  // Wait for a USB serial connection for up to 30 seconds
  waitFor(Serial.isConnected, 30000);

  // print ip address
  Serial.println(WiFi.localIP());

  // led stripe presets
  powerState = off;
  r = 255;
  g = 0;
  b = 0;
  wait = 50;
}


void loop() {
  if (powerState == off){
    colorWipe(0, 0, 0, wait);
  }
  if (powerState == on){
    colorWipe(r, g, b, wait);
  }

  if (client.connected()) {

    while (client.available()) {
      
      incoming = client.read();

      // great to show on telnet what has been printed
      // client.write(incoming);

      if (incoming == off) {
        setPowerState(off);
        client.println("Turned power off");
        // client.println(strip.getPixelColor(0));
      }

      if (incoming == on) {
        setPowerState(on);
        client.println("Turned power on");
      }

      delay(wait);
    }
  } else {
    Serial.println("No client connected");
    // if no client is yet connected, check for a new connection
    client = server.available();
  }
}

/* Getter functions */

char getPowerState() {
  return powerState;
}

void getBrightness() {

}

void getHue() {

}

void getSaturation(){

}

uint32_t getPixelColor(uint16_t n){
  return strip.getPixelColor(n);
}

/* Setter functions */

void setPowerState(char state){
  powerState = state;
}

void setBrightness() {

}

void setHue() {

}

void setSaturation(){

}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, r, g, b);
    strip.show();
    delay(wait);
  }
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
    return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
