'''
Created on 03 ott 2016

@author: lorenzocioni
'''
import cv2
import numpy as np
import illuminantMaps
import config
import utils
import scipy.io

from sklearn.svm import SVC
from sklearn.externals import joblib

'''
Splicing detection main procedure. The result of the output
will be a number of regions of the image that can be spliced
over an image
'''

class RegionSplicingDetector:

    def __init__(self, extractMaps = True, extractFeatures = True, crossVal = False, verbose = False, displayResult = False):
        self.verbose = verbose
        self.extract_maps = extractMaps
        self.extract_features = extractFeatures
        self.cross_validation = crossVal
        self.display_result = displayResult

        #Bands
        self.bands = {}
        self.horizontalBands = 0
        self.verticalBands = 0

        #Reference color
        self.horizontalReferences = {}
        self.verticalReferences = {}

        #Illuminant algorithms
        self.algorithms = [
            'grayworld',
            'grayedge',
            'maxrgb',
            'shadesofgray',
            'secondgrayedge'
        ]

        self.filename = ''

        utils.createTempFolder()

        self.clf = {
            'vertical':  joblib.load(config.data_folder + 'regions_module/models/trained_model_vertical_dso.pkl'),
            'vertical_global': joblib.load(config.data_folder + 'regions_module/models/trained_model_vertical_dso_global.pkl'),
            'horizontal': joblib.load(config.data_folder + 'regions_module/models/trained_model_horizontal_dso.pkl'),
            'horizontal_global': joblib.load(config.data_folder + 'regions_module/models/trained_model_horizontal_dso_global.pkl')
        }


    def evaluate(self, images, output):
        for i in range(len(images)):
            img = images[len(images) - 20 - i]
            self.detect(img, output, True)
            if i == 15:
                break

    def train(self, images, labels, direction = 'vertical'):
        featureFile = open('trained_features.txt', 'w');
        featureFile.close()
        for i in range(len(images)):
            featureFile = open('trained_features' + direction +'.txt', 'a');
            img = images[i]
            label = labels[i]
            self.filename = utils.getFilename(img)
            print('Processing ' + self.filename)
            # Reads image
            image = cv2.imread(img)
            rows, cols, _ = image.shape
            maskImage = np.zeros((rows, cols))

            try:
                if direction == 'vertical':
                    maskBand = np.ones((rows, label[2])) * 255
                    maskImage[:, label[0]:label[1]] = maskBand
                elif direction == 'horizontal':
                    maskBand = np.ones((label[2], cols)) * 255
                    maskImage[label[0]:label[1], :] = maskBand

            except:
                print("Error evaluating band " + img)
                continue

            global_references = self.extractGlobalReferences(img)

            self.evaluateIlluminantMaps(image, maskImage, direction)

            # Evaluate distances
            medians = {}
            for i in range(self.verticalBands):
                filename = config.temp_folder + 'vertical_band_' + str(i) + "_gge_map_"
                for alg in self.algorithms:
                    medians[alg] = utils.evaluateRGBMedian(filename + alg + ".png")

                band_feature = []  #Feature vector evaluated on medians
                band_feature_global = [] #Feature vector evaluated on the global references
                band = self.bands['vertical'][i]

                for alg in self.algorithms:
                    distance = utils.euclideanDistanceRGB(medians[alg], self.verticalReferences[alg])
                    band_feature.append(distance)
                    distance = utils.euclideanDistanceRGB(medians[alg], global_references[alg])
                    band_feature_global.append(distance)


                print('\tEvaluated ' + str(i + 1) + '/' + str(self.verticalBands) + ' vertical bands')

                featureFile.write(str(band.label) + ": ")
                for feat in band_feature:
                    featureFile.write(str(feat) + " ")
                featureFile.write(":")
                for feat in band_feature_global:
                    featureFile.write(str(feat) + " ")
                featureFile.write(":" + str(band.colorAvg))
                featureFile.write("\n")


            for i in range(self.horizontalBands):
                filename = config.temp_folder + 'horizontal_band_' + str(i) + "_gge_map_"
                for alg in self.algorithms:
                    medians[alg] = utils.evaluateRGBMedian(filename + alg + ".png")

                band = self.bands['horizontal'][i]
                band_feature = [] #Feature vector evaluated medians
                band_feaure_global = [] #Feature vector evaluated on the global references
                for alg in self.algorithms:
                    distance = utils.euclideanDistanceRGB(medians[alg], self.horizontalReferences[alg])
                    band_feature.append(distance)
                    distance = utils.euclideanDistanceRGB(medians[alg], global_references[alg])
                    band_feaure_global.append(distance)

                print('\tEvaluated ' + str(i + 1) + '/' + str(self.horizontalBands) + ' horizontal bands')

                featureFile.write(str(band.label) + ": ")
                for feat in band_feature:
                    featureFile.write(str(feat) + " ")
                featureFile.write(":")
                for feat in band_feaure_global:
                    featureFile.write(str(feat) + " ")
                featureFile.write(":" + str(band.colorAvg))
                featureFile.write("\n")

            featureFile.close()

    '''
    Detection algorithm
    '''
    def detect(self, img, output, groundtruth = True):
        self.filename = utils.getFilename(img)
        print('Processing ' + self.filename)

        #Reads image
        image = cv2.imread(img)
        image = utils.resizeImage(image, 1200)

        #Mask
        if groundtruth:
            maskImage = cv2.imread(config.masks_folder + self.filename + '.png', cv2.IMREAD_GRAYSCALE)
            if maskImage is not None:
                maskImage = np.invert(maskImage)
        else:
            maskImage = None

        if image is not None:

            self.evaluateIlluminantMaps(image, maskImage)
            rows, cols, _ = image.shape
            detectionMap = np.zeros((rows, cols))
            detectionMap_global = np.zeros((rows, cols))
            detectionMap_svm = np.zeros((rows, cols))
            detectionMap_svm_global = np.zeros((rows, cols))

            global_references = self.extractGlobalReferences(img)

            detected = 0

            #Evaluate distances
            medians = {}
            for i in range(self.verticalBands):
                filename = config.temp_folder + 'vertical_band_' + str(i) + "_gge_map_"
                for alg in self.algorithms:
                    medians[alg] = utils.evaluateRGBMedian(filename + alg + ".png")

                band = self.bands['vertical'][i]
                band_feature = []
                band_feaure_global = []  # Feature vector evaluated on the global references

                for alg in self.algorithms:
                    distance = utils.euclideanDistanceRGB(medians[alg], self.verticalReferences[alg])
                    band_feature.append(distance)
                    detectionMap = band.incrementDetection(detectionMap, distance)

                    distance = utils.euclideanDistanceRGB(medians[alg], global_references[alg])
                    band_feaure_global.append(distance)
                    detectionMap_global = band.incrementDetection(detectionMap_global, distance)

                prediction = self.clf['vertical'].predict_proba(np.asarray(band_feature).reshape((1, -1)))
                detectionMap_svm = band.incrementDetection(detectionMap_svm, prediction[0][1])
                prediction = self.clf['vertical_global'].predict_proba(np.asarray(band_feaure_global).reshape((1, -1)))
                detectionMap_svm_global = band.incrementDetection(detectionMap_svm_global, prediction[0][1])

                print('\tEvaluated ' + str(i + 1) + '/' + str(self.verticalBands) + ' vertical bands')


            for i in range(self.horizontalBands):
                filename = config.temp_folder + 'horizontal_band_' + str(i) + "_gge_map_"
                for alg in self.algorithms:
                    medians[alg] = utils.evaluateRGBMedian(filename + alg + ".png")

                band = self.bands['horizontal'][i]
                band_feature = []
                band_feaure_global = []  # Feature vector evaluated on the global references

                for alg in self.algorithms:
                    distance = utils.euclideanDistanceRGB(medians[alg], self.horizontalReferences[alg])
                    band_feature.append(distance)
                    detectionMap = band.incrementDetection(detectionMap, distance)

                    distance = utils.euclideanDistanceRGB(medians[alg], global_references[alg])
                    band_feaure_global.append(distance)
                    detectionMap_global = band.incrementDetection(detectionMap_global, distance)

                prediction = self.clf['horizontal'].predict_proba(np.asarray(band_feature).reshape((1, -1)))
                detectionMap_svm = band.incrementDetection(detectionMap_svm, prediction[0][1])
                prediction = self.clf['horizontal_global'].predict_proba(np.asarray(band_feaure_global).reshape((1, -1)))
                detectionMap_svm_global = band.incrementDetection(detectionMap_svm_global, prediction[0][1])

                print('\tEvaluated ' + str(i + 1) + '/' + str(self.horizontalBands) + ' horizontal bands')


            print('Detected: ' + str(detected))


            # Recover detection map max value
            max_value = np.ndarray.max(detectionMap)
            score = max_value/8
            if score < 0.8:
                print('Image is NORMAL - Score: ' + str(score))
            else:
                print('Image is FAKE - Score: ' + str(score))

            # Normalization
            detectionMap = detectionMap / max_value
            max_value = np.ndarray.max(detectionMap_svm)
            detectionMap_svm = detectionMap_svm / max_value
            max_value = np.ndarray.max(detectionMap_svm_global)
            detectionMap_svm_global = detectionMap_svm_global / max_value

            if maskImage is not None:
                maskImage = utils.resizeImage(maskImage, 1200)
                scipy.io.savemat(self.filename + '_detection_map.mat', dict(positive=detectionMap[maskImage > 200], negative=detectionMap[maskImage <= 200]))
                scipy.io.savemat(self.filename + '_detection_map_global.mat', dict(positive=detectionMap_global[maskImage > 200], negative=detectionMap_global[maskImage <= 200]))
                scipy.io.savemat(self.filename + '_detection_map_svm.mat', dict(positive=detectionMap_svm[maskImage > 200], negative=detectionMap_svm[maskImage <= 200]))
                scipy.io.savemat(self.filename + '_detection_map_svm_global.mat', dict(positive=detectionMap_svm_global[maskImage > 200], negative=detectionMap_svm_global[maskImage <= 200]))

            outputMask = detectionMap.copy()
            outputMask[outputMask < 0.8] = 0
            outputMask[outputMask >= 0.8] = 1

            detectionMap *= 255

            if self.display_result:
                # Display color map
                color_map = detectionMap
                color_map = color_map.astype(np.uint8)
                color_map = cv2.applyColorMap(color_map, cv2.COLORMAP_JET)
                out = np.concatenate((utils.resizeImage(image, 500), utils.resizeImage(color_map, 500)), axis=1)
                cv2.imshow('output', out)
                cv2.waitKey(0)

                #Display spliced regions
                regionMask = np.zeros(image.shape, 'uint8')
                regionMask[..., 0] = outputMask.copy()
                regionMask[..., 1] = outputMask.copy()
                regionMask[..., 2] = outputMask.copy()
                splicedRegions = np.multiply(image, regionMask)
                out = np.concatenate((utils.resizeImage(image, 500), utils.resizeImage(splicedRegions, 500)), axis=1)
                cv2.imshow('output', out)
                cv2.waitKey(0)

            if maskImage is not None:
                # Normalization
                maskImage[maskImage < 120] = 0
                maskImage[maskImage >= 120] = 1
                w, h = maskImage.shape
                totalSplicedPixels = (maskImage == 1).sum()
                diffMask = maskImage - outputMask
                fn = (diffMask == 1).sum()
                fp = (diffMask == -1).sum()
                tp = totalSplicedPixels - fn
                tn = w * h - totalSplicedPixels - fp

                precision = tp/(tp + fp)
                recall = tp/(tp + fn)
                accuracy = (tp + tn)/(tp + tn + fp + fn)
                print('Precision: ' + str(precision) + ' - Recall: ' + str(recall))

            #Write output mask
            #outputMask *= 255
            #cv2.imwrite(output, outputMask)

            #featureFile.close()
            # Print score
            print("Splicing score: " + str(score))

        else:
            print('No image found: ' + img)

    '''
    Evaluate Illuminant map and references
    '''
    def evaluateIlluminantMaps(self, image, mask, direction = None):
        config.forceMapsExtraction = True
        segmented = None
        # Dividing bands
        if direction == 'vertical' or direction is None:
            self.verticalBands = self.extractImageBands(image, config.bandWidth/config.bandDeltaFactor, segmented, 'vertical', mask)
        if direction == 'horizontal' or direction is None:
            self.horizontalBands = self.extractImageBands(image, config.bandHeight/config.bandDeltaFactor, segmented, 'horizontal', mask)

        # Exctract illuminant maps
        if direction == 'vertical' or direction is None:
            self.extractIlluminants('vertical')
        if direction == 'horizontal' or direction is None:
            self.extractIlluminants('horizontal')

        # Evaluating illuminant references
        if direction == 'vertical' or direction is None:
            self.verticalReferences = self.getReferenceIlluminant('vertical')
        if direction == 'horizontal' or direction is None:
            self.horizontalReferences = self.getReferenceIlluminant('horizontal')


    '''
    Extract global illuminant map references
    '''
    def extractGlobalReferences(self, img):
        image = cv2.imread(img)
        filename = utils.getFilename(img)

        W, H, c = image.shape
        base = np.zeros((W, H), 'uint8')
        segmented = np.zeros((W, H, 3), 'uint8')
        segmented[..., 0] = base.copy()
        segmented[..., 1] = base.copy()
        segmented[..., 2] = base.copy() + 255

        segmentedPath = config.temp_folder + 'global_segmented.png'
        cv2.imwrite(segmentedPath, segmented)

        illuminantMaps.estimateGrayEdge(img, segmentedPath, config.temp_folder, self.verbose)
        illuminantMaps.estimateGrayWorld(img, segmentedPath, config.temp_folder, self.verbose)
        illuminantMaps.estimateMaxRGB(img, segmentedPath, config.temp_folder, self.verbose)
        illuminantMaps.estimateShadesOfGray(img, segmentedPath, config.temp_folder, self.verbose)
        illuminantMaps.estimateSecondGrayEdge(img, segmentedPath, config.temp_folder, self.verbose)

        global_references = {}
        for alg in self.algorithms:
            global_references[alg] = utils.evaluateRGBMedian(config.temp_folder + filename + "_gge_map_" + alg + ".png")[0]
        return global_references

    '''
    Extract bands from the images with width
    configured in configuration file
    @:parameter image: OpenCV matrix
    @:returns number of extracted bands
    '''
    def extractImageBands(self, image, delta, segmented = None, direction = 'vertical', mask = None):
        self.bands[direction] = []

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
        counter, i = 0, -size + delta
        # Exctracting vertical bands
        while i < limit:
            start = 0
            if i > 0:
                start = i
            diff = i + size
            if diff < limit:
                end = diff
            else:
                end = limit - diff - 1

            if end - start < 10:
                break

            if direction == 'vertical':
                band = image[:, start:end]
                if segmented is not None:
                    bandSeg = segmented[:, start:end]

                if mask is not None:
                    maskSeg = mask[:, start:end]

            else:
                band = image[start:end, :]
                if segmented is not None:
                    bandSeg = segmented[start:end, :]

                if mask is not None:
                    maskSeg = mask[start:end, :]

            W, H, _ = band.shape

            if (band is not None) and W > 0 and H > 0:
                #Store band and segmented band
                bandPath = config.temp_folder + direction + '_band_' + str(counter) + '.png'
                bandSegPath = config.temp_folder + direction + '_band_segmented_' + str(counter) + '.png'

                bandLabel = 0

                #Mark a band as fake (if spliced region is more than the 5% of the area)
                if mask is not None:
                    fakes = np.sum(np.sum(maskSeg))
                    if fakes > 0:
                        percent = fakes/(W * H * 255)
                        if percent > 0.2:
                            bandLabel = 1
                            #cv2.imshow('band', band)
                            #cv2.waitKey()

                b = DetectionBand(i, direction, bandLabel)
                b.colorAvg = utils.averageRGBColor(band)
                self.bands[direction].append(b)

                cv2.imwrite(bandPath, band)
                if segmented is None:
                    base = np.zeros((W, H), 'uint8')
                    bandSeg = np.zeros((W, H, 3), 'uint8')
                    bandSeg[..., 0] = base.copy()
                    bandSeg[..., 1] = base.copy()
                    bandSeg[..., 2] = base.copy() + 255

                cv2.imwrite(bandSegPath, bandSeg)
                counter += 1

            i += delta

        return counter

    '''
      Extract bands illuminant color maps
      @:parameter direction
      @:returns number of extracted bands
      '''
    def extractIlluminants(self, direction = 'vertical'):
        print('\tExctracting ' + direction + ' illuminant maps')
        if direction == 'vertical':
            dimension = self.verticalBands
        else:
            dimension = self.horizontalBands

        for i in range(dimension):
            if self.verbose:
                print('\tExtracting ' + str(i + 1) + '/' + str(dimension) + ' ' + direction + ' band')
            bandPath = config.temp_folder + direction + '_band_' + str(i) + '.png'
            bandSegPath = config.temp_folder + direction + '_band_segmented_' + str(i) + '.png'
            illuminantMaps.estimateGrayEdge(bandPath, bandSegPath, config.temp_folder, self.verbose)
            illuminantMaps.estimateGrayWorld(bandPath, bandSegPath, config.temp_folder, self.verbose)
            illuminantMaps.estimateMaxRGB(bandPath, bandSegPath, config.temp_folder, self.verbose)
            illuminantMaps.estimateShadesOfGray(bandPath, bandSegPath, config.temp_folder, self.verbose)
            illuminantMaps.estimateSecondGrayEdge(bandPath, bandSegPath, config.temp_folder, self.verbose)
            if self.verbose:
                print('Extracted ' + direction + ' ' + str(i) + " illuminant maps")
            print('\tGenerated ' + str(i + 1) + '/' + str(dimension) + ' ' + direction + ' illuminant maps')


    '''
    Evaluates reference illuminant for each dimension
    vertical or horizontal
    '''
    def getReferenceIlluminant(self, direction = 'vertical'):
        if self.verbose:
            print('Evaluating ' + direction + ' illuminant reference color')
        if direction == 'vertical':
            dimension = self.verticalBands
        else:
            dimension = self.horizontalBands

        medians = {
            'grayworld': np.array([[0., 0., 0.]]),
            'maxrgb': np.array([[0., 0., 0.]]),
            'grayedge': np.array([[0., 0., 0.]]),
            'shadesofgray': np.array([[0., 0., 0.]]),
            'secondgrayedge': np.array([[0., 0., 0.]])
        }

        for i in range(dimension):
            filename = config.temp_folder + direction + '_band_' + str(i) + "_gge_map_"

            for alg in self.algorithms:
                medians[alg] = np.concatenate((medians[alg], utils.evaluateRGBMedian(filename + alg + ".png")), axis=0)

        for med in medians:
            medians[med] = np.delete(medians[med], 0, 0)
            medians[med] = np.median(medians[med], axis=0)
        return medians


    '''Clear all images'''
    def __del__(self):
        print('Cleaning up')
        utils.removeTempFolder()


'''Detection band object'''
class DetectionBand:

    def __init__(self, rect, direction, label):
        self.start = rect
        self.label = label
        self.direction = direction
        self.colorAvg = 0;

    def incrementDetection(self, detectionMap, value = 1):
        height, width = detectionMap.shape
        if self.direction == 'vertical':
            limit = width
            size = config.bandWidth
        else:
            limit = height
            size = config.bandHeight

        diff = self.start + size
        if diff < limit:
            end = diff
        else:
            end = limit - diff - 1

        if self.direction == 'vertical':
            detectionMap[:, self.start:end] = detectionMap[:, self.start:end] + value
        else:
            detectionMap[self.start:end, :] = detectionMap[self.start:end, :] + value

        return detectionMap


