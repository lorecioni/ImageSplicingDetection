import os
from subprocess import *

# Extract GrayWorldMaps from all images
# IN: 		scale -- image scale folder
#		sigma -- gray-world parameters
#	            n -- gray-world parameters
#	     	    p -- gray-world parameters
# OUT: gray-world maps

def extractNewGrayWorldMaps(sourcefolder, segmentedfolder, outputfolder, sigma,n,p):
    
    command = "rm ../data-base/GGE/*.png"
    os.system(command)
        
    im = os.listdir("../data-base/" + segmentedfolder + "/")
    for i in im:
        try:
            command = "../illuminants/build/bin/./vole lgrayworld --img.image " + "../data-base/" + str(sourcefolder) + "/" + i + " -S " + "../data-base/" + str(segmentedfolder) + "/" + i + " -O ../data-base/" + str(outputfolder) + "/" + i[:-4] + "_fhs.png --n " +  str(n) + " --p " + str(p) + " --sigma " + str(sigma)
            os.system(command)
        except:
            print "Erro ao processar imagem ",i,"\n"

extractNewGrayWorldMaps("images","segmented","GGE",1,1,3)
