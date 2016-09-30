import os
import cv2
from subprocess import *

# Crop faces from a given image
# IN:
#   fileName -- image name to crop faces
#   illuminantType -- IIC or GGE
# OUT:
#   the numbers of segmented faces in te image

def segmentImage(fileName,illuminantType):
    nameFaces = "../face-positions/" + fileName[:-4] + ".txt"
    facesFile = open(nameFaces,"rt")
    lines = facesFile.readlines()
    facesFile.close()
    cont = 1
    numberOfFaces = 0
    print("Segmenting Faces...")
    for i in lines:
        temp = i.split("\t")
        if (len(temp) != 1):
            topleftx = int(temp[2])
            toplefty = int(temp[4])
            bottomrightx = int(temp[3])
            bottomrighty = int(temp[5])
            box = (topleftx,toplefty,bottomrightx,bottomrighty)
            pathImage = "../data-base/" + illuminantType + "/" + fileName[:-4] + "_fhs.png"
            im = cv2.imread(pathImage)
            region = im[toplefty:bottomrighty,topleftx:bottomrightx]
            nameFace = "../data-base/faces/face-" + str(cont) + ".png"
            cv2.imwrite(nameFace,region)
            numberOfFaces = cont
            cont = cont + 1
    return numberOfFaces
