'''
Created on 08 gen 2017

@author: lorenzocioni
'''

import sys
import os
import cv2
import config
import utils
import subprocess

# Extract an specific descriptor from one image (Ex. SASI, BIC, etc)
# IN:
#   fileName -- image file name
#    descriptor -- descriptor name
#   space -- image color space
#   channel -- image color channel where descriptor will be extracted
#
# OUT:
#   a text file containing descriptor values
devnull = open(os.devnull, 'w')

def extractDescriptor(img, descriptor, space, channel):
    filename = utils.getFilename(img)
    
    descriptorName = config.faces_folder + filename + "-" + descriptor.lower() + "-desc.txt"
    nname = img

    newName = nname[:-3] + "ppm"
    sourceImg = cv2.imread(img)
    if space == 0:
        destImg = cv2.cvtColor(sourceImg, cv2.COLOR_BGR2HSV)
    elif space == 1:
        destImg = cv2.cvtColor(sourceImg, cv2.COLOR_BGR2RGB)
    elif space == 2:
        destImg = cv2.cvtColor(sourceImg, cv2.COLOR_BGR2YCR_CB)
    elif space == 4:
        destImg = cv2.cvtColor(sourceImg, cv2.COLOR_BGR2LAB)
    else:
        destImg = sourceImg
        
    cv2.imwrite(nname, destImg)

    command = config.convertBinary + " " + nname + " " + newName
    subprocess.call([command], stdout = devnull, stderr = devnull, shell = True)
    command = "descriptors/" + descriptor.lower() + "/source/bin/./" + descriptor.lower() + "_extraction " + newName + " " + descriptorName
    subprocess.call([command], stdout = devnull, stderr = devnull, shell = True)
