import sys
import os
from subprocess import *


config = "config.txt"

# Extract IlluminantMaps from all images
# IN: 		scale -- image scale folder
#		configFile -- illuminants config parameters file
# OUT: illuminant maps

def extractIlluminantMaps(sourcefolder, segmentedfolder, outputfolder):
    
    command = "rm ../data-base/IIC/*.png"
    os.system(command)

    im = os.listdir("../data-base/" + str(segmentedfolder) + "/")
    for i in im:
    #try:
            command = "../illuminants/build/bin/./vole liebv --img.image " + "../data-base/" + str(sourcefolder) + "/" + i + " -S " + "../data-base/" + str(segmentedfolder) + "/" + i + " -O ../data-base/" + str(outputfolder) + "/" + i[:-4] + "_fhs.png --iebv_config ../illuminants/build/bin/" + config
            os.system(command)
    #except:
    #print("Erro ao processar imagem ",i,"\n")

extractIlluminantMaps("images","segmented","IIC")

