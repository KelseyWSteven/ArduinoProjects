/*
JewelButton
By Kelsey Steven

Use a button to flip through various colored twinkle effects for a single Neopixel Jewel. 
Settings (in order): OFF > GREEN_BLUE > RED_PURPLE_BLUE > YELLOW_ORANGE_RED > (back to OFF)

Can be adapted to other form factors and numbers of NeoPixels.
With this code, each LED must hold at least 7 

Before running, always make sure BUTTON_PIN, LED_PIN, and TOTAL_LEDS 
are set to the correct pins, and number of NeoPixels you're driving.

*/

#include <Adafruit_NeoPixel.h>


/* ~~~~~~~~~~ Constants ~~~~~~~~~~ */

// Constants for color palettes
// Used to determine allowed ranges for red, green, and blue values
const int LIGHT_BLUE = 1;
const int GREEN_BLUE = 2;
const int RED_PURPLE_BLUE = 3;
const int YELLOW_ORANGE_RED = 4;
const int RED = 5;

// This must be the total number of light color settings 
// (listed above) + 1 more for `Off` setting
// If you add or remove settings, change this as applicable
const int TOTAL_SETTINGS = 6;   

// Constants for LED's state
// Used to determine how to update an LED each time loop() is called
const int OFF = 0;
const int BRIGHTENING = 1;
const int DARKENING = 2;

// Harware constants
const int PRESS_LENGTH = 200; // How long a button press neeeds to be (in ms) to trigger an action 
const int BUTTON_PIN = 2;   // Push button data pin
const int LED_PIN = 3;      // Neopixel array data pin
const int TOTAL_LEDS = 7;   // total LEDs in the array

// Color change constants
const int HIGHEST_INTENSITY = 255;  // May want to decrease this if LEDs are too bright
const int COLOR_CHANGE_SPEED = 1;   // Can increase for faster animation, but choppier color progression
const int DELAY_TIME = 15;   // Artificial delay between loop cycles. Can increase for slower animation

/* ~~~~~~~~~~ Variables ~~~~~~~~~~ */

// Declare the Neopixel strip using settings for Jewel V7 RGB neopixels 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Button state tracking variables
int currButtonState = 0;  // Currently selected color palette or Off (0)
int prevButtonState = 0;  // State of button on previous loop
int currentButtonPressLength = 0;  // Length of the current button press (when applicable)

// Light state tracking variables
int lightState[TOTAL_LEDS];   // State for each LED (OFF/BRIGHTENING/DARKENING)
int finalColors[TOTAL_LEDS][3];     // Holds the color each LED is currently assigned
int previousColors[TOTAL_LEDS][3];  // Last color each LED was set to

/* ~~~~~~~~~~ Functions ~~~~~~~~~~ */

void setup() {
  // Seed random for color picking
  randomSeed(analogRead(0));

  // Initialize button
  pinMode(BUTTON_PIN, INPUT);

  // Neopixel initialization
  strip.begin();
}

void loop() {
  // Only run light code if lights are on
  if (currButtonState > OFF) {

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

  updateButtonState();
}

void updateButtonState() {
  // Get current button state
  int newButtonState = digitalRead(BUTTON_PIN);

  // Button was pressed or released
  if (prevButtonState != newButtonState) {
    
    // The button was pressed down, so start the press counter
    if (newButtonState == HIGH) {   
      currentButtonPressLength = DELAY_TIME;

    // The button was released
    } else {  
      // We've held it long enough to take action
      if (currentButtonPressLength >= PRESS_LENGTH) {
        
        // Cycle to next color palette or turn `Off`
        // Increases currButtonState by 1 or cycles back to 0
        currButtonState = (currButtonState + 1) % TOTAL_SETTINGS;

        // If setting just changed to `Off`, update all LEDS to off
        if (currButtonState == 0) {
          for(int index = 0; index < TOTAL_LEDS; index++) {
            strip.setPixelColor(index, strip.Color(0, 0, 0));
          }

          strip.show();       // Send Updates to lights
          delay(DELAY_TIME);  // Short delay before next cycle
        }
      }

      // Clear buton press timer
      currentButtonPressLength = 0;
    }
    
  // Button state did not change since last loop  
  } else {  
    // If button is pressed, increase the press counter
    if (prevButtonState == HIGH) {
      currentButtonPressLength += DELAY_TIME;
    }
  }
  
  // Update prevButtonState variable
  prevButtonState = newButtonState;
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

/* ~~~~~~~~ Color Picking Functions ~~~~~~~~ */

void pickNewColor(int index) {
  switch(currButtonState) {
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

    case RED:  
      setRedColor(index);
      break;
  }
}

void setLightBlueColor(int index) {
  // Get random blue value with a min of 1, cutting out darkest blues 
  // (cuts out last 50 values if HIGHEST_INTENSITY is high enough to allow)
  int loweredBlueEndVal = HIGHEST_INTENSITY - 50 + 1;
  int highestBlueVal = (loweredBlueEndVal > 1) ? loweredBlueEndVal : HIGHEST_INTENSITY + 1;
  
  updateFinalColor(index, 0, 0, random(1, highestBlueVal));
}

void setGreenBlueColor(int index) {
  // Get random blue & green values from 0-HIGHEST_INTENSITY 
  updateFinalColor(index, 0, random(0, HIGHEST_INTENSITY + 1), random(0, HIGHEST_INTENSITY + 1));
}

void setRedPurpleBlueColor(int index) {
  int loweredBlueEndVal = HIGHEST_INTENSITY - 50 + 1;
  int highestBlueVal = (loweredBlueEndVal > 1) ? loweredBlueEndVal : HIGHEST_INTENSITY + 1;
   
  updateFinalColor(index, random(1, HIGHEST_INTENSITY + 1), 0, highestBlueVal);
}

void setYellowOrangeRedColor(int index) {
  // Use low values to prevent green colors
  int randGreen = random(0, 6);

  // If green channel is over 0, ensure red channel is noticeably higher
  // to prevent creating colors more green than desired
  const int increasedRedStart = randGreen + 15;
  const int randRedStart = ((randGreen > 0) && (increasedRedStart <=  HIGHEST_INTENSITY)) ? increasedRedStart : 0;

  // Cuts out last 75 red values if HIGHEST_INTENSITY is high enough to allow
  const int loweredRedMaxVal = HIGHEST_INTENSITY - 75 + 1;
  const int randRedMax = (loweredRedMaxVal >= randRedStart) ? loweredRedMaxVal : HIGHEST_INTENSITY + 1;

  updateFinalColor(index, random(randRedStart, randRedMax), randGreen, 0);
}

void setRedColor(int index) {
  // Get random red value with a min of 50   
  updateFinalColor(index, random(50, HIGHEST_INTENSITY + 1), 0, 0);
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

