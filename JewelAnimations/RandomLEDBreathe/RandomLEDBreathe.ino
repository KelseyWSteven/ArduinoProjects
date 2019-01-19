#include <Adafruit_NeoPixel.h>

const int PIN = 12;
const int TOTAL_LEDS = 7;
const int BREATHE_SPEED = 1;
const int BREATHE_DELAY = 200;
const int NUM_COLORS = 10;
const int GREEN_COLORS [NUM_COLORS][3] = { {102, 204, 0},
                                           {0, 153, 0},
                                           {153, 255, 255},
                                           {0, 128, 102},
                                           {178, 255, 102},
                                           {204, 255, 153},
                                           {0, 204, 102},
                                           {0, 204, 204},
                                           {102, 255, 178},
                                           {102, 178, 255} };           

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // Seed random for determining if light should breath or stay off
  randomSeed(analogRead(0));

  // Neopixel initialization
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {
  breathAll(GREEN_COLORS);

  delay(BREATHE_DELAY / BREATHE_SPEED);
}

// int possibleColors[][3] : Assumes an array of `int [7][3]`
void breathAll(int possibleColors[][3]) {
  // Init all LEDs & colors to off for breathe cycle
  bool onForBreath[TOTAL_LEDS] = {true, true, true, true, true, true, true};
  int colorsForBreath[TOTAL_LEDS][3] = {{0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0},
                                        {0, 0, 0}}; 

  int indexOfColor = random(0, NUM_COLORS);
                                         
  // Decide which LEDs will breathe and in which color                                      
  for(int index = 0; index < TOTAL_LEDS; index++) {
    colorsForBreath[index][0] = possibleColors[indexOfColor][0];
    colorsForBreath[index][1] = possibleColors[indexOfColor][1];
    colorsForBreath[index][2] = possibleColors[indexOfColor][2];
  }

  performBreatheCycle(onForBreath, colorsForBreath);
}

//// int possibleColors[][3] : Assumes an array of `int [7][3]`
//void breathAll(int possibleColors[][3]) {
//  // Init all LEDs & colors to off for breathe cycle
//  bool onForBreath[TOTAL_LEDS] = {false, false, false, false, false, false, false};
//  int colorsForBreath[TOTAL_LEDS][3] = {{0, 0, 0},
//                                        {0, 0, 0},
//                                        {0, 0, 0},
//                                        {0, 0, 0},
//                                        {0, 0, 0},
//                                        {0, 0, 0},
//                                        {0, 0, 0}}; 
//                                        
//  // Decide which LEDs will breathe and in which color                                      
//  for(int index = 0; index < TOTAL_LEDS; index++) {
//    // Randomly get a 0 or 1 to decide if LED is on or off for this cycle
//    bool isIndexOn = (random(0, 2) == 0);
//    onForBreath[index] = isIndexOn;
//
//    // Randomly choose a color from palette only for the LEDs that will breathe
//    if (isIndexOn == true) {
//      int indexOfColor = random(0, NUM_COLORS);
//      colorsForBreath[index][0] = possibleColors[indexOfColor][0];
//      colorsForBreath[index][1] = possibleColors[indexOfColor][1];
//      colorsForBreath[index][2] = possibleColors[indexOfColor][2];
//    }
//  }
//
//  performBreatheCycle(onForBreath, colorsForBreath);
//}

// bool onForBreath[] : Assumes an array of `int [7]`
// int colorsForBreath[][3] : Assumes an array of `int [7][3]`
void performBreatheCycle(bool onForBreath[], int colorsForBreath[][3]) {
  performAnimation(true, onForBreath, colorsForBreath);
  performAnimation(false, onForBreath, colorsForBreath);
}

// bool isIncreasing : Whether light intensity should be increasing
// bool onForBreath[] : Assumes an array of `int [7]`
// int colorsForBreath[][3] : Assumes an array of `int [7][3]`
void performAnimation(bool isIncreasing, bool onForBreath[], int colorsForBreath[][3]) {
  int speedToUse = isIncreasing ? BREATHE_SPEED : -BREATHE_SPEED;
  int startingIntensity = isIncreasing ? 0 : 255;

  for (int intensity = startingIntensity; (intensity <= 255) && (intensity >= 0) ; intensity += speedToUse) {
    for(int index = 0; index < TOTAL_LEDS; index++) {
      
      // LED is on, set color
     if (onForBreath[index] == true) {
        strip.setPixelColor(index, getColorForIndex(intensity, colorsForBreath[index]));
        
      } else {
        // LED is not breathing, set to off
        strip.setPixelColor(index, strip.Color(0, 0, 0));
      }
    }
    
    strip.show();
    delay(10);
  }
}

// int intensity : assumes integer 0-255
// int colorForBreath[] : assumes int[3]
uint32_t getColorForIndex(int intensity, int colorForBreath[]) {
  // For colors, ensure it never goes over max r, g, or b for color
  int red = (intensity <= colorForBreath[0]) ? intensity : colorForBreath[0];
  int green = (intensity <= colorForBreath[1]) ? intensity : colorForBreath[1];
  int blue = (intensity <= colorForBreath[2]) ? intensity : colorForBreath[2];
        
  return strip.Color(red, green, blue);
}

