import os
from subprocess import *

max_intensity=0.98823529411764705882
min_intensity=.05882352941176470588


def segmentAllImagesForIlluminantMethod(database, segmentedDBOutput, sigma,k,min_size, maxintensity, minintensity):
    
    im = os.listdir("../datasets/" + str(database) + "/")
    for i in im:
        try:
            tt = i.split(".")
            newname = i
            if (tt[1] != "png"):
                cmd = "convert ../datasets/" + str(database) + "/" + i + " ../datasets/" + str(database) + "/" + tt[0] + ".png"
                os.system(cmd)
                newname = tt[0] + ".png"
            print(newname)
            command = "illum_maps/build/./vole felzenszwalb -I ../datasets/" + str(database) + "/" + newname + " --deterministic_coloring -O ../datasets/" + str(segmentedDBOutput)+ "/" + newname + " --sigma " + str(sigma) + " --k " + str(k) + " --min_size " + str(min_size) + " --max_intensity " + str(maxintensity) + " --min_intensity " + str(minintensity)
            print(command)
            os.system(command)
        except:
            print("Erro ao processar imagem \n")

segmentAllImagesForIlluminantMethod("DSO-1","segmented",0.2,300,15,max_intensity,min_intensity)
