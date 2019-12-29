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
#define BUILD "19123001"



/****************************************
 * CORE LIBRARIES
 ***/
 
#include <Arduino.h>
#include "configuration.h"



/****************************************
 * DECLARE CONSTANTS
 ***/

#define ZERO 0

#define X_AXIS_SELECTED 1
#define Y_AXIS_SELECTED 2
#define Z_AXIS_SELECTED 3

#define JOG_SPEED_X1_SELECTED 1
#define JOG_SPEED_X10_SELECTED 2
#define JOG_SPEED_X100_SELECTED 3


/****************************************
 * DECLARE GLOBALS
 ***/

volatile int previousValue = 0;
volatile int counterValue = 0;
volatile int clickCount = 0;
volatile int clickStore = 0;
volatile bool jogActive = false;
volatile bool stepActive = false;
volatile int jogDirection = 0;

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

       
    pinMode(JOG_SPEED_X100_PIN, INPUT_PULLUP);   
    pinMode(JOG_SPEED_X10_PIN, INPUT_PULLUP);    
    pinMode(JOG_SPEED_X1_PIN, INPUT_PULLUP);     
    
    pinMode(X_AXIS_SELECT_PIN, INPUT_PULLUP);   
    pinMode(Y_AXIS_SELECT_PIN, INPUT_PULLUP); 
    pinMode(Z_AXIS_SELECT_PIN, INPUT_PULLUP);  

    pinMode(ENABLE_OUT_PIN, OUTPUT);
    pinMode(X_AXIS_OUT_PIN, OUTPUT);
    pinMode(Y_AXIS_OUT_PIN, OUTPUT);  
    pinMode(Z_AXIS_OUT_PIN, OUTPUT);  

    currentTimeMs = millis();
    targetTimeMs = millis();
    stepActive = false;
  
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
        clickCount++;
    }
   
    if(combined == 0b0001 ||
        combined == 0b0111 ||
        combined == 0b1110 ||
        combined == 0b1000) {
        counterValue--;
        clickCount--;
    }

    previousValue = currentValue;

   // Decode the direction
    if(counterValue >= 4) {
        counterValue -= 4;
        jogDirection =  1;
        
    } else if(counterValue <= -4) {
        counterValue += 4;
        jogDirection =  -1;
    }    

    lastTriggeredTimeMs = millis();

    stepActive = true;
}




/****************************************
 * GET AXIS
 * Decode selected axis
 * Designed for rotary switches where only selected axis is active / high
 * Switch to ground (input low to trigger)
 ***/
int getAxis() {
    
    if (digitalRead(X_AXIS_SELECT_PIN) == LOW) return X_AXIS_SELECTED;
    if (digitalRead(Y_AXIS_SELECT_PIN) == LOW) return Y_AXIS_SELECTED;
    if (digitalRead(Z_AXIS_SELECT_PIN) == LOW) return Z_AXIS_SELECTED;
    
}




/****************************************
 * GET SELECTED SPEED
 * Decode selected speed
 * Designed for rotary switches where only selected speed is active / high
 * Switch to ground (input low to trigger)
 ***/
int getSpeed() {

    if (digitalRead(JOG_SPEED_X1_PIN) == LOW) return JOG_SPEED_X1_SELECTED;
    if (digitalRead(JOG_SPEED_X10_PIN) == LOW) return JOG_SPEED_X10_SELECTED;
    if (digitalRead(JOG_SPEED_X100_PIN) == LOW) return JOG_SPEED_X100_SELECTED;

    return ZERO;

}




/****************************************
 * SEND JOG SIGNAL
 ***/
void sendJogSignal(int selectedAxis, int selectedSpeed, int jogDirection) {

    int moveVector;
    int xCenter = ((JOY_X_VALUE / 16384) * 256);
    int yCenter = ((JOY_Y_VALUE / 16384) * 256);
    int zCenter = ((JOY_Z_VALUE / 16384) * 256);

    //check we have not already moved this click
    if (clickCount == clickStore) {
      return;
    } else {
      clickCount = clickStore;
    }

    // Decode which speed is selected and determine output voltage from move vector and multiplier
    switch (selectedSpeed)
    {

        case JOG_SPEED_X1_SELECTED:
            moveVector = (xCenter + JOG_SPEED_X1) * jogDirection;
        break;

        case JOG_SPEED_X10_SELECTED:
            moveVector = (yCenter + JOG_SPEED_X10) * jogDirection;
        break;

        case JOG_SPEED_X100_SELECTED:
            moveVector = (zCenter + JOG_SPEED_X100) * jogDirection;
        break;
    }


    // Select the axis and send the output. Lets also make sure other axis are not moved.
    switch (selectedAxis)
    {
        case X_AXIS_SELECTED:
            analogWrite(X_AXIS_OUT_PIN, moveVector);
            analogWrite(Y_AXIS_OUT_PIN, yCenter);
            analogWrite(Z_AXIS_OUT_PIN, zCenter);
        break;

        case Y_AXIS_SELECTED:
            analogWrite(X_AXIS_OUT_PIN, xCenter);
            analogWrite(Y_AXIS_OUT_PIN, moveVector);
            analogWrite(Z_AXIS_OUT_PIN, zCenter);
        break;

        case Z_AXIS_SELECTED:
            analogWrite(X_AXIS_OUT_PIN, xCenter);
            analogWrite(Y_AXIS_OUT_PIN, yCenter);
            analogWrite(Z_AXIS_OUT_PIN, moveVector);
        break;

    }

}



/****************************************
 * MAIN PROGRAM LOOP
 ***/
void loop(){ 

    // What's the time?
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

    // If X1 speed selected only move one pulse per click
    if (selectedSpeed == JOG_SPEED_X1_SELECTED ) {

          //make sure we only move one click
          if (stepActive == true) {
              stepActive = false; 
          } else {
              jogDirection = 0;
          }  

          // Do the move
          sendJogSignal(selectedAxis, selectedSpeed, jogDirection);        

          //delay by pulse width
          delay(X1_PULSE_WIDTH);
          
          #if defined SERIAL_DEBUG
              if (jogDirection == 1){
                  Serial.println(">  >  >  >  >  >  >");      
              }
              if (jogDirection == -1){
                  Serial.println("<  <  <  <  <  <  <");      
              }
          #endif  
    
    // Else move in accordance with smoothing factor (try to maintain smooth movement)
    } else if ((currentTimeMs - smoothingFactor) < lastTriggeredTimeMs) {  

          // Do the move
          sendJogSignal(selectedAxis, selectedSpeed, jogDirection);        

          #if defined SERIAL_DEBUG
              if (jogDirection == 1){
                  Serial.println(">>>>>>>>>>>>>>>>>>");      
              }
              if (jogDirection == -1){
                  Serial.println("<<<<<<<<<<<<<<<<<<");      
              }
          #endif  
                     
    } else {

          // We are not trying to move so send a zero vector
          sendJogSignal(selectedAxis, selectedSpeed, ZERO);              
    }

}
