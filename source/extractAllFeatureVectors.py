import os
import sys
import composeFinalFeatureVector
from subprocess import *
import getSpaceChannelName as sc



#channel values are associated like that: first channel is higher, middle channel is midle value, last channel is smaller( Ex. R = 2, G = 1, B = 0)
#space 0 HSV
#space 1 RGB
#space 2 Ycbcr
#space 4 Lab

def extractAllFeatureVectors(descriptor,space,channel):
    path = "../data-base/images/"
    dirList = os.listdir(path)
    dirList.sort()
    nameSpace,nameChannel = sc.getSpaceChannelName(space,channel)
    tt = descriptor.upper()
    npath = "../extracted-feature-vectors/" + tt + "-" + "IIC-" + nameSpace + "-" + nameChannel
    if not os.path.exists(npath):
        os.makedirs(npath)

    print("\n..........................Processing IIC Maps..........................\n")
    for fname in dirList:
        try:
            img = fname[:-3] + "png"
            print("\nExtracting feature vector from image ",img)
            faces = composeFinalFeatureVector.composeFinalFeatureVector(img,descriptor,space,channel,"IIC")
            name = "../temp/vectors/fv-" + img[:-3] + "txt"
            command = "cp " + name + " " + npath + "/"
            os.system(command)
            command = "rm ../data-base/faces/*.ppm"
            os.system(command)
            command = "rm ../temp/faces/*"
            os.system(command)
            command = "rm ../temp/vectors/*"
            os.system(command)
        except:
            print("Erro to process image ", fname)



    npath = "../extracted-feature-vectors/" + tt + "-" + "GGE-" + nameSpace + "-" + nameChannel
    if not os.path.exists(npath):
        os.makedirs(npath)

    print("\n..........................Processing GGE Maps..........................\n")
    for fname in dirList:
        try:
            img = fname[:-3] + "png"
            print("\nExtracting feature vectors from image ",img)
            faces = composeFinalFeatureVector.composeFinalFeatureVector(img,descriptor,space,channel,"GGE")
            name = "../temp/vectors/fv-" + img[:-3] + "txt"
            command = "cp " + name + " " + npath + "/"
            os.system(command)
            command = "rm ../data-base/faces/*.ppm"
            os.system(command)
            command = "rm ../temp/faces/*"
            os.system(command)
            command = "rm ../temp/vectors/*"
            os.system(command)
        except:
            print("Erro to process image ", fname)


def main():
    if (len(sys.argv) != 2):
        print("Number of parameters invalid! extractAllFeatureVectors.py <string with descriptors names separed by space>")
    else:
        desc = sys.argv[1].split(" ")
        for i in desc:
            try:
                tt = i.upper()
                if (tt == "ACC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing ACC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    extractAllFeatureVectors("acc",4,3)
                    extractAllFeatureVectors("acc",1,3)
                    extractAllFeatureVectors("acc",2,3)
                elif (tt == "BIC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing BIC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    extractAllFeatureVectors("bic",4,3)
                    extractAllFeatureVectors("bic",1,3)
                    extractAllFeatureVectors("bic",2,3)
                elif (tt == "CCV"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing CCV Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    extractAllFeatureVectors("ccv",4,3)
                    extractAllFeatureVectors("ccv",1,3)
                    extractAllFeatureVectors("ccv",2,3)
                elif (tt == "LCH"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing LCH Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    extractAllFeatureVectors("lch",4,3)
                    extractAllFeatureVectors("lch",1,3)
                    extractAllFeatureVectors("lch",2,3)
                elif (tt == "SASI"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing SASI Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    extractAllFeatureVectors("sasi",4,2)
                    extractAllFeatureVectors("sasi",0,0)
                    extractAllFeatureVectors("sasi",2,2)
                elif (tt == "LAS"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing LAS Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    extractAllFeatureVectors("las",4,2)
                    extractAllFeatureVectors("las",0,0)
                    extractAllFeatureVectors("las",2,2)
                elif (tt == "UNSER"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing UNSER Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    extractAllFeatureVectors("unser",4,2)
                    extractAllFeatureVectors("unser",0,0)
                    extractAllFeatureVectors("unser",2,2)
                elif (tt == "EOAC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing EOAC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    extractAllFeatureVectors("eoac",4,2)
                    extractAllFeatureVectors("eoac",0,0)
                    extractAllFeatureVectors("eoac",2,2)
                elif (tt == "SPYTEC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing SPYTEC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    extractAllFeatureVectors("spytec",4,2)
                    extractAllFeatureVectors("spytec",0,0)
                    extractAllFeatureVectors("spytec",2,2)
                else:
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>> Image Descriptor %s not available! <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n " %tt)
            except ValueError:
                print("Image Descriptor needs to be a valid string")



if __name__ == "__main__":
    main()