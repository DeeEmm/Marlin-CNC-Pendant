/****************************************
 * The Marlin CNC Pendant project
 *
 * A jogwheel based pendant controller to interface to Marlin / Arduino controlled 3D printers & CNCs 
 * 
 * Can be used with ebay stye jogwheel pendants and custom setups
 *
 * Interfacing to Marlin joystick code: https://github.com/vector76/Marlin/commit/2d258d649795fa062b95afac60ea639e61bc7585
 * 
 * Encoder reference - https://playground.arduino.cc/Main/RotaryEncoders/
 * 
 ***/

// Development and release version - Don't forget to update the changelog!!
#define VERSION "V1.0-Alpha"
#define BUILD "19122702"


/****************************************
 * CORE LIBRARIES
 ***/
 
#include <Arduino.h>
#include "configuration.h"


/****************************************
 * DECLARE CONSTANTS
 ***/

#define OFF 0
#define ON 1 

#define X_AXIS_SELECTED 1
#define Y_AXIS_SELECTED 2
#define Z_AXIS_SELECTED 3

#define JOG_SPEED_1_SELECTED 1
#define JOG_SPEED_10_SELECTED 2
#define JOG_SPEED_100_SELECTED 3


/****************************************
 * DECLARE GLOBALS
 ***/

volatile int previousValue = 0;
volatile int counterValue = 0;
volatile bool jogActive = false;
volatile bool stepActive = false;
volatile int moveVector = 0;

volatile unsigned long lastTriggeredTimeMs;
volatile unsigned long targetTimeMs;
unsigned long currentTimeMs;
const unsigned long smoothingFactor = SMOOTHING_FACTOR;

/****************************************
 * INITIALISATION
 ***/
void setup ()
{

    #if defined SERIAL_DEBUG
        Serial.begin(9600);
    #endif    
    
    pinMode(JOGWHEEL_PIN_A, INPUT_PULLUP); 
    pinMode(JOGWHEEL_PIN_B, INPUT_PULLUP);

    attachInterrupt(INTERRUPT_A, jogwheelInterrupt, CHANGE); 
    attachInterrupt(INTERRUPT_B, jogwheelInterrupt, CHANGE);

       
    pinMode(JOG_SPEED_100_PIN, INPUT_PULLUP);   
    pinMode(JOG_SPEED_10_PIN, INPUT_PULLUP);    
    pinMode(JOG_SPEED_1_PIN, INPUT_PULLUP);     
    
    pinMode(X_AXIS_SELECT_PIN, INPUT_PULLUP);   
    pinMode(Y_AXIS_SELECT_PIN, INPUT_PULLUP); 
    pinMode(Z_AXIS_SELECT_PIN, INPUT_PULLUP);  

    pinMode(ENABLE_OUT_PIN, OUTPUT);
    pinMode(X_AXIS_OUT_PIN, OUTPUT);
    pinMode(Y_AXIS_OUT_PIN, OUTPUT);  
    pinMode(Z_AXIS_OUT_PIN, OUTPUT);  

    currentTimeMs = millis();
    targetTimeMs = millis();

    stepActive = true;
  
}



/****************************************
 * JOGWHEEL INTERRUPT
 * Decode jogwheel data. Will work with any two phase encoder wheel
 * NOTE: Inputs pins need to be interrupt 0 & 1 (pins 2 & 3 on a nano)
 ***/
void jogwheelInterrupt() {
    int A = digitalRead(JOGWHEEL_PIN_A); 
    int B = digitalRead(JOGWHEEL_PIN_B);

    int currentValue = (A << 1) | B;
    int combined  = (previousValue << 2) | currentValue;
   
    if(combined == 0b0010 || 
        combined == 0b1011 ||
        combined == 0b1101 || 
        combined == 0b0100) {
        counterValue++;
    }
   
    if(combined == 0b0001 ||
        combined == 0b0111 ||
        combined == 0b1110 ||
        combined == 0b1000) {
        counterValue--;
    }

    previousValue = currentValue;

   // Decode the direction
    if(counterValue >= 4) {
        counterValue -= 4;
        moveVector =  1;
    } else if(counterValue <= -4) {
        counterValue += 4;
        moveVector =  -1;
    }    

    lastTriggeredTimeMs = millis();

    stepActive = true;
}




/****************************************
 * GET AXIS
 * Decode selected axis
 * Designed for rotary switches where only selected axis is active / high
 ***/
int getAxis() {
    
    int X_AXIS_ACTIVE = digitalRead(X_AXIS_SELECT_PIN);
    int Y_AXIS_ACTIVE = digitalRead(Y_AXIS_SELECT_PIN);
    int Z_AXIS_ACTIVE = digitalRead(Z_AXIS_SELECT_PIN);

    if (X_AXIS_ACTIVE == LOW) return X_AXIS_SELECTED;
    if (Y_AXIS_ACTIVE == LOW) return Y_AXIS_SELECTED;
    if (Z_AXIS_ACTIVE == LOW) return Z_AXIS_SELECTED;
    
}






