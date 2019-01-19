/******************************************************************
CoMotion MakerSpace 2018
Edited by: Ramon Qu
Arduino 101 Basic Workshop
 SparkFun Inventor's Kit
 Example sketch 11 - temperature Sensor

  Use the "serial monitor" window to read a temperature sensor.

  The TMP36 is an easy-to-use temperature sensor that outputs
  a voltage that's proportional to the ambient temperature.
  You can use it for all kinds of automation tasks where you'd
  like to know or control the temperature of something.

  More information on the sensor is available in the datasheet:
  http://dlnmh9ip6v2uc.cloudfront.net/datasheets/Sensors/Temp/TMP35_36_37.pdf

  Even more exciting, we'll start using the Arduino's serial port
  to send data back to your main computer! Up until now, we've
  been limited to using simple LEDs for output. We'll see that
  the Arduino can also easily output all kinds of text and data.

This sketch was written by SparkFun Electronics,
with lots of help from the Arduino community.
This code is completely free for any use.
Visit http://learn.sparkfun.com/products/2 for SIK information.
Visit http://www.arduino.cc to learn about the Arduino.

Version 2.0 6/2012 MDG
******************************************************************/


// We'll use analog input 0 to measure the temperature sensor's
// signal pin.

const int TEMP_PIN = A0;


void setup()
{
  Serial.begin(9600); //Initialize serial port & set baud rate to 9600 bits per second (bps)

  Serial.println(F("reading temperature begin. \n"));
}


void loop()
{
  // read the LM35 sensor value and convert to the degrees every 100ms.
  static unsigned long sensortStamp = 0;
  
  if(millis() - sensortStamp > 1000){
    sensortStamp = millis();

    // Convert reading to voltage. For 5v arduino use 5.0
    float voltage = analogRead(TEMP_PIN) * 3.3;
    voltage = voltage / 1024.0; 
    
    Serial.print(voltage); 
    Serial.println(" volts");

    // Convert from 10 mv per degree with 500 mV offset
    // to degrees ((voltage - 500mV) times 100)
    float temperatureC = (voltage - 0.5) * 100 ;  
    float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0; // Convert to Fahrenheit

//    int reading = analogRead(0); 
//    float temperatureC = reading *0.0048828125*100;
//    float temperatureF = temp * (9.0 / 5.0) + 32.0; //Convert degrees Celsius to Fahrenheit
    
    Serial.print(F("Real Time Temp C: ")); 
    Serial.println(temperatureC);

    Serial.print(F("Real Time Temp F: ")); 
    Serial.println(temperatureF);
  }
}
