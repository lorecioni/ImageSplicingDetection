'''
Created on 10 gen 2017

@author: lorenzocioni
'''

import cv2
import numpy as np
import math
import os
import config
import shutil

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


def gray2binary(img):
    return img * 1/255
'''
Resize image maintaining aspect ratio
'''
def resizeImage(img, width, height = None):
    if height is None:
        r = width / img.shape[1]
        height = int(img.shape[0] * r)
    dim = (width, height)
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

def euclideanDistanceRGB(x, y):
    return math.sqrt(pow(x[0, 0] - y[0], 2) + pow(x[0, 1] - y[1], 2) + pow(x[0, 2] - y[2], 2))


def evaluateRGBMedian(img):
    map = cv2.imread(img)
    map_b, map_g, map_r = cv2.split(map)
    return np.array([[np.median(map_b), np.median(map_g), np.median(map_b)]])


def removeTempFolder():
    if os.path.exists(config.temp_folder):
        shutil.rmtree(config.temp_folder)


def removeFile(path):
    if os.path.isfile(path):
        os.remove(path)

def createTempFolder():
    if not os.path.exists(config.temp_folder):
        os.makedirs(config.temp_folder)

def evaluateEuclideanDistances(first, second, display):
    output = None
    # Extract image features from each images in training set
    if first is not None and second is not None:
        gge_b, gge_g, gge_r = cv2.split(first)
        iic_b, iic_g, iic_r = cv2.split(second)
        # Get maps dimensions
        rows, cols, _ = first.shape
        # Building heat map
        heat_map = np.sqrt(
            pow(gge_b[0:rows - 1, 0:cols - 1] - iic_b[0:rows - 1, 0:cols - 1], 2) +
            pow(gge_g[0:rows - 1, 0:cols - 1] - iic_g[0:rows - 1, 0:cols - 1], 2) +
            pow(gge_r[0:rows - 1, 0:cols - 1] - iic_r[0:rows - 1, 0:cols - 1], 2))
        # Recover heat map max value
        max_value = np.ndarray.max(heat_map)
        # Normalization
        heat_map = heat_map / max_value
        output = np.sum(heat_map)

        if display:
            heat_map *= 255
            heat_map = heat_map.astype(np.uint8)
            # Display color map
            color_map = cv2.applyColorMap(heat_map, cv2.COLORMAP_JET)
            cv2.imshow('img', resizeImage(color_map, 500))
            cv2.waitKey(0)
    return output


def averageRGBColor(img):
    avg_row = np.average(img, axis=0)
    avg_color= np.average(avg_row, axis=0)
    return np.uint8(avg_color)

def rgb2grayValue(rgb):
    r, g, b = rgb[0], rgb[1], rgb[2]
    gray = 0.2989 * r + 0.5870 * g + 0.1140 * b
    return gray


'''
Utility function for parsing a string into an integer
'''
def str2int(s):
    try:
        value = int(s)
    except ValueError:
        value = int(float(s))
    return value

'''
Reading detected faces bounding boxes
'''
def readExtractedFacesFile(path):
    if os.path.isfile(path):
        fid = open(path, 'r')
        fid.seek(0)
        lines = fid.readlines()
        fid.close()
        faces = []
        for i in lines:
            entry = i.strip().split(" ")
            x_min = str2int(entry[0])
            y_min = str2int(entry[1])
            x_max = str2int(entry[2])
            y_max = str2int(entry[3])
            #xmin ymin xmax ymax
            face = (x_min, y_min, x_max - x_min, y_max - y_min)
            faces.append(face)
        return faces, None
    else:
        return [], None
