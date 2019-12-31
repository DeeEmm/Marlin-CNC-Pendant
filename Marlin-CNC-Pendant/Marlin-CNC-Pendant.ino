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
#define VERSION "V1.0-RC1"
#define BUILD "19123101"



/****************************************
 * CORE LIBRARIES
 ***/
 
#include <Arduino.h>
#include "configuration.h"
#include <SPI.h>


/****************************************
 * DECLARE CONSTANTS
 ***/

#define ZERO 0
#define ON 1
#define OFF 0

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
volatile bool moveEnabled = false;
volatile int moveVector = 0;

volatile unsigned long lastTriggeredTimeMs;
volatile unsigned long targetTimeMs;
volatile float clickSpeed = 0;
unsigned long currentTimeMs;

// Encoder (don't forget to debounce it with some caps)
volatile byte aFlag = 0; // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0; // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile byte encoderPos = 0; //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
volatile byte oldEncPos = 0; //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)
volatile byte reading = 0; //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent


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

    attachInterrupt(INTERRUPT_A, jogwheelInterruptA, RISING); 
    attachInterrupt(INTERRUPT_B, jogwheelInterruptB, RISING); 
       
    pinMode(JOG_SPEED_X100_PIN, INPUT_PULLUP);   
    pinMode(JOG_SPEED_X10_PIN, INPUT_PULLUP);    
    pinMode(JOG_SPEED_X1_PIN, INPUT_PULLUP);     
    
    pinMode(X_AXIS_SELECT_PIN, INPUT_PULLUP);   
    pinMode(Y_AXIS_SELECT_PIN, INPUT_PULLUP); 
    pinMode(Z_AXIS_SELECT_PIN, INPUT_PULLUP);  

    pinMode(ADAPTIVE_SPEED_PIN, INPUT_PULLUP);  

    pinMode(ENABLE_OUT_PIN, OUTPUT);
    pinMode(X_AXIS_CS_PIN, OUTPUT);
    pinMode(Y_AXIS_CS_PIN, OUTPUT);  
    pinMode(Z_AXIS_CS_PIN, OUTPUT);  

    currentTimeMs = millis();
    targetTimeMs = millis();
    moveEnabled = false;

    // initialize SPI:
    SPI.begin();

    // Initialise VCRs
    digitalWrite(X_AXIS_CS_PIN, HIGH);
    digitalWrite(Y_AXIS_CS_PIN, HIGH);
    digitalWrite(Z_AXIS_CS_PIN, HIGH);


}



/****************************************
 * JOGWHEEL INTERRUPT ROUTINES
 * Decode jogwheel data. Will work with any two phase encoder wheel
 * NOTE: Inputs pins need to be interrupt 0 & 1 (pins 2 & 3 on a nano)
 ***/
void jogwheelInterruptA(){
    cli(); //stop interrupts happening before we read pin values
    reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
    if(reading == B00001100 && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
      encoderPos --; //decrement the encoder's position count
      bFlag = 0; //reset flags for the next turn
      aFlag = 0; //reset flags for the next turn
      clickSpeed =  1000 / (millis() - lastTriggeredTimeMs);               //clicks per second
      lastTriggeredTimeMs = millis();
    }
    else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
    sei(); //restart interrupts
    moveEnabled = true;
}

