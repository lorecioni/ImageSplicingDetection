'''
Created on 03 ott 2016

@author: lorenzocioni
'''
import cv2
import numpy as np
import numpy.linalg as npl
import illuminantMaps

def detectSplice(img, heat_map, verbose):
    ''' 1. Extracting GGE and IIC illuminant maps '''
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    filename = filename[:-4]
    
    ''' 1.1 Preparing image for illuminant methods '''
    #Felzenszwalb algorithm parameters
    max_intensity = 0.98823529411764705882
    min_intensity = .05882352941176470588
    sigma = 0.2
    k = 300
    min_size = 15
    #illuminantMaps.prepareImageIlluminants(img, sigma, k, min_size, min_intensity, max_intensity, verbose)
    
    ''' 1.2 Extracting GGE illuminant map '''
    #GGE algorithm parameters
    sigma = 1
    n = 1
    p = 3
    #illuminantMaps.extractGGEMap(img, filename + "_segmented.png", 1, 1, 3, verbose)

    ''' 1.3 Extracting IIC illuminant map '''    
    #illuminantMaps.extractIICMap(img, filename + "_segmented.png", verbose)
    
    ''' 2. Statistical difference between IIC and GGE maps '''
    gge_map = cv2.imread('maps/' + filename + '_gge_map.png')
    iic_map = cv2.imread('maps/' + filename + '_iic_map.png')
    
    ''' 2.1 Building heat map (only for visualizations) '''
    if heat_map:    
        visualizeHeatMap(gge_map, iic_map)
    
    ''' 2.2 Extract maps principal components '''
    gge_pcs = extractPrincipalComponents(gge_map)
    iic_pcs = extractPrincipalComponents(iic_map)
    
    ''' 3 Build feature vector '''
    
    # 3 ROI descriptors
    
    # 4 Classifications
    
    return

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
    print(heat_map)   
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
                    s[2,0], s[2,1], s[2,2]], np.float64)
    return pcs
