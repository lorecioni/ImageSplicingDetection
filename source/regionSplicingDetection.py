'''
Created on 03 ott 2016

@author: lorenzocioni
'''
import cv2
import numpy as np
import os
from classification import FaceTrainingSample, splitDataset, KNNClassifier
import descriptors
import illuminantMaps
import config
import utils

'''
Splicing detection main procedure. The result of the output
will be a number of regions of the image that can be spliced
over an image
'''



class RegionSplicingDetector:

    def __init__(self, extractMaps, extractFeatures, crossVal, verbose, heatMap):
        self.verbose = verbose
        self.extract_maps = extractMaps
        self.extract_features = extractFeatures
        self.cross_validation = crossVal
        self.heat_map = heatMap

        #Bands
        self.horizontalBands = 0
        self.verticalBands = 0


    def detect(self, img):
        filename = utils.getFilename(img)
        print('Processing ' + filename)

        #Reads image
        image = cv2.imread(img)
        if image is not None:
            #Dividing bands
            self.verticalBands = self.extractImageBands(image, 'vertical')
            self.horizontalBands = self.extractImageBands(image, 'horizontal')




    '''
    Extract vertical bands from the images with width
    configured in configuration file
    @:parameter image: OpenCV matrix
    @:returns number of extracted bands
    '''
    def extractImageBands(self, image, direction = 'vertical'):
        if self.verbose:
            print('Extracting ' + direction + ' bands')

        height, width, _ = image.shape

        if direction == 'vertical':
            limit = width
            size = config.bandWidth
        else:
            limit = height
            size = config.bandHeight

        # Band subdivisions
        counter, i = 0, 0
        # Exctracting vertical bands
        while i < limit:
            diff = i + size
            if diff < limit:
                end = diff
            else:
                end = limit - diff - 1

            if direction == 'vertical':
                band = image[:, i:end]
            else:
                band = image[i:end, :]

            W, H, _ = band.shape

            if (band is not None) and W > 0 and H > 0:
                #Store band
                bandPath = config.maps_folder + direction + '_band_' + str(counter) + '.png'
                cv2.imwrite(bandPath, band)
                counter += 1
            i += config.size

        return counter







