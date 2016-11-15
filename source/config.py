'''
Created on 05 ott 2016

@author: lorenzocioni
'''

#dataset = 'DSO-1'
dataset = 'COLUMBIA'

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

#IIC Map
config_iic = "illuminants/build/config.txt"
#config_iic = "illuminants/lille/config/config_iebv_lenient.txt"

#Training
feature_vector_length = 8

# Outputs
maps_folder = '../datasets/' + dataset + '/maps/'
features_folder = '../datasets/' + dataset + '/features/'
svm_model = 'data/model.pkl'
maps_out_suffix = ''