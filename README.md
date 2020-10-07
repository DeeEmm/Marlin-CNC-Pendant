# Marlin-CNC-Pendant

## Version Information
###

Version 1.0-RC.1




## What is it?
###

This repository contains the hardware schematics and software code needed to build a jogwheel based pendant controller to interface to Marlin / Arduino controlled 3D printers & CNCs 

The pendant uses an Arduino nano controller to decode data received from a CNC jogwheel / pulse encoder and send it to the Printer / CNC.

The nano mimics an analog joystick and so leverages the new analog joystick functionality available from Marlin 2.0.0. This limits the move queue to prevent overrun.


### So why not just use the joystick?

The joystick is a very good control method and works great on both 3D printers and CNCs, however the traditional control input for a CNC machine is a jogwheel and axis selector switch, which gives a very different operating experience.

This method allows existing traditional jogwheel setups to be utilised when retrofitting as well as using universally and commonly available MPG pendant / handwheel controls.



## Contributors
###

- DeeEmm AKA Michael Percy



## Project Goals
###

Universal pendant / jogwheel / handwheel interface that interfaces to standard Marlin codebase using the joystick function


## Project Status
###

- The project is mostly finalised with the current release considered release candidate 1
- Current stable release available from https://github.com/DeeEmm/Marlin-CNC-Pendant/releases
- Code has been tested to work with new Marlin 2.0 release


## Usage / Getting started
###

- Compile and upload to an Arduino nano
- Connect wiring according to pin connections in configuration.h
- Connect X, Y, Z and Enable outputs to your CNC / 3D printer in accordance with Marlin instructions.
- Enable joystick control in Marlin
- Fine tune speed values as necessary
- Speed can be set as X1 / X10 / X100 / Adaptive. 
- Adaptive speed mode moves at speed proportional to rotation speed of encoder.


Here's a video of it in operation

[![Rotary encoder jog wheel](http://img.youtube.com/vi/_DIcDE9QY0A/0.jpg)](http://www.youtube.com/watch?v=_DIcDE9QY0A "Video Title")


## BOM
###

- Arduino Nano
- 3 off MPC4151 Voltage Controlled Resistors (10kohm)
- X,Y,Z selector switch
- 1x,10x,100x selector switch
- Encoder jogwheel
- OR MPG jogwheel pendant

## License
###

This project is released under the GPLV3 license. For details on usage please refer to the license file included with this project



## Further Information
###

- https://github.com/DeeEmm/Marlin-CNC-Pendant
- https://deeemm.com/index.php/entry/general/marlin-cnc-pendant

## Reference
###

- http://cdn.sparkfun.com/datasheets/Components/General%20IC/22060b.pdf
- http://www.learningaboutelectronics.com/Articles/MCP4131-digital-potentiometer-circuit.php
