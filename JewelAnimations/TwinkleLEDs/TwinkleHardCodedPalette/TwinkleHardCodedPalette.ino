/*
Color Twinkle
By Kelsey Steven

Made to run twinkle effects for a single Neopixel Jewel.
*/

#include <Adafruit_NeoPixel.h>


// Constants for color palettes
// Used to determine allowed ranges for red, green, and blue values
const int GREEN_BLUES = 0;
const int RED_PURPLE_BLUES = 1;
const int YELLOW_ORANGE_REDS = 2;

// Constants for state values
const int OFF = 0;
const int BRIGHTENING = 1;
const int DARKENING = 2;

const int DATA_PIN = 3;  // Data pin for LEDs
const int TOTAL_LEDS = 7; // Total LEDs in array
const int COLOR_CHANGE_SPEED = 1; // Speed at which light intensity increases

const int NUM_COLORS = 10;  // Total colors in pre programmed array
const int GREEN_BLUE [NUM_COLORS][3] = { {0, 128, 102},
                                         {0, 153, 0},
                                         {0, 178, 255},    
                                         {0, 195, 250},
                                         {0, 200, 100},
                                         {0, 205, 50},
                                         {0, 255, 0},      
                                         {0, 255, 150},     
                                         {0, 255, 205},      
                                         {0, 255, 255} };

const int RED_BLUE_PURPLE [NUM_COLORS][3] = { {128, 0, 102},
                                              {153, 0, 0},
                                              {178, 0, 255},    
                                              {195, 0, 250},
                                              {200, 0, 100},
                                              {205, 0, 50},
                                              {255, 0, 0},      
                                              {255, 0, 150},     
                                              {255, 0, 205},      
                                              {255, 0, 255} };

const int RED_YELLOW_ORANGE [NUM_COLORS][3] = { {128, 102, 0},
                                                {153, 0, 0},
                                                {178, 255, 0},    
                                                {195, 250, 0},
                                                {200, 100, 0},
                                                {205, 50, 0},
                                                {255, 0, 0},      
                                                {255, 150, 0},     
                                                {255, 205, 0},      
                                                {255, 255, 0} };
  

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, DATA_PIN, NEO_GRB + NEO_KHZ800);

int lightPalette = 0;
int lightState[TOTAL_LEDS];
int previousColors[TOTAL_LEDS][3];
int finalColors[TOTAL_LEDS][3];

void setup() {
  // Seed random for determining if light should breath or stay off
  randomSeed(analogRead(0));

  // Neopixel initialization
  strip.begin();
}

void loop() {
  for(int index = 0; index < TOTAL_LEDS; index++) {
    // Handle each LED based on their light state
    switch(lightState[index]) {
      case OFF: {   
        handleLightOff(index);
        break;
      }
    
      case BRIGHTENING: {          
        handleBrightening(index);
        break;
      }
       
      case DARKENING: {  
        handleDarkening(index);
        break;
      }
    }
  }

  // Send Updates to lights
  strip.show();
  delay(10);  // Short delay before next cycle
}

void handleLightOff(int index) {  
  // Decide if light should turn on next cycle 
  lightState[index] = random(0, 2);
  
  // Pick a color if applicable
  if (lightState[index] == 1) {
    pickNewColor(index);
  }
}

void pickNewColor(int index) {
  int indexOfColor = random(0, NUM_COLORS);

  switch(lightPalette) {
    case GREEN_BLUES: {   
      setGreenBlueColor(index, indexOfColor);
      break;
    }
  
    case RED_PURPLE_BLUES: {          
      setRedPurpleBlueColor(index, indexOfColor);
      break;
    }
     
    case YELLOW_ORANGE_REDS: {  
      setYellowOrangeRedColor(index, indexOfColor);
      break;
    }
  }
}

void setGreenBlueColor(int index, int indexOfColor) {
  // Get random blue & green values from 0-HIGHEST_INTENSITY 
  updateFinalColor(index, GREEN_BLUE[indexOfColor][0], GREEN_BLUE[indexOfColor][1], GREEN_BLUE[indexOfColor][2]);
}

void setRedPurpleBlueColor(int index, int indexOfColor) {
  updateFinalColor(index, RED_BLUE_PURPLE[indexOfColor][0], RED_BLUE_PURPLE[indexOfColor][1], RED_BLUE_PURPLE[indexOfColor][2]);
}

void setYellowOrangeRedColor(int index, int indexOfColor) {
  updateFinalColor(index, RED_YELLOW_ORANGE[indexOfColor][0], RED_YELLOW_ORANGE[indexOfColor][1], RED_YELLOW_ORANGE[indexOfColor][2]);
}

void handleBrightening(int index) {
  // Brighten the light
  int maxRed = finalColors[index][0];
  int red = previousColors[index][0] + COLOR_CHANGE_SPEED;
  red = (red <= maxRed) ? red : maxRed;

  int maxGreen = finalColors[index][1];
  int green = previousColors[index][1] + COLOR_CHANGE_SPEED;
  green = (green <= maxGreen) ? green : maxGreen;

  int maxBlue = finalColors[index][2];
  int blue = previousColors[index][2] + COLOR_CHANGE_SPEED;
  blue = (blue <= maxBlue) ? blue : maxBlue;

  strip.setPixelColor(index, strip.Color(red, green, blue));

  // Update previous color
  updatePreviousColor(index, red, green, blue);
  
  // Check if light should start darken cycle next & update state if so
  if (red == maxRed && green == maxGreen && blue == maxBlue) {
    lightState[index] = DARKENING;
  }
}

void handleDarkening(int index) {
  // Darken the light
  int red = previousColors[index][0] - COLOR_CHANGE_SPEED;
  red = (red >= 0) ? red : 0;

  int green = previousColors[index][1] - COLOR_CHANGE_SPEED;
  green = (green >= 0) ? green : 0;

  int blue = previousColors[index][2] - COLOR_CHANGE_SPEED;
  blue = (blue >= 0) ? blue : 0;

  strip.setPixelColor(index, strip.Color(red, green, blue));
  
  // Update previous color
  updatePreviousColor(index, red, green, blue);
  
  // Check if light should turn off
  if (red == 0 && green == 0 && blue == 0) {
    lightState[index] = OFF;

    // Clear finalColors entry
    updateFinalColor(index, 0, 0, 0);
  }
}

// Set an color entry in finalColors array
void updateFinalColor(int index, int r, int g, int b) {
  finalColors[index][0] = r;
  finalColors[index][1] = g;
  finalColors[index][2] = b;
}

// Set an color entry in previousColors array
void updatePreviousColor(int index, int r, int g, int b) {
  previousColors[index][0] = r;
  previousColors[index][1] = g;
  previousColors[index][2] = b;
}

