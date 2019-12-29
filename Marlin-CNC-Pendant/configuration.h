/****************************************
 * configurations.h - define variables and configure hardware
 * This file is part of the Marlin-CNC-Pendant. For more information including usage and licensing please refer to: https://github.com/DeeEmm/Marlin-CNC-Pendant
 * 
 * 
 * NOTE
 * output pins (*_OUT_PIN) connected to control boards as follows:
 * physical > logical pin reference  https://www.arduino.cc/en/Hacking/PinMapping2560 
 * 
 * 
 * RAMPS physical pins
 * JOY_X_PIN   - AUX2 pin A5  (logical Pin 59)
 * JOY_Y_PIN   - AUX2 pin A10 (logical Pin 60)
 * JOY_Z_PIN   - AUX2 pin A12 (logical Pin 63)
 * JOY_EN_PIN  - AUX2 pin D44 (logical Pin 64)
 * 
 * 
 * PWM OUTPUT SMOOTHING
 * You will need to connect an R/C network to smooth the PWM output into a 'steady' state DC voltage.
 * For this you will need one 100uf capacitor and a resistor between 160-330 ohms connected as follows:
 * 
 * O/P o-----[ 330R ]-------------o CNC
 *                      |                             
 *                     ___
 *                     ___ 100uf
 *                      |
 *                      |
 *                      o GND (0v)
 * 
 * This will filter much of the square wave PWM output into a much smaller ripple 
 * 
 *  ___   ___
 *  |  |  |  |    Will become...  /\/\/\/\/\/\/\/
 * _|  |__|  |_  
 * 
 * There's still some small ripple there, so we need to open up the dead band in the joystick settings to compensate...
 * 
 * 
 * MARLIN CONFIGURATION
 * The following values can be found in Configuration_adv.h in Marlin 
 * 
 * Uncomment the following line...
 * #define JOYSTICK
 * 
 * Next we need to tune the joystick values by using the M118 command in your printer host program (eg repetier)
 * For the Gcode command to work you will need to add the following line
 * #define JOYSTICK_DEBUG
 * 
 * Then issue the M119 command a nuber of times and average out the reading. This is the centre (the 7300 value below)
 * Next you want to addjust the deadband limits (the +/-1300 value below). This value has to be larger than any of the readings you 
 * got from the M119 command.
 * 
 * #define JOY_X_LIMITS { 5600, 7300-1300, 7300+1300, 10800 } // min, deadzone start, deadzone end, max
 * #define JOY_Y_LIMITS { 5600, 7300-1300, 7300+1300, 11000 }
 * #define JOY_Z_LIMITS { 4800, 7300-1300, 7300+1300, 11550 }
 * 
 * 
 * Notes:
 * 
 * Nano O/P resolution = 256 steps
 * .: center position = 128 steps
 * Marlin joystick range = 2^10 = 16384 (10 bit resolution)
 * .: 16384 points / 256 = 64 points per step
 * Voltage range = 5v
 * 5/256 = 19.6mv per step
 * Approx deadband required for a 10khz RC low pass filter
 * 2600 points (approx 41 steps or 0.8v) 
 * .: 128+21 or 128-21 steps before movement (approx 0.4v)
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

//#define SERIAL_DEBUG                    // Enable Serial debug mode

#define JOY_X_VALUE 5627                  // X value returned when stationary using M119
#define JOY_Y_VALUE 7759                  // Y value returned when stationary using M119
#define JOY_Z_VALUE 7635                  // Z value returned when stationary using M119

#define X1_PULSE_WIDTH 10                 // Jog pulse duration in ms (X1 speed only)
#define SMOOTHING_FACTOR 100               // Increase to smooth movement (Disabled for X1 speed)
#define VECTOR_VALUE 1                    // Vector multiplier (Default 1).
#define JOG_SPEED_X1 30                   // Speed adjustment (yCenter + JOG_SPEED_X1 * VECTOR_VALUE = move speed)     
#define JOG_SPEED_X10 40         
#define JOG_SPEED_X100 50       

#define SPEED_SWITCH_ENABLE true          // Disable system if speed selector switch is open: [0] - X1 - X10 - X100
                                          // Otherwise enable signal should be mnaged externally with a physical switch



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

    #define JOG_SPEED_X1_PIN 4           // MPG - orange
    #define JOG_SPEED_X10_PIN 5          // MPG - grey/black
    #define JOG_SPEED_X100_PIN 13        // MPG - grey
    
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
