import os
import random

# Create folds files based on dataset images and five fold cross validation protocol.
# This script assumes that the number of normal and doctored images is the same and multiple of 5
#
# OUT:
#   5 files of folds, each one with y images

def buildFolds():
    folds = 5
    path = "../data-base/images/"
    dirList = os.listdir(path)
    dirList.sort()
    totalNormal = 0
    totalSplicing = 0
    validNormal = []
    validSplicing = []
    for fname in dirList:
        clas = fname.split("-")
        nm = clas[1].split(".")
        clas = clas[0]
        nm = nm[0]
        if (clas == "normal"):
            totalNormal = totalNormal + 1
            validNormal.append(nm)
        elif (clas == "splicing"):
            totalSplicing = totalSplicing + 1
            validSplicing.append(nm)
    imagesFoldsNormal = totalNormal/folds
    cont = 1
    normalFolds = []
    while (cont <= folds):
        normalFolds.append(imagesFoldsNormal)
        cont = cont + 1
    cont = 0
    while (sum(normalFolds) < totalNormal):
        normalFolds[cont] = int(normalFolds[cont]) + 1
        cont = cont + 1
    cont = 1
    contFolds = 1
    contNum = 1
    listNormal = []
    listSplicing = []
    while(contNum <= totalNormal):
        temp = random.randint(0,(totalNormal - 1))
        temp2 = validNormal[temp]
        if (temp2 not in listNormal):
            listNormal.append(temp2)
            contNum = contNum + 1
    contNum = 1
    while(contNum <= totalSplicing):
        temp = random.randint(0,(totalSplicing-1))
        temp2 = validSplicing[temp]
        if (temp2 not in listSplicing):
            listSplicing.append(temp2)
            contNum = contNum + 1
    for i in normalFolds:
        total = i
        cont2 = 1
        name = "../folds/fold-" + str(contFolds) + ".txt"
        files = open(name,"wb")
        files.seek(0)
        while (cont2 <= total):
            name = ""
            if (int(listNormal[cont - 1]) < 10):
                name = "normal-" +  str(listNormal[cont - 1]) + ".txt\n"
            else:
                name = "normal-" +  str(listNormal[cont - 1]) + ".txt\n"
            files.write(name)
            cont2 = cont2 + 1
            cont = cont + 1
        contFolds = contFolds + 1
        files.close()
    imagesFoldsSplicing = totalSplicing/folds
    cont = 1
    splicingFolds = []
    while (cont <= folds):
        splicingFolds.append(imagesFoldsSplicing)
        cont = cont + 1
    cont = 0
    while (sum(splicingFolds) < totalSplicing):
        splicingFolds[cont] = int(splicingFolds[cont]) + 1
        cont += 1
    cont = 1
    contFolds = 1
    for i in splicingFolds:
        total = i
        cont2 = 1
        name = "../folds/fold-" + str(contFolds) + ".txt"
        files = open(name,"rb")
        files.seek(0)
        lines = files.readlines()
        files.close()
        files = open(name,"wb")
        files.seek(0)
        for i in lines:
            files.write(i)
        while (cont2 <= total):
            name = ""
            if (int(listSplicing[cont - 1]) < 10):
                name = "splicing-" +  str(listSplicing[cont - 1]) + ".txt\n"
            else:
                name = "splicing-" +  str(listSplicing[cont - 1]) + ".txt\n"
            files.write(name)
            cont2 = cont2 + 1
            cont = cont + 1
        contFolds = contFolds + 1
        files.close()


def main():
    buildFolds()


if __name__ == "__main__":
    main()