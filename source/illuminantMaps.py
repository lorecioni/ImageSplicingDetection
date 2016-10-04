'''
Created on 03 ott 2016

@author: lorenzocioni
'''

import sys
import os
from subprocess import *

#Segment image

max_intensity = 0.98823529411764705882
min_intensity = .05882352941176470588

#Preparing image for illuminant methods
def prepareImageIlluminants(img, sigma, k, min_size, max_intensity, min_intensity):
    print('Segmenting image for illuminant methods...')
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    command = "illuminants/build/./vole felzenszwalb -I " + img + " --deterministic_coloring -O maps/" + filename[:-4] + "_segmented.png --sigma " + str(sigma) + " --k " + str(k) + " --min_size " + str(min_size) + " --max_intensity " + str(max_intensity) + " --min_intensity " + str(min_intensity)
    os.system(command)
    #print(command)
    print('Image segmented')
    
    
#Extracting GGE illuminant map
def extractGGEMap(img, segmentedImg, sigma, n, p):
    print('Extracting GGE map...')
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    command = "illum_maps/build/./vole lgrayworld --img.image " + img + " -S " + "maps/" + filename[:-4] + "_gge_map.png --n " +  str(n) + " --p " + str(p) + " --sigma " + str(sigma)
    os.system(command)
    print('GGE map extracted')

    
#Extracting IIC illuminant map
config = "config.txt"

def extractIICMap(img, segmentedImg):
    print('Extracting IIC map...')
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    command = "illuminants/build/./vole liebv --img.image " + img + " -S maps/" + segmentedImg + " -O maps/"  + filename[:-4] + "_iic_map.png --iebv_config illuminants/build/" + config
    os.system(command)
    #print(command)
    print('IIC map extracted')