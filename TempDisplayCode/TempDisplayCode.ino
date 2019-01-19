const int TEMP_PIN = A0;

const int GREEN_LED_PIN = 11;
const int ORANGE_LED_PIN = 10;
const int RED_LED_PIN = 9;

const int GREEN_TEMP = 60;
const int ORANGE_TEMP = 70;
const int RED_TEMP = 80;

const int ORANGE_TIME_INTERVAL = 1000;
const int RED_TIME_INTERVAL = 200;

int greenLedState = LOW;             
int orangeLedState = LOW;             
int redLedState = 0;             

unsigned long orangeTimer = 0;
unsigned long redTimer = 0;


void setup() {
  // Init LEDs to off
  pinMode(GREEN_LED_PIN, OUTPUT);    
  pinMode(ORANGE_LED_PIN, OUTPUT); 
  pinMode(RED_LED_PIN, redLedState);    
  
  digitalWrite(GREEN_LED_PIN, greenLedState);  
  digitalWrite(ORANGE_LED_PIN, orangeLedState);  
  analogWrite(RED_LED_PIN, greenLedState); 
  
  Serial.begin(9600); // Init temp sensor    
}

void loop() {
  // Diagnostic
//  float degreesC = analogRead(TEMP_PIN) * 0.0048828125 * 100;
//  Serial.print(F("Real Time Temp C: ")); 
//  Serial.println(degreesC);
//  Serial.print(F("Real Time Temp F: ")); 
//  Serial.println(temperatureF);
//  Serial.println("");

  float temperatureF = (analogRead(TEMP_PIN) * 0.0048828125 * 100) * (9.0 / 5.0) + 32.0;

  if (millis() - redTimer >= RED_TIME_INTERVAL) {
    if (temperatureF >= RED_TEMP) {
      setHighLED((redLedState == 0) ? 255 : 0); 
    } else {
      setHighLED(0);
    }
  }

  if (millis() - orangeTimer >= ORANGE_TIME_INTERVAL) {
    if (temperatureF >= ORANGE_TEMP) {
      setMediumLED((orangeLedState == LOW) ? HIGH : LOW);
    } else {
      setMediumLED(LOW);
    }
  } 

  setLowLED(temperatureF);
}

void setHighLED(int value) {
  redLedState = value; 
  analogWrite(RED_LED_PIN, redLedState); 
   
  redTimer = millis();
}

void setMediumLED(boolean value) {
  orangeLedState = value; 
  digitalWrite(ORANGE_LED_PIN, orangeLedState); 
   
  orangeTimer = millis();
}

void setLowLED(int temp) {
  greenLedState = (temp >= GREEN_TEMP) ? HIGH : LOW;
  digitalWrite(GREEN_LED_PIN, greenLedState);   
}
