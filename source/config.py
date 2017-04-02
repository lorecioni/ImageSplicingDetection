'''
Created on 05 ott 2016

@author: lorenzocioni
'''


'''General configuration'''
dataset = 'DSI-1'
#dataset = 'DSI-1'
#dataset = 'COLORCHECKER'
#dataset = 'SPLICED_COLORCHECKER'
# dataset = 'SPLICED_DSO1'

#imagesFolder = '../datasets/SplicedColorChecker/vertical/'
imagesFolder = '../datasets/' + dataset + '/Images/'
#imagesFolder = '../datasets/ColorChecker/srgb8bit/'
#imagesFolder = '../datasets/SplicedDSO1/vertical/'

labelsFolder = '../datasets/' + dataset + '/Labels/'
maps_folder = '../datasets/' + dataset + '/Maps/'
#maps_folder = '../datasets/ColorChecker/maps/'

features_folder = '../datasets/' + dataset + '/Features/'
faces_folder = '../datasets/' + dataset + '/Faces/'
descriptors_folder = '../datasets/' + dataset + '/Descriptors/'
masks_folder = '../datasets/' + dataset + '/Masks/'


data_folder = 'data/'
temp_folder = 'temp/'

classification_folder = data_folder + 'face_module/DSI-1/'

output_spliced_dataset_folder = '../datasets/SplicedDSO1/'

#Convert binaries
convertBinary = '/opt/local/bin/convert'
voleBinary= 'illuminants/build/bin/./vole'

#Face detector cascade
cascadePath = 'data/face_module/haarcascade_frontalface_default.xml'

#Illuminant maps extraction
forceMapsExtraction = False

'''Illuminant maps extraction'''
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
#config_iic = "illuminants/lille/config/config_iebv_lenient.txt"


'''Face splicing detector'''
faceMinSize = (120, 150)
inverseFacePosition = True
positiveLabel = 'NORMAL'
negativeLabel = 'FAKE'
illuminantTypes = ['GGE', 'IIC'] #Must be an array
descriptors = ['ACC', 'BIC', 'CCV', 'LCH'] #Color descriptors
#descriptors_weights = {'ACC': 0.8, 'BIC': 1.2, 'CCV': 0.6, 'LCH': 1.5}
descriptors_weights = {'ACC': 1, 'BIC': 1, 'CCV': 1, 'LCH': 1}

KNeighbours = 5
crossvalidation = False
folds = 10
label_position = 1
feature_vector_length = 6


'''Region splicing detection'''
bandWidth = 120
bandHeight = 120
bandDeltaFactor = 4 #Delta move is bandWidth/bandDeltaFactor

splicedTolerance = 0.05 # Min area percentage spliced

fakeThreshold = 300 # Map global best value
