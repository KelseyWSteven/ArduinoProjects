/*
CycleFadeInOut
By Kelsey Steven

Made to run effects for a single Neopixel Jewel. 

Turns on each LED with a delay from the previous, starting at low values and increasing to max brightness. 
LEDs then begin to fade out in the opposite sequence, each LED begins to darken with a delay from the previous.

Before running, always make sure START_INDEX, LED_PIN, and TOTAL_LEDS 
are set to the correct index, pin, and number of NeoPixels you're driving.

*/

#include <Adafruit_NeoPixel.h>

/* ~~~~~~~~~~ Constants ~~~~~~~~~~ */

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
const int LED_PIN = 12;    // Data pin for LEDs
const int TOTAL_LEDS = 7;   // Total LEDs in array
const int START_INDEX = 1;  // Ignore Center LED due to metal housing. Use 0 for all 7 LEDs

// Color change constants
const int HIGHEST_INTENSITY = 200;  // May want to decrease this if LEDs are too bright. Max: 255
const int COLOR_CHANGE_SPEED = 1;   // Can increase for faster animation, but choppier color progression
const int LOOP_DELAY_TIME = 20;     // Artificial delay between loop cycles in ms. Can increase for slower animation
const int LED_STAGGER_TIME = 50;   // Time in msto stagger LED brightening/darkening cycles

/* ~~~~~~~~~~ Variables ~~~~~~~~~~ */

// Declare the Neopixel strip using settings for Jewel V7 RGB neopixels 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Light state tracking variables
int lightPalette = GREEN_BLUE;
int lightState[TOTAL_LEDS];
int previousColors[TOTAL_LEDS][3];
int finalColors[TOTAL_LEDS][3];
unsigned long startTimer = 0;
unsigned long endTimer = 0;

/* ~~~~~~~~~~ Functions ~~~~~~~~~~ */

void setup() {
  // Seed random for determining if light should breath or stay off
  randomSeed(analogRead(0));

  // Neopixel initialization
  strip.begin();

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
  delay(LOOP_DELAY_TIME);  // Short delay before next cycle
}

void lightCycle() {
  
}

void handleLightOff(int index) {
  // If we are on the first LED
  if (index == START_INDEX) {
    lightState[index] = 1;
    pickNewColor(index);
    startTimer = millis();  // Reset timer
    
  } else if (index > START_INDEX) {
    // If first LED is brightening
    if (lightState[START_INDEX] == BRIGHTENING) {

      // Check if enough time has passed to turn on this LED
      long intervalForIndex = LED_STAGGER_TIME * index;      
      if (millis() - startTimer >= intervalForIndex) {
        lightState[index] = 1;
        // Set to same color as first LED
        updateFinalColor(index, finalColors[START_INDEX][0], finalColors[START_INDEX][1], finalColors[START_INDEX][2]);
      }
    }
  } 
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
  int loweredHighestIntensity = HIGHEST_INTENSITY * 0.75;
  int loweredBlueMaxVal = loweredHighestIntensity - 50 + 1;
  int highestBlueVal = (loweredBlueMaxVal > 1) ? loweredBlueMaxVal : loweredHighestIntensity + 1;
  
  updateFinalColor(index, random(0, 3), random(0, 3), random(5, highestBlueVal));
}

void setGreenBlueColor(int index) {
  // Get random blue & green values from 0-HIGHEST_INTENSITY 
  updateFinalColor(index, 0, random(0, HIGHEST_INTENSITY + 1), random(0, HIGHEST_INTENSITY + 1));
}

void setRedPurpleBlueColor(int index) {
  // Get random red & blue values from 0-HIGHEST_INTENSITY 
  updateFinalColor(index, random(0, HIGHEST_INTENSITY + 1), 0, random(0, HIGHEST_INTENSITY + 1));
}

void setYellowOrangeRedColor(int index) {
  // Use low values to prevent green colors
  int randGreen = random(0, 6);

  // If green channel is over 0, ensure red channel is noticeably higher
  // to prevent creating colors more green than desired
  const int increasedRedStart = randGreen + 20;
  const int redRandStart = ((randGreen > 0) && (increasedRedStart <=  HIGHEST_INTENSITY)) ? increasedRedStart : 0;

  // Cuts out last 75 red values if HIGHEST_INTENSITY is high enough to allow
  const int loweredRedMaxVal = HIGHEST_INTENSITY - 75 + 1;
  const int redRandMax = (loweredRedMaxVal >= redRandStart) ? loweredRedMaxVal : HIGHEST_INTENSITY + 1;

  updateFinalColor(index, random(redRandStart, redRandMax), randGreen, 0);
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
    
    // If we are on last LED, start the darkening cycle
    if (index == (TOTAL_LEDS - 1)) {
      lightState[index] = DARKENING;
      endTimer = millis();  // Reset timer
      
    } else {
      // If the last LED is darkening, see if we should start to darken
      if (lightState[TOTAL_LEDS - 1] == DARKENING) {
        
        // Check the timer and see if LED should start darkening
        long intervalForIndex = LED_STAGGER_TIME * (TOTAL_LEDS - index - 1);      
        if (millis() - endTimer >= intervalForIndex) {
          lightState[index] = DARKENING;
        }
      }
    }
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
  
  // Check if light should turn off
  if (red == 0 && green == 0 && blue == 0) {
    handleLightOff(index);
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

