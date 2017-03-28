'''
Created on 08 gen 2017

@author: lorenzocioni
'''

import sys
import os
import cv2
import config
import utils
import numpy as np
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

def extractDescriptor(img, descriptor, space = 0, channel = 3):
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

    if descriptor.lower() == 'sasi' or descriptor.lower() == 'las' or descriptor.lower() == 'unser':
        command += ' 1'
    subprocess.call([command], stdout = devnull, stderr = devnull, shell = True)


def buildFaceFeatureVector(firstPath, secondPath, descriptor):
    #Open files
    filesFirst = open(firstPath, "rt")
    filesSecond = open(secondPath, "rt")
    descriptor = descriptor.upper()
    #Switch on descriptor type
    if descriptor == 'ACC' or descriptor == 'BIC':
        firstFace = (filesFirst.read().splitlines())[1]
        secondFace = (filesSecond.read().splitlines())[1]
        feature = firstFace + secondFace
        feature = " ".join(feature)
    elif descriptor == 'CCV':
        firstFace = filesFirst.read().splitlines()
        firstFace.pop(0)
        secondFace = filesSecond.read().splitlines()
        secondFace.pop(0)
        feature = firstFace + secondFace
        feature = " ".join(feature)
    elif descriptor == 'LCH':
        firstFace = filesFirst.read().splitlines()
        firstFace = firstFace[1]
        secondFace = filesSecond.read().splitlines()
        secondFace = secondFace[1]
        feature = firstFace + " " + secondFace
    elif descriptor == 'SASI':
        firstFace = filesFirst.read().splitlines()
        firstFace = firstFace[1]
        secondFace = filesSecond.read().splitlines()
        secondFace = secondFace[1]
        feature = firstFace + " " + secondFace
    elif descriptor == 'UNSER':
        firstFace = filesFirst.read().splitlines()
        firstFace = firstFace[1]
        secondFace = filesSecond.read().splitlines()
        secondFace = secondFace[1]
        feature = firstFace + " " + secondFace

    filesSecond.close()
    filesFirst.close()

    return feature