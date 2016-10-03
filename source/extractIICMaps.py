import sys
import os
from subprocess import *


config = "config.txt"

# Extract IlluminantMaps from all images
# IN: 		scale -- image scale folder
#		configFile -- illuminants config parameters file
# OUT: illuminant maps

def extractIlluminantMaps(sourcefolder, segmentedfolder, outputfolder):
    im = os.listdir("../datasets/" + str(segmentedfolder) + "/")
    for i in im:
    #try:
            command = "illum_maps/build/./vole liebv --img.image " + "../datasets/" + str(sourcefolder) + "/" + i + " -S " + "../datasets/" + str(segmentedfolder) + "/" + i + " -O ../datasets/" + str(outputfolder) + "/" + i[:-4] + "_fhs.png --iebv_config illum_maps/build/" + config
            print(command)
            os.system(command)
    #except:
    #print("Erro ao processar imagem ",i,"\n")

extractIlluminantMaps("DSO-1","segmented","IIC")

