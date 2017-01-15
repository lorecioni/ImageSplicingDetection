'''
Created on 03 ott 2016

@author: lorenzocioni
'''
import cv2
import numpy as np

import os

from sklearn.externals import joblib
from sklearn.model_selection import KFold
from sklearn import neighbors
import descriptors
import illuminantMaps
import config
import utils


'''
Splicing detection main procedure. The result of the output
will be a number of regions of the image that can be spliced
over an image
'''
class FaceSplicingDetection:

    def __init__(self, extractMaps, extractFeatures, crossVal, verbose, heatMap):
        self.verbose = verbose
        self.extract_maps = extractMaps
        self.extract_features = extractFeatures
        self.face_cascade = cv2.CascadeClassifier(config.cascadePath)
        self.cross_validation = crossVal
        self.heat_map = heatMap
        
        #Descriptors
        self.descriptors = config.descriptors


    def detectSplice(self, img, heat_map, depth):
        filename = utils.getFilename(img)
        # Extracting image features
        #features = self.processImage(img, False, self.verbose, heat_map)

        # 2. Statistical difference between IIC and GGE maps
        gge_map = cv2.imread(config.maps_folder + filename + '_gge_map.png')
        iic_map = cv2.imread(config.maps_folder + filename + '_iic_map.png')

        return

    '''
    Train model for further splicing detection
    @param images: the list of images filenames
    @param labels: the list of image labels
    '''
    def train(self, images, labels):        
        # Extract image features from each images in training set
        if self.extract_features or self.extract_maps:
            for i in range(len(images)):
                filename = utils.getFilename(images[i])
                print('Processing ' + filename) 
                self.extractIlluminationMaps(images[i])
                # Extract image descriptors and features
                for desc in self.descriptors:
                    self.extractFeatures(images[i], labels[i], descriptor = desc)

        # Sample training
        if not config.crossvalidation:
            # Train one model for each descriptor
            for desc in self.descriptors:
                trainingData, trainingLabels = self.getTrainingData(images, desc)

                # Creates an instance of Neighbours Classifier and fit the data.
                clf = neighbors.KNeighborsClassifier(config.KNeighbours, weights = 'uniform')
                clf.fit(trainingData, trainingLabels)

                print(desc.upper() +  ' classification model created correctly')
                joblib.dump(clf, config.classification_folder + 'model_' + desc.lower() + '.pkl')

        else:
            #Crossvalidate dataset witk 10-fold crossvalidation
            print('Evaluate dataset with crossvalidation')
            kf = KFold(n_splits = config.folds, shuffle = True)
            trainingDesc = {}

            for desc in self.descriptors:
                trainingDesc[desc] = self.getTrainingData(images, desc)

            misclassified = 0
            for (trainIndex, testIndex) in kf.split(trainingDesc[self.descriptors[0]][0]):

                classifiers = {}
                for desc in self.descriptors:
                    trainingData, trainingLabels = trainingDesc[desc]
                    trainingData = trainingData[trainIndex]
                    trainingLabels = trainingLabels[trainIndex]

                    clf = neighbors.KNeighborsClassifier(config.KNeighbours, weights='uniform')
                    clf.fit(trainingData, trainingLabels)
                    classifiers[desc] = clf

                outputs = []
                testLabels = None

                for desc in self.descriptors:
                    testData, testLabels = trainingDesc[desc]
                    testData = testData[testIndex]
                    testLabels = testLabels[testIndex]
                    outputs.append(classifiers[desc].predict(testData))

                output = np.zeros(len(testIndex))
                for predictions in outputs:
                    output += predictions

                #If voting is majority, classify as fake

                counter = 0

                for val in np.nditer(output):
                    if val > len(self.descriptors)/2:
                       if testLabels[counter] != 1:
                           misclassified += 1
                    else:
                        if testLabels[counter] != 0:
                            misclassified += 1
                    counter += 1

            totalSamples = len(trainingDesc[self.descriptors[0]][0])
            print('Misclassified: ' + str(misclassified) + '/' + str(totalSamples))
            accuracy = (totalSamples - misclassified)/totalSamples
            print('Accuracy: ' + str(accuracy))

    def getTrainingData(self, images, descriptor):
        trainingData = []
        trainingLabels = []
        for i in range(len(images)):
            filename = utils.getFilename(images[i])
            path = config.features_folder + filename + '_' + descriptor.lower() + ".txt"
            if os.path.isfile(path):
                imageFeatures = open(path, "r")
                features = [pair.split(":") for pair in imageFeatures.read().splitlines()]
                for pair in features:
                    trainingLabels.append(int(pair[0]))
                    #pairData = [float(val) for val in list()]
                    trainingData.append(np.array(pair[1].split(), dtype=float))
        return np.asarray(trainingData), np.asarray(trainingLabels)

    '''
    Extract feature vector for a selected image
    '''
    def extractSingleFeatures(self, img):
        self.processImage(img, True)

    '''
    Extract image illuminant maps 
    GGE map
    IIC map
    '''
    def extractIlluminationMaps(self, img):
        filename = utils.getFilename(img)
        illuminantMaps.prepareImageIlluminants(img, config.seg_sigma, config.seg_k, config.seg_min_size, config.min_intensity, config.max_intensity, self.verbose)
        if config.illuminantType == 'GGE':
            illuminantMaps.extractGGEMap(img, filename + "_segmented" + config.maps_out_suffix +".png", config.gge_sigma, config.gge_n, config.gge_p, self.verbose)
        elif config.illuminantType == 'IIC':
            illuminantMaps.extractIICMap(img, filename + "_segmented" + config.maps_out_suffix + ".png", self.verbose)


    '''
    Segment current image extracting human faces
    '''
    def extractFaces(self, img, label):
        orig = cv2.imread(img, cv2.COLOR_BGR2GRAY)
        if self.verbose:
            print('Detecting image faces')
        if label is not None:
            faces = []
            for entry in label:
                face = (int(entry[2]), int(entry[4]), int(entry[3]) - int(entry[2]), int(entry[5]) - int(entry[4]))
                faces.append(face)
        else:
            faces = self.face_cascade.detectMultiScale(
                orig,
                scaleFactor = 1.1,
                minNeighbors = 5,
                minSize = (30, 30),
                flags = cv2.CASCADE_SCALE_IMAGE
            )
        if self.verbose:
            print(str(len(faces)) + ' faces detected')
        return faces


    '''
    Builds feature vector given two set of principal
    components
    '''
    def extractFeatures(self, img, label = None, descriptor = "ACC", space = 0, channel = 3):
        filename = utils.getFilename(img)
        illuminantMap = cv2.imread(config.maps_folder + filename + '_' + config.illuminantType.lower() + '_map.png')
        faces = self.extractFaces(img, label)

        #Storing faces extracted from maps
        count = 0
        for (x, y, w, h) in faces:
            face = illuminantMap[y:y + h, x:x + w]
            path = config.faces_folder + "face-" + config.illuminantType.upper() + "-" + str(count) + ".png"
            cv2.imwrite(path, face)
            descriptors.extractDescriptor(path, descriptor, space)
            count += 1

        #Image features
        features = []
        first = 0
        while first < len(faces) - 1:
            second = first + 1
            firstFaceFeat = config.faces_folder + 'face-' + config.illuminantType + '-' + str(first) + "-" + descriptor.lower() + "-desc.txt"
            while second < len(faces):
                secondFaceFeat = config.faces_folder + 'face-' + config.illuminantType + '-'  + str(second) + "-" + descriptor.lower() + "-desc.txt"
                #Concat the two feature vector
                facePairFeature = descriptors.buildFaceFeatureVector(firstFaceFeat, secondFaceFeat, descriptor)

                pairLabel = 0
                if label[first][1] != config.positiveLabel or label[second][1] != config.positiveLabel:
                    pairLabel = 1
                features.append((facePairFeature, pairLabel))
                second += 1
            first += 1

        nameFile = config.features_folder + filename + '_' + descriptor.lower() + ".txt"
        files = open(nameFile, "w")
        for (pairFeature, pairLabel) in features:
            files.write(str(pairLabel) + ":" + pairFeature + "\n")
        files.close()
        
        print('\tFeatures extracted with ' + descriptor + ' descriptor')

    '''
    Builds and visualize the heat map in order to visually evaluate difference
    between two different maps. Using OpenCV COLORMAP_JET, red values indicates
    a more significant difference, blue values indicates lower difference.
    '''
    def visualizeHeatMap(self, gge, iic):
        #Splits all the channels
        gge_b, gge_g, gge_r = cv2.split(gge)
        iic_b, iic_g, iic_r = cv2.split(iic)
        #Get maps dimensions
        rows, cols, _ = gge.shape
        #Building heat map
        heat_map = np.sqrt(pow(gge_b[0:rows-1, 0:cols-1] - iic_b[0:rows-1, 0:cols-1], 2) + pow(gge_g[0:rows-1, 0:cols-1] - iic_g[0:rows-1, 0:cols-1], 2) +  pow(gge_r[0:rows-1, 0:cols-1] - iic_r[0:rows-1, 0:cols-1], 2))
        #Recover heat map max value
        max_value = np.ndarray.max(heat_map)
        #Normalization
        heat_map = heat_map / max_value
        heat_map = heat_map * 255
        heat_map = heat_map.astype(np.uint8)
        #Display color map
        color_map = cv2.applyColorMap(heat_map, cv2.COLORMAP_JET)
        cv2.imshow('img', self.resizeImage(color_map, 500))
        cv2.waitKey(0)
            

