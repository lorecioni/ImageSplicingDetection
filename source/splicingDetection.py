'''
Created on 03 ott 2016

@author: lorenzocioni
'''
import cv2
import numpy as np
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
    
    ''' 2.1 Building heat maps (only for visualizations) '''
    if heat_map:
        gge_map = cv2.imread('maps/' + filename + '_gge_map.png')
        iic_map = cv2.imread('maps/' + filename + '_iic_map.png')
        visualizeHeatMap(gge_map, iic_map)
    
    ''' 2.2 Evaluate statistical difference '''
    
    ''' 2.2.1 Use a set of different metrics '''
    
    # 3 ROI descriptors
    
    # 4 Classifications
    
    return
    

def visualizeHeatMap(gge, iic):
    #Splits all the channels
    gge_b, gge_g, gge_r = cv2.split(gge)
    iic_b, iic_g, iic_r = cv2.split(iic)
    rows, cols, channels = gge.shape
  
    heat_map = np.zeros(shape=(rows, cols))
    for i in range(rows):
        for j in range(cols):
            heat_map[i, j] = np.sqrt(pow(gge_b[i, j] - iic_b[i, j], 2) + pow(gge_g[i, j] - iic_g[i, j], 2) +  pow(gge_r[i, j] - iic_r[i, j], 2))
            
    heat_map = cv2.applyColorMap(heat_map, cv2.COLORMAP_JET)
    cv2.imshow('img', heat_map)
    cv2.waitKey(0)
    
    