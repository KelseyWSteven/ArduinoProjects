/*
Color Twinkle
By Kelsey Steven

Made to run twinkle effects for a single Neopixel Jewel.

Before running, always make sure START_INDEX, LED_PIN, and TOTAL_LEDS 
are set to the correct index, pin, and number of NeoPixels you're driving.

*/

#include <Adafruit_NeoPixel.h>


// Constants for color palettes
// Used to determine allowed ranges for red, green, and blue values
const int LIGHT_BLUE = 0;
const int GREEN_BLUE = 1;
const int RED_PURPLE_BLUE = 2;
const int YELLOW_ORANGE_RED = 3;
const int GREEN_PURPLE = 4;

// Constants for state values
const int OFF = 0;
const int BRIGHTENING = 1;
const int DARKENING = 2;

// Harware constants
const int LED_PIN = 12;  // Data pin for LEDs
const int TOTAL_LEDS = 7; // Total LEDs in array
const int START_INDEX = 1;  // Ignore Center LED due to metal housing. Use 0 for all 7 LEDs

// Color change constants
const int HIGHEST_INTENSITY = 200;  // May want to decrease this if LEDs are too bright. Max: 255
const int COLOR_CHANGE_SPEED = 1;   // Can increase for faster animation, but choppier color progression
const int DELAY_TIME = 20;   // Artificial delay between loop cycles. Can increase for slower animation

// Declare the Neopixel strip using settings for Jewel V7 RGB neopixels 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Light state tracking variables
int lightPalette = GREEN_PURPLE;
int lightState[TOTAL_LEDS];
int previousColors[TOTAL_LEDS][3];
int finalColors[TOTAL_LEDS][3];

void setup() {
  // Seed random for determining if light should breath or stay off
  randomSeed(analogRead(0));

  // Neopixel initialization
  strip.begin();

  // Turn off any LEDs before START_INDEX
  if (START_INDEX > 0) {
    for(int index = 0; index < START_INDEX; index++) {
      strip.setPixelColor(index, strip.Color(0, 0, 0));
    }
  }
}

