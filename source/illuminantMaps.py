'''
Created on 03 ott 2016

@author: lorenzocioni
'''

import os
import subprocess

#Segment image
devnull = open(os.devnull, 'w')

#Preparing image for illuminant methods
def prepareImageIlluminants(img, sigma, k, min_size, max_intensity, min_intensity, verbose):
    if verbose:
        print('Segmenting image for illuminant methods...')
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    command = "illuminants/build/./vole felzenszwalb -I " + img + " --deterministic_coloring -O " + config.maps_folder + filename[:-4] + "_segmented.png --sigma " + str(sigma) + " --k " + str(k) + " --min_size " + str(min_size) + " --max_intensity " + str(max_intensity) + " --min_intensity " + str(min_intensity)    
    subprocess.call([command], stdout = devnull, stderr = devnull, shell = True)
    if verbose:
        print('Image segmented')
    
    
#Extracting GGE illuminant map
def extractGGEMap(img, segmentedImg, sigma, n, p, verbose):
    if verbose:
        print('Extracting GGE map...')
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    command = "illuminants/build/./vole lgrayworld --img.image " + img + " -S " + config.maps_folder + segmentedImg + " -O " + config.maps_folder + filename[:-4] + "_gge_map.png --n " +  str(n) + " --p " + str(p) + " --sigma " + str(sigma)
    subprocess.call([command], stdout = devnull, stderr = devnull, shell = True)
    if verbose:
        print('GGE map extracted')

    
#Extracting IIC illuminant map
config = "config.txt"

def extractIICMap(img, segmentedImg, verbose):
    if verbose:
        print('Extracting IIC map...')
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    command = "illuminants/build/./vole liebv --img.image " + img + " -S " + config.maps_folder + segmentedImg + " -O " + config.maps_folder + filename[:-4] + "_iic_map.png --iebv_config illuminants/build/" + config
    subprocess.call([command], stdout = devnull, stderr = devnull, shell = True)
    if verbose:
        print('IIC map extracted')

    
