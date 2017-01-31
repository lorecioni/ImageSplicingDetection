'''
Created on 03 ott 2016

@author: lorenzocioni
'''
import cv2
import numpy as np
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
        #self.verbose = verbose
        self.verbose = False
        self.extract_maps = extractMaps
        self.extract_features = extractFeatures
        self.cross_validation = crossVal
        self.heat_map = heatMap

        #Bands
        self.horizontalBands = 0
        self.verticalBands = 0

        #Reference color
        self.horizontalReferences = {}
        self.verticalReferences = {}


    def detect(self, img, mask = False):
        filename = utils.getFilename(img)
        print('Processing ' + filename)

        #Reads image
        image = cv2.imread(img)

        #Mask
        if mask:
            maskImage = cv2.imread(config.masks_folder + filename + '.png', cv2.IMREAD_GRAYSCALE)
            maskImage = np.invert(maskImage)
        else:
            maskImage = None

        if image is not None:
            illuminantMaps.prepareImageIlluminants(img, config.seg_sigma, config.seg_k, config.seg_min_size,
                                                        config.min_intensity, config.max_intensity, self.verbose)

            segmented = cv2.imread(config.maps_folder + filename + "_segmented.png")
            #Dividing bands
            self.bands = {}

            self.verticalBands = self.extractImageBands(image, segmented, 'vertical', maskImage)
            self.horizontalBands = self.extractImageBands(image, segmented, 'horizontal', maskImage)

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

            #Evaluate distances

            medians = {}
            for i in range(self.verticalBands):
                filename = config.maps_folder + 'vertical_band_' + str(i) + "_gge_map_"
                medians['grayedge'] = utils.evaluateRGBMedian(filename + "grayedge.png")
                medians['maxrgb'] = utils.evaluateRGBMedian(filename + "maxrgb.png")
                medians['grayworld'] = utils.evaluateRGBMedian(filename + "grayworld.png")
                medians['shadesofgray'] = utils.evaluateRGBMedian(filename + "shadesofgray.png")
                medians['secondgrayedge'] = utils.evaluateRGBMedian(filename + "secondgrayedge.png")

                band = self.bands['vertical'][i]

                for med in medians:
                    distance = utils.euclideanDistanceRGB(medians[med], self.verticalReferences[med])
                    if distance > thresholds[med]:
                        detectionMap = band.incrementDetection(detectionMap)


            for i in range(self.horizontalBands):
                filename = config.maps_folder + 'horizontal_band_' + str(i) + "_gge_map_"
                medians['grayedge'] = utils.evaluateRGBMedian(filename + "grayedge.png")
                medians['maxrgb'] = utils.evaluateRGBMedian(filename + "maxrgb.png")
                medians['grayworld'] = utils.evaluateRGBMedian(filename + "grayworld.png")
                medians['shadesofgray'] = utils.evaluateRGBMedian(filename + "shadesofgray.png")
                medians['secondgrayedge'] = utils.evaluateRGBMedian(filename + "secondgrayedge.png")

                band = self.bands['horizontal'][i]

                for med in medians:
                    distance = utils.euclideanDistanceRGB(medians[med], self.horizontalReferences[med])
                    if distance > thresholds[med]:
                        detectionMap = band.incrementDetection(detectionMap)

            # Recover detection map max value
            max_value = np.ndarray.max(detectionMap)
            # Normalization
            detectionMap = detectionMap / max_value
            detectionMap *= 255
            detectionMap = detectionMap.astype(np.uint8)
            # Display color map
            color_map = cv2.applyColorMap(detectionMap, cv2.COLORMAP_JET)
            cv2.imshow('img', utils.resizeImage(color_map, 500))
            cv2.waitKey(0)


    '''
    Extract bands from the images with width
    configured in configuration file
    @:parameter image: OpenCV matrix
    @:returns number of extracted bands
    '''
    def extractImageBands(self, image, segmented, direction = 'vertical', mask = None):
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
                bandSeg = segmented[:, i:end]

                if mask is not None:
                    maskSeg = mask[:, i:end]

            else:
                band = image[i:end, :]
                bandSeg = segmented[i:end, :]

                if mask is not None:
                    maskSeg = mask[i:end, :]

            W, H, _ = band.shape

            if (band is not None) and W > 0 and H > 0:
                #Store band and segmented band
                bandPath = config.maps_folder + direction + '_band_' + str(counter) + '.png'
                bandSegPath = config.maps_folder + direction + '_band_segmented_' + str(counter) + '.png'

                bandLabel = 0
                #bandLabelPath = config.maps_folder + direction + '_band_' + str(counter) + '.txt'

                if mask is not None:
                    fakes = np.sum(np.sum(maskSeg))
                    if fakes > 0:
                        percent = fakes/(W * H * 255)
                        if percent > 0.05:
                            bandLabel = 1

                self.bands[direction].append(DetectionBand(i, direction, bandLabel))
                #np.savetxt(bandLabelPath, np.array([bandLabel]))

                cv2.imwrite(bandPath, band)
                cv2.imwrite(bandSegPath, bandSeg)
                counter += 1
            i += size

        return counter

    '''
      Extract bands illuminant color maps
      @:parameter direction
      @:returns number of extracted bands
      '''
    def extractIlluminants(self, direction = 'vertical'):
        print('Exctracting ' + direction + ' illuminant maps')
        if direction == 'vertical':
            dimension = self.verticalBands
        else:
            dimension = self.horizontalBands

        for i in range(dimension):
            bandPath = config.maps_folder + direction + '_band_' + str(i) + '.png'
            bandSegPath = direction + '_band_segmented_' + str(i) + '.png'
            illuminantMaps.estimateGrayEdge(bandPath, bandSegPath, self.verbose)
            illuminantMaps.estimateGrayWorld(bandPath, bandSegPath, self.verbose)
            illuminantMaps.estimateMaxRGB(bandPath, bandSegPath, self.verbose)
            illuminantMaps.estimateShadesOfGray(bandPath, bandSegPath, self.verbose)
            illuminantMaps.estimateSecondGrayEdge(bandPath, bandSegPath, self.verbose)
            if self.verbose:
                print('Extracted ' + direction + ' ' + str(i) + " illuminant maps")


    '''
    Evaluates reference illuminant for each dimension
    vertical or horizontal
    '''
    def getReferenceIlluminant(self, direction = 'vertical'):
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
            filename = config.maps_folder + direction + '_band_' + str(i) + "_gge_map_"

            medians['grayedge'] = np.concatenate((medians['grayedge'], utils.evaluateRGBMedian(filename + "grayedge.png")), axis=0)
            medians['maxrgb'] = np.concatenate((medians['maxrgb'], utils.evaluateRGBMedian(filename + "maxrgb.png")), axis=0)
            medians['grayworld'] = np.concatenate((medians['grayworld'], utils.evaluateRGBMedian(filename + "grayworld.png")), axis=0)
            medians['shadesofgray'] = np.concatenate((medians['shadesofgray'], utils.evaluateRGBMedian(filename + "shadesofgray.png")), axis=0)
            medians['secondgrayedge'] = np.concatenate((medians['secondgrayedge'], utils.evaluateRGBMedian(filename + "secondgrayedge.png")), axis=0)

        for med in medians:
            medians[med] = np.delete(medians[med], 0, 0)
            medians[med] = np.median(medians[med], axis=0)
        return medians


class DetectionBand:

    def __init__(self, rect, direction, label):
        self.start = rect
        self.label = label
        self.direction = direction

    def incrementDetection(self, map):
        height, width = map.shape
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
             map[:, self.start:end] = map[:, self.start:end] + 1
        else:
             map[self.start:end, :] = map[self.start:end, :] + 1

        return map


