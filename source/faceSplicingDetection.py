'''
Created on 03 ott 2016

@author: lorenzocioni
'''
import cv2
import numpy as np
import numpy.linalg as npl
from sklearn import svm, model_selection
import os
from sklearn.externals import joblib
from sklearn.model_selection import LeaveOneOut
from sklearn.metrics import accuracy_score
import descriptors
import illuminantMaps
import config
import math

'''
Splicing detection main procedure. The result of the output
will be a number of regions of the image that can be spliced
over an image
'''
class FaceSplicingDetection:

    def __init__(self, extractMaps, extractFeatures, verbose):
        self.verbose = verbose
        self.extract_maps = extractMaps
        self.extract_features = extractFeatures
        self.faceCascade = cv2.CascadeClassifier(config.cascadePath)


    def detectSplice(self, img, heat_map, depth):
        filename = self.getFilename(img)
        # Extracting image features
        #features = self.processImage(img, False, self.verbose, heat_map)

        # 2. Statistical difference between IIC and GGE maps
        gge_map = cv2.imread(config.maps_folder + filename + '_gge_map.png')
        iic_map = cv2.imread(config.maps_folder + filename + '_iic_map.png')

        return


    '''
    Train model for further splicing detection
    @param images: the list of images filenames
    @param labels: the list of image labels (0 if pristine, 1 if spliced)
    '''
    def train(self, images, labels, cross_validate = False, extract_features = True, extract_maps = True, heat_map = False):
        return

    '''
    Extract feature vector for a selected image
    '''
    def extractFeatures(self, img, heat_map = False):
        self.processImage(img, True, self.verbose, heat_map)


    '''
    Process image for extracting features
    - Prepare the image for illuminant methods (segmentation)
    - Extracting GGE illuminant map
    - Extracting IIC illulimant map
    - Evaluate principal components for each maps
    - Build feature vectors
    @param img: the path of the image to be processed
    @param verbose: display extended output
    '''
    def processImage(self, img, label, store = False, extract_features = True, extract_maps = True, verbose = False, heat_map = False):
        #Extract filename from image path
        filename = self.getFilename(img)
        
        print('Processing: ' + filename)

        if self.extract_maps:
            # 1. Extracting GGE and IIC illuminant maps
            print('\t-Segmenting image')
            illuminantMaps.prepareImageIlluminants(img, config.seg_sigma, config.seg_k, config.seg_min_size, config.min_intensity, config.max_intensity, verbose)

            # 1.2 Extracting GGE illuminant map
            print('\t-Extracting GGE map')
            illuminantMaps.extractGGEMap(img, filename + "_segmented" + config.maps_out_suffix +".png", config.gge_sigma, config.gge_n, config.gge_p, verbose)

            # 1.3 Extracting IIC illuminant map
            print('\t-Extracting IIC map')
            illuminantMaps.extractIICMap(img, filename + "_segmented" + config.maps_out_suffix + ".png", verbose)


        

        if self.extract_features:
            # 2.1 Building heat map (only for visualizations)
            #if heat_map:
            #    self.visualizeHeatMap(gge_map, iic_map)

            
            # 2.2 Extract maps principal components
            print('\t-Extracting PCs')
            #gge_pcs = self.extractPrincipalComponents(gge_map)
            #iic_pcs = self.extractPrincipalComponents(iic_map)

            # 3 Build feature vectors
            print('\t-Building feature vector')
            features = self.buildFeatureVector(img)

            return features

        return None

    '''
    Segment current image extracting human faces
    '''
    def extractFaces(self, img):
        orig = cv2.imread(img, cv2.COLOR_BGR2GRAY)
        faces = self.faceCascade.detectMultiScale(
            orig,
            scaleFactor = 1.1,
            minNeighbors = 5,
            minSize = (30, 30),
            flags = cv2.CASCADE_SCALE_IMAGE
        )
        return faces

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


    '''
    PCA analysis on a give map in order to evaluate significant
    eigenvalues
    '''
    def extractPrincipalComponents(self, X):
        try:
            #Image normalization
            X = cv2.normalize(X, X, 0, 1, cv2.NORM_MINMAX, cv2.CV_32F)
            # singular value decomposition of a data matrix such that:  X = U*S*V.T
            # * U and V are the singular matrices
            # * S is a diagonal matrix
            grayimg = self.rgb2gray(X)

            heat_map = grayimg * 255
            heat_map = heat_map.astype(np.uint8)
            #cv2.imshow('img', heat_map);
            #cv2.waitKey(0)
            # s has eigenvalues and V columns are eigenvectors
            _, s, _ = npl.svd(grayimg, full_matrices = False)
            pcs = s[0:9]
        except:
            pcs = np.zeros(9)

        return pcs

    '''
    Builds feature vector given two set of principal
    components
    '''
    def buildFeatureVector(self, img, descriptor = "ACC", space = 4, channel = 3, illumType = "GGE"):
        filename = self.getFilename(img)
        gge_map = cv2.imread(config.maps_folder + filename + '_gge_map.png')
        iic_map = cv2.imread(config.maps_folder + filename + '_iic_map.png')
        faces = self.extractFaces(img)
        
        #Storing faces extracted from maps
        count = 0
        for (x, y, w, h) in faces:
            face_gge = gge_map[y:y + h, x:x + w]
            face_iic = iic_map[y:y + h, x:x + w]
            path = config.faces_folder + "face-GGE-" + str(count) + ".png"
            #extract gge maps descriptors
            cv2.imwrite(path, face_gge)
            descriptors.extractDescriptor(path, descriptor, space, channel)
            #extract iic maps descriptors
            path = config.faces_folder + "face-IIC-" + str(count) + ".png"
            cv2.imwrite(path, face_iic)
            descriptors.extractDescriptor(path, descriptor, space, channel)
            count = count + 1
        
        descriptor = descriptor.upper()
        counter = 1
        contVectors = 0
        # Inherent loops compose a descriptor for each pair of faces at the image
        allVectors = []
    
        # Label 1 point a pristine pair and label -1 point a pair that contains at least one fake image
        while counter < len(faces):
            firstFace = counter
            secondFace = counter + 1
            
            while secondFace < len(faces):
                newVector = []
                contVectors = contVectors + 1
                
                firstFaceFeat = config.faces_folder + 'face-GGE-'  + str(firstFace) + "-" + descriptor.lower() + "-descriptor.txt"
                secondFaceFeat = config.faces_folder + 'face-GGE-'  + str(secondFace) + "-" + descriptor.lower() + "-descriptor.txt"
                files = open(firstFaceFeat, "rt")
                files.seek(0)
                temp = files.readline()
                linesf1 = files.readlines()
                files.close()
                for i in linesf1:
                    desc = list(i)
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
                    files = open(secondFaceFeat, "rb")
                    files.seek(0)
                    temp = files.readline()
                    linesf2 = files.readlines()
                    files.close()
                    for i in linesf2:
                        desc = list(i)
                        cont = 0
                        while (cont < (len(desc) - 1)):
                            newVector.append(float(desc[cont]))
                            cont = cont + 1
                
                allVectors.append(newVector)
                secondFace = secondFace + 1
            counter = counter + 1 
            nameFile = "../temp/vectors/fv-" + filename + ".txt"
            files = open(nameFile,"wt")
            files.seek(0)
            for i in allVectors:
                temp = i
                cont = 0
                for j in temp:
                    if (cont == 0):
                        frase = str(j) + " "
                        files.write(frase)
                    else:
                        frase = str(cont) + ":" + str(j) + " "
                        files.write(frase)
                    cont = cont + 1
                files.write("\n")
            files.close()  
        features = []
            
        return features

    '''
    Evaluate Euclidean distances between each image IMs
    @param images: the list of images filenames
    '''
    def evaluateEuclideanDistances(self, images, extract_features = True, visualize_heat_map = False):
        # Extract image features from each images in training set

        for i in range(len(images)):
            img = images[i]
            if extract_features:
                self.processImage(img, True, self.verbose, visualize_heat_map)

            print(img)
            filename = img.split('/')
            filename = filename[len(filename) - 1]
            filename = filename[:-4]

            #Reads IMs
            gge = cv2.imread(config.maps_folder + filename + '_gge_map' + config.maps_out_suffix + '.png')
            iic = cv2.imread(config.maps_folder + filename + '_iic_map' + config.maps_out_suffix + '.png')

            gge = self.resizeImage(gge, 200)
            iic = self.resizeImage(iic, 200)
            if gge == None or iic == None:
                continue

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
            sum = np.sum(heat_map)
            #if filename.startswith('splicing'):
            #    sum = sum + 10000;

            print(type(sum))
            if visualize_heat_map:
                heat_map = heat_map * 255
                heat_map = heat_map.astype(np.uint8)
                #Display color map
                color_map = cv2.applyColorMap(heat_map, cv2.COLORMAP_JET)
                cv2.imshow('img', self.resizeImage(color_map, 500))
                cv2.waitKey(0)
            #np.savetxt(config.features_folder + filename + '_sum_distances.txt', np.array(sum).reshape(1,), delimiter=' ')
            print(sum)



        features = []
        files = os.listdir(config.features_folder)
        for i in files:
            if not i.startswith('.'):
                features.append(np.loadtxt(config.features_folder + i))

        features = np.asanyarray(features)


    def getFilename(self, img):
        filename = img.split('/')
        filename = filename[len(filename) - 1]
        filename = filename[:-4]
        return filename
    
    '''
    Convert image to grayscale
    '''
    def rgb2gray(self, img):
        return np.dot(img[...,:3], [0.299, 0.587, 0.144])

    '''
    Resize image maintaining aspect ratio
    '''
    def resizeImage(self, img, width):
        r = width / img.shape[1]
        dim = (width, int(img.shape[0] * r))
        # perform the actual resizing of the image and show it
        return cv2.resize(img, dim, interpolation = cv2.INTER_AREA)
