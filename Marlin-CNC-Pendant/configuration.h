/****************************************
 * configurations.h - define variables and configure hardware
 * This file is part of the Marlin-CNC-Pendant. For more information including usage and licensing please refer to: https://github.com/DeeEmm/Marlin-CNC-Pendant
 * 
 * NOTES
 * output pins (*_OUT_PIN) connected to control boards as follows:
 *  
 * RAMPS physical pins
 * JOY_X_PIN   59  // RAMPS: Suggested pin A5  on AUX2
 * JOY_Y_PIN   60  // RAMPS: Suggested pin A10 on AUX2
 * JOY_Z_PIN   63  // RAMPS: Suggested pin A12 on AUX2
 * JOY_EN_PIN  64  // RAMPS: Suggested pin D44 on AUX2
 * 
 * RUMBA physical pins on EXP3 header (left 4 pins)
 * JOY_X_PIN   - pin 14 (Bottom/Left)
 * JOY_Y_PIN   - pin 13 (Top/Left)
 * JOY_Z_PIN   - pin 12 (Bottom/Right)
 * JOY_EN_PIN  - pin 11 (Top/Right)
 * 
 ***/

 #pragma once

 /****************************************
 * SELECT BOARD TYPE 
 ***/

#define ARDUINO_NANO //Default



/****************************************
 * GENERAL SETTINGS
 ***/
  

//#define SERIAL_DEBUG                      // Enable Serial debug mode

#define DEAD_BAND_CENTER_VALUE 128         // Value normally returned when joystick is in center (can be read from M119)

#define SMOOTHING_FACTOR 100               // Increase to smooth movement (Disabled for X1 speed)

#define JOG_SPEED_1_MULTIPLIER 1           // X1 Single step multiplier value
#define JOG_SPEED_10_MULTIPLIER 10         // X10 10% multiplier value
#define JOG_SPEED_100_MULTIPLIER 100       // X100 100% multiplier value

#define SPEED_SWITCH_ENABLE true            // Disable system if speed switch is in zero position?



/****************************************
 * CONFIGURE PINS 
 * Default configuration is for a nano
 * Add additional boards / configurations as required
 ***/

// ARDUINO NANO
#ifdef ARDUINO_NANO

    // Define interrupts: 
    // Jogwheel pins and interrupts need to be set for each board variant
    // Default interrupts 0 & 1 are on pins 2 & 3 on a nano. This may differ for other boards.
    #define INTERRUPT_A 0               // Default 0 = pin 2 on nano (JOGWHEEL_PIN_A)
    #define INTERRUPT_B 1               // Default 1 = pin 3 on nano (JOGWHEEL_PIN_B)


    // Define Physical Pins: 
    // (colour codes for generic MPG handwheel pendant included)
    #define JOGWHEEL_PIN_A 2            // MPG - green
    #define JOGWHEEL_PIN_B 3            // MPG - white

    #define JOG_SPEED_1_PIN 4           // MPG - orange
    #define JOG_SPEED_10_PIN 5          // MPG - grey/black
    #define JOG_SPEED_100_PIN 13        // MPG - grey
    
    #define X_AXIS_SELECT_PIN 6         // MPG - yellow
    #define Y_AXIS_SELECT_PIN 7         // MPG - yellow/black
    #define Z_AXIS_SELECT_PIN 8         // MPG - brown


    // Define outputs
    // See notes at top RE pin connection schema for RAMPs / RUMBA
    // NOTE: must be PWM pins
    #define X_AXIS_OUT_PIN 9            // JOY_X_PIN
    #define Y_AXIS_OUT_PIN 10           // JOY_Y_PIN 
    #define Z_AXIS_OUT_PIN 11           // JOY_Z_PIN 
    #define ENABLE_OUT_PIN 12           // JOY_EN_PIN 

#endif
