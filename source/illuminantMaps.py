'''
Created on 03 ott 2016

@author: lorenzocioni
'''

import sys
import os
from subprocess import *

#IIC configuration file
config = "config.txt"

def extractIICMap(img, segmentedImg):
    print('Extracting IIC map...')
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    command = "illum_maps/build/./vole liebv --img.image " + img + " -S " + segmentedImg + " -O maps/"  + filename[:-4] + "_fhs.png --iebv_config illum_maps/build/" + config
    os.system(command)
    print('IIC map extracted')