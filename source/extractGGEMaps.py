import os
from subprocess import *

# Extract GrayWorldMaps from all images
# IN: 		scale -- image scale folder
#		sigma -- gray-world parameters
#	            n -- gray-world parameters
#	     	    p -- gray-world parameters
# OUT: gray-world maps

def extractNewGrayWorldMaps(sourcefolder, segmentedfolder, outputfolder, sigma,n,p):
    
    im = os.listdir("../datasets/" + segmentedfolder + "/")
    for i in im:
        try:
            command = "illum_maps/build/./vole lgrayworld --img.image " + "../datasets/" + str(sourcefolder) + "/" + i + " -S " + "../datasets/" + str(segmentedfolder) + "/" + i + " -O ../datasets/" + str(outputfolder) + "/" + i[:-4] + "_fhs.png --n " +  str(n) + " --p " + str(p) + " --sigma " + str(sigma)
            os.system(command)
            print(command)
        except:
            print ("Erro ao processar imagem ")

extractNewGrayWorldMaps("DSO-1","segmented","GGE",1,1,3)