/****************************************
 * GET SPEED
 * Decode selected speed
 * Designed for rotary switches where only selected speed is active / high
 * Switch to ground (input low to trigger)
 ***/
int getSpeed() {

    if (digitalRead(JOG_SPEED_1_PIN) == LOW) return JOG_SPEED_1_SELECTED;
    if (digitalRead(JOG_SPEED_10_PIN) == LOW) return JOG_SPEED_10_SELECTED;
    if (digitalRead(JOG_SPEED_100_PIN) == LOW) return JOG_SPEED_100_SELECTED;

    return 0;

}



/****************************************
 * SEND JOG SIGNAL
 ***/
void sendJogSignal(int selectedAxis, int selectedSpeed, int moveVector) {

    int outputValue;

    // Info from Joystick function
    // Use M114 to determine reasonable values for these after connecting your hardware
    // { Minimum value, dead zone start, dead zone end, maximum value }
    //#define JOY_X_LIMITS { 5600, 8190-100, 8190+100, 10800 }
    //#define JOY_Y_LIMITS { 5600, 8250-100, 8250+100, 11000 }
    //#define JOY_Z_LIMITS { 4800, 8080-100, 8080+100, 11550 }
    
    // Decode which speed is selected and determine output voltage from move vector and multiplier
    switch (selectedSpeed)
    {

        case JOG_SPEED_100_SELECTED:
            outputValue = DEAD_BAND_CENTER_VALUE + (JOG_SPEED_100_MULTIPLIER * moveVector);
        break;

        case JOG_SPEED_10_SELECTED:
            outputValue = DEAD_BAND_CENTER_VALUE + (JOG_SPEED_10_MULTIPLIER * moveVector);
        break;

        case JOG_SPEED_1_SELECTED:
            outputValue = DEAD_BAND_CENTER_VALUE + (JOG_SPEED_1_MULTIPLIER * moveVector);
        break;

    }

    // Select the axis and send the output (lets also make sure other axis are not moved)
    switch (selectedAxis)
    {
        case X_AXIS_SELECTED:
            analogWrite(X_AXIS_OUT_PIN, outputValue);
            analogWrite(Y_AXIS_OUT_PIN, DEAD_BAND_CENTER_VALUE);
            analogWrite(Z_AXIS_OUT_PIN, DEAD_BAND_CENTER_VALUE);
        break;

        case Y_AXIS_SELECTED:
            analogWrite(X_AXIS_OUT_PIN, DEAD_BAND_CENTER_VALUE);
            analogWrite(Y_AXIS_OUT_PIN, outputValue);
            analogWrite(Z_AXIS_OUT_PIN, DEAD_BAND_CENTER_VALUE);
        break;

        case Z_AXIS_SELECTED:
            analogWrite(X_AXIS_OUT_PIN, DEAD_BAND_CENTER_VALUE);
            analogWrite(Y_AXIS_OUT_PIN, DEAD_BAND_CENTER_VALUE);
            analogWrite(Z_AXIS_OUT_PIN, outputValue);
        break;

    }

}



/****************************************
 * MAIN PROGRAM LOOP
 ***/
void loop(){ 

    currentTimeMs = millis();

    // Decode axis & speed selector switches
    int selectedAxis = getAxis();
    int selectedSpeed = getSpeed();

    // If speed selector <> 0 Send enable signal to Marlin 
    if ((SPEED_SWITCH_ENABLE == true) && (selectedSpeed != 0)) {
      digitalWrite(ENABLE_OUT_PIN, HIGH);
    } else {
      digitalWrite(ENABLE_OUT_PIN, LOW);      
    }

    // If X1 speed selected only move one click
    if (selectedSpeed == JOG_SPEED_1_SELECTED ) {

          if (stepActive == true) {
              stepActive = false; 
          } else {
              moveVector = 0;
          }  
          sendJogSignal(selectedAxis, selectedSpeed, moveVector);        

          #if defined SERIAL_DEBUG
              if (moveVector == 1){
                  Serial.println(">  >  >  >  >  >  >");      
              }
              if (moveVector == -1){
                  Serial.println("<  <  <  <  <  <  <");      
              }
          #endif  
    
    // Else move in accordance with smoothing factor (try to maintain smooth movement)
    } else if ((currentTimeMs - smoothingFactor) < lastTriggeredTimeMs) {  
          sendJogSignal(selectedAxis, selectedSpeed, moveVector);        

          #if defined SERIAL_DEBUG
              if (moveVector == 1){
                  Serial.println(">>>>>>>>>>>>>>>>>>");      
              }
              if (moveVector == -1){
                  Serial.println("<<<<<<<<<<<<<<<<<<");      
              }
          #endif  
                     
    } else {
          sendJogSignal(selectedAxis, selectedSpeed, int(DEAD_BAND_CENTER_VALUE));              
    }



}
