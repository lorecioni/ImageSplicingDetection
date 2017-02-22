import loadDatasets
import illuminantMaps
import config
import utils
import cv2
import numpy as np
import matplotlib.pyplot as plt
import regionSplicingDetection
import math

verbose = False
display = False
evaluate = True
#channels = ['blue', 'green', 'red']
channels = ['red']

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


def normalImageStatistics(num):
    delta = 20
    detector = regionSplicingDetection.RegionSplicingDetector()
    counter = 0
    images, _ = loadDatasets.load('COLORCHECKER')

    I_b = {
        'blue': [],
        'green': [],
        'red': []
    }
    E_b = {
        'blue': [],
        'green': [],
        'red': []
    }

    for img in images:
        print("Processing " + img)
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
            illuminantMaps.estimateGrayEdge(bandPath, bandSegPath, config.temp_folder, verbose)
            illuminantMaps.estimateMaxRGB(bandPath, bandSegPath, config.temp_folder, verbose)
            illuminantMaps.estimateShadesOfGray(bandPath, bandSegPath, config.temp_folder, verbose)
            illuminantMaps.estimateSecondGrayEdge(bandPath, bandSegPath, config.temp_folder, verbose)

            references = np.array([[0., 0., 0.]]);
            medians = {
                'grayworld': np.array([[0., 0., 0.]]),
                'maxrgb': np.array([[0., 0., 0.]]),
                'grayedge': np.array([[0., 0., 0.]]),
                'shadesofgray': np.array([[0., 0., 0.]]),
                'secondgrayedge': np.array([[0., 0., 0.]])
            }

            for alg in algs:
                imPath = config.temp_folder + 'vertical_band_' + str(v) + "_gge_map_" + alg + ".png"
                medians[alg] = np.concatenate((medians[alg], utils.evaluateRGBMedian(imPath)), axis=0)

            for med in medians:
                medians[med] = np.delete(medians[med], 0, 0)
                medians[med] = np.median(medians[med], axis=0)
                references = np.concatenate((references, np.array([medians[med]])), axis=0)

            references = np.delete(references, 0, 0)
            references = np.median(references, axis=0)

            avg_colors.append(utils.averageRGBColor(band))
            color_references.append(references)

        print("\tExtracted vertical band")


        for v in range(horizontalBands):
            bandPath = config.temp_folder + 'horizontal_band_' + str(v) + '.png'
            band = cv2.imread(bandPath)
            bandSegPath = config.temp_folder + 'horizontal_band_segmented_' + str(v) + '.png'
            illuminantMaps.estimateGrayWorld(bandPath, bandSegPath, config.temp_folder, verbose)
            illuminantMaps.estimateGrayEdge(bandPath, bandSegPath, config.temp_folder, verbose)
            illuminantMaps.estimateMaxRGB(bandPath, bandSegPath, config.temp_folder, verbose)
            illuminantMaps.estimateShadesOfGray(bandPath, bandSegPath, config.temp_folder, verbose)
            illuminantMaps.estimateSecondGrayEdge(bandPath, bandSegPath, config.temp_folder, verbose)

            references = np.array([[0., 0., 0.]]);
            medians = {
                'grayworld': np.array([[0., 0., 0.]]),
                'maxrgb': np.array([[0., 0., 0.]]),
                'grayedge': np.array([[0., 0., 0.]]),
                'shadesofgray': np.array([[0., 0., 0.]]),
                'secondgrayedge': np.array([[0., 0., 0.]])
            }

            for alg in algs:
                imPath = config.temp_folder + 'horizontal_band_' + str(v) + "_gge_map_" + alg + ".png"
                medians[alg] = np.concatenate((medians[alg], utils.evaluateRGBMedian(imPath)), axis=0)

            for med in medians:
                medians[med] = np.delete(medians[med], 0, 0)
                medians[med] = np.median(medians[med], axis=0)
                references = np.concatenate((references, np.array([medians[med]])), axis=0)

            references = np.delete(references, 0, 0)
            references = np.median(references, axis=0)

            avg_colors.append(utils.averageRGBColor(band))
            color_references.append(references)

        print("\tExtracted horizontal band")

        avg_cr = np.zeros(color_references[0].shape)
        for cr in color_references:
            avg_cr = np.vstack((avg_cr, cr))

        avg_cr = np.delete(avg_cr, 0, 0)
        avg_cr = np.median(avg_cr, axis=0)


        resolution_width = 50

        for channel in range(3):
            CR = avg_cr[channel]

            for v in range(verticalBands):
                I_b[channels[channel]].append(avg_colors[v][channel])
                C = color_references[v][channel]
                E_b[channels[channel]].append(abs(C - CR))

            for h in range(verticalBands, verticalBands + horizontalBands):
                I_b[channels[channel]].append(avg_colors[h][channel])
                C = color_references[h][channel]
                E_b[channels[channel]].append(abs(C - CR))

        counter += 1
        if counter == num:
            break


    #Plotting
    for ch in channels:
        I = np.asarray(I_b[ch])
        E = np.asarray(E_b[ch])

        np.savetxt('I_' + ch + '.txt', I);
        np.savetxt('E_' + ch + '.txt', E);

        idx = I.argsort()
        I, E = I[idx], E[idx]

        i = 0
        X = np.arange(255)
        Y = np.zeros(255, dtype=np.float32)
        while i < 255:
            end = i + resolution_width + 1
            if end >= 255:
                end = 254
            subset = E[(I >= i) & (I < end)]
            variance = np.var(subset)
            if math.isnan(variance):
                variance = 0

            Y[i:end] = variance
            i += resolution_width

        plt.plot(X, Y, color=ch)
    plt.show()


'''Test splicing regions'''

#testGGEMaps()
#normalImageStatistics(30)


def testDifferentResolution(res):
    # Plotting
    for ch in channels:
        I = np.loadtxt('I_' + ch + '.txt')
        E = np.loadtxt('E_' + ch + '.txt')

        #np.savetxt('I_' + ch + '.txt', I);
        #np.savetxt('E_' + ch + '.txt', E);

        idx = I.argsort()
        I, E = I[idx], E[idx]

        i = 0
        X = np.arange(255)
        Y = np.zeros(255, dtype=np.float32)
        Y_hist = np.zeros(255)

        while i < 255:
            end = i + res + 1
            if end >= 255:
                end = 254
            subset = E[(I >= i) & (I < end)]
            variance = np.var(subset)
            if math.isnan(variance):
                variance = 0
            Y[i:end] = variance
            i += res

        i = 0
        while i < len(I):
            mean = int(I[i])
            Y_hist[mean] += 1
            i += 1

        plt.plot(X, Y, color=ch)
        #np.savetxt('test.txt', Y_hist)
        plt.plot(X, Y_hist)

    plt.show()

testDifferentResolution(10)