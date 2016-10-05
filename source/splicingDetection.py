'''
Created on 03 ott 2016

@author: lorenzocioni
'''
import cv2
import numpy as np
import numpy.linalg as npl
import scipy.spatial
import illuminantMaps

max_intensity = 0.98823529411764705882
min_intensity = .05882352941176470588


def detectSplice(img, heat_map, verbose):
    ''' 1. Extracting GGE and IIC illuminant maps '''
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    filename = filename[:-4]
    
    ''' 1.1 Preparing image for illuminant methods '''
    #illuminantMaps.prepareImageIlluminants(img, 0.2, 300, 15, min_intensity, max_intensity, verbose)
    
    ''' 1.2 Extracting GGE illuminant map '''
    #illuminantMaps.extractGGEMap(img, filename + "_segmented.png", 1, 1, 3, verbose)

    ''' 1.3 Extracting IIC illuminant map '''    
    #illuminantMaps.extractIICMap(img, filename + "_segmented.png", verbose)
    
    ''' 2. Statistical difference between IIC and GGE maps '''
    gge_map = cv2.imread('maps/' + filename + '_gge_map.png')
    iic_map = cv2.imread('maps/' + filename + '_iic_map.png')
    
    ''' 2.1 Building heat map (only for visualizations) '''
    if heat_map:    
        visualizeHeatMap(gge_map, iic_map)
    
    ''' 2.2 Evaluate statistical difference '''
    #eigs = npl.eigvals(gge_map)
    #print(eigs)
    
    ''' 2.2.1 Use a set of different metrics '''
    
    # 3 ROI descriptors
    
    # 4 Classifications
    
    return
    

def visualizeHeatMap(gge, iic):
    #Splits all the channels
    gge_b, gge_g, gge_r = cv2.split(gge)
    iic_b, iic_g, iic_r = cv2.split(iic)
    rows, cols, _ = gge.shape
    heat_map = np.sqrt(pow(gge_b[0:rows-1, 0:cols-1] - iic_b[0:rows-1, 0:cols-1], 2) + pow(gge_g[0:rows-1, 0:cols-1] - iic_g[0:rows-1, 0:cols-1], 2) +  pow(gge_r[0:rows-1, 0:cols-1] - iic_r[0:rows-1, 0:cols-1], 2))
    max_value = np.ndarray.max(heat_map)
    heat_map = heat_map / max_value    
    heat_map = cv2.applyColorMap(heat_map, cv2.COLORMAP_JET)
    cv2.imshow('img', heat_map)
    cv2.waitKey(0)
    
    