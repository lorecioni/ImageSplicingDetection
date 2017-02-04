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
class FaceSplicingDetector:

    def __init__(self, extractMaps, extractFeatures, crossVal, verbose, heatMap):
        self.verbose = verbose
        self.extract_maps = extractMaps
        self.extract_features = extractFeatures
        self.face_cascade = cv2.CascadeClassifier(config.cascadePath)
        self.cross_validation = crossVal
        self.heat_map = heatMap
        
        #Descriptors
        self.descriptors = config.descriptors

    '''
    Detect splicing in the given image
    @param img: the image filenames
    '''
    def detect(self, img, output):
        filename = utils.getFilename(img)
        print('Processing ' + filename)

        #Loads classifier

        # Extracting image features
        # Extract the faces in the image
        faces = self.extractFaces(img, display=True)

        #Prediction map
        predictions = {}
        for i in range(len(faces)):
            predictions[i] = 0
        #Image is precessable if there are more than one image
        counter = 0
        if len(faces) > 1:
            # If there are two or more faces, process the image
            # Extract maps
            self.extractIlluminationMaps(img)
            # Extract image descriptors and features
            for illum in config.illuminantTypes:
                for desc in self.descriptors:
                    clf = KNNClassifier.load(config.classification_folder + 'model_' + illum + '_' + desc.lower() + '.pkl')
                    features = self.extractFeatures(img, faces=faces, illum=illum, descriptor=desc, output=True)
                    #Predict over sample
                    for sample in features:
                        prediction = clf.predict(np.array(sample.feature.split(), dtype=float).reshape((1, -1)))
                        if prediction == 1:
                            predictions[sample.first] += 1
                            predictions[sample.second] += 1
                        counter += 1

            #Majority voting
            threshold = 0.5 #len(config.illuminantTypes) * len(config.descriptors) * len(faces)
            detected = False
            for i in predictions:
                if predictions[i]/counter > threshold:
                    print('\tFace ' + str(i + 1) + ' is FAKE. Score ' + str(predictions[i]/counter) )
                    if not detected:
                        detected = not detected
                else:
                    print('\tFace ' + str(i + 1) + ' is NORMAL. Score ' + str(predictions[i]/counter))

            if detected:
                print('Image is FAKE')
            else:
                print('Image is NORMAL')

        else:
            # else discard the current image
            print('Not suitable number of faces found in the image')

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
                #Extract the faces in the image
                faces = self.extractFaces(images[i], labels[i])
                if len(faces) > 1:
                    #If there are two or more faces, process the image
                    filename = utils.getFilename(images[i])
                    print('Processing ' + filename)
                    #Extract maps
                    if self.extract_maps:
                        self.extractIlluminationMaps(images[i])
                    #Extract face paired features
                    if self.extract_features:
                        # Extract image descriptors and features
                        for illum in config.illuminantTypes:
                            for desc in self.descriptors:
                                self.extractFeatures(images[i], labels[i], faces = faces, illum = illum, descriptor = desc)
                else:
                    #else discard the current image
                    print('Not suitable number of faces found in the image')


        # Sample training
        if not self.cross_validation:
            # Train one model for each descriptor
            for illum in config.illuminantTypes:
                for desc in self.descriptors:
                    trainingData, trainingLabels = self.getTrainingData(images, desc, illum = illum)

                    # Creates an instance of Neighbours Classifier and fit the data.
                    clf = KNNClassifier(config.KNeighbours, 'uniform')
                    clf.train(trainingData, trainingLabels)
                    clf.store(config.classification_folder + 'model_' + illum + '_' + desc.lower() + '.pkl')

                    print(illum + "/" + desc.upper() +  ' classification model created correctly')

        else:
            #Crossvalidate dataset witk 10-fold crossvalidation
            print('Evaluate dataset with crossvalidation')
            trainingDesc = {}

            for illum in config.illuminantTypes:
                for desc in self.descriptors:
                    key = illum + "_" + desc
                    trainingDesc[key] = self.getTrainingData(images, desc, illum = illum)

            #Counting misclassified samples for accuracy score
            misclassified = 0
            refKey = config.illuminantTypes[0] + "_" + self.descriptors[0]
            #Splits dataset in train and test for crossvalidation
            splits = splitDataset(trainingDesc[refKey][0], config.folds)
            totalModels = len(self.descriptors) * len(config.illuminantTypes)

            for (trainIndex, testIndex) in splits:
                classifiers = {}
                for illum in config.illuminantTypes:
                    for desc in self.descriptors:
                        key = illum + "_" + desc
                        trainingData, trainingLabels = trainingDesc[key]
                        trainingData = trainingData[trainIndex]
                        trainingLabels = trainingLabels[trainIndex]
                        #Training model for illum type and descriptor
                        clf = KNNClassifier(config.KNeighbours, 'uniform')
                        clf.train(trainingData, trainingLabels)
                        classifiers[key] = clf

                outputs = []
                testLabels = None

                for desc in self.descriptors:
                    for illum in config.illuminantTypes:
                        key = illum + "_" + desc
                        testData, testLabels = trainingDesc[key]
                        testData = testData[testIndex]
                        testLabels = testLabels[testIndex]
                        outputs.append(classifiers[key].predict(testData))

                output = np.zeros(len(testIndex))
                for predictions in outputs:
                    output += predictions

                #If voting is majority, classify as fake

                counter = 0

                for val in np.nditer(output):
                    if val >  totalModels/2:
                       if testLabels[counter] != 1:
                           misclassified += 1
                    else:
                        if testLabels[counter] != 0:
                            misclassified += 1
                    counter += 1

            print('Number of classifiers: ' + str(totalModels))
            totalSamples = len(trainingDesc[refKey][0])
            print('Misclassified: ' + str(misclassified) + '/' + str(totalSamples))
            accuracy = (totalSamples - misclassified)/totalSamples
            print('Accuracy: ' + str(accuracy))

    '''
    Reads and return training samples
    '''
    def getTrainingData(self, images, descriptor, illum = 'GGE'):
        trainingData = []
        trainingLabels = []
        for i in range(len(images)):
            filename = utils.getFilename(images[i])
            path = config.features_folder + filename + '_' + illum + "_" + descriptor.lower() + ".txt"
            if os.path.isfile(path):
                imageFeatures = open(path, "r")
                features = [FaceTrainingSample.fromTxt(pair) for pair in imageFeatures.read().splitlines()]
                for sample in features:
                    trainingLabels.append(sample.label)
                    #pairData = [float(val) for val in list()]
                    trainingData.append(np.array(sample.feature.split(), dtype=float))
        return np.asarray(trainingData), np.asarray(trainingLabels)

    '''
    Extract image illuminant maps 
    GGE map
    IIC map
    '''
    def extractIlluminationMaps(self, img):
        filename = utils.getFilename(img)
        illuminantMaps.prepareImageIlluminants(img, config.seg_sigma, config.seg_k, config.seg_min_size, config.min_intensity, config.max_intensity, self.verbose)
        if 'GGE' in config.illuminantTypes:
            illuminantMaps.extractGGEMap(img, filename + "_segmented.png", config.gge_sigma, config.gge_n, config.gge_p, self.verbose)
        if 'IIC' in config.illuminantTypes:
            illuminantMaps.extractIICMap(img, filename + "_segmented.png", self.verbose)


    '''
    Segment current image extracting human faces
    '''
    def extractFaces(self, img, label = None, display = False):
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
                minSize = config.faceMinSize,
                flags = cv2.CASCADE_SCALE_IMAGE
            )
        #if self.verbose:
        print(str(len(faces)) + ' faces detected')

        if display:
            for (x, y, w, h) in faces:
                cv2.rectangle(orig, (x, y), (x + w, y + h), (255, 0, 0), 2)
            orig = utils.resizeImage(orig, 500)
            cv2.imshow('img', orig)
            cv2.waitKey(0)

        return faces


    '''
    Builds feature vector given  image, descriptor and illuminant map type
    '''
    def extractFeatures(self, img, label = None, faces = [], descriptor = "ACC", space = 0, illum = 'GGE', channel = 3, output = False):
        filename = utils.getFilename(img)

        if illum == 'GGE':
            map_path = config.maps_folder + filename + '_' + illum.lower() + '_map_' + str(config.gge_n) + "_" + str(config.gge_p) + ".png"
        else:
            map_path = config.maps_folder + filename + '_' + illum.lower() + '_map.png'

        illuminantMap = cv2.imread(map_path)

        #Storing faces extracted from maps
        count = 0
        for (x, y, w, h) in faces:
            face = illuminantMap[y:y + h, x:x + w]
            path = config.faces_folder + "face-" + illum.upper() + "-" + str(count) + ".png"
            cv2.imwrite(path, face)
            descriptors.extractDescriptor(path, descriptor, space)
            count += 1

        #Image features
        features = []
        first = 0
        while first < len(faces) - 1:
            second = first + 1
            firstFaceFeat = config.faces_folder + 'face-' + illum + '-' + str(first) + "-" + descriptor.lower() + "-desc.txt"
            while second < len(faces):
                secondFaceFeat = config.faces_folder + 'face-' + illum + '-'  + str(second) + "-" + descriptor.lower() + "-desc.txt"
                #Concat the two feature vector
                facePairFeature = descriptors.buildFaceFeatureVector(firstFaceFeat, secondFaceFeat, descriptor)

                if config.inverseFacePosition:
                    inverseFacePairFeature = descriptors.buildFaceFeatureVector(secondFaceFeat, firstFaceFeat, descriptor)

                pairLabel = None
                if label is not None:
                    pairLabel = 0
                    if label[first][1] != config.positiveLabel or label[second][1] != config.positiveLabel:
                        pairLabel = 1

                sample = FaceTrainingSample(facePairFeature, pairLabel, first, second, filename)
                features.append(sample)

                if config.inverseFacePosition:
                    sample = FaceTrainingSample(inverseFacePairFeature, pairLabel, second, first)
                    features.append(sample)

                second += 1
            first += 1

        if self.verbose:
            print('\tFeatures extracted with ' + descriptor + ' descriptor in ' + illum + ' map')

        if not output:
            #Storing image feaures in file
            nameFile = config.features_folder + filename + '_' + illum + "_" + descriptor.lower() + ".txt"
            files = open(nameFile, "w")
            for sample in features:
                files.write(str(sample))
            files.close()
        else:
            return features

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
            

