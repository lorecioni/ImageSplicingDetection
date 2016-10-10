'''
Created on 05 ott 2016

@author: lorenzocioni
'''

#Implementing method for recovering values from file

# Image segmentation
max_intensity = 0.98823529411764705882
min_intensity = .05882352941176470588
seg_sigma = 0.2
seg_k = 300
seg_min_size = 15

# GGE Map
gge_sigma = 1
gge_n = 1
gge_p = 3

# Outputs
maps_folder = 'maps/'
features_folder = 'features/'
svm_model = 'data/model.pkl'