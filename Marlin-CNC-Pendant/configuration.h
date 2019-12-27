/****************************************
 * configurations.h - define variables and configure hardware
 * This file is part of the Marlin-CNC-Pendant. For more information including usage and licensing please refer to: https://github.com/DeeEmm/Marlin-CNC-Pendant
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

#define DEV_MODE



/****************************************
 * CONFIGURE PINS 
 * Default configuration is for a nano
 ***/

// ARDUINO NANO
#ifdef ARDUINO_NANO

    // Define interrupts
    // Jogwheel pins and interrupts need to be set for board
    // Default interrupts 0 & 1 = pins 2 & 3 on a nano
    #define INTERRUPT_A 0         // Default 0 = pin 2 on nano (JOGWHEEL_PIN_A)
    #define INTERRUPT_B 1         // Default 1 = pin 3 on nano (JOGWHEEL_PIN_B)

    // Define Physical Pins
    #define JOGWHEEL_PIN_A 2      // MPG - green
    #define JOGWHEEL_PIN_B 3      // MPG - white

    #define ENABLE_PIN 5          // MPG - orange/black
    #define JOG_SPEED_100_PIN 6   // MPG - orange
    #define JOG_SPEED_10_PIN 7    // MPG - grey/black
    #define JOG_SPEED_1_PIN 8     // MPG - grey
    
    #define X_AXIS_SELECT_PIN 9   // MPG - Yellow
    #define Y_AXIS_SELECT_PIN 10  // MPG - yellow/black
    #define Z_AXIS_SELECT_PIN 11  // MPG - brown

    #define ENABLE_OUT_PIN 12     // JOY_EN_PIN (see notes below)
    #define X_AXIS_OUT_PIN A0     // JOY_X_PIN (see notes below)
    #define Y_AXIS_OUT_PIN A1     // JOY_Y_PIN (see notes below)
    #define Z_AXIS_OUT_PIN A2     // JOY_Z_PIN (see notes below)

    // Define Voltage Levels
    // Voltage normally returned when joystick is in center (can be read from M119)
    #define DEAD_BAND_CENTRE_VALUE 25 
    
    // May need other multipler levels depending on actual jog speed
    #define JOG_SPEED_100_MULTIPLER 100
    #define JOG_SPEED_10_MULTIPLER 10
    #define JOG_SPEED_1_MULTIPLER 1

#endif





/****************************************
 * NOTES
 * output pins (*_OUT_PIN)
 * connected to control boards as follows:
 * 
 ***/

/*
RAMPS physical pins
JOY_X_PIN   59  // RAMPS: Suggested pin A5  on AUX2
JOY_Y_PIN   60  // RAMPS: Suggested pin A10 on AUX2
JOY_Z_PIN   63  // RAMPS: Suggested pin A12 on AUX2
JOY_EN_PIN  64  // RAMPS: Suggested pin D44 on AUX2
  

RUMBA physical pins on EXP3 header (left 4 pins)
JOY_X_PIN   - pin 14 (Bottom/Left)
JOY_Y_PIN   - pin 13 (Top/Left)
JOY_Z_PIN   - pin 12 (Bottom/Right)
JOY_EN_PIN  - pin 11 (Top/Right)
 */
