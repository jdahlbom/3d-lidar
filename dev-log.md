Development log
===============

Monday, Jan 22nd, 2018
----------------------
Picked up from where I left a week ago:
- Servo control is working, but the pan servo is aligned off to the side.
- Attachment plate for the lidar<->servo connection needs to be designed,
  it's more trouble than worth to rig something temporary compared to ordering 3d printed plate.
- Lidar measurements & I2C is untested, circuit schematic created for the wiring.

Decided to focus solely on the Lidar measurements and I2C aspect.
I begun by grounding the unused PWM-pins in the Lidar wiring just in case. I used 12 kohm resistors between the pin and GND.
Could not get any reading from the Lidar - not sure if the measurement should be visible laser but if so, the whole
measurement is not working at all. Tried using the I2C for reading, but didn't really measure I2C properly for it.

I'm still missing the recommended 680uF capacitor between 5V and GND, will have to scrounge older projects for the capacitor.

/ Jukka


Monday, Jan 29th, 2018
----------------------
We're using DSSCircuits.com Arduino I2C Master Library in order to use multibyte read support available in Lidar Lite v3
dsscircuits.com/articles/86-articles/66-arduino-i2c-master-library

Earlier code didn't send the mearurement byte.

Useful example of I2C was found at https://github.com/PulsedLight3D/LIDARLite_StateMachine

Rewrote the read logic to similar to PulsedLight3D example.
Got huge 680uF 200V capacitors to wait for installation.

Next up: Set up the capacitor, test the new I2C code. I expect it's still not working at first try.
If there are problems, one more potential reason might be the pullup resistor setup which is
a "best guess" at the moment.

/ Jukka


Saturday, Dec 25, 2021
----------------------
Reviving old project: Last recollection was that I never got the I2C working back in 2018.

Got possibly better hookup guide: https://learn.sparkfun.com/tutorials/lidar-lite-v3-hookup-guide

Guide mentions that I2C devices are very sensitive to connection failures, and recommends
soldering wires instead of relying on just a breadboard.
This is probably why I ordered protoboards back when I was last working on the project.


Monday, Jan 17, 2022
--------------------
I got the LIDAR part done with stripboard. The hookup guide above mentioned that
I2C is very sensitive to contact errors, so I soldered everything together with the stripboard
and soldered the I2C wires to male pin headers which connect to Arduino.
Worked with first attempt.

Next part is adding the pan-tilt servo control.
Looks like the easiest approach to that is using Arduino Servo-library.
https://github.com/arduino-libraries/Servo/blob/master/src/Servo.h

Found useful example code here:
https://maker.pro/arduino/tutorial/how-to-set-up-pan-tilt-camera-stand-with-arduino-uno-and-joystick-module

Someone somewhere also mentioned that the servo controls seems wrong,
and the servo doesn't actually turn as much as it should. They had fixed it
by using shorter PWM cycle length. Look into it if servos are not working well.


Sunday, Jan 30, 2022
--------------------

Theory notes:
- Easy way of logging the arduino output might be with screen:
  https://www.pragmaticlinux.com/2021/11/how-to-monitor-the-serial-port-in-linux/

- I should add a reset switch to the project - something that leads to servos resetting to
  their initial position, generating a new run id for the scan and starting the scan anew.

- In order to map the scanner outputs values (tilt, pan, distance) I will need to 
  compute the conversion matrces - for example: 
  https://www.brainvoyager.com/bv/doc/UsersGuide/CoordsAndTransforms/SpatialTransformationMatrices.html
