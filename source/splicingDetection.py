'''
Created on 03 ott 2016

@author: lorenzocioni
'''
import cv2
import numpy as np
import numpy.linalg as npl
from sklearn import svm
import os
import illuminantMaps
import distanceMetrics


''' 
Splicing detection main procedure. The result of the output
will be a number of regions of the image that can be spliced
over an image
'''
def detectSplice(img, heat_map, verbose):
    # Extracting image features
    extractFeatures(img, verbose, heat_map)
    
    # Classification
    
    # TODO ...
    
    return


''' 
Train model for further splicing detection
@param images: the list of images filenames
@param labels: the list of image labels (0 if pristine, 1 if spliced)
'''
def train(images, labels, extract_features = True, verbose = False):
    # Extract image features from each images in training set
    if extract_features:
        for i in range(len(images)):
            extractFeatures(images[i], verbose)
    
    features = []
    files = os.listdir("features/")
    for i in files:
        if not i.startswith('.'):
            print(i)

    
    classifier = svm.SVC()
    #classifier.fit(X, y, sample_weight)
    
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
def extractFeatures(img, verbose = False, heat_map = False):
    #Extract filename from image path
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    filename = filename[:-4]
    print('Processing: ' + filename)

    #Configuration //TODO move from here
    max_intensity = 0.98823529411764705882
    min_intensity = .05882352941176470588
    sigma = 0.2
    k = 300
    min_size = 15
    # 1. Extracting GGE and IIC illuminant maps
    illuminantMaps.prepareImageIlluminants(img, sigma, k, min_size, min_intensity, max_intensity, verbose)
        
    sigma = 1
    n = 1
    p = 3
    # 1.2 Extracting GGE illuminant map
    illuminantMaps.extractGGEMap(img, filename + "_segmented.png", 1, 1, 3, verbose)
    # 1.3 Extracting IIC illuminant map
    illuminantMaps.extractIICMap(img, filename + "_segmented.png", verbose)
    
    # 2. Statistical difference between IIC and GGE maps
    gge_map = cv2.imread('maps/' + filename + '_gge_map.png')
    iic_map = cv2.imread('maps/' + filename + '_iic_map.png')
    
    # 2.1 Building heat map (only for visualizations) 
    if heat_map:    
        visualizeHeatMap(gge_map, iic_map)
    
    # 2.2 Extract maps principal components
    gge_pcs = extractPrincipalComponents(gge_map)
    iic_pcs = extractPrincipalComponents(iic_map)
    
    # 3 Build feature vectors
    features = buildFeatureVector(gge_pcs, iic_pcs)
    # Store file for future evaluations
    np.savetxt('features/' + filename + '.txt', features, delimiter=',')
    

''' 
Builds and visualize the heat map in order to visually evaluate difference
between two different maps. Using OpenCV COLORMAP_JET, red values indicates
a more significant difference, blue values indicates lower difference.
'''
def visualizeHeatMap(gge, iic):
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
def extractPrincipalComponents(X):
    # singular value decomposition of a data matrix such that:  X = U*S*V.T
    # * U and V are the singular matrices
    # * S is a diagonal matrix  
    X = X - np.mean(X, axis = 0)
    [_, s, _] = npl.svd(X, full_matrices = False)
    
    # PCs are already sorted by descending order  of the singular values
    # nedd to extract num values    
    s = s[:3,:]
 
    pcs = np.array([s[0,0], s[0,1], s[0,2],
                    s[1,0], s[1,1], s[1,2],
                    s[2,0], s[2,1], s[2,2]], np.float32)
    return pcs

''' 
Builds feature vector given two set of principal
components 
'''
def buildFeatureVector(gge_pcs, iic_pcs, metric = 'logarithmic'):
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

