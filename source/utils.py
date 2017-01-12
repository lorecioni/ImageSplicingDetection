'''
Created on 10 gen 2017

@author: lorenzocioni
'''

import cv2
import numpy as np
'''
Utilities functions
'''

'''
Get the filename of a file
'''
def getFilename(path):
    filename = path.split('/')
    filename = filename[len(filename) - 1]
    filename = filename[:-4]
    return filename

'''
Convert image to grayscale
'''
def rgb2gray(img):
    return np.dot(img[...,:3], [0.299, 0.587, 0.144])

'''
Resize image maintaining aspect ratio
'''
def resizeImage(img, width):
    r = width / img.shape[1]
    dim = (width, int(img.shape[0] * r))
    # perform the actual resizing of the image and show it
    return cv2.resize(img, dim, interpolation = cv2.INTER_AREA)


def readTrainingTestFiles(outfile):
    ofid = open(outfile,'rt')
    ofid.seek(0)
    lines = ofid.readlines()
    ofid.close()
    features = []
    labels = []
    for i in lines:
        label = 0
        tmp = i[:-2].split(" ")
        row = []
        for j in tmp:
            if (label != 0):
                tmp2 = j.split(":")
                row.append(tmp2[1])
            else:
                label = j
                labels.append(j)
        features.append(row)
    return(features,labels)