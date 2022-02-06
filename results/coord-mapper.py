import csv
from sys import argv
from math import radians, sin, cos

# Rotation arm constant translation values.
panTranslationX = -2.7
panTranslationZ = -3.5
tiltTranslationX = -2.5
tiltTranslationZ = -1


# Input 
# - distance in cm
# - pan angle in degrees relative to initial position 0
# - tilt angle in degrees relative to initial position 0
# Return: Cartesian coordinates of the point relative to origin: [ x y z ]
def convert(distance, panDegree, tiltDegree):
    dist = float(distance)
    panRad = radians(int(panDegree))
    tiltRad = radians(int(tiltDegree))
    cosPan = cos(panRad)
    cosTilt = cos(tiltRad)
    sinPan = sin(panRad)
    sinTilt = sin(tiltRad)
    
    x = dist * cosPan * cosTilt + panTranslationX + tiltTranslationX * cosPan
    y = dist * sinPan * cosTilt + sinPan * tiltTranslationX
    z = dist * sinTilt + tiltTranslationZ + panTranslationZ
    return [x, y, z]

def main():
    with open(argv[1], mode='r') as file:
        csvFile = csv.reader(file)
        for line in csvFile:
            [runId, dist, pan, tilt] = line
            [x, y, z] = convert(dist, pan, tilt)
            print("{},{},{},{}".format(runId, x, y, z))

if __name__ == '__main__':
    main()

