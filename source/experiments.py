import loadDatasets
import illuminantMaps
import config
import utils
import cv2
import numpy as np
import matplotlib.pyplot as plt
import regionSplicingDetection

verbose = False
display = False
evaluate = True
channels = ['blue', 'green', 'red']
algs = ['grayedge', 'grayworld', 'maxrgb', 'secondgrayedge', 'shadesofgray']

def testGGEMaps():
    images, labels = loadDatasets.load()
    length = len(algs)
    if evaluate:
        for img in images:
            filename = utils.getFilename(img)
            print('Processing ' + filename)
            illuminantMaps.prepareImageIlluminants(img, config.seg_sigma, config.seg_k, config.seg_min_size,
                                                            config.min_intensity, config.max_intensity, False)
            illuminantMaps.estimateGrayEdge(img, filename + "_segmented.png", verbose)
            illuminantMaps.estimateGrayWorld(img, filename + "_segmented.png", verbose)
            illuminantMaps.estimateMaxRGB(img, filename + "_segmented.png", verbose)
            illuminantMaps.estimateShadesOfGray(img, filename + "_segmented.png", verbose)
            illuminantMaps.estimateSecondGrayEdge(img, filename + "_segmented.png", verbose)

            distanceMap = np.zeros((length, length), dtype=np.float64)
            for i in range(length):
                first = cv2.imread(config.maps_folder + filename + "_gge_map_" + algs[i] + ".png")
                for j in range(i + 1, length):
                    second = cv2.imread(config.maps_folder + filename + "_gge_map_" + algs[j] + ".png")
                    dist = utils.evaluateEuclideanDistances(first, second, display)
                    distanceMap[i, j], distanceMap[j, i] = dist, dist

            np.savetxt(config.maps_folder + filename + '.out', distanceMap)


    '''Plot histogram'''
    for i in range(length):
        for j in range(i + 1, length):
            normal_x = []
            spliced_x = []
            normal_y = []
            spliced_y = []

            for k in range(len(images)):
                filename = utils.getFilename(images[k])
                label = 0
                if filename.find("splicing") > -1:
                    label = 1
                distanceMap = np.loadtxt(config.maps_folder + filename + '.out')
                if label == 0:
                    normal_x.append(k)
                    normal_y.append(distanceMap[i, j])
                else:
                    spliced_x.append(k)
                    spliced_y.append(distanceMap[i, j])

            normal_x = np.asarray(normal_x)
            spliced_x = np.asarray(spliced_x)
            normal_y = np.asarray(normal_y)
            spliced_y = np.asarray(spliced_y)

            plt.scatter(normal_x, normal_y, color='blue')
            plt.scatter(spliced_x, spliced_y, color='red')
            plt.savefig(config.maps_folder + algs[i] + '_' + algs[j])
            plt.close()


def normalImageStatistics():
    delta = 20
    detector = regionSplicingDetection.RegionSplicingDetector()
    counter = 0
    images, _ = loadDatasets.load('COLORCHECKER')
    for img in images:
        image = cv2.imread(img)
        verticalBands = detector.extractImageBands(image, delta, direction = 'vertical')
        horizontalBands = detector.extractImageBands(image, delta, direction = 'horizontal')

        avg_colors = []
        color_references = []

        for v in range(verticalBands):
            bandPath = config.temp_folder + 'vertical_band_' + str(v) + '.png'
            band = cv2.imread(bandPath)
            bandSegPath = config.temp_folder + 'vertical_band_segmented_' + str(v) + '.png'
            illuminantMaps.estimateGrayWorld(bandPath, bandSegPath, config.temp_folder, verbose)
            imPath = config.temp_folder + 'vertical_band_' + str(v) + "_gge_map_grayworld.png"
            bandMap = cv2.imread(imPath)
            print(utils.averageRGBColor(band)[1])
            avg_colors.append(utils.averageRGBColor(band))
            color_references.append(bandMap[0, 0])


        for v in range(horizontalBands):
            bandPath = config.temp_folder + 'horizontal_band_' + str(v) + '.png'
            band = cv2.imread(bandPath)
            bandSegPath = config.temp_folder + 'horizontal_band_segmented_' + str(v) + '.png'
            illuminantMaps.estimateGrayWorld(bandPath, bandSegPath, config.temp_folder, verbose)
            imPath = config.temp_folder + 'horizontal_band_' + str(v) + "_gge_map_grayworld.png"
            bandMap = cv2.imread(imPath)
            print(utils.averageRGBColor(band)[1])
            avg_colors.append(utils.averageRGBColor(band))
            color_references.append(bandMap[0, 0])


        avg_cr = np.zeros(color_references[0].shape)
        for cr in color_references:
            avg_cr = np.vstack((avg_cr, cr))

        avg_cr = np.delete(avg_cr, 0, 0)
        avg_cr = np.average(avg_cr, axis=0)


        for channel in range(1, 2):
            CR = avg_cr[channel]
            X, Y = [], []

            for v in range(verticalBands):
                X.append(avg_colors[v][channel])
                C = color_references[v][channel]
                Y.append(abs(C - CR))

            for h in range(verticalBands, verticalBands + horizontalBands):
                X.append(avg_colors[h][channel])
                C = color_references[h][channel]
                Y.append(abs(C - CR))

            X = np.asarray(X)
            Y = np.asarray(Y)

            idx = X.argsort()
            plt.plot(X[idx], Y[idx], color = channels[channel])

        plt.show()
        if counter == 0:
            break




'''Test splicing regions'''

#testGGEMaps()
normalImageStatistics()