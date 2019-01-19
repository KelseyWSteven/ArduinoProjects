#include <Adafruit_NeoPixel.h>


// Harware constants
const int TEMP_PIN = 1;
const int LED_PIN = 3;   // Neopixel array data pin
const int TOTAL_LEDS = 14; // total LEDs in the array

const int TIME_INTERVAL = 2000;
const int TEMP_CHANGE = 10;


// Declare the Neopixel strip using settings for Jewel V7 RGB neopixels 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

float prevVoltage;
unsigned long timer = 0;


void setup() {
  // Init serial for debugging
  Serial.begin(9600);
  
  // Neopixel initialization
  strip.begin();
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show(); 
  delay(10);
}

void loop() {
  float voltage = getVoltage();
  Serial.print(voltage); Serial.println(" volts");
  
  if (voltage > .15) {
    strip.setPixelColor(0, strip.Color(0, 245, 0));
    strip.setPixelColor(9, strip.Color(0, 245, 0));
    strip.show();       
    delay(2000);
    
    strip.setPixelColor(0, strip.Color(0, 0, 0));
    strip.setPixelColor(9, strip.Color(0, 0, 0));
    strip.show(); 
  }

  delay(2000);
  
//  float temperatureF = getTemperatureF();
//
//  if (millis() - timer >= TIME_INTERVAL) {
//    // If temperature rose 10 degrees F in the last TIME_INTERVAL ms
//    bool shouldTurnOn = (temperatureF - prevTempF) >= 10;
//    // If temperature fell 10 degrees F in last TIME_INTERVAL ms
//    bool shouldTurnOff = (prevTempF - temperatureF) >= 10 ;
//    
//    if (shouldTurnOn) {
//      strip.setPixelColor(0, strip.Color(0, 245, 0)); 
//    } else if (shouldTurnOff){
//      strip.setPixelColor(0, strip.Color(0, 0, 0));
//    }
//
//    prevTempF = temperatureF;  // Update prev temperature
//    timer = millis();         // Reset timer
//  }
}

float getVoltage() {
  // Convert reading to voltage. For 5v arduino use 5.0
  float voltage = analogRead(TEMP_PIN) * 3.3;
  voltage = voltage / 1024.0; 

  return voltage;

//  // Convert from 10 mv per degree with 500 mV offset
//  // to degrees ((voltage - 500mV) times 100)
//  float temperatureC = (voltage - 0.5) * 100 ;  
//
//  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0; // Convert to Fahrenheit
//  return temperatureF;
  
//  return ((analogRead(TEMP_PIN) * 0.0048828125 * 100) * (9.0 / 5.0) + 32.0);
}

