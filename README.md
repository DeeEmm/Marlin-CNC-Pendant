# Marlin-CNC-Pendant

## Version Information
###

Version 1.0-Alpha.1




## What is it?
###

This repository contains the hardware schematics and software code needed to build a jogwheel based pendant controller to interface to Marlin / Arduino controlled 3D printers & CNCs 

The pendant uses an Arduino nano controller to decode data received from a CNC jogwheel / pulse encoder and send it to the Printer / CNC.

The nano mimics an analog joystick and so leverages the new analog joystick functionality available from Marlin 2.0.0. This limits the move queue to prevent overrun.


### So why not just use the joystick?

The joystick is a very good control method and works great on both 3D printers and CNCs, however the traditional control input for a CNC machine is a jogwheel and axis selector switch, which gives a very different operating experience.

This method also allows existing traditional jogwheel setups to be ustilised when retrofitting, as well as using universally and commonly available MPG pendant / handwheel controls.



## Contributors
###

- DeeEmm AKA Michael Percy



## Project Goals
###

Universal pendant / jogwheel / handwheel interface that interfaces to standard Marlin codebase using the joystick function


## Project Status
###

- The project is currently in Alpha stage
- Code compiles and works but needs validation
- Speed control and zero points need defining and testing


## Usage / Getting started
###

- Compile and upload to an Arduino nano
- Connect wiring according to pin connections in configuration.h

## BOM
###

- Arduino Nano
- X,Y,Z selector switch
- 1x,10x,100x selector switch
- Encoder jogwheel
- OR MPG jogwheel pendant

## License
###

This project is released under the GPLV3 license. For details on usage please refer to the license file included with this project



## Further Information
###

https://github.com/DeeEmm/Marlin-CNC-Pendant
