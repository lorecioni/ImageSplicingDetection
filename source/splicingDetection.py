'''
Created on 03 ott 2016

@author: lorenzocioni
'''

import illuminantMaps

max_intensity = 0.98823529411764705882
min_intensity = .05882352941176470588


def detectSplice(img):
    ''' 1. Extracting GGE and IIC illuminant maps '''
    filename = img.split('/')
    filename = filename[len(filename) - 1]
    filename = filename[:-4]
    
    ''' 1.1 Preparing image for illuminant methods '''
    illuminantMaps.prepareImageIlluminants(img, 0.2, 300, 15, min_intensity, max_intensity)
    
    ''' 1.2 Extracting IIC illuminant map '''
    illuminantMaps.extractIICMap(img, filename + "_segmented.png")
    
    # 2. Statistical difference between IIC and GGE maps
    
    # 2.1 Building heat maps (only for visualizations)
    
    # 2.2 Evaluate statistical difference
    
    # 2.2.1 Use a set of different metrics
    
    # 3 ROI descriptors
    
    # 4 Classifications
    
    return
    
    
    