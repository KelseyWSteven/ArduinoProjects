/*
AlternatingColorFadeInOut
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
const int LED_PIN = 3;  // Data pin for LEDs
const int TOTAL_LEDS = 7; // Total LEDs in array
const int START_INDEX = 1;  // Ignore Center LED due to metal housing. Use 0 for all 7 LEDs

// Color change constants
const int HIGHEST_INTENSITY = 200;  // May want to decrease this if LEDs are too bright. Max: 255
const int COLOR_CHANGE_SPEED = 1;   // Can increase for faster animation, but choppier color progression
const int DELAY_TIME = 20;  // Artificial delay between loop cycles. Can increase for slower animation
const int OFF_VALUE = 10;   // Intensity value to wait at if one color decrease cycle ends before the other

// Declare the Neopixel strip using settings for Jewel V7 RGB neopixels 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Light state tracking variables
int lightPalette = GREEN_BLUE;
int lightState[TOTAL_LEDS];
int previousColors[TOTAL_LEDS][3];

int colorOne[3];
int colorTwo[3];
bool isEvenColorOne = true;


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
  // Determine if all LEDs are off
  bool allOff = true;
  for(int currIndex = START_INDEX; currIndex < TOTAL_LEDS; currIndex++) {
    allOff = ((lightState[currIndex] == OFF) && allOff);
  }

  // If LEDs are off
  if (allOff == true) {
    // On first index pick new colors & start brightening
    if (index == START_INDEX) {
      // Swap value of isEvenColorOne so LEDs change color
      isEvenColorOne = !isEvenColorOne;      
      pickNewColor(index);
            
      // If LED is not off, turn it off to start new color from 0
      strip.setPixelColor(index, strip.Color(0, 0, 0));
      lightState[index] = BRIGHTENING;
   }
  }
  
  // If index is past START_INDEX
  if (index > START_INDEX) {

   // Check if all indices before this are brightening
   int allPrevBrightening = true;
    for(int currIndex = (index - 1); currIndex >= START_INDEX; currIndex--) {
      allPrevBrightening = ((lightState[currIndex] == BRIGHTENING) && allPrevBrightening);
    }

    // If all indices before this are brightening, turn on
    if (allPrevBrightening == true) {
      // If LED is not off, turn it off to start new color from 0
      strip.setPixelColor(index, strip.Color(0, 0, 0));
      lightState[index] = BRIGHTENING;
    }
  }
}

void handleBrightening(int index) {
  bool isIndexEven = (index % 2 == 0);
  bool useColorOne = (isIndexEven && isEvenColorOne) || (!isIndexEven && !isEvenColorOne);

  // Get new RGB values
  int maxRed = (useColorOne) ? colorOne[0] : colorTwo[0];
  int red = getBrighteningValue(maxRed, previousColors[index][0]);

  int maxGreen = (useColorOne) ? colorOne[1] : colorTwo[1];
  int green = getBrighteningValue(maxGreen, previousColors[index][1]);

  int maxBlue = (useColorOne) ? colorOne[2] : colorTwo[2];
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
  if (red <= OFF_VALUE && green <= OFF_VALUE && blue <= OFF_VALUE) {
    lightState[index] = OFF;
  }
}

// Decrease the value by COLOR_CHANGE_SPEED while making sure we do not go below 0
int getDarkeningValue(int previousColor) {
  int newColor = previousColor - COLOR_CHANGE_SPEED;
  newColor = (newColor > OFF_VALUE) ? newColor : OFF_VALUE;

  return newColor;
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
  
  updateColorOne(random(0, 3), random(0, 3), random(5, highestBlueVal));
  updateColorTwo(random(0, 3), random(0, 3), random(5, highestBlueVal));
}

void setGreenBlueColor(int index) {
  // Get random blue & green values from 0-HIGHEST_INTENSITY 
  int maxVal =  HIGHEST_INTENSITY + 1;
  
  int lowerVal = random(0, maxVal / 2);
  int higherStart = (lowerVal + 20) < maxVal ? (lowerVal + 20) : maxVal;
  updateColorOne(0, lowerVal, random(higherStart, maxVal));

  lowerVal = random(0, maxVal / 2);
  higherStart = (lowerVal + 20) < maxVal ? (lowerVal + 20) : maxVal;
  updateColorTwo(0, random(higherStart, maxVal), lowerVal);
}

void setRedPurpleBlueColor(int index) {
  int maxVal =  HIGHEST_INTENSITY + 1;
  updateColorOne(random(0, maxVal), 0, random(0, maxVal));
  updateColorTwo(random(0, maxVal), 0, random(0, maxVal));
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
  
  updateColorOne(random(redMinVal, redMaxVal), green, 0);

  green = random(0, 6);
  redMinVal = green + 20;
  redMinVal = ((green > 0) && (redMinVal <=  HIGHEST_INTENSITY)) ? redMinVal : 0;
  updateColorTwo(random(redMinVal, redMaxVal), green, 0);
}

void setGreenPurpleColor(int index) {
  // Green: Create with very low red & blue values for variation
  int greenRand = random(15, HIGHEST_INTENSITY + 1);
  updateColorOne(random(0, 5), greenRand, random(0, 5));

  // Purple: Want red & blue values within changeAmt of each other in either direction
  int randMax = HIGHEST_INTENSITY + 1;
  const int red = random(1, randMax);
  const int maxRBDiff = 40;
  const int blueMin = ((red - maxRBDiff) > 0) ? (red - maxRBDiff) : 0;
  const int blueMax = ((red + maxRBDiff) < randMax) ? (red + maxRBDiff) :randMax;
  const int blue = random(blueMin, blueMax);
  updateColorTwo(red, 0, blue);    
}

void updateColorOne(int r, int g, int b) {
  colorOne[0] = r;
  colorOne[1] = g;
  colorOne[2] = b;
}

void updateColorTwo(int r, int g, int b) {
  colorTwo[0] = r;
  colorTwo[1] = g;
  colorTwo[2] = b;
}

// Set an color entry in previousColors array
void updatePreviousColor(int index, int r, int g, int b) {
  previousColors[index][0] = r;
  previousColors[index][1] = g;
  previousColors[index][2] = b;
}

