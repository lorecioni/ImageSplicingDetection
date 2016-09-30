import segmentImage
import extractDescriptor
import os
import time

# Build face's pairs of descriptors
# IN:
#   image -- image name to extract descriptor
#   descriptor -- descriptor name
#	space -- image color space
#   channel -- image color channel where descriptor will be extracted
#   illuminantType -- IIC or GGE
#
# OUT:
#   all possible combination of faces pairs for a given image

def composeFinalFeatureVector(image,descriptor="ACC",space=4,channel=3,illuminantType="GGE"):
    numberFaces = segmentImage.segmentImage(image,illuminantType)
    cont = 1
    print("Number of faces: %d\nCharacterizing faces..." %numberFaces)
    while (cont <= numberFaces):
        faceName = "../data-base/faces/face-" + str(cont) + ".png"
        extractDescriptor.extractDescriptor(faceName,descriptor,space,channel)
        cont = cont + 1
        nameFaces = "../face-positions/" + image[:-4] + ".txt"
    facesFile = open(nameFaces,"rt")
    lines = facesFile.readlines()
    facesFile.close()
    firstFace = 1
    contVectors = 0
    # Inherent loops compose a descriptor for each pair of faces at the image
    allVectors = []
    descriptor = descriptor.lower()
    print("Composing features vectors for faces pairs...")
    while (firstFace < numberFaces):
        secondFace = firstFace + 1
        lineFace1 = lines[(firstFace - 1)].split("\t")
        stateFace1 = lineFace1[1]
        while (secondFace <= numberFaces):
            contVectors = contVectors + 1
            newVector = []
            # Label 1 point a pristine pair and label -1 point a pair that contains at least one fake image
            label = 1
            lineFace2 = lines[(secondFace - 1)].split("\t")
            stateFace2 = lineFace2[1]
            if ((stateFace1 != "NORMAL") or (stateFace2 != "NORMAL")):
                label = -1
            newVector.append(label)
            if (descriptor == "acc"):
                nf1 = "../temp/faces/face-" + str(firstFace) + "-acc-descriptor.txt"
                nf2 = "../temp/faces/face-" + str(secondFace) + "-acc-descriptor.txt"
                files = open(nf1,"rt")
                files.seek(0)
                temp = files.readline()
                linesf1 = files.readlines()
                files.close()
                for i in linesf1:
                    desc = list(i)
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
                files = open(nf2,"rb")
                files.seek(0)
                temp = files.readline()
                linesf2 = files.readlines()
                files.close()
                for i in linesf2:
                    desc = list(i)
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
            elif (descriptor == "bic"):
                nf1 = "../temp/faces/face-" + str(firstFace) + "-bic-descriptor.txt"
                nf2 = "../temp/faces/face-" + str(secondFace) + "-bic-descriptor.txt"
                files = open(nf1,"rb")
                files.seek(0)
                temp = files.readline()
                linesf1 = files.readlines()
                files.close()
                for i in linesf1:
                    desc = list(i)
                    cont = 0
                    totalFeat = len(desc)
                    while (cont < (totalFeat - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
                files = open(nf2,"rt")
                files.seek(0)
                temp = files.readline()
                linesf2 = files.readlines()
                files.close()
                for i in linesf2:
                    desc = list(i)
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
            elif (descriptor == "las"):
                nf1 = "../temp/faces/face-" + str(firstFace) + "-las-descriptor.txt"
                nf2 = "../temp/faces/face-" + str(secondFace) + "-las-descriptor.txt"
                files = open(nf1,"rt")
                files.seek(0)
                temp = files.readline()
                linesf1 = files.readlines()
                files.close()
                for i in linesf1:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
                files = open(nf2,"rt")
                files.seek(0)
                temp = files.readline()
                linesf2 = files.readlines()
                files.close()
                for i in linesf2:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
            elif (descriptor == "sasi"):
                nf1 = "../temp/faces/face-" + str(firstFace) + "-sasi-descriptor.txt"
                nf2 = "../temp/faces/face-" + str(secondFace) + "-sasi-descriptor.txt"
                files = open(nf1,"rt")
                files.seek(0)
                temp = files.readline()
                linesf1 = files.readlines()
                files.close()
                for i in linesf1:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
                files = open(nf2,"rt")
                files.seek(0)
                temp = files.readline()
                linesf2 = files.readlines()
                files.close()
                for i in linesf2:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
            elif (descriptor == "unser"):
                nf1 = "../temp/faces/face-" + str(firstFace) + "-unser-descriptor.txt"
                nf2 = "../temp/faces/face-" + str(secondFace) + "-unser-descriptor.txt"
                files = open(nf1,"rt")
                files.seek(0)
                temp = files.readline()
                linesf1 = files.readlines()
                files.close()
                for i in linesf1:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
                files = open(nf2,"rt")
                files.seek(0)
                temp = files.readline()
                linesf2 = files.readlines()
                files.close()
                for i in linesf2:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
            elif (descriptor == "spytec"):
                nf1 = "../temp/faces/face-" + str(firstFace) + "-spytec-descriptor.txt"
                nf2 = "../temp/faces/face-" + str(secondFace) + "-spytec-descriptor.txt"
                files = open(nf1,"rt")
                files.seek(0)
                temp = files.readline()
                linesf1 = files.readlines()
                files.close()
                for i in linesf1:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
                files = open(nf2,"rt")
                files.seek(0)
                temp = files.readline()
                linesf2 = files.readlines()
                files.close()
                for i in linesf2:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
            elif (descriptor == "ccv"):
                nf1 = "../temp/faces/face-" + str(firstFace) + "-ccv-descriptor.txt"
                nf2 = "../temp/faces/face-" + str(secondFace) + "-ccv-descriptor.txt"
                files = open(nf1,"rt")
                files.seek(0)
                temp = files.readline()
                linesf1 = files.readlines()
                files.close()
                for i in linesf1:
                    desc = i.split("\n")
                    cont = 0
                    newVector.append(float(desc[0]))
                files = open(nf2,"rt")
                files.seek(0)
                temp = files.readline()
                linesf2 = files.readlines()
                files.close()
                for i in linesf2:
                    desc = i.split("\n")
                    cont = 0
                    newVector.append(float(desc[0]))
            elif (descriptor == "lch"):
                nf1 = "../temp/faces/face-" + str(firstFace) + "-lch-descriptor.txt"
                nf2 = "../temp/faces/face-" + str(secondFace) + "-lch-descriptor.txt"
                files = open(nf1,"rt")
                files.seek(0)
                temp = files.readline()
                linesf1 = files.readlines()
                files.close()
                for i in linesf1:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
                files = open(nf2,"rt")
                files.seek(0)
                temp = files.readline()
                linesf2 = files.readlines()
                files.close()
                for i in linesf2:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
            elif (descriptor == "eoac"):
                nf1 = "../temp/faces/face-" + str(firstFace) + "-eoac-descriptor.txt"
                nf2 = "../temp/faces/face-" + str(secondFace) + "-eoac-descriptor.txt"
                files = open(nf1,"rt")
                files.seek(0)
                temp = files.readline()
                linesf1 = files.readlines()
                files.close()
                for i in linesf1:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
                files = open(nf2,"rt")
                files.seek(0)
                temp = files.readline()
                linesf2 = files.readlines()
                files.close()
                for i in linesf2:
                    desc = i.split(" ")
                    cont = 0
                    while (cont < (len(desc) - 1)):
                        newVector.append(float(desc[cont]))
                        cont = cont + 1
            nnf1 = "../temp/faces/" + image[:-4] + "-" + nf1[14:-4] + "-label-" + str(stateFace1) + ".txt"
            command = "cp " + nf1 + " " + nnf1
            os.system(command)
            nnf2 = "../temp/faces/" + image[:-4] + "-" + nf2[14:-4] + "-label-" + str(stateFace2) + ".txt"
            command = "cp " + nf2 + " " + nnf2
            os.system(command)
            allVectors.append(newVector)
            secondFace = secondFace + 1
        firstFace = firstFace + 1
        nameFile = "../temp/vectors/fv-" + image[:-4] + ".txt"
        files = open(nameFile,"wt")
        files.seek(0)
        for i in allVectors:
            temp = i
            cont = 0
            for j in temp:
                if (cont == 0):
                    frase = str(j) + " "
                    files.write(frase)
                else:
                    frase = str(cont) + ":" + str(j) + " "
                    files.write(frase)
                cont = cont + 1
            files.write("\n")
        files.close()
    return numberFaces





#composeFinalFeatureVector("splicing-01.png")
