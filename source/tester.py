import loadDatasets
import illuminantMaps
import config
import utils
import cv2
import numpy as np
import matplotlib.pyplot as plt

verbose = False
display = False
evaluate = True
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

testGGEMaps()