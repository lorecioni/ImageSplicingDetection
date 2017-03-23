'''
Created on 14 nov 2016

@author: lorenzocioni
'''

import os
import config
import utils
import re
import cv2
from random import randint
import numpy as np

def load(name = None):
    if name is None:
        name = config.dataset

    images = []
    labels = []
    
    if name == 'DSO-1' or name == 'DSI-1':
        print('Loading ' + name + ' dataset')
        #Retrieving file list
        files = os.listdir(config.imagesFolder)
        for i in files:
            filename = utils.getFilename(i)
            try:
                img = config.imagesFolder + i
                if os.path.isfile(img) and not i.startswith('.'):
                    images.append(img)
                    #Retrieving label
                    labelFile = open(config.labelsFolder + filename + '.txt', 'r')
                    imageLabels = [re.split(r'\t+', s.strip()) for s in labelFile.readlines()]
                    labelFile.close()
                    labels.append(imageLabels)
            except:
                print("Error on processing image: " + i)
                
        print('Loaded ' + name + ' dataset')


    if name == 'COLORCHECKER':
        print('Loading color checked dataset')
        files = os.listdir(config.imagesFolder)
        for i in files:
            try:
                img = config.imagesFolder + i
                if os.path.isfile(img) and not i.startswith('.'):
                    images.append(img)
                    labels.append(1)
            except:
                print("Error on processing image: " + i)

        print('Loaded color checker dataset')

    return images, labels


def generateSplicedTrainingSet(direction):
    print('Loading color checked dataset')

    resized_width = 800
    band_size = 120
    total = 200
    min_padding = 100

    images = []
    labels = []
    files = os.listdir(config.imagesFolder)
    for i in files:
        try:
            img = config.imagesFolder + i
            if os.path.isfile(img) and not i.startswith('.'):
                images.append(img)
                labels.append(1)
        except:
            print("Error on processing image: " + i)
    print('Loaded color checker dataset')

    counter = 0
    for i in images:
        randIdx = randint(0, len(images))
        host = cv2.imread(i)
        alien = cv2.imread(images[randIdx])

        host = utils.resizeImage(host, resized_width)
        resized_height, _, _ = host.shape
        alien = utils.resizeImage(alien, resized_width, resized_height)

        start = 0


        if direction == 'vertical':
            found = False
            while not found:
                start = randint(0, resized_width)
                if min_padding < start < (resized_width - band_size - min_padding):
                    found = True

            end = start + band_size
            band = alien[:, start:end]
            host[:, start:end] = band

        elif direction == 'horizontal':
            found = False
            while not found:
                start = randint(0, resized_height)
                if start < resized_height - band_size - 1:
                    found = True

            end = start + band_size
            band = alien[start:end, :]
            host[start:end, :] = band

        path = config.output_spliced_dataset_folder + direction + '/'
        filename = path + direction + '_spliced_' + str(counter)
        cv2.imwrite(filename + '.png', host)
        np.savetxt(filename + '.txt', np.array([start, end, band_size]))

        counter += 1
        if counter == total:
            break

generateSplicedTrainingSet('vertical')
generateSplicedTrainingSet('horizontal')





