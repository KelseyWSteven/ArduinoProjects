/*
Color Twinkle
By Kelsey Steven

Made to run twinkle effects for 2 Neopixel Jewels, but can be 
adapted to other form factors and numbers of NeoPixels.

Before running, always make sure `DATA_PIN` and `TOTAL_LEDS` are set
to the correct pin and number of NeoPixels you're driving.

*/

#include <Adafruit_NeoPixel.h>


// Constants for color palettes
// Used to determine allowed ranges for red, green, and blue values
const int GREEN_BLUE = 0;
const int RED_PURPLE_BLUE = 1;
const int YELLOW_ORANGE_RED = 2;

// Constants for LED's state
// Used to determine how to update an LED each time loop() is called
const int OFF = 0;
const int BRIGHTENING = 1;
const int DARKENING = 2;

// Harware constants
const int DATA_PIN = 3;   // Neopixel array data pin
const int TOTAL_LEDS = 14; // total LEDs in the array

// Color change constants
const int HIGHEST_INTENSITY = 255;  // May want to decrease this if LEDs are too bright
const int COLOR_CHANGE_SPEED = 1;   // Can increase for faster animation, but choppier color progression
const int DELAY_TIME = 15;   // Artificial delay between loop cycles. Can increase for slower animation


// Declare the Neopixel strip using settings for Jewel V7 RGB neopixels 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

// Light state tracking variables
//int lightPalette = 1;       // Currently selected color palette
int lightState[TOTAL_LEDS]; // State for each LED (OFF/BRIGHTENING/DARKENING)
int previousColors[TOTAL_LEDS][3];  // Last color each LED was set to
int finalColors[TOTAL_LEDS][3];     // Holds the color each LED is currently assigned


void setup() {
  // Seed random for color picking
  randomSeed(analogRead(0));

  // Neopixel initialization
  strip.begin();
}

void loop() {
  // Handle each LED based on their light state
  for(int index = 0; index < TOTAL_LEDS; index++) {
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

  strip.show();       // Send Updates to lights
  delay(DELAY_TIME);  // Short delay before next cycle
}

void handleLightOff(int index) {
  // Turn LED off
  strip.setPixelColor(index, strip.Color(0, 0, 0));
    
   // No need to update previousColors, will have been set to 0s in handleDarkening()
   // Decide if light should turn on next cycle 
  lightState[index] = random(0, 2);
  
  // Pick a color if applicable
  if (lightState[index] == 1) {
    pickNewColor(index);  
  }
}

void pickNewColor(int index) {
  int lightPalette = (index >= (TOTAL_LEDS / 2)) ? 0 : 1;
  
  // Get random numbers for new color
  int randOne = random(0, HIGHEST_INTENSITY + 1);
  int randTwo = random(0, HIGHEST_INTENSITY + 1);

  switch(lightPalette) {
    case GREEN_BLUE: {   
      updateFinalColor(index, 0, randOne, randTwo);
      break;
    }
  
    case RED_PURPLE_BLUE: {  
      // Min 1 for red & blue to remove most intense red and blue hues
      randOne = random(1, HIGHEST_INTENSITY + 1);
      randTwo = random(1, HIGHEST_INTENSITY + 1);
              
      updateFinalColor(index, randOne, 0, randTwo);
      break;
    }
     
    case YELLOW_ORANGE_RED: { 
      // Use low values to prevent green colors
      randTwo = random(0, 7);

      // If green channel is over 0, ensure red channel is noticeably higher
      // to prevent creating colors more green than desired
      int randOneStart = ((randTwo > 0) && (randTwo + 15 <=  HIGHEST_INTENSITY)) ? randTwo + 15 : 0;
      randOne = random(randOneStart, HIGHEST_INTENSITY + 1);
      
      updateFinalColor(index, randOne, randTwo, 0);
      break;
    }
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
  // Get new RGB values
  int red = getDarkeningValue(previousColors[index][0]);
  int green = getDarkeningValue(previousColors[index][1]);
  int blue = getDarkeningValue(previousColors[index][2]);

  // Update LEDs & tracking array
  strip.setPixelColor(index, strip.Color(red, green, blue));
  updatePreviousColor(index, red, green, blue);
  
  // Check if light should turn off
  if (red == 0 && green == 0 && blue == 0) {
    lightState[index] = OFF;

    // Clear finalColors entry
    updateFinalColor(index, 0, 0, 0);
  }
}

// Decrease the value by COLOR_CHANGE_SPEED while making sure we do not go below 0
int getDarkeningValue(int previousColor) {
  int newColor = previousColor - COLOR_CHANGE_SPEED;
  newColor = (newColor >= 0) ? newColor : 0;

  return newColor;
}

void updateFinalColor(int index, int r, int g, int b) {
  finalColors[index][0] = r;
  finalColors[index][1] = g;
  finalColors[index][2] = b;
}

void updatePreviousColor(int index, int r, int g, int b) {
  previousColors[index][0] = r;
  previousColors[index][1] = g;
  previousColors[index][2] = b;
}

