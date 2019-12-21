/****************************************
 * The Marlin CNC Pendant project
 *
 * A jogwheel based pendant controller to interface to Marlin / Arduino controlled 3D printers & CNCs 
 * 
 ***/

// Development and release version - Don't forget to update the changelog!!
#define VERSION "V1.0-Alpha"
#define BUILD "19122001"


/****************************************
 * CORE LIBRARIES
 ***/
 
#include <Arduino.h>
#include "configuration.h"


/****************************************
 * DECLARE CONSTANTS
 ***/

X_AXIS_SELECTED 1
Y_AXIS_SELECTED 2
Z_AXIS_SELECTED 3
JOG_SPEED_100_SELECTED 1
JOG_SPEED_10_SELECTED 2
JOG_SPEED_1_SELECTED 3
JOG_SPEED_01_SELECTED 4

/****************************************
 * DECLARE GLOBALS
 ***/

volatile int previousValue = 0;
volatile int counterValue = 0;


/****************************************
 * INITIALISATION
 ***/
void setup ()
{
    pinMode(JOGWHEEL_PIN_A, INPUT_PULLUP); 
    pinMode(JOGWHEEL_PIN_B, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(JOGWHEEL_PIN_A), jogwheelInterrupt, CHANGE); 
    attachInterrupt(digitalPinToInterrupt(JOGWHEEL_PIN_B), jogwheelInterrupt, CHANGE);
  
    Mouse.begin();
}



/****************************************
 * JOGWHEEL INTERRUPT
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
}



/****************************************
 * DECODE JOGWHEEL
 ***/
int decodeJogwheel() {
  
    if(counterValue >= 4) {
        counterValue -= 4;
        return 1;

    } else if(counterValue <= -4) {
        counterValue += 4;
        return -1;
    } 
}



/****************************************
 * GET AXIS
 ***/
integer getAxis() {
    
    int X_AXIS_ACTIVE = digitalRead(X_AXIS_SELECT_PIN);
    int Y_AXIS_ACTIVE = digitalRead(Y_AXIS_SELECT_PIN);
    int Z_AXIS_ACTIVE = digitalRead(Z_AXIS_SELECT_PIN);

    if X_AXIS_ACTIVE return X_AXIS_SELECTED;
    if Y_AXIS_ACTIVE return Y_AXIS_SELECTED;
    if Z_AXIS_ACTIVE return Z_AXIS_SELECTED;
    
}






/****************************************
 * GET SPEED
 ***/
integer getSpeed() {

    int JOG_SPEED_100_ACTIVE = digitalRead(JOG_SPEED_100_PIN); 
    int JOG_SPEED_10_ACTIVE = digitalRead(JOG_SPEED_10_PIN); 
    int JOG_SPEED_1_ACTIVE = digitalRead(JOG_SPEED_1_PIN); 
    int JOG_SPEED_01_ACTIVE = digitalRead(JOG_SPEED_01_PIN); 

    if JOG_SPEED_100_ACTIVE return JOG_SPEED_100_SELECTED;
    if JOG_SPEED_10_ACTIVE return JOG_SPEED_10_SELECTED;
    if JOG_SPEED_1_ACTIVE return JOG_SPEED_1_SELECTED;
    if JOG_SPEED_01_ACTIVE return JOG_SPEED_01_SELECTED;

}



/****************************************
 * SEND JOG SIGNAL
 ***/
void sendJogSignal(selectedAxis, selectedSpeed, moveVector) {

    int selectedAxis;
    int selectedSpeed;
    int moveVector; 
    int outputVoltage;

    //decode voltage value to send

    switch (selectedSpeed)
    {
        case JOG_SPEED_100_SELECTED;
            outputVoltage = JOG_SPEED_100_VOLTAGE * moveVector
        break;

        case JOG_SPEED_10_SELECTED;
            outputVoltage = JOG_SPEED_10_VOLTAGE * moveVector
        break;

        case JOG_SPEED_1_SELECTED;
            outputVoltage = JOG_SPEED_1_VOLTAGE * moveVector
        break;

        case JOG_SPEED_01_SELECTED;
            outputVoltage = JOG_SPEED_01_VOLTAGE * moveVector
        break;

    }


    switch (selectedAxis)
    {
        case X_AXIS_SELECTED;
            analogWrite(X_AXIS_OUT_PIN, outputVoltage)
        break;

        case Y_AXIS_SELECTED;
            analogWrite(Y_AXIS_OUT_PIN, outputVoltage)
        break;

        case Z_AXIS_SELECTED;
            analogWrite(Z_AXIS_OUT_PIN, outputVoltage)
        break;

    }


}



/****************************************
 * MAIN PROGRAM LOOP
 ***/
void loop(){ 

    int selectedAxis = getAxis();
    int selectedSpeed = getSpeed();
    int moveVector = decodeJogwheel(); 


    
    //send aapropriate output to Marlin
    sendJogSignal(selectedAxis, selectedSpeed, moveVector)

}

