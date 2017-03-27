import loadDatasets
import illuminantMaps
import config
import utils
import cv2
import numpy as np
import matplotlib.pyplot as plt
import regionSplicingDetection
import math
from sklearn.svm import SVC
from sklearn.externals import joblib

verbose = False
display = False
evaluate = True
colors = ['blue', 'green', 'red']
#channels = ['blue', 'green', 'red']
#channels = ['red']
channels = ["y", "cb", "cr"]
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


def normalImageStatistics(num, type = 'rgb'):
    delta = 20
    detector = regionSplicingDetection.RegionSplicingDetector()
    counter = 0
    images, _ = loadDatasets.load('COLORCHECKER')

    if type == 'rgb':
        print('Color space: RGB')
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

    if type == 'ycbcr':
        print('Color space: YCbCr')
        I_b = {
            'y': [],
            'cb': [],
            'cr': []
        }
        E_b = {
            'y': [],
            'cb': [],
            'cr': []
        }

    t = 0
    for img in images:
        if t < 25:
            t += 1
            continue

        print("Processing " + img)
        image = cv2.imread(img)
        if type == 'ycbcr':
            image = cv2.cvtColor(image, cv2.COLOR_BGR2YCR_CB)
        elif type == 'hsv':
            image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

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
    color_index = 0
    for ch in channels:
        I = np.asarray(I_b[ch])
        E = np.asarray(E_b[ch])

        np.savetxt(config.data_folder + 'channels/I_' + ch + '_' + type + '.txt', I);
        np.savetxt(config.data_folder + 'channels/E_' + ch + '_' + type + '.txt', E);

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

        plt.plot(X, Y, color=colors[color_index])
        color_index += 1
    plt.show()


'''Test splicing regions'''

#testGGEMaps()
#normalImageStatistics(25, 'ycbcr')


def getPlotData(res, color_space, channel, type = 'normal'):
    I = np.loadtxt(config.data_folder + 'channels/dsi-1/I_' + channel + '_' + color_space + '_' + type + '.txt')
    E = np.loadtxt(config.data_folder + 'channels/dsi-1/E_' + channel + '_' + color_space + '_' + type + '.txt')

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

    return X, Y, Y_hist

def testDifferentResolution(res, color_space, channel = 'all', type = 'all'):

    if channel == 'all':
        channel_idx = 0
        for ch in channels:

            if type == 'all':
                X, Y, Y_hist = getPlotData(res, color_space, ch, 'normal')
                plt.plot(X, Y, color=colors[0])
                X, Y, Y_hist = getPlotData(res, color_space, ch, 'spliced')
                plt.plot(X, Y, color=colors[2])
            else:
                X, Y, Y_hist = getPlotData(res, color_space, ch, type)
                plt.plot(X, Y, color=colors[0])

            channel_idx += 1
    else:
        if type == 'all':
            X, Y, Y_hist = getPlotData(res, color_space, channel, 'normal')
            plt.plot(X, Y, color=colors[0])
            X, Y, Y_hist = getPlotData(res, color_space, channel, 'spliced')
            plt.plot(X, Y, color=colors[2])
        else:
            X, Y, Y_hist = getPlotData(res, color_space, channel, type)
            plt.plot(X, Y, color=colors[1])

    # np.savetxt('test.txt', Y_hist)
    plt.plot(X, Y_hist)
    plt.show()

#testDifferentResolution(10,'ycbcr', channel='y', type='spliced')


def trainSVM(balanced = False):
    ofid = open('trained_features_horizontal.txt', 'rt')
    ofid.seek(0)
    lines = ofid.readlines()
    ofid.close()
    X = []
    Y = []
    total = len(lines)
    print('Total samples: ' + str(total))

    for line in lines:
        segments = line.split(":")
        Y.append(int(segments[0]))
        X.append(np.array(segments[1].split(), dtype=float))

    X = np.asarray(X)
    Y = np.asarray(Y)

    if balanced:
        X_bal = []
        Y_bal = []

        negative_samples = len(Y[Y == 0])
        positive_samples = len(Y[Y > 0])

        if negative_samples != positive_samples:
            if negative_samples <= positive_samples:
                num_samples = negative_samples
            else:
                num_samples = positive_samples
            i = 0
            num_pos, num_neg = 0, 0
            balance_reached = False
            while not balance_reached and i < total:
                label = Y[i]
                feature = X[i]
                if num_neg < num_samples and label == 0:
                    num_neg += 1
                    X_bal.append(feature)
                    Y_bal.append(label)
                elif num_pos < num_samples and label == 1:
                    num_pos += 1
                    X_bal.append(feature)
                    Y_bal.append(label)
                elif num_pos == num_neg:
                    balance_reached = True
                i += 1

        print('Balanced samples: ' + str(len(X_bal)))
        X = np.asarray(X_bal)
        Y = np.asarray(Y_bal)

    clf = SVC(probability=True)
    clf.fit(X, Y)
    joblib.dump(clf, 'TEST_NEW.pkl')
    print(clf.score(X, Y))

trainSVM(True)