void jogwheelInterruptB(){
    cli(); //stop interrupts happening before we read pin values
    reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
    if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
      encoderPos ++; //increment the encoder's position count
      bFlag = 0; //reset flags for the next turn
      aFlag = 0; //reset flags for the next turn
      clickSpeed =  1000 / (millis() - lastTriggeredTimeMs);               //clicks per second
      lastTriggeredTimeMs = millis();
    }
    else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
    sei(); //restart interrupts
    moveEnabled = true;
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
void sendJogSignal(int selectedAxis, int selectedSpeed, int moveDirection) {

    float wiperPosition;
    float moveVector;
    int adaptiveSpeedEnabled = !digitalRead(ADAPTIVE_SPEED_PIN);
    float adaptiveSpeed;

    float xCenter = (JOY_X_CENTER_VALUE / 16384) * VCR_POT_RES;
    float yCenter = (JOY_Y_CENTER_VALUE / 16384) * VCR_POT_RES;
    float zCenter = (JOY_Z_CENTER_VALUE / 16384) * VCR_POT_RES;

    byte address = BYTE_ADDRESS;

    // Decode which speed is selected and determine output voltage from move vector and multiplier
    switch (selectedSpeed)
    {    
        case JOG_SPEED_X1_SELECTED:
                moveVector = JOG_SPEED_X1 * moveDirection;
                // Additional delay to allow individual encoder clicks to result in move
                // (Otherwise pulse duration is too short for Marlin to see it)
                delay(CLICK_PULSE_WIDTH);
        break;

        case JOG_SPEED_X10_SELECTED:
                moveVector = JOG_SPEED_X10 * moveDirection;
        break;

        case JOG_SPEED_X100_SELECTED:
                moveVector = JOG_SPEED_X100 * moveDirection;
        break;
    }

    // Adaptive speed - calculate output speed based on the speed that encoder is rotated
    if (adaptiveSpeedEnabled == 1) {

        // Lets limit the speed just in case
        if (clickSpeed > MAX_CLICK_SPEED){
            clickSpeed = MAX_CLICK_SPEED;
        } else if ((clickSpeed < MIN_CLICK_SPEED) && (clickSpeed > 0)) {          
            clickSpeed = MIN_CLICK_SPEED;
        }
      
        // Calculate the speed
        adaptiveSpeed = ((float)MAX_ADAPTIVE_SPEED / (float)ADAPTIVE_SPEED_RANGE) * clickSpeed;

        // Create the move vector
        moveVector = adaptiveSpeed * moveDirection;

    }

    // Select the correct VCR and set the wiper position.
    switch (selectedAxis)
    {
        case X_AXIS_SELECTED:
            wiperPosition = xCenter + moveVector;
            digitalWrite(X_AXIS_CS_PIN, LOW);
            SPI.transfer(address);
            SPI.transfer(wiperPosition);
            digitalWrite(X_AXIS_CS_PIN, HIGH);       
        break;

        case Y_AXIS_SELECTED:
            wiperPosition = yCenter + moveVector;
            digitalWrite(Y_AXIS_CS_PIN, LOW);
            SPI.transfer(address);
            SPI.transfer(wiperPosition);
            digitalWrite(Y_AXIS_CS_PIN, HIGH);
        break;

        case Z_AXIS_SELECTED:
            wiperPosition = zCenter + moveVector;
            digitalWrite(Z_AXIS_CS_PIN, LOW);
            SPI.transfer(address);
            SPI.transfer(wiperPosition);
            digitalWrite(Z_AXIS_CS_PIN, HIGH);
        break;
    }

    #ifdef SERIAL_DEBUG
//        Serial.println(wiperPosition);
    #endif
}



/****************************************
 * MAIN PROGRAM LOOP
 ***/
void loop(){ 

    // Decode axis & speed selector switches
    int selectedAxis = getAxis();
    int selectedSpeed = getSpeed();

    // If speed selector <> 0 Send enable signal to Marlin 
    if ((SPEED_SWITCH_ENABLE == true) && (selectedSpeed != 0)) {
      digitalWrite(ENABLE_OUT_PIN, HIGH);
    } else {
      digitalWrite(ENABLE_OUT_PIN, LOW);      
    }


    if((moveEnabled == 1) && (oldEncPos != encoderPos)) {

        // what direction are we moving?
        if ((oldEncPos > encoderPos)) {
            //we are rotating CCW
            sendJogSignal(selectedAxis, selectedSpeed, -1);    
            #ifdef SERIAL_DEBUG
                Serial.println("<<<<<<<<<<<<<<<<<<<");
            #endif
          
        } else {

            //we are rotating CW
            sendJogSignal(selectedAxis, selectedSpeed, 1);    
            #ifdef SERIAL_DEBUG
                Serial.println(">>>>>>>>>>>>>>>>>>>");
            #endif

        }
             
        // Encoder rolls over at 255 so lets put it back in the middle of the count
        if ((encoderPos == 255) || (encoderPos == 0)) {
            encoderPos = 128;
        }

        oldEncPos = encoderPos;               
        moveEnabled = false;
            
  } else {
    
        // We are not trying to move so send a zero vector to reset wiper position
        sendJogSignal(selectedAxis, selectedSpeed, ZERO);        
  }

  // Delay to create pulse width
  delay(JOG_PULSE_WIDTH);
    


}
