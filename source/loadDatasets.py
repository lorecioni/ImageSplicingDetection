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


    if name == 'SPLICED_COLORCHECKER' or name == 'SPLICED_DSO1':
        print('Loading spliced color checked dataset')
        files = os.listdir(config.imagesFolder)
        for i in files:
            filename = utils.getFilename(i)
            try:
                img = config.imagesFolder + i
                _, file_extension = os.path.splitext(img)
                if os.path.isfile(img) and not i.startswith('.') and file_extension == '.png':
                    images.append(img)
                    label = np.loadtxt(config.imagesFolder + filename + '.txt').tolist()
                    labels.append(label)
                    print('added ' + str(img))
            except:
                print("Error on processing image: " + i)

        print('Loaded splicing color checker dataset')

    return images, labels


def generateSplicedTrainingSet(direction):
    print('Loading color checked dataset')

    resized_width = 1200
    band_size = 200
    total = 100

    images = []
    labels = []
    files = os.listdir(config.imagesFolder)
    for i in files:
        try:
            img = config.imagesFolder + i
            if os.path.isfile(img) and not i.startswith('.') and i.startswith('normal'):
                images.append(img)
                labels.append(1)
        except:
            print("Error on processing image: " + i)
    print('Loaded color checker dataset')

    counter = 0
    for i in images:
        found = False
        while not found:
            randIdx = randint(0, len(images) - 1)
            if randIdx != counter:
                found = True

        host = cv2.imread(i)
        alien = cv2.imread(images[randIdx])

        host = utils.resizeImage(host, resized_width)
        resized_height, _, _ = host.shape
        alien = utils.resizeImage(alien, resized_width, resized_height)

        if direction == 'vertical':
            start = randint(1, resized_width/band_size - 1)
            start *= band_size

            end = start + band_size
            band = alien[:, start:end]
            host[:, start:end] = band

        elif direction == 'horizontal':
            found = False
            while not found:
                start = randint(0, int(resized_height/band_size) - 1)
                start *= band_size
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

#generateSplicedTrainingSet('vertical')
#generateSplicedTrainingSet('horizontal')





