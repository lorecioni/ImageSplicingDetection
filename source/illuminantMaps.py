'''
Created on 03 ott 2016

@author: lorenzocioni
'''

import os
import subprocess
import config
import utils

#Segment image
devnull = open(os.devnull, 'w')

#Preparing image for illuminant methods
def prepareImageIlluminants(img, sigma, k, min_size, max_intensity, min_intensity, verbose):
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    outfile = config.maps_folder + filename[:-4] + "_segmented.png"
    if not os.path.isfile(outfile) or config.forceMapsExtraction:
        if verbose:
            print('Segmenting image for illuminant methods...')
        
        if not config.use_riess_default:
            command = config.voleBinary + " felzenszwalb -I " + img + " --deterministic_coloring -O " + outfile + " --sigma " + str(sigma) + " --k " + str(k) + " --min_size " + str(min_size) + " --max_intensity " + str(max_intensity) + " --min_intensity " + str(min_intensity)    
        else:
            command = config.voleBinary + " felzenszwalb -I " + img + " --deterministic_coloring -O " + outfile +" --k " + str(k) + " --max_intensity 255"   
        subprocess.call([command], stdout = devnull, stderr = devnull, shell = True)
        if verbose:
            print('Image segmented')


#Extracting GGE illuminant map
def extractGGEMap(img, segmentedImg, sigma, n, p, verbose, output = None):
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    if output is None:
        outfile = config.maps_folder + filename[:-4] + "_gge_map_" + str(n) + "_" + str(p) + ".png"
    else:
        outfile = output
    if not os.path.isfile(outfile) or config.forceMapsExtraction:
        if verbose:
            print('Extracting GGE map...')
        
        if not config.use_riess_default:
            command = config.voleBinary + " lgrayworld --img.image " + img + " -S " + config.maps_folder + segmentedImg + " -O " + outfile + " --n " +  str(n) + " --p " + str(p) + " --sigma " + str(sigma)
        else:
            command = config.voleBinary + " lgrayworld --img.image " + img + " -S " + config.maps_folder + segmentedImg + " -O " + outfile + " --n " + str(n) + " --p " + str(p) + " --sigma " + str(sigma)
        subprocess.call([command], stdout = devnull, stderr = devnull, shell = True)
        if verbose:
            print('GGE map extracted')


def estimateGrayWorld(img, segmentedImg, verbose):
    filename = utils.getFilename(img)
    output = config.maps_folder + filename + "_gge_map_grayworld.png"
    extractGGEMap(img, segmentedImg, 1, 0, 1, verbose, output = output)

def estimateMaxRGB(img, segmentedImg, verbose):
    filename = utils.getFilename(img)
    output = config.maps_folder + filename + "_gge_map_maxrgb.png"
    extractGGEMap(img, segmentedImg, 1, 0, 20, verbose, output = output)

def estimateShadesOfGray(img, segmentedImg, verbose):
    filename = utils.getFilename(img)
    output = config.maps_folder + filename + "_gge_map_shadesofgray.png"
    extractGGEMap(img, segmentedImg, 1, 0, 5, verbose, output = output)

def estimateGrayEdge(img, segmentedImg, verbose):
    filename = utils.getFilename(img)
    output = config.maps_folder + filename + "_gge_map_grayedge.png"
    extractGGEMap(img, segmentedImg, 1, 1, 1, verbose, output = output)

def estimateSecondGrayEdge(img, segmentedImg, verbose):
    filename = utils.getFilename(img)
    output = config.maps_folder + filename + "_gge_map_secondgrayedge.png"
    extractGGEMap(img, segmentedImg, 1, 2, 1, verbose, output = output)


#Extracting IIC illuminant map
def extractIICMap(img, segmentedImg, verbose):
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    outfile = config.maps_folder + filename[:-4] + "_iic_map.png"
    if not os.path.isfile(outfile) or config.forceMapsExtraction:
        if verbose:
            print('Extracting IIC map...')
        filename = img.split('/')
        filename = filename[len(filename) - 1]
        if not config.use_riess_default:
            command = config.voleBinary + " liebv --img.image " + img + " -S " + config.maps_folder + segmentedImg + " -O " + outfile + " --iebv_config " + config.config_iic
        else:
            command = config.voleBinary + " liebv --img.image " + img + " -S " + config.maps_folder + segmentedImg + " -O " + outfile + " --iebv_config illuminants_riess/lille/config/config_iebv_lenient.txt"
        subprocess.call([command], stdout = devnull, stderr = devnull, shell = True)
        if verbose:
            print('IIC map extracted')
