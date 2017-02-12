'''
Created on 14 nov 2016

@author: lorenzocioni
'''

import os
import config
import utils
import re

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


