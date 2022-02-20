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


Sunday, Feb 06, 2022
--------------------

Math part finally nearing potential usefulness. There are several conversions to note
which are detailed in the LaTeX document.
Current implementation gives correct height value within the +-2.5cm measurement error range
when measuring from the floor to device origin.

Results are stored as CSV for intermediate use.

There is a lidar data visualizer site which I intend to make use of for validating the lidar data
conversions and usage - as far as I need to for current measurements: 
http://lidarview.com/

Lidarview uses LAS 1.2 formatted input files, so next part is converting cartesian xyz-coordinate CSV into
LAS 1.2 file. Automating this pipeline improves my feedback time from device measurements to 
visually inspectable image that can be improved. Same goes for combining possible future measurements from several 
device locations in the same scene / 3d space.


Sunday, Feb 13, 2022
--------------------

Added intensity measurement. Noticed that my matte hanged images show difference in lidar intensity depending
on the viewing angle: From straight ahead it does not differ from matte wall, from more 
acute angle it shows significant difference.

Updated the pan/tilt angle step to 1 degree, and was happy to notice that even current wobbly construction
of lidar arm gives fairly proper results.

Open issue: Serial printing in different rows seems to occasionally skip some characters. This might
have to do with printing several entries in sequence, rather than printing it all in one printf


Sunday, Feb 20, 2022
--------------------

Was able to run the scan with 1 degree differences. Realized that in order to build a proper scene,
I need to patch together a set of different angles from same origin pointi. Instead of patching several
measurement sets together, I'd much rather build a better mounting for the lidar that allows free 360 degree
panning.
Ideally one that would get rid of the vibration and wobbliness issues. It would be possible to also set
up the lidar so that the lidar itself woulbe in the origin point of the measurements, even without translating
the angular measurements by the lidar control arm dimensions. With proper wiring we should be able to get
rid of the current situation where lidar has to drag its wiring at more extreme angles.

So next part is learning about motors or continuous rotation servos. In order to deliver power and signals
to freely panning system, we need to have a slip ring. Promising cheap option for it is the 2A 12 wire Sparkfun slip ring
https://www.sparkfun.com/products/13065

For now, I will see if I could build the lidar mount system using some readily available build kits such
as the Actobotics product line: https://www.sparkfun.com/videos#actobotics/d3MBTT10pxk

