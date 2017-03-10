'''
Created on 03 ott 2016

@author: lorenzocioni
'''
import cv2
import numpy as np
import illuminantMaps
import config
import utils

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

        #self.featureFile = open('features_new.txt', "w");
        self.clf = joblib.load(config.data_folder + 'regions/trained_data.pkl')

    def evaluate(self, images, output):
        for i in range(len(images)):
            img = images[len(images) - 40 - i]
            self.detect(img, output, True)
            if i == 2:
                break
        self.featureFile.close()

    def detect(self, img, output, groundtruth = True):
        self.filename = utils.getFilename(img)
        print('Processing ' + self.filename)

        #Reads image
        image = cv2.imread(img)

        #Mask
        if groundtruth:
            maskImage = cv2.imread(config.masks_folder + self.filename + '.png', cv2.IMREAD_GRAYSCALE)
            if maskImage is not None:
                maskImage = np.invert(maskImage)
        else:
            maskImage = None

        if image is not None:
            config.forceMapsExtraction = True

            segmented = None
            #Dividing bands
            self.verticalBands = self.extractImageBands(image, config.bandWidth/4, segmented, 'vertical', maskImage)
            self.horizontalBands = self.extractImageBands(image, config.bandHeight/4, segmented, 'horizontal', maskImage)

            #Exctract illuminant maps
            self.extractIlluminants('vertical')
            self.extractIlluminants('horizontal')

            #Evaluating illuminant references
            self.verticalReferences = self.getReferenceIlluminant('vertical')
            self.horizontalReferences = self.getReferenceIlluminant('horizontal')

            rows, cols, _ = image.shape
            detectionMap = np.zeros((rows, cols), dtype=int)

            thresholds = {
                'grayedge': 3.6,
                'maxrgb': 6,
                'grayworld': 6,
                'shadesofgray': 6,
                'secondgrayedge': 2
            }

            score_svm = 0
            score_old = 0

            #Evaluate distances
            medians = {}
            for i in range(self.verticalBands):
                filename = config.temp_folder + 'vertical_band_' + str(i) + "_gge_map_"
                for alg in self.algorithms:
                    medians[alg] = utils.evaluateRGBMedian(filename + alg + ".png")

                band = self.bands['vertical'][i]
                band_feature = []
                detected = 0
                for med in medians:
                    distance = utils.euclideanDistanceRGB(medians[med], self.verticalReferences[med])
                    band_feature.append(distance)
                    if distance > thresholds[med]:
                        detected += 1

                if detected >= 4:
                    detected = 1
                else:
                    detected = 0

                prediction = self.clf.predict(np.asarray(band_feature).reshape((1, -1)))
                if prediction[0] == 1:
                    detectionMap = band.incrementDetection(detectionMap)

                print('True: ' + str(band.label) + ' Predicted: ' + str(prediction[0]) + ' Old: ' + str(detected))

                if band.label == prediction[0]:
                    score_svm += 1
                if band.label == detected:
                    score_old += 1

                print('\tEvaluated ' + str(i + 1) + '/' + str(self.verticalBands))

                #if groundtruth:
                #    self.featureFile.write(str(band.label) + ": ")
                #    for feat in band_feature:
                #        self.featureFile.write(str(feat) + " ")
                #    self.featureFile.write("\n")

            for i in range(self.horizontalBands):
                filename = config.temp_folder + 'horizontal_band_' + str(i) + "_gge_map_"
                for alg in self.algorithms:
                    medians[alg] = utils.evaluateRGBMedian(filename + alg + ".png")

                band = self.bands['horizontal'][i]
                band_feature = []
                detected = 0
                for med in medians:
                    distance = utils.euclideanDistanceRGB(medians[med], self.horizontalReferences[med])
                    band_feature.append(distance)
                    if distance > thresholds[med]:
                        detected += 1

                if detected >= 4:
                    detected = 1
                else:
                    detected = 0

                prediction = self.clf.predict(np.asarray(band_feature).reshape((1, -1)))
                if prediction[0] == 1:
                    detectionMap = band.incrementDetection(detectionMap)

                print('True: ' + str(band.label) + ' Predicted: ' + str(prediction[0]) + ' Old: ' + str(detected))
                print('\tEvaluated ' + str(i + 1) + '/' + str(self.horizontalBands))

                if band.label == prediction[0]:
                    score_svm += 1
                if band.label == detected:
                    score_old += 1

                #if groundtruth:
                #    self.featureFile.write(str(band.label) + ": ")
                #    for feat in band_feature:
                #        self.featureFile.write(str(feat) + " ")
                #    self.featureFile.write("\n")

            print('Score SVM: ' + str(score_svm/(self.verticalBands + self.horizontalBands)))
            print('Score Old: ' + str(score_old / (self.verticalBands + self.horizontalBands)))

            # Recover detection map max value
            max_value = np.ndarray.max(detectionMap)
            score = max_value/8
            if score < 0.8:
                print('Image is NORMAL - Score: ' + str(score))
            else:
                print('Image is FAKE - Score: ' + str(score))

            # Normalization
            detectionMap = detectionMap / max_value

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

            #Write output mask
            outputMask *= 255
            # cv2.imwrite(output, outputMask)

            # Print score
            print("Splicing score: " + str(score))

        else:
            print('No image found: ' + img)
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
                        if percent > 0.05:
                            bandLabel = 1

                self.bands[direction].append(DetectionBand(i, direction, bandLabel))

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

    def incrementDetection(self, detectionMap):
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
            detectionMap[:, self.start:end] = detectionMap[:, self.start:end] + 1
        else:
            detectionMap[self.start:end, :] = detectionMap[self.start:end, :] + 1

        return detectionMap


