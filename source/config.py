'''
Created on 05 ott 2016

@author: lorenzocioni
'''

dataset = 'DSO-1'

imagesFolder = '../datasets/' + dataset + '/Images/'
labelsFolder = '../datasets/' + dataset + '/Labels/'

#Convert binary
convertBinary = '/opt/local/bin/convert'
voleBinary= 'illuminants/build/bin/./vole'
cascadePath = 'data/haarcascade_frontalface_default.xml'

#Illuminant maps extraction
forceMapsExtraction = False

# Image segmentation
max_intensity = 0.98823529411764705882
min_intensity = .05882352941176470588

seg_sigma = 0.2
seg_k = 300
seg_min_size = 15

use_riess_default = False

# GGE Map
gge_sigma = 1
gge_n = 1
gge_p = 1

#IIC Map
config_iic = "illuminants/build/config.txt"
#config_iic = "illuminants/lille/config/config_iebv_lenient.txt"

#Training
inverseFacePosition = True
positiveLabel = 'NORMAL'
negativeLabel = 'FAKE'
illuminantTypes = ['GGE'] #Must be an array
descriptors = ['ACC']
#descriptors = ['ACC', 'BIC', 'CCV', 'LCH'] #Color descriptors
KNeighbours = 3
crossvalidation = False
folds = 10
label_position = 1
feature_vector_length = 6

# Outputs
maps_folder = '../datasets/' + dataset + '/maps/'
features_folder = '../datasets/' + dataset + '/features/'
faces_folder = '../datasets/' + dataset + '/faces/'
descriptors_folder = '../datasets/' + dataset + '/descriptors/' 
classification_folder = 'data/'

#Region detection
bandWidth = 200
bandHeight = 200

