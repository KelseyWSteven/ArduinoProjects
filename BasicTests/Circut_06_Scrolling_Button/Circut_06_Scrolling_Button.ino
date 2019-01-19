/*
 CoMotion MakerSpace 3019
 Edited by: Ramon Qu
 Arduino 101 Basic Workshop
 6 - scrolling LEDs (For Loop)
 Demonstrates the use of a for() loop.
 Lights multiple LEDs in sequence, then in reverse.
 The circuit:
 * LEDs from pins 3 through 8 to ground
This example code is in the public domain.
 http://www.arduino.cc/en/Tutorial/ForLoop
 */

int timer = 100;           // The higher the number, the slower the timing.
const int buttonPin = 2;     // the number of the pushbutton pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
  pinMode(buttonPin, INPUT);

  for (int pin = 3; pin < 9; pin++) {
    //The loop will go through [3,9) which is 3 to 8.
    pinMode(pin, OUTPUT); // Set the pin as OUTPUT mode
  }
}

void loop() {
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {

    // loop from the lowest pin to the highest:
    for (int pin = 3; pin < 9; pin++) {
      // turn the pin on:
      digitalWrite(pin, HIGH);
      delay(timer);
      // turn the pin off:
      digitalWrite(pin, LOW);
    }

    // loop from the highest pin to the lowest:
    for (int pin = 8; pin >= 3; pin--) {
      // turn the pin on:
      digitalWrite(pin, HIGH);
      delay(timer);
      // turn the pin off:
      digitalWrite(pin, LOW);
    }
  }
}
