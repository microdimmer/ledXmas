#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <BlynkSimpleEsp8266.h> //TODO OTA updates
#include <Adafruit_NeoPixel.h> //https://github.com/adafruit/Adafruit_NeoPixel
#include <SimpleTimer.h> //https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer

const uint8_t CONTROL_PIN = 5;
const uint8_t MAX_MODES = 6;

SimpleTimer timer;
char blynk_token[] PROGMEM = {"179160c1078a4f2bb4256740577a00c0"}; // Blynk token
char SSID_NAME[] PROGMEM = {"Siskotryas2"};
char PASS[] PROGMEM = {"Gigadi750N82D"};

uint8_t modeStrip = 0;
uint8_t modeChanged = 0;
uint8_t r = 255;
uint8_t g = 0;
uint8_t b = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(180, CONTROL_PIN, NEO_GRB + NEO_KHZ800);

#define DEBUG
#ifdef DEBUG
#define PRINTLNF(s)       \
  {                       \
    Serial.println(F(s)); \
  }
#define PRINTLN(s, v)   \
  {                     \
    Serial.print(F(s)); \
    Serial.println(v);  \
  }
#define PRINT(s, v)     \
  {                     \
    Serial.print(F(s)); \
    Serial.print(v);    \
  }
#else
#define PRINTLNF(s)
#define PRINTLN(s, v)
#define PRINT(s, v)
#endif

void BlynkUpdate() {
  Blynk.run();
  yield();
}

void setup() {
  Serial.begin(115200);
  //  connectWF("microdimmer", "88888811");
  connectWF(SSID_NAME, PASS);

  Blynk.config(blynk_token); // Start blynk
  PRINTLNF("Connecting to blynk...");
  while (!Blynk.connect()) {
    delay(3000);
    PRINTLNF("Connecting to blynk...");
  }
  Serial.println("Connected");
  strip.begin();
  strip.show(); //Initialize all pixels to 'off'
  strip.setBrightness(20);

  timer.setInterval(1000L, BlynkUpdate);
}

void loop() {
  timer.run();

  switch (modeStrip) {
    case 0:
      PRINTLNF("theaterChaseRainbow");
      theaterChaseRainbow(50);
      break;
    case 1:
      PRINTLNF("rainbow");
      rainbow(20);
      break;
    case 2:
      PRINTLNF("rainbowCycle");
      rainbowCycle(20);
      break;
    case 3:
      PRINTLNF("theaterChase");
      theaterChase(strip.Color(r, g, b), 50);
      break;
    case 4:
      PRINTLNF("color wipe");
      if (modeChanged)
      colorWipe(strip.Color(r, g, b), 50); //static  
      break;
     case 5:
      PRINTLNF("OFF");
      if (modeChanged)
      colorWipe(strip.Color(0, 0, 0), 50);  //off
      break;  
    default:
      PRINTLNF("OFF");
      if (modeChanged)
        colorWipe(strip.Color(0, 0, 0), 50);    //off
  }
}

BLYNK_WRITE(V23) {
  PRINTLNF("V23 is pressed");
  if (++modeStrip >= MAX_MODES) modeStrip = 0;
  modeChanged = 1;
}

BLYNK_WRITE(V22) {
  PRINTLNF("V22 is pressed");
  uint8_t sliderValue = param.asInt();
  if (sliderValue > 100)
    sliderValue = 100;
  PRINTLN("sliderValue=", sliderValue);
  strip.setBrightness(sliderValue);
  modeChanged = 1;
}

BLYNK_WRITE(V21) {
  PRINTLNF("V21 is pressed");
  r = param[0].asInt();
  g = param[1].asInt();
  b = param[2].asInt();
  modeChanged = 1;
}

uint8_t connectWF(char *ssid, char *pass) {
  WiFi.begin(ssid, pass);
  PRINTLNF("Connecting");
  for (uint8_t i = 0; i < 10; i++)
  {
    delay(1000);
    Serial.print(".");
    Serial.printf("Connection status: %d\n", WiFi.status());
    WiFi.printDiag(Serial);
    Serial.println();
    Serial.println(millis());
    if (WiFi.status() == WL_CONNECTED) {
      PRINTLN("Connected, IP address: ", WiFi.localIP());
      return 1;
    }
  }
  return 0;
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    yield();
    if (modeChanged) {
      modeChanged = 0;
      return;
    }
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    yield();
    if (modeChanged) {
      modeChanged = 0;
      return;
    }
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    yield();
    if (modeChanged) {
      modeChanged = 0;
      return;
    }
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();
      yield();
      if (modeChanged) {
        modeChanged = 0;
        return;
      }
      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();
      yield();
      if (modeChanged) {
        modeChanged = 0;
        return;
      }
      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
