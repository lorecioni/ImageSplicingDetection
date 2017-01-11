'''
Created on 03 ott 2016

@author: lorenzocioni
'''

import os
import subprocess
import config

#Segment image
devnull = open(os.devnull, 'w')

#Preparing image for illuminant methods
def prepareImageIlluminants(img, sigma, k, min_size, max_intensity, min_intensity, verbose):
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    outfile = config.maps_folder + filename[:-4] + "_segmented" + config.maps_out_suffix + ".png"
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
def extractGGEMap(img, segmentedImg, sigma, n, p, verbose):
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    outfile = config.maps_folder + filename[:-4] + "_gge_map" + config.maps_out_suffix + ".png"
    if not os.path.isfile(outfile) or config.forceMapsExtraction:
        if verbose:
            print('Extracting GGE map...')
        
        if not config.use_riess_default:
            command = config.voleBinary + " lgrayworld --img.image " + img + " -S " + config.maps_folder + segmentedImg + " -O " + outfile + " --n " +  str(n) + " --p " + str(p) + " --sigma " + str(sigma)
        else:
            command = config.voleBinary + " lgrayworld --img.image " + img + " -S " + config.maps_folder + segmentedImg + " -O " + outfile + " --n 1 --p 1 --sigma 1"
        subprocess.call([command], stdout = devnull, stderr = devnull, shell = True)
        if verbose:
            print('GGE map extracted')

    
#Extracting IIC illuminant map
def extractIICMap(img, segmentedImg, verbose):
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    outfile = config.maps_folder + filename[:-4] + "_iic_map" + config.maps_out_suffix + ".png"
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
        