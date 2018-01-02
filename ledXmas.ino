//#include <FS.h>
//#include <Arduino.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino


//#include <Ticker.h>
//Ticker TickerUpd;

// HTTP requests
//#include <ESP8266HTTPClient.h>
// OTA updates
//#include <ESP8266httpUpdate.h>
// Blynk
#include <BlynkSimpleEsp8266.h>
// Blynk token
//char blynk_token[33] {"a7dbbe1d255441579a3389ecfdd63477"};
char blynk_token[33] {"179160c1078a4f2bb4256740577a00c0"};
char blynk_server[64] {"blynk-cloud.com"};
const uint16_t blynk_port {8442};

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 5

#define MAX_MODES 2
uint8_t modeStrip = 0;
uint8_t modeChanged = 0;
//uint8_t bLevel = 2;
//uint8_t brightnessMass[5] = {0,20,50,100,255};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(180, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(115200);
  connectWF("Siskotryas", "Gigadi750N82D");
  // Start blynk
  Blynk.config(blynk_token, blynk_server, blynk_port);
  Serial.print("blynk server: ");
  Serial.println(blynk_server);
  Serial.print("port: ");
  Serial.println(blynk_port);
  Serial.print("token: ");
  Serial.println(blynk_token);
  Serial.println("Connecting to blynk...");
  while (!Blynk.connect()) {
      delay(3000);
      Serial.println("Connecting to blynk...");
  }
  Serial.println("Connected");
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Blynk.virtualWrite(V22, 0);
  strip.setBrightness(0);
}

void loop() {
//  Blynk.run();
//  timer2.run();
  // Some example procedures showing how to display to the pixels:
//  colorWipe(strip.Color(255, 0, 0), 50); // Red
//  colorWipe(strip.Color(0, 255, 0), 50); // Green
//  colorWipe(strip.Color(0, 0, 255), 50); // Blue
//colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
//  theaterChase(strip.Color(127, 127, 127), 50); // White
  
  switch (modeStrip) {
//  case 0:
//    Serial.println("theaterChase red");
//    theaterChase(strip.Color(127, 0, 0), 50); // Red
//    break;
//  case 1:
//    Serial.println("theaterChase blue");  
//    theaterChase(strip.Color(0, 0, 127), 50); // Blue
//    break;
  case 0:
    Serial.println("rainbow");
    rainbow(20);
    break;
  default:
    Serial.println("rainbowCycle");
    rainbowCycle(20);
    break;
//  default:
//    Serial.println("theaterChaseRainbow");  
//    theaterChaseRainbow(50);
  }
}

BLYNK_WRITE(V23) {
  Serial.println("V23 is pressed");
  if (++modeStrip >= MAX_MODES-1) modeStrip = 0;
  modeChanged = 1; 
}

BLYNK_WRITE(V22) {
  Serial.println("V22 is pressed");
  uint8_t sliderValue0 = param.asInt();
  if (sliderValue0 > 100)
    sliderValue0 = 100;
  Serial.println(sliderValue0);
  strip.setBrightness(sliderValue0);
  modeChanged = 1; 
//  if (++bLevel >= 4) bLevel = 0;
//  Serial.println("V22 is pressed");
//  strip.setBrightness(brightnessMass[bLevel]);
//  modeChanged = 1; 
}

uint8_t connectWF(String ssid, String pass) {
  WiFi.begin(ssid.c_str(), pass.c_str());
//  WiFi.begin("Siskotryas", "Gigadi750N82D");
  Serial.print("Connecting");
  for (uint8_t i=0;i<10;i++)
  {
    delay(1000);
    Serial.print(".");
    Serial.printf("Connection status: %d\n", WiFi.status());
    WiFi.printDiag(Serial);
    Serial.println();
//    Serial.println(millis());
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.print("Connected, IP address: ");
      Serial.println(WiFi.localIP());
      return 1;
    }
  }
    return 0;  
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    Blynk.run();
    delay(wait);
    if (modeChanged) { modeChanged = 0; return; }
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    Blynk.run();
    delay(wait);
    if (modeChanged) { modeChanged = 0; return; }
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    Blynk.run();
    delay(wait);
    if (modeChanged) { modeChanged = 0; return; }
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      Blynk.run();
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
      if (modeChanged) { modeChanged = 0; return; }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();
      Blynk.run();
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
      if (modeChanged) { modeChanged = 0; return; }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
