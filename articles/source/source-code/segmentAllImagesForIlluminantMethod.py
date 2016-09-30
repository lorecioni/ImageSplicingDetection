import os
from subprocess import *

max_intensity=0.98823529411764705882
min_intensity=.05882352941176470588


def segmentAllImagesForIlluminantMethod(database, segmentedDBOutput, sigma,k,min_size, maxintensity, minintensity):
    
    command = "rm ../data-base/segmented/*.png"
    os.system(command)

    im = os.listdir("../data-base/" + str(database) + "/")
    for i in im:
        try:
            tt = i.split(".")
            newname = i
            if (tt[1] != "png"):
                cmd = "convert ../data-base/" + str(database) + "/" + i + " ../data-base/" + str(database) + "/" + tt[0] + ".png"
                os.system(cmd)
                newname = tt[0] + ".png"
            print(newname)
            #command = "../illuminants/build/bin/./vole felzenszwalb -I ../data-base/" + str(database) + "/" + newname + " --deterministic_coloring -O ../data-base/" + str(segmentedDBOutput)+ "/" + newname + " --sigma " + str(sigma) + " --k " + str(k) + " --min_size " + str(min_size) + " --max_intensity " + str(maxintensity) + " --min_intensity " + str(minintensity)
            command = "../illuminants/build/./vole felzenszwalb -I ../data-base/" + str(database) + "/" + newname + " --deterministic_coloring -O ../data-base/" + str(segmentedDBOutput)+ "/" + newname + " --sigma " + str(sigma) + " --k " + str(k) + " --min_size " + str(min_size) + " --max_intensity " + str(maxintensity) + " --min_intensity " + str(minintensity)
            print(command)
            os.system(command)
        except:
            print("Erro ao processar imagem ",targetImage,"\n")





segmentAllImagesForIlluminantMethod("images","segmented",0.2,300,15,max_intensity,min_intensity)
