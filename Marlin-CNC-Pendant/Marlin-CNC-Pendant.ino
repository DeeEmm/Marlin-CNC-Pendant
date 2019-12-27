/****************************************
 * The Marlin CNC Pendant project
 *
 * A jogwheel based pendant controller to interface to Marlin / Arduino controlled 3D printers & CNCs 
 *
 * Interfacing to Marlin joystick code: https://github.com/vector76/Marlin/commit/2d258d649795fa062b95afac60ea639e61bc7585
 * 
 * Encoder reference - https://playground.arduino.cc/Main/RotaryEncoders/
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

#define X_AXIS_SELECTED 1
#define Y_AXIS_SELECTED 2
#define Z_AXIS_SELECTED 3
#define JOG_SPEED_100_SELECTED 1
#define JOG_SPEED_10_SELECTED 2
#define JOG_SPEED_1_SELECTED 3
#define JOG_SPEED_01_SELECTED 4


/****************************************
 * DECLARE GLOBALS
 ***/

volatile int previousValue = 0;
volatile int counterValue = 0;
volatile bool jogActive = false;


/****************************************
 * INITIALISATION
 ***/
void setup ()
{

    #if defined DEV_MODE
        Serial.begin(9600);
    #endif    
    
    pinMode(JOGWHEEL_PIN_A, INPUT_PULLUP); 
    pinMode(JOGWHEEL_PIN_B, INPUT_PULLUP);

    attachInterrupt(0, jogwheelInterrupt, CHANGE); 
    attachInterrupt(1, jogwheelInterrupt, CHANGE);
  
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

    jogActive = true;
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
int getAxis() {
    
    int X_AXIS_ACTIVE = digitalRead(X_AXIS_SELECT_PIN);
    int Y_AXIS_ACTIVE = digitalRead(Y_AXIS_SELECT_PIN);
    int Z_AXIS_ACTIVE = digitalRead(Z_AXIS_SELECT_PIN);

    if (X_AXIS_ACTIVE) return X_AXIS_SELECTED;
    if (Y_AXIS_ACTIVE) return Y_AXIS_SELECTED;
    if (Z_AXIS_ACTIVE) return Z_AXIS_SELECTED;
    
}






/****************************************
 * GET SPEED
 ***/
int getSpeed() {

    int JOG_SPEED_100_ACTIVE = digitalRead(JOG_SPEED_100_PIN); 
    int JOG_SPEED_10_ACTIVE = digitalRead(JOG_SPEED_10_PIN); 
    int JOG_SPEED_1_ACTIVE = digitalRead(JOG_SPEED_1_PIN); 
    int JOG_SPEED_01_ACTIVE = digitalRead(JOG_SPEED_01_PIN); 

    if (JOG_SPEED_100_ACTIVE) return JOG_SPEED_100_SELECTED;
    if (JOG_SPEED_10_ACTIVE) return JOG_SPEED_10_SELECTED;
    if (JOG_SPEED_1_ACTIVE) return JOG_SPEED_1_SELECTED;
    if (JOG_SPEED_01_ACTIVE) return JOG_SPEED_01_SELECTED;

}



/****************************************
 * SEND JOG SIGNAL
 ***/
void sendJogSignal(int selectedAxis, int selectedSpeed, int moveVector) {

    int outputVoltage;

    //TODO #1 decode voltage value to send
    // Use M114 to determine reasonable values for these after connecting your hardware
    // { Minimum value, dead zone start, dead zone end, maximum value }
    //#define JOY_X_LIMITS { 5600, 8190-100, 8190+100, 10800 }
    //#define JOY_Y_LIMITS { 5600, 8250-100, 8250+100, 11000 }
    //#define JOY_Z_LIMITS { 4800, 8080-100, 8080+100, 11550 }
    

    switch (selectedSpeed)
    {
        case JOG_SPEED_100_SELECTED:
            outputVoltage = JOG_SPEED_100_VOLTAGE * moveVector;
        break;

        case JOG_SPEED_10_SELECTED:
            outputVoltage = JOG_SPEED_10_VOLTAGE * moveVector;
        break;

        case JOG_SPEED_1_SELECTED:
            outputVoltage = JOG_SPEED_1_VOLTAGE * moveVector;
        break;

        case JOG_SPEED_01_SELECTED:
            outputVoltage = JOG_SPEED_01_VOLTAGE * moveVector;
        break;

    }


    switch (selectedAxis)
    {
        case X_AXIS_SELECTED:
            analogWrite(X_AXIS_OUT_PIN, outputVoltage);
        break;

        case Y_AXIS_SELECTED:
            analogWrite(Y_AXIS_OUT_PIN, outputVoltage);
        break;

        case Z_AXIS_SELECTED:
            analogWrite(Z_AXIS_OUT_PIN, outputVoltage);
        break;

    }


    #if defined DEV_MODE
 //       Serial.print("Jog Speed:  ");
 //       Serial.print(selectedSpeed);
 //       Serial.print(" Axis: ");
 //       Serial.println(selectedAxis);
    #endif  


}



/****************************************
 * MAIN PROGRAM LOOP
 ***/
void loop(){ 

    // Decode axis & speed selector switches
    int selectedAxis = getAxis();
    int selectedSpeed = getSpeed();
    int moveVector;     

    // If encoder motion detected, decode move direction
    if (jogActive == true) {
        moveVector = decodeJogwheel();
        jogActive = false; 
    } else {
        moveVector = 0;
        counterValue = 0;
    }

    #if defined DEV_MODE
        if (moveVector == 1){
            Serial.println(">>>>>>>>>>>>>>>>>");      
        }
        if (moveVector == -1){
            Serial.println("<<<<<<<<<<<<<<<<<");      
        }
    #endif  
        
    //send appropriate output to Marlin
    sendJogSignal(selectedAxis, selectedSpeed, moveVector);


}
