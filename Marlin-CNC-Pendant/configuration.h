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
 * CONFIGURE PINS 
  ***/


// ARDUINO NANO
#ifdef ARDUINO_NANO

    // Define Physical Pins
    #define TX_PIN 1
    #define RX_PIN 2

    #define JOGWHEEL_PIN_A 2
    #define JOGWHEEL_PIN_B 3

    #define JOG_SPEED_100_PIN 5
    #define JOG_SPEED_10_PIN 6
    #define JOG_SPEED_1_PIN 7
    #define JOG_SPEED_01_PIN 8
    #define X_AXIS_SELECT_PIN 9
    #define Y_AXIS_SELECT_PIN 10
    #define Z_AXIS_SELECT_PIN 11

    #define ENABLE_OUT_PIN 12
    #define X_AXIS_OUT_PIN A0 
    #define Y_AXIS_OUT_PIN A1 
    #define Z_AXIS_OUT_PIN A2 


    // Define Voltage Levels
    #define JOG_SPEED_100_VOLTAGE 10000
    #define JOG_SPEED_10_VOLTAGE 1000
    #define JOG_SPEED_1_VOLTAGE 500
    #define JOG_SPEED_01_VOLTAGE 10



#endif


/****************************************
 * GENERAL SETTINGS
  ***/

#define DEV_MODE





/*
3. Installing joystick - 

change pins in configuration_adv.h and enable it

#define JOYSTICK
#if ENABLED(JOYSTICK)
  #define JOY_X_PIN   59  // RAMPS: Suggested pin A5  on AUX2
  #define JOY_Y_PIN   60  // RAMPS: Suggested pin A10 on AUX2
  #define JOY_Z_PIN   63  // RAMPS: Suggested pin A12 on AUX2
  #define JOY_EN_PIN  64  // RAMPS: Suggested pin D44 on AUX2
  

RUMBA physical pins on EXP3 header (left 4 pins)

JOY_X_PIN   - pin 14 (Bottom/Left)
JOY_Y_PIN   - pin 13 (Top/Left)
JOY_Z_PIN   - pin 12 (Bottom/Right)
JOY_EN_PIN  - pin 11 (Top/Right)
 */
