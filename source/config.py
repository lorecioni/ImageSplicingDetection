'''
Created on 05 ott 2016

@author: lorenzocioni
'''


'''General configuration'''

#The name of the dataset to be load
dataset = 'DSO-1'

output_score_file = 'output.txt'

#The path of the images
imagesFolder = '../datasets/' + dataset + '/Images/'
#The path of the images labels
labelsFolder = '../datasets/' + dataset + '/Labels/'
#Output path where will be stored the IMs
maps_folder = '../datasets/' + dataset + '/Maps/'

#Path of the features
features_folder = '../datasets/' + dataset + '/Features/'
#Temp path for saving faces
faces_folder = '../datasets/' + dataset + '/Faces/'
#Temp path for saving descriptors
descriptors_folder = '../datasets/' + dataset + '/Descriptors/'
#Path of the mask (if provided)
masks_folder = '../datasets/' + dataset + '/Masks/'

#Data folder (for application models)
data_folder = 'data/'
#Temporary folder
temp_folder = 'temp/'


''' 
----------------------------
ILLUMINANT MAPS EXTRACTION
----------------------------
'''
voleBinary= 'illuminants/build/./vole'

#Segmentation parameters
max_intensity = 0.98823529411764705882
min_intensity = .05882352941176470588

seg_sigma = 0.2
seg_k = 300
seg_min_size = 15

#Use Riess default parameters for IM extraction
use_riess_default = True

# GGE Map
gge_sigma = 1
gge_n = 1
gge_p = 1

#IIC Map
config_iic = "illuminants/build/config.txt"

#Illuminant maps extraction. If False, if a map is already present, skipt the extraction
forceMapsExtraction = False

### Descriptors ###

#Image converting tool
# ImageMagick http://www.imagemagick.org
convertBinary = '/opt/local/bin/convert'

''' 
----------------------------
FACE DETECTION CONFIGURATION 
----------------------------
'''

faceMinSize = (100, 120)
inverseFacePosition = True
positiveLabel = 'NORMAL'
negativeLabel = 'FAKE'
illuminantTypes = ['GGE', 'IIC'] #Must be an array
descriptors = ['ACC', 'BIC', 'CCV', 'LCH'] #Color descriptors
descriptors_weights = {'ACC': 1, 'BIC': 1, 'CCV': 1, 'LCH': 1}

KNeighbours = 5
crossvalidation = False
folds = 10
label_position = 1
feature_vector_length = 6

#Classification
majorityVotingThreshold = 0.5

#Path of the kNN models
classification_folder = data_folder + 'face_module/DSO-1/'

#Face detector cascade
cascadePath = 'data/face_module/haarcascade_frontalface_default.xml'

faceOutputDetectionImage = 'face_detection_output.png'

''' 
----------------------------
REGION DETECTION CONFIGURATION 
----------------------------
'''

#Segmented band width (for vertical bands)
bandWidth = 120
#Segmented band height (for horizontal bands)
bandHeight = 120
bandDeltaFactor = 4 #Slinding window move is bandWidth/bandDeltaFactor
splicedTolerance = 0.05 # Min area percentage spliced

#fakeThreshold = 2.82 # Map global best value
fakeThreshold = 200

#Training model ('svm' or None)
regionalTrainingType = None
#Reference color used ('global' or 'median')
referenceColorType = 'global'

#The path for the generated dataset
output_spliced_dataset_folder = '../datasets/SplicedDSI1/'

regionOutputDetectionImage = 'regional_detection_output.png'