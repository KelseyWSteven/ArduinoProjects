/*
JewelTempSensor
By Kelsey Steven

Made to run twinkle effects for a single Neopixel Jewel in response to the wearer's  
body temperature, but can be adapted to other form factors and numbers of NeoPixels.

Before running, always make sure TEMP_PIN, TEMP_PIN_VOLTAGE, LED_PIN, and TOTAL_LEDS 
are set to the correct pins, voltage, and number of NeoPixels you're driving.

*/

#include <Adafruit_NeoPixel.h>


/* ~~~~~~~~ Constants ~~~~~~~~ */

// Temperature constants
const float COLD_THRESHOLD = 0.85;
const float COOL_THRESHOLD = 0.95;
const float BASE_THRESHOLD = 1.05;
const float WARM_THRESHOLD = 1.15;
const float HOT_THRESHOLD = 2.0;
//const float COLD_THRESHOLD = 0.12;
//const float COOL_THRESHOLD = 0.15;
//const float BASE_THRESHOLD = .18;
//const float WARM_THRESHOLD = .20;
//const float HOT_THRESHOLD = .22;


// Color palette constants
// Used to determine allowed ranges for red, green, and blue values
const int LIGHT_BLUE = 0;
const int GREEN_BLUE = 1;
const int RED_PURPLE_BLUE = 2;
const int YELLOW_ORANGE_RED = 3;
const int RED = 4;

// LED state constants
// Used to determine how to update an LED each time loop() is called
const int OFF = 0;
const int BRIGHTENING = 1;
const int DARKENING = 2;

// Harware constants
const int TEMP_PIN = 1;
const int TEMP_PIN_VOLTAGE = 5;  // Voltage of power pin. For 5v use 5.0, for 3.3v, use 3.3
const int LED_PIN = 12;     // Neopixel array data pin
const int TOTAL_LEDS = 7;   // total LEDs in the array
const int LED_START_INDEX = 1;  // Use 0 for all LEDs. Set to 1 to skip center LED (saves power if occluded by housing)

// Color change constants
const int HIGHEST_INTENSITY = 240;  // May want to decrease this if LEDs are too bright. Max: 255
const int COLOR_CHANGE_SPEED = 1;   // Can increase for faster animation, but choppier color progression
const int DELAY_TIME = 15;   // Artificial delay between loop cycles. Can increase for slower animation
const int TEMP_CHECK_INTERVAL = 5000;

/* ~~~~~~~~ Variables ~~~~~~~~ */

// Light state tracking variables
int lightPalette = 1;       // Currently selected color palette
int lightState[TOTAL_LEDS]; // State for each LED (OFF/BRIGHTENING/DARKENING)
int previousColors[TOTAL_LEDS][3];  // Last color each LED was set to
int finalColors[TOTAL_LEDS][3];     // Holds the color each LED is currently assigned

// Temp timer
unsigned long timer = 0;

// Declare the Neopixel strip using settings for RGB neopixels (such as Jewel V7)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);


/* ~~~~~~~~ Constants ~~~~~~~~ */

void setup() {
  // Init serial for debugging if board allows
//  Serial.begin(9600);

  // Seed random for color picking
  randomSeed(analogRead(0));
  
  // Neopixel initialization
  strip.begin();
}


/* ~~~~~~~~ Functions ~~~~~~~~ */

void loop() {
  // Set palette for new colors during this loop
  setPaletteByTemp();  

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

void setPaletteByTemp() {
  // Only update palette every TEMP_CHECK_INTERVAL ms
  if (millis() - timer >= TEMP_CHECK_INTERVAL) {

    float voltage = getVoltage();
//    Serial.println(voltage);
  
    if (voltage < COLD_THRESHOLD){
      lightPalette = LIGHT_BLUE;
      
    } else if (voltage >= COOL_THRESHOLD && voltage < BASE_THRESHOLD) {
      lightPalette = GREEN_BLUE;
      
    } else if (voltage >= BASE_THRESHOLD && voltage < WARM_THRESHOLD) {
      lightPalette = RED_PURPLE_BLUE;
      
    } else if (voltage >= WARM_THRESHOLD && voltage < HOT_THRESHOLD) {
      lightPalette = YELLOW_ORANGE_RED;
      
    } else if (voltage >= HOT_THRESHOLD) {
      lightPalette = RED;
      
    }

    timer = millis();   // Reset timer
  }
}

float getVoltage() {
  // Convert reading to voltage.
  float voltage = analogRead(TEMP_PIN) * TEMP_PIN_VOLTAGE;
  return (voltage / 1024.0); 
}

void handleLightOff(int index) {
  // Decide if light should turn on next cycle. 1 in 5 chance 
  if (random(0, 4) == 0) {
    lightState[index] = 1;
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

    case RED:  
      setRedColor(index);
      break;
  }
}

void setLightBlueColor(int index) {
  // Get random blue value with a min of 1, cutting out darkest blues 
  // (cuts out last 50 values if HIGHEST_INTENSITY is high enough to allow)
  int loweredHighestIntensity = HIGHEST_INTENSITY * 0.75;
  int loweredBlueMaxVal = loweredHighestIntensity - 50 + 1;
  int highestBlueVal = (loweredBlueMaxVal > 1) ? loweredBlueMaxVal : loweredHighestIntensity + 1;
  
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

