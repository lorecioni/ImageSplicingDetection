'''
Created on 03 ott 2016

@author: lorenzocioni
'''
import cv2
import numpy as np
from classification import FaceTrainingSample, splitDataset, KNNClassifier
import descriptors
import illuminantMaps
import config
import utils
import os
import scipy.io


'''
Splicing detection main procedure. The result of the output
will be a number of regions of the image that can be spliced
over an image
'''
class FaceSplicingDetector:

    def __init__(self, extractMaps, extractFeatures, crossVal, verbose, displayResult):
        self.verbose = verbose
        self.extract_maps = extractMaps
        self.extract_features = extractFeatures
        self.face_cascade = cv2.CascadeClassifier(config.cascadePath)
        self.cross_validation = crossVal
        self.display_result = displayResult

        # Creating temporary folder
        utils.createTempFolder()

        #Descriptors
        self.descriptors = config.descriptors

    '''
    Detect splicing in the given image
    @param img: the image filenames
    '''
    def detect(self, img, groudtruth = None):
        filename = utils.getFilename(img)

        config.maps_folder = config.temp_folder
        config.faces_folder = config.temp_folder
        config.descriptors_folder = config.temp_folder

        print('Processing ' + filename)

        image = cv2.imread(img)
        if image is None:
            print('Error processing ' + filename + ': image not found')
            return

        #Loads classifier
        # Extracting image features
        # Extract the faces in the image
        faces, labels = self.extractFaces(img, groudtruth)

        #Prediction map
        predictions = {}
        counters = {}
        for i in range(len(faces)):
            predictions[i], counters[i] = 0, 0

        #Image is precessable if there are more than one image
        if len(faces) > 1:
            # If there are two or more faces, process the image
            # Extract maps
            self.extractIlluminationMaps(img)
            # Extract image descriptors and features
            for illum in config.illuminantTypes:
                for desc in self.descriptors:
                    clfPath = config.classification_folder + 'model_' + illum + '_' + desc.lower() + '.pkl'
                    if os.path.isfile(clfPath):
                        clf = KNNClassifier.load(clfPath)
                        features = self.extractFeatures(img, label=groudtruth, faces=faces, illum=illum, descriptor=desc, output=True)
                        #Predict over sample
                        for sample in features:
                            prediction = clf.predict(np.array(sample.feature.split(), dtype=float).reshape((1, -1)), True)[0][1]
                            predictions[sample.first] += prediction
                            predictions[sample.second] += prediction
                            counters[sample.first] += 1
                            counters[sample.second] += 1

            #Majority voting
            threshold = config.majorityVotingThreshold
            score = 0
            detected = False
            fakeFaces = []

            #Evaluation (if GT provided)
            TP, TN, FN, FP = 0, 0, 0, 0

            for i in predictions:
                if score < predictions[i] / counters[i]:
                    score = predictions[i] / counters[i]

                if predictions[i]/counters[i] > threshold:
                    fakeFaces.append(i)
                    print('\tFace ' + str(i + 1) + ' is FAKE. Score ' + str(score) )

                    if groudtruth is not None:
                        if labels[i] == 1:
                            TP += 1
                        else:
                            FP += 1

                    if not detected:
                        detected = not detected
                else:
                    print('\tFace ' + str(i + 1) + ' is NORMAL. Score ' + str(predictions[i]/counters[i]))
                    if groudtruth is not None:
                        if labels[i] == 0:
                            TN += 1
                        else:
                            FN += 1

            if detected:
                print('Image is FAKE - Score: ' + str(score))
            else:
                print('Image is ORIGINAL - Score: ' + str(score))


            orig = cv2.imread(img, cv2.COLOR_BGR2GRAY)
            #Display spliced faces
            rows, cols, _ = orig.shape
            outputMask = np.zeros((rows, cols))

            faceScores = orig.copy()
            idx = 0
            font = cv2.FONT_HERSHEY_SIMPLEX
            for (x, y, w, h) in faces:
                face_score = predictions[idx]/counters[i]
                if idx not in fakeFaces:
                    cv2.rectangle(faceScores, (x, y), (x + w, y + h), (0, 255, 0), 8)
                    cv2.putText(faceScores, str("{:.3f}".format(face_score)), (x, y + h + 80), font, 1.8, (0, 255, 0), 3)
                else:
                    cv2.rectangle(faceScores, (x, y), (x + w, y + h), (0, 0, 255), 8)
                    cv2.putText(faceScores, str("{:.3f}".format(face_score)), (x, y + h + 80), font, 1.8, (0, 0, 255), 3)

                #Set score in detection map
                outputMask[y:y + h, x:x + w] = face_score
                idx += 1
            faceScores = utils.resizeImage(faceScores, 1000)

            if self.display_result:
                cv2.imshow('output', faceScores)
                cv2.waitKey()

            regionMask = np.zeros(orig.shape)
            regionMask[..., 0] = outputMask.copy()
            regionMask[..., 1] = outputMask.copy()
            regionMask[..., 2] = outputMask.copy()
            # Write output mask
            outputMask *= 255
            cv2.imwrite(config.faceOutputDetectionImage, outputMask)

            return TP, TN, FP, FN

        else:
            # discard the current image
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
                faces, _ = self.extractFaces(images[i], labels[i])
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
                    trainingData, trainingLabels, _ = self.getTrainingData(images, desc, illum = illum)
                    if len(trainingData) > 0:
                        # Creates an instance of Neighbours Classifier and fit the data.
                        clf = KNNClassifier(config.KNeighbours, 'uniform')
                        clf.train(trainingData, trainingLabels)
                        clf.store(config.classification_folder + 'model_' + illum + '_' + desc.lower() + '.pkl')
                        print(illum + "/" + desc.upper() +  ' classification model created correctly')

        else:
            #Crossvalidate dataset witk K-fold crossvalidation
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


            outputs_labels = trainingDesc[refKey][1]
            outputs_scores = np.zeros(len(trainingDesc[refKey][0]))

            for (trainIndex, testIndex) in splits:
                classifiers = {}
                for illum in config.illuminantTypes:
                    for desc in self.descriptors:
                        key = illum + "_" + desc
                        trainingData, trainingLabels, _ = trainingDesc[key]
                        if len(trainingData) > 0 and len(trainingLabels) > 0:
                            trainingData = trainingData[trainIndex]
                            trainingLabels = trainingLabels[trainIndex]
                            #Training model for illum type and descriptor
                            clf = KNNClassifier(config.KNeighbours, 'uniform')
                            clf.train(trainingData, trainingLabels)
                            classifiers[key] = clf

                outputs = []
                outputs_probs = []

                for illum in config.illuminantTypes:
                    for desc in self.descriptors:
                        key = illum + "_" + desc
                        testData, _, _ = trainingDesc[key]
                        if len(testData) > 0 :
                            testData = testData[testIndex]
                            prediction = classifiers[key].predict(testData) * config.descriptors_weights[desc]
                            outputs.append(prediction)
                            prediction = classifiers[key].predict(testData, True) * config.descriptors_weights[desc]
                            outputs_probs.append(prediction)

                output = np.zeros(len(testIndex))
                output_prob = np.zeros(len(testIndex))

                for i in range(len(outputs)):
                    predictions = outputs[i]
                    output += predictions
                    predictions_prob = outputs_probs[i]
                    for j in range(len(predictions)):
                        output_prob[j] += predictions_prob[j][1]


                #If voting is majority, classify as fake
                outputs_scores[testIndex] = output_prob
                counter = 0
                _, trainingLabels, _ = trainingDesc[refKey]
                testLabels = trainingLabels[testIndex]
                totalModels = len(classifiers)

                for val in np.nditer(output):
                    if val > totalModels/2:
                       if testLabels[counter] != 1:
                           misclassified += 1
                    else:
                        if testLabels[counter] != 0:
                            misclassified += 1
                    counter += 1

            scipy.io.savemat('classification_output.mat', dict(labels=outputs_labels, scores=outputs_scores))

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
        trainingSource = []
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
                    trainingSource.append(utils.getFilename(path))

        return np.asarray(trainingData), np.asarray(trainingLabels), np.asarray(trainingSource)

    '''
    Extract image illuminant maps 
    GGE map
    IIC map
    '''
    def extractIlluminationMaps(self, img):
        filename = utils.getFilename(img)
        illuminantMaps.prepareImageIlluminants(img, config.seg_sigma, config.seg_k, config.seg_min_size, config.min_intensity, config.max_intensity, self.verbose)
        if 'GGE' in config.illuminantTypes:
            illuminantMaps.extractGGEMap(img, config.maps_folder + filename + "_segmented.png", config.gge_sigma, config.gge_n, config.gge_p, self.verbose)
        if 'IIC' in config.illuminantTypes:
            illuminantMaps.extractIICMap(img, config.maps_folder + filename + "_segmented.png", self.verbose)


    '''
    Segment current image extracting human faces
    '''
    def extractFaces(self, img, label = None, display = False):
        orig = cv2.imread(img, cv2.COLOR_BGR2GRAY)
        labels = []
        if self.verbose:
            print('Detecting image faces')
        if label is not None:
            faces = []
            for entry in label:
                face = (int(entry[2]), int(entry[4]), int(entry[3]) - int(entry[2]), int(entry[5]) - int(entry[4]))
                if entry[1] != config.positiveLabel:
                    labels.append(1)
                else:
                    labels.append(0)
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
                cv2.rectangle(orig, (x, y), (x + w, y + h), (0, 255, 0), 8)
            orig = utils.resizeImage(orig, 500)
            cv2.imshow('img', orig)
            cv2.waitKey(0)

        return faces, labels


    '''
    Builds feature vector given  image, descriptor and illuminant map type
    '''
    def extractFeatures(self, img, label = None, faces = [], descriptor = "ACC", space = 0, illum = 'GGE', output = False):
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
    Evaluate current trained models over a set of images
    '''
    def evaluate(self, images, labels):
        print('Evaluate dataset on pre-trained model')

        pairData, pairLabels = None, None
        outputs = []
        outputs_probs = []
        for illum in config.illuminantTypes:
            for desc in self.descriptors:
                clfPath = config.classification_folder + 'model_' + illum + '_' + desc.lower() + '.pkl'
                if os.path.isfile(clfPath):
                    clf = KNNClassifier.load(clfPath)
                    testData, testLabels, _ = self.getTrainingData(images, desc, illum=illum)
                    if len(testData) > 0:
                        pairData, pairLabels = testData, testLabels
                        prediction = clf.predict(testData, False) * config.descriptors_weights[desc]
                        outputs.append(prediction)
                        prediction = clf.predict(testData, True) * config.descriptors_weights[desc]
                        outputs_probs.append(prediction)

        if pairData is not None:
            output = np.zeros(len(pairData))
            output_prob = np.zeros(len(pairData))

            for i in range(len(outputs)):
                predictions = outputs[i]
                output += predictions
                predictions_prob = outputs_probs[i]
                for j in range(len(predictions)):
                    output_prob[j] += predictions_prob[j][1]


            # If voting is majority, classify as fake
            counter = 0
            misclassified = 0

            scipy.io.savemat('classification_output.mat', dict(labels=pairLabels, scores=output_prob))

            totalModels = len(outputs)
            for val in np.nditer(output):
                if val > totalModels / 2:
                    if pairLabels[counter] != 1:
                        misclassified += 1
                else:
                    if pairLabels[counter] != 0:
                        misclassified += 1
                counter += 1

            print('Number of classifiers: ' + str(totalModels))
            totalSamples = len(pairData)
            print('Misclassified: ' + str(misclassified) + '/' + str(totalSamples))
            accuracy = (totalSamples - misclassified) / totalSamples
            print('Accuracy: ' + str(accuracy))


    '''Clear temp folder'''
    def __del__(self):
        print('Cleaning up')
        utils.removeTempFolder()