void loop() {
  // Handle each LED based on their light state
  for(int index = START_INDEX; index < TOTAL_LEDS; index++) {
    
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
  delay(DELAY_TIME);  // Short delay before next cycle
}

void handleLightOff(int index) {  
  lightState[index] = BRIGHTENING;
  pickNewColor(index);

// Comment above & Uncomment below for more random off cycles

//  // Decide if light should turn on next cycle
//  let turnOn = random(0, 5); 
//  if (turnOn == 0) {
//      lightState[index] = BRIGHTENING;
//  }
//  
//  // Pick a color if applicable
//  if (lightState[index] == BRIGHTENING) {
//    pickNewColor(index);
//  }
}

void pickNewColor(int index) {
  switch(lightPalette) {

    case LIGHT_BLUE:    
      setLightBlueColor(index);
      break;
      
    case GREEN_BLUE:   
      setGreenBlueColor(index);
      break;
  
    case RED_PURPLE_BLUE:          
      setRedPurpleBlueColor(index);
      break;
      
    case YELLOW_ORANGE_RED:   
      setYellowOrangeRedColor(index);
      break;

    case GREEN_PURPLE:   
      setGreenPurpleColor(index);
      break;
  }
}

void setLightBlueColor(int index) {
  // Get random blue value with a min of 1, cutting out darkest blues 
  // (cuts out last 50 values if HIGHEST_INTENSITY is high enough to allow)
  int loweredMaxVal = HIGHEST_INTENSITY * 0.75;
  int blueMaxVal = loweredMaxVal - 50 + 1;
  blueMaxVal = (blueMaxVal > 1) ? blueMaxVal : loweredMaxVal + 1;
  
  updateFinalColor(index, random(0, 3), random(0, 3), random(5, blueMaxVal));
}

void setGreenBlueColor(int index) {
  // Get random blue & green values from 0-HIGHEST_INTENSITY 
  int maxVal =  HIGHEST_INTENSITY + 1;
  updateFinalColor(index, 0, random(0, maxVal), random(0, maxVal));
}

void setRedPurpleBlueColor(int index) {
  // Get random red & blue values from 0-HIGHEST_INTENSITY 
  int maxVal =  HIGHEST_INTENSITY + 1;
  updateFinalColor(index, random(0, maxVal), 0, random(0, maxVal));
}

void setYellowOrangeRedColor(int index) {
  // Use low values to prevent green colors
  int green = random(0, 6);

  // If green channel is over 0, ensure red channel is noticeably higher
  // to prevent creating colors more green than desired
  int redMinVal = green + 20;
  redMinVal = ((green > 0) && (redMinVal <=  HIGHEST_INTENSITY)) ? redMinVal : 0;

  // Cuts out last 75 red values if HIGHEST_INTENSITY is high enough to allow
  int redMaxVal = HIGHEST_INTENSITY - 75 + 1;
  redMaxVal = (redMaxVal >= redMinVal) ? redMaxVal : HIGHEST_INTENSITY + 1;

  updateFinalColor(index, random(redMinVal, redMaxVal), green, 0);
}

void setGreenPurpleColor(int index) {
  // Use rand for 50/50 chance green or purple      
  if (random(0, 2) == 0) {  // Green
    // Create green with very low red & blue values for variation
    int greenRand = random(15, HIGHEST_INTENSITY + 1);
    updateFinalColor(index, random(0, 5), greenRand, random(0, 5));
    
  } else {  // Purple
    int randMax = HIGHEST_INTENSITY + 1;
    const int red = random(1, randMax);

    // Want red & blue values within changeAmt of each other in either direction
    const int maxRBDiff = 40;
    const int blueMin = ((red - maxRBDiff) > 0) ? (red - maxRBDiff) : 0;
    const int blueMax = ((red + maxRBDiff) < randMax) ? (red + maxRBDiff) :randMax;
    const int blue = random(blueMin, blueMax);
  
    updateFinalColor(index, red, 0, blue);
  }
}

void handleBrightening(int index) {
  // Get new RGB values
  int maxRed = finalColors[index][0];
  int red = getBrighteningValue(maxRed, previousColors[index][0]);

  int maxGreen = finalColors[index][1];
  int green = getBrighteningValue(maxGreen, previousColors[index][1]);

  int maxBlue = finalColors[index][2];
  int blue = getBrighteningValue(maxBlue, previousColors[index][2]);

  // Update LEDs & tracking array
  strip.setPixelColor(index, strip.Color(red, green, blue));
  updatePreviousColor(index, red, green, blue);
  
  // Check if light should start darken cycle next & update state if so
  if (red == maxRed && green == maxGreen && blue == maxBlue) {
    lightState[index] = DARKENING;
  }
}

// Return the value of previousColor increased by COLOR_CHANGE_SPEED 
// while making sure we do not exceed maxColorValue
int getBrighteningValue(int maxColorValue, int previousColor) {
  int newColor = previousColor + COLOR_CHANGE_SPEED;
  newColor = (newColor <= maxColorValue) ? newColor : maxColorValue;

  return newColor;
}

void handleDarkening(int index) {
  int prevR = previousColors[index][0];
  int prevG = previousColors[index][1];
  int prevB = previousColors[index][2];

  // Get new RGB values
  int red = prevR;
  int green = prevG;
  int blue = prevB;

  if ((prevR >= prevG) && (prevR >= prevB)) {
    red = getDarkeningValue(prevR);
  }

  if ((prevG >= prevR) && (prevG >= prevB)) {
    green = getDarkeningValue(prevG);
  }

  if ((prevB >= prevR) && (prevB >= prevG)) {
    blue = getDarkeningValue(prevB);
  }

  // Update LEDs & tracking array
  strip.setPixelColor(index, strip.Color(red, green, blue));
  updatePreviousColor(index, red, green, blue);
  
  // Check if light turned off
  if (red == 0 && green == 0 && blue == 0) {
    handleLightOff(index);

//    lightState[index] = OFF;
//    updateFinalColor(index, 0, 0, 0);
  }
}

// Decrease the value by COLOR_CHANGE_SPEED while making sure we do not go below 0
int getDarkeningValue(int previousColor) {
  int newColor = previousColor - COLOR_CHANGE_SPEED;
  newColor = (newColor > 0) ? newColor : 0;

  return newColor;
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

