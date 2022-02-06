import numpy as np
import laspy
from sys import argv

def convert_csv_to_las(filename, target):
    coords = np.loadtxt(filename,
            delimiter=',', dtype=float)
    
    header = laspy.LasHeader(point_format=3, version="1.2")
    header.offsets = np.array([0, 0, 0])
    header.scales = np.array([0.01, 0.01, 0.01])

    las = laspy.LasData(header)
    las.x = coords[:,1]
    las.y = coords[:,2]
    las.z = coords[:,3]
    las.write(target)

if __name__=='__main__':
    srcFile = argv[1]
    target = srcFile + "-1_2.las"
    convert_csv_to_las(srcFile, target)

