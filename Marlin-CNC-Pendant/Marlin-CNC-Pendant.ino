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

#define X_AXIS_SELECTED 1
#define Y_AXIS_SELECTED 2
#define Z_AXIS_SELECTED 3

#define JOG_SPEED_1000_SELECTED 1
#define JOG_SPEED_100_SELECTED 2
#define JOG_SPEED_10_SELECTED 3
#define JOG_SPEED_1_SELECTED 4


/****************************************
 * DECLARE GLOBALS
 ***/

volatile int previousValue = 0;
volatile int counterValue = 0;
volatile bool jogActive = false;
volatile int moveVector = 0;


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

    attachInterrupt(INTERRUPT_A, jogwheelInterrupt, CHANGE); 
    attachInterrupt(INTERRUPT_B, jogwheelInterrupt, CHANGE);

    pinMode(ENABLE_PIN, INPUT_PULLUP);          
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

    // Control the loop cycle - we only want to process actual encoder clicks
    jogActive = true;
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

    if (X_AXIS_ACTIVE) return X_AXIS_SELECTED;
    if (Y_AXIS_ACTIVE) return Y_AXIS_SELECTED;
    if (Z_AXIS_ACTIVE) return Z_AXIS_SELECTED;
    
}






/****************************************
 * GET SPEED
 * Decode selected speed
 * Designed for rotary switches where only selected speed is active / high
 ***/
int getSpeed() {

    int JOG_SPEED_100_ACTIVE = digitalRead(JOG_SPEED_100_PIN); 
    int JOG_SPEED_10_ACTIVE = digitalRead(JOG_SPEED_10_PIN); 
    int JOG_SPEED_1_ACTIVE = digitalRead(JOG_SPEED_1_PIN); 

    if (JOG_SPEED_100_ACTIVE) return JOG_SPEED_100_SELECTED;
    if (JOG_SPEED_10_ACTIVE) return JOG_SPEED_10_SELECTED;
    if (JOG_SPEED_1_ACTIVE) return JOG_SPEED_1_SELECTED;

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
            outputValue = DEAD_BAND_CENTRE_VALUE + (JOG_SPEED_100_MULTIPLER * moveVector);
        break;

        case JOG_SPEED_10_SELECTED:
            outputValue = DEAD_BAND_CENTRE_VALUE + (JOG_SPEED_10_MULTIPLER * moveVector);
        break;

        case JOG_SPEED_1_SELECTED:
            outputValue = DEAD_BAND_CENTRE_VALUE + (JOG_SPEED_1_MULTIPLER * moveVector);
        break;

    }

    // Select the axis and send the output (lets also make sure other axis are not moved)
    switch (selectedAxis)
    {
        case X_AXIS_SELECTED:
            analogWrite(X_AXIS_OUT_PIN, outputValue);
            analogWrite(Y_AXIS_OUT_PIN, DEAD_BAND_CENTRE_VALUE);
            analogWrite(Z_AXIS_OUT_PIN, DEAD_BAND_CENTRE_VALUE);
        break;

        case Y_AXIS_SELECTED:
            analogWrite(X_AXIS_OUT_PIN, DEAD_BAND_CENTRE_VALUE);
            analogWrite(Y_AXIS_OUT_PIN, outputValue);
            analogWrite(Z_AXIS_OUT_PIN, DEAD_BAND_CENTRE_VALUE);
        break;

        case Z_AXIS_SELECTED:
            analogWrite(X_AXIS_OUT_PIN, DEAD_BAND_CENTRE_VALUE);
            analogWrite(Y_AXIS_OUT_PIN, DEAD_BAND_CENTRE_VALUE);
            analogWrite(Z_AXIS_OUT_PIN, outputValue);
        break;

    }

}



/****************************************
 * MAIN PROGRAM LOOP
 ***/
void loop(){ 

    // Decode axis & speed selector switches
    int selectedAxis = getAxis();
    int selectedSpeed = getSpeed();    

    // If encoder motion not detected, zero the move vector
    if (jogActive == true) {
        jogActive = false; 
    } else {
       moveVector = 0;
    }

    #if defined DEV_MODE
        if (moveVector == 1){
            Serial.println(">>>>>>>>>>>>>>>>>");      
        }
        if (moveVector == -1){
            Serial.println("<<<<<<<<<<<<<<<<<");      
        }
    #endif  
        
    // Send move data to Marlin
    sendJogSignal(selectedAxis, selectedSpeed, moveVector);


}
