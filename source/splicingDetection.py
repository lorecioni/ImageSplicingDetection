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
import illuminantMaps
import distanceMetrics
import config
import math


''' 
Splicing detection main procedure. The result of the output
will be a number of regions of the image that can be spliced
over an image
'''
class SplicingDetection:
    
    def __init__(self, verbose):
        self.verbose = verbose
        
    def detectSplice(self, img, heat_map, depth):
        filename = img.split('/')
        filename = filename[len(filename) - 1]
        filename = filename[:-4]
        # Extracting image features
        #features = self.extractFeatures(img, False, self.verbose, heat_map)
                
        # 2. Statistical difference between IIC and GGE maps
        gge_map = cv2.imread(config.maps_folder + filename + '_gge_map.png')
        iic_map = cv2.imread(config.maps_folder + filename + '_iic_map.png')
        
        rows, cols,_ = gge_map.shape
        
        self.detection = np.zeros((rows, cols), dtype=np.uint8)
        self.depth = depth
        clf = joblib.load(config.svm_model)
        
        self.quadTreeDetection(0, clf, gge_map, iic_map, 0, 0)
            
            
        max_value = np.ndarray.max(self.detection)
        heat_map = self.detection / max_value 
        heat_map = heat_map * 255
        heat_map = heat_map.astype(np.uint8)
        
        np.savetxt('out.txt', self.resizeImage(heat_map, 100), fmt='%i')
        
        color_map = cv2.applyColorMap(heat_map, cv2.COLORMAP_SUMMER)

        
        orig = cv2.imread(img)
        out = np.concatenate((self.resizeImage(orig, 100), self.resizeImage(color_map, 100)), axis = 1)
        cv2.imshow('img', out)
        cv2.waitKey(0)
                
        # Classification
        '''
        classifier = joblib.load(config.svm_model)
        prediction = clf.predict(features.reshape(1, -1) )
            
        if prediction[0] == 0:
            print('Image is pristine')
        else:
            print('Image is fake')
        '''
        return
    
    
    def quadTreeDetection(self, depth, clf, gge, iic, x, y):
        rows, cols, _ = gge.shape
        gge_pcs = self.extractPrincipalComponents(gge)
        iic_pcs = self.extractPrincipalComponents(iic)
        features = self.buildFeatureVector(gge_pcs, iic_pcs)
        prediction = clf.predict(features.reshape(1, -1))
        if(prediction[0] == 1):
            self.detection[x:x + rows -1, y: y + cols - 1] = self.detection[x:x + rows -1, y: y + cols - 1] + 1
        
        if depth == self.depth:
            return
        
        rows, cols, _ = gge.shape        
        first_gge = gge[0:math.floor(rows/2), 0:math.floor(cols/2)]
        first_iic = iic[0:math.floor(rows/2), 0:math.floor(cols/2)]
        second_gge = gge[math.floor(rows/2):, 0:math.floor(cols/2)]
        second_iic = iic[math.floor(rows/2):, 0:math.floor(cols/2)]
        third_gge = gge[0:math.floor(rows/2), math.floor(cols/2):]
        third_iic = iic[0:math.floor(rows/2), math.floor(cols/2):]
        fourth_gge = gge[math.floor(rows/2):, math.floor(cols/2):]
        fourth_iic = iic[math.floor(rows/2):, math.floor(cols/2):]
        
        # Starts recursion dividing image in four
        self.quadTreeDetection(depth + 1, clf, first_gge, first_iic, x, y)
        self.quadTreeDetection(depth + 1, clf, second_gge, second_iic, x + math.floor(rows/2), y)
        self.quadTreeDetection(depth + 1, clf, third_gge, third_iic,  x, y + math.floor(cols/2))
        self.quadTreeDetection(depth + 1, clf, fourth_gge, fourth_iic,  x + math.floor(rows/2), y + math.floor(cols/2))

    
    ''' 
    Train model for further splicing detection
    @param images: the list of images filenames
    @param labels: the list of image labels (0 if pristine, 1 if spliced)
    '''
    def train(self, images, labels, cross_validate = False, extract_features = True):
        # Extract image features from each images in training set
        if extract_features:
            for i in range(len(images)):
                self.extractFeatures(images[i], True, self.verbose)
        
        features = []
        files = os.listdir(config.features_folder)
        for i in files:
            if not i.startswith('.'):
                features.append(np.loadtxt(config.features_folder + i))
        
        features = np.asanyarray(features)
        labels = np.asanyarray(labels)
    
    
        if cross_validate:
            #CROSS VALIDATION 
            scores = []
            loo = LeaveOneOut()
            C_2d_range = np.logspace(-2, 10, 13)
            gamma_2d_range = np.logspace(-9, 3, 13)
            
            #C_2d_range = [1e-2, 1, 1e2]
            #gamma_2d_range = [1e-1, 1, 1e1]
            
            best_index = None
            c_values = []
            gamma_values = []
            index = 0
            for C in C_2d_range:
                for gamma in gamma_2d_range:
                    clf = svm.SVC(C=C, gamma=gamma)
                    predicted = model_selection.cross_val_predict(clf, features, labels, cv=loo)
                    score = accuracy_score(labels, predicted)
                    scores.append(score)
                    c_values.append(C)
                    gamma_values.append(gamma)
                    if best_index != None and scores[best_index] < score:
                        best_index = index
                        print('Best C: ' + str(C))
                        print('Best gamma: ' + str(gamma))
                    elif best_index == None:
                        best_index = index
                        print('Best C: ' + str(C))
                        print('Best gamma: ' + str(gamma)) 
                    index = index + 1
                    print('Current score is ' + str(score))        
            
            print("The best parameters are %s with a score of %0.2f" % ((c_values[best_index], gamma_values[best_index]), scores[best_index]))
        
        loo = LeaveOneOut()
        classifier = svm.SVC()
        predicted = model_selection.cross_val_predict(classifier, features, labels, cv=loo)
        score = accuracy_score(labels, predicted) 
    
        #classifier = svm.SVC(C = 10000.0, gamma = 0.001)
        #classifier.fit(features, labels)
        
        classifier = svm.SVR(kernel = 'rbf', C = 10000.0, gamma = 0.001)
        classifier.fit(features, labels)
        
        print('Classification model created correctly')
        joblib.dump(classifier, config.svm_model)
        
        
    ''' 
    Extracting image features for an image. 
    - Prepare the image for illuminant methods (segmentation)
    - Extracting GGE illuminant map
    - Extracting IIC illulimant map
    - Evaluate principal components for each maps
    - Build feature vectors
    @param img: the path of the image to be processed
    @param verbose: display extended output
    '''
    def extractFeatures(self, img, store = False, verbose = False, heat_map = False):
        #Extract filename from image path
        filename = img.split('/')
        filename = filename[len(filename) - 1]
        filename = filename[:-4]
        print('Processing: ' + filename)
    
        # 1. Extracting GGE and IIC illuminant maps
        print('\t-Segmenting image')
        illuminantMaps.prepareImageIlluminants(img, config.seg_sigma, config.seg_k, config.seg_min_size, config.min_intensity, config.max_intensity, verbose)
            
        # 1.2 Extracting GGE illuminant map
        print('\t-Extracting GGE map')
        illuminantMaps.extractGGEMap(img, filename + "_segmented.png", config.gge_sigma, config.gge_n, config.gge_p, verbose)
        
        # 1.3 Extracting IIC illuminant map
        print('\t-Extracting IIC map')
        illuminantMaps.extractIICMap(img, filename + "_segmented.png", verbose)
        
        # 2. Statistical difference between IIC and GGE maps
        gge_map = cv2.imread(config.maps_folder + filename + '_gge_map.png')
        iic_map = cv2.imread(config.maps_folder + filename + '_iic_map.png')
        
        # 2.1 Building heat map (only for visualizations) 
        if heat_map:    
            self.visualizeHeatMap(gge_map, iic_map)
        
        # 2.2 Extract maps principal components
        print('\t-Extracting PCs')
        gge_pcs = self.extractPrincipalComponents(gge_map)
        iic_pcs = self.extractPrincipalComponents(iic_map)
        
        # 3 Build feature vectors
        print('\t-Building feature vector')
        features = self.buildFeatureVector(gge_pcs, iic_pcs)
        # Store file for future evaluations
        np.savetxt(config.features_folder + filename + '.txt', features, delimiter=',')
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
        cv2.imshow('img', color_map)
        cv2.waitKey(0)
    
    
    ''' 
    PCA analysis on a give map in order to evaluate significant 
    eigenvalues
    '''
    def extractPrincipalComponents(self, X):
        # singular value decomposition of a data matrix such that:  X = U*S*V.T
        # * U and V are the singular matrices
        # * S is a diagonal matrix  
        X = X - np.mean(X, axis = 0)
        [_, s, _] = npl.svd(X, full_matrices = False)
        
        # PCs are already sorted by descending order  of the singular values
        # nedd to extract num values    
        #s = s[:3,:]
        pcs = np.array([], np.float32)
        for i in range (5):
            for j in range(3):
                pcs = np.append(pcs, s[i, j])
        #pcs = np.array([s[0,0], s[0,1], s[0,2],
        #                s[1,0], s[1,1], s[1,2],
        #                s[2,0], s[2,1], s[2,2]], np.float32)
        return pcs
    
    ''' 
    Builds feature vector given two set of principal
    components 
    '''
    def buildFeatureVector(self, gge_pcs, iic_pcs, metric = 'logarithmic'):
        dim = gge_pcs.shape[0]
        if dim != iic_pcs.shape[0]:
            print('Error building feature vector: sizes don\'t match')
        
        features = np.zeros((dim,), dtype=np.float32)
        for i in range(dim):
            if metric == 'linear':
                features[i] = distanceMetrics.linearDistance(gge_pcs[i], iic_pcs[i])
            elif metric == 'quadratic':
                features[i] = distanceMetrics.quadraticDistance(gge_pcs[i], iic_pcs[i])
            elif metric == 'logarithmic':
                features[i] = distanceMetrics.logarithmicDistance(gge_pcs[i], iic_pcs[i])
            elif metric == 'square':
                features[i] = distanceMetrics.squareRootDistance(gge_pcs[i], iic_pcs[i])
            elif metric == 'cubic':
                features[i] = distanceMetrics.cubicRootDistance(gge_pcs[i], iic_pcs[i])
            else:
                features[i] = distanceMetrics.logarithmicDistance(gge_pcs[i], iic_pcs[i])
    
        return features
    
    def resizeImage(self, img, width):
        r = width / img.shape[1]
        dim = (width, int(img.shape[0] * r))   
        # perform the actual resizing of the image and show it
        return cv2.resize(img, dim, interpolation = cv2.INTER_AREA)
