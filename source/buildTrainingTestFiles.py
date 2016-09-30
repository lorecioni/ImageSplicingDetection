import os
import sys
import getSpaceChannelName as sc

# Create training, validation and test files based on five fold cross-validation protocol
# IN:
#   descriptor -- string representing descriptor name (ACC, BIC, CCV, EOAC, LAS, LCH, SASI, SPYTEC, UNSER)
#   illuminantType -- IIC or GGE
# OUT:
#   save in folder "../training-test-files/descriptor/illuminantType" training, validation and test files

def buildTrainingTestFiles(dataset, descriptor,space, channel,trainingFolds=[1,2,3,4,5],validationFolds=[],testFolds=[]):
    cont = 1
    nameSpace,nameChannel = sc.getSpaceChannelName(space,channel)
    tt = descriptor.upper()

    # Creating Training Set IIC
    npath = "../training-test-files/" + tt + "-" + "IIC-" + nameSpace + "-" + nameChannel
    if not os.path.exists(npath):
        os.makedirs(npath)


    if (len(trainingFolds) > 0) and (trainingFolds[0] != ''):
        fd = ""
        for i in trainingFolds:
            fd = fd+str(i)+"-"
        fd = fd[:-1]
        outfile = "../training-test-files/" + tt + "-" + "IIC-" + nameSpace + "-" + nameChannel + "/" + dataset + "-training-folds-" + fd
        ofid = open(outfile,'wt')
        ofid.seek(0)
        outfilesvm = "../training-test-files/" + tt + "-" + "IIC-" + nameSpace + "-" + nameChannel + "/" + dataset + "-svm-training-folds-" + fd
        ofidsvm = open(outfilesvm,'wt')
        ofidsvm.seek(0)
        for i in trainingFolds:
            name = "../folds/fold-" + str(i) + ".txt"
            actualFile = open(name,"rt")
            actualFile.seek(0)
            lines = actualFile.readlines()
            actualFile.close()
            for j in lines:
                temp = j.split("\n")
                temp = temp[0]
                name = "../extracted-feature-vectors/" + tt + "-" + "IIC-" + nameSpace + "-" + nameChannel + "/fv-" + j[:-1]
                print(name)
                actualFile = open(name,"rt")
                linesDesc = actualFile.readlines()
                actualFile.close()
                for j in linesDesc:
                    ofid.write("%s %s \n" %(j[:-2],temp))
                    ofidsvm.write("%s" %j)
        ofid.close()
        ofidsvm.close()

    # Creating Validation Set IIC
    fd = ""
    if (len(validationFolds) > 0) and (validationFolds[0] != ''):
        for i in validationFolds:
            fd = fd+str(i)+"-"
        fd = fd[:-1]
        outfile = "../training-test-files/" + tt + "-" + "IIC-" + nameSpace + "-" + nameChannel + "/" + dataset + "-validation-folds-" + fd
        ofid = open(outfile,'wt')
        ofid.seek(0)
        outfilesvm = "../training-test-files/" + tt + "-" + "IIC-" + nameSpace + "-" + nameChannel + "/" + dataset + "-svm-validation-folds-" + fd
        ofidsvm = open(outfilesvm,'wt')
        ofidsvm.seek(0)
        for i in validationFolds:
            name = "../folds/fold-" + str(i) + ".txt"
            actualFile = open(name,"rt")
            actualFile.seek(0)
            lines = actualFile.readlines()
            actualFile.close()
            for j in lines:
                temp = j.split("\n")
                temp = temp[0]
                name = "../extracted-feature-vectors/" + tt + "-" + "IIC-" + nameSpace + "-" + nameChannel + "/fv-" + j[:-1]
                print(name)
                actualFile = open(name,"rt")
                linesDesc = actualFile.readlines()
                actualFile.close()
                for j in linesDesc:
                    ofid.write("%s %s \n" %(j[:-2],temp))
                    ofidsvm.write("%s" %j)
        ofid.close()
        ofidsvm.close()

    #Creating Test Set IIC
    fd = ""
    if (len(testFolds) > 0) and (testFolds[0] != ''):
        for i in testFolds:
            fd = fd+str(i)+"-"
        fd = fd[:-1]
        outfile = "../training-test-files/" + tt + "-" + "IIC-" + nameSpace + "-" + nameChannel + "/" + dataset + "-test-folds-" + fd
        ofid = open(outfile,'wt')
        ofid.seek(0)
        outfilesvm = "../training-test-files/" + tt + "-" + "IIC-" + nameSpace + "-" + nameChannel + "/" + dataset + "-svm-test-folds-" + fd
        ofidsvm = open(outfilesvm,'wt')
        ofidsvm.seek(0)
        for i in testFolds:
            name = "../folds/fold-" + str(i) + ".txt"
            actualFile = open(name,"rt")
            actualFile.seek(0)
            lines = actualFile.readlines()
            actualFile.close()
            for j in lines:
                temp = j.split("\n")
                temp = temp[0]
                name = "../extracted-feature-vectors/" + tt + "-" + "IIC-" + nameSpace + "-" + nameChannel + "/fv-" + j[:-1]
                print(name)
                actualFile = open(name,"rt")
                linesDesc = actualFile.readlines()
                actualFile.close()
                for j in linesDesc:
                    ofid.write("%s %s \n" %(j[:-2],temp))
                    ofidsvm.write("%s" %j)
        ofid.close()
        ofidsvm.close()


    # Creating Training Set GGE
    npath = "../training-test-files/" + tt + "-" + "GGE-" + nameSpace + "-" + nameChannel
    if not os.path.exists(npath):
        os.makedirs(npath)
    fd = ""
    if (len(trainingFolds) > 0) and (trainingFolds[0] != ''):
        for i in trainingFolds:
            fd = fd+str(i)+"-"
        fd = fd[:-1]
        outfile = "../training-test-files/" + tt + "-" + "GGE-" + nameSpace + "-" + nameChannel + "/" + dataset + "-training-folds-" + fd
        ofid = open(outfile,'wt')
        ofid.seek(0)
        outfilesvm = "../training-test-files/" + tt + "-" + "GGE-" + nameSpace + "-" + nameChannel + "/" + dataset + "-svm-training-folds-" + fd
        ofidsvm = open(outfilesvm,'wt')
        ofidsvm.seek(0)
        for i in trainingFolds:
            name = "../folds/fold-" + str(i) + ".txt"
            actualFile = open(name,"rt")
            actualFile.seek(0)
            lines = actualFile.readlines()
            actualFile.close()
            for j in lines:
                temp = j.split("\n")
                temp = temp[0]
                name = "../extracted-feature-vectors/" + tt + "-" + "GGE-" + nameSpace + "-" + nameChannel + "/fv-" + j[:-1]
                print(name)
                actualFile = open(name,"rt")
                linesDesc = actualFile.readlines()
                actualFile.close()
                for j in linesDesc:
                    ofid.write("%s %s \n" %(j[:-2],temp))
                    ofidsvm.write("%s" %j)
        ofid.close()
        ofidsvm.close()

    # Creating Validation Set GGE
    fd = ""
    if (len(validationFolds) > 0) and (validationFolds[0] != ''):
        for i in validationFolds:
            fd = fd+str(i)+"-"
        fd = fd[:-1]
        outfile = "../training-test-files/" + tt + "-" + "GGE-" + nameSpace + "-" + nameChannel + "/" + dataset + "-validation-folds-" + fd
        ofid = open(outfile,'wt')
        ofid.seek(0)
        outfilesvm = "../training-test-files/" + tt + "-" + "GGE-" + nameSpace + "-" + nameChannel + "/" + dataset + "-svm-validation-folds-" + fd
        ofidsvm = open(outfilesvm,'wt')
        ofidsvm.seek(0)
        for i in validationFolds:
            name = "../folds/fold-" + str(i) + ".txt"
            actualFile = open(name,"rt")
            actualFile.seek(0)
            lines = actualFile.readlines()
            actualFile.close()
            for j in lines:
                temp = j.split("\n")
                temp = temp[0]
                name = "../extracted-feature-vectors/" + tt + "-" + "GGE-" + nameSpace + "-" + nameChannel + "/fv-" + j[:-1]
                print(name)
                actualFile = open(name,"rt")
                linesDesc = actualFile.readlines()
                actualFile.close()
                for j in linesDesc:
                    ofid.write("%s %s \n" %(j[:-2],temp))
                    ofidsvm.write("%s" %j)
        ofid.close()
        ofidsvm.close()

    # Creating Test Set GGE
    fd = ""
    if (len(testFolds) > 0) and (testFolds[0] != ''):
        for i in testFolds:
            fd = fd+str(i)+"-"
        fd = fd[:-1]
        outfile = "../training-test-files/" + tt + "-" + "GGE-" + nameSpace + "-" + nameChannel + "/" + dataset + "-test-folds-" + fd
        ofid = open(outfile,'wt')
        ofid.seek(0)
        outfilesvm = "../training-test-files/" + tt + "-" + "GGE-" + nameSpace + "-" + nameChannel + "/" + dataset + "-svm-test-folds-" + fd
        ofidsvm = open(outfilesvm,'wt')
        ofidsvm.seek(0)
        for i in testFolds:
            name = "../folds/fold-" + str(i) + ".txt"
            actualFile = open(name,"rt")
            actualFile.seek(0)
            lines = actualFile.readlines()
            actualFile.close()
            for j in lines:
                temp = j.split("\n")
                temp = temp[0]
                name = "../extracted-feature-vectors/" + tt + "-" + "GGE-" + nameSpace + "-" + nameChannel + "/fv-" + j[:-1]
                print(name)
                actualFile = open(name,"rt")
                linesDesc = actualFile.readlines()
                actualFile.close()
                for j in linesDesc:
                    ofid.write("%s %s \n" %(j[:-2],temp))
                    ofidsvm.write("%s" %j)
        ofid.close()
        ofidsvm.close()

def main():
    if len(sys.argv) < 3:
        print("Provide correct number of arguments - buildTrainingTestFiles.py <dataset> <descriptorName> <training set - default \"1,2,3,4,5\"> <validation set - default\"\"> <test set - default\"\">")
        return
    else:
        try:
            tt = sys.argv[2].upper()
            dtst = sys.argv[1]
            if (len(sys.argv) == 3):
                if(tt == "ACC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing ACC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"acc",4,3)
                    buildTrainingTestFiles(dtst,"acc",1,3)
                    buildTrainingTestFiles(dtst,"acc",2,3)
                elif(tt == "BIC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing BIC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"bic",4,3)
                    buildTrainingTestFiles(dtst,"bic",1,3)
                    buildTrainingTestFiles(dtst,"bic",2,3)
                elif(tt == "CCV"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing CCV Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"ccv",4,3)
                    buildTrainingTestFiles(dtst,"ccv",1,3)
                    buildTrainingTestFiles(dtst,"ccv",2,3)
                elif(tt == "LCH"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing LCH Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"lch",4,3)
                    buildTrainingTestFiles(dtst,"lch",1,3)
                    buildTrainingTestFiles(dtst,"lch",2,3)
                elif(tt == "SASI"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing SASI Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"sasi",4,2)
                    buildTrainingTestFiles(dtst,"sasi",0,0)
                    buildTrainingTestFiles(dtst,"sasi",2,2)
                elif(tt == "LAS"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing LAS Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"las",4,2)
                    buildTrainingTestFiles(dtst,"las",0,0)
                    buildTrainingTestFiles(dtst,"las",2,2)
                elif(tt == "UNSER"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing UNSER Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"unser",4,2)
                    buildTrainingTestFiles(dtst,"unser",0,0)
                    buildTrainingTestFiles(dtst,"unser",2,2)
                elif(tt == "EOAC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing EOAC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"eoac",4,2)
                    buildTrainingTestFiles(dtst,"eoac",0,0)
                    buildTrainingTestFiles(dtst,"eoac",2,2)
                elif(tt == "SPYTEC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing SPYTEC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"spytec",4,2)
                    buildTrainingTestFiles(dtst,"spytec",0,0)
                    buildTrainingTestFiles(dtst,"spytec",2,2)
                else:
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>> Image Descriptor %s not available! <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n " %tt)
            elif(len(sys.argv) == 4):
                tmp = sys.argv[3].split(",")
                if(tt == "ACC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing ACC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"acc",4,3,tmp)
                    buildTrainingTestFiles(dtst,"acc",1,3,tmp)
                    buildTrainingTestFiles(dtst,"acc",2,3,tmp)
                elif(tt == "BIC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing BIC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"bic",4,3,tmp)
                    buildTrainingTestFiles(dtst,"bic",1,3,tmp)
                    buildTrainingTestFiles(dtst,"bic",2,3,tmp)
                elif(tt == "CCV"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing CCV Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"ccv",4,3,tmp)
                    buildTrainingTestFiles(dtst,"ccv",1,3,tmp)
                    buildTrainingTestFiles(dtst,"ccv",2,3,tmp)
                elif(tt == "LCH"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing LCH Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"lch",4,3,tmp)
                    buildTrainingTestFiles(dtst,"lch",1,3,tmp)
                    buildTrainingTestFiles(dtst,"lch",2,3,tmp)
                elif(tt == "SASI"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing SASI Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"sasi",4,2,tmp)
                    buildTrainingTestFiles(dtst,"sasi",0,0,tmp)
                    buildTrainingTestFiles(dtst,"sasi",2,2,tmp)
                elif(tt == "LAS"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing LAS Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"las",4,2,tmp)
                    buildTrainingTestFiles(dtst,"las",0,0,tmp)
                    buildTrainingTestFiles(dtst,"las",2,2,tmp)
                elif(tt == "UNSER"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing UNSER Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"unser",4,2,tmp)
                    buildTrainingTestFiles(dtst,"unser",0,0,tmp)
                    buildTrainingTestFiles(dtst,"unser",2,2,tmp)
                elif(tt == "EOAC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing EOAC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"eoac",4,2,tmp)
                    buildTrainingTestFiles(dtst,"eoac",0,0,tmp)
                    buildTrainingTestFiles(dtst,"eoac",2,2,tmp)
                elif(tt == "SPYTEC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing SPYTEC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"spytec",4,2,tmp)
                    buildTrainingTestFiles(dtst,"spytec",0,0,tmp)
                    buildTrainingTestFiles(dtst,"spytec",2,2,tmp)
                else:
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>> Image Descriptor %s not available! <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n " %tt)
            elif (len(sys.argv) == 5):
                tmp = sys.argv[3].split(",")
                tmp2 = sys.argv[4].split(",")
                if(tt == "ACC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing ACC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"acc",4,3,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"acc",1,3,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"acc",2,3,tmp,tmp2)
                elif(tt == "BIC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing BIC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"bic",4,3,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"bic",1,3,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"bic",2,3,tmp,tmp2)
                elif(tt == "CCV"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing CCV Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"ccv",4,3,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"ccv",1,3,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"ccv",2,3,tmp,tmp2)
                elif(tt == "LCH"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing LCH Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"lch",4,3,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"lch",1,3,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"lch",2,3,tmp,tmp2)
                elif(tt == "SASI"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing SASI Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"sasi",4,2,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"sasi",0,0,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"sasi",2,2,tmp,tmp2)
                elif(tt == "LAS"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing LAS Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"las",4,2,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"las",0,0,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"las",2,2,tmp,tmp2)
                elif(tt == "UNSER"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing UNSER Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"unser",4,2,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"unser",0,0,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"unser",2,2,tmp,tmp2)
                elif(tt == "EOAC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing EOAC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"eoac",4,2,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"eoac",0,0,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"eoac",2,2,tmp,tmp2)
                elif(tt == "SPYTEC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing SPYTEC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"spytec",4,2,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"spytec",0,0,tmp,tmp2)
                    buildTrainingTestFiles(dtst,"spytec",2,2,tmp,tmp2)
                else:
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>> Image Descriptor %s not available! <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n " %tt)
            elif (len(sys.argv) == 6):
                tmp = sys.argv[3].split(",")
                tmp2 = sys.argv[4].split(",")
                tmp3 = sys.argv[5].split(",")
                print(tmp2)
                if(tt == "ACC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing ACC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"acc",4,3,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"acc",1,3,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"acc",2,3,tmp,tmp2,tmp3)
                elif(tt == "BIC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing BIC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"bic",4,3,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"bic",1,3,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"bic",2,3,tmp,tmp2,tmp3)
                elif(tt == "CCV"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing CCV Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"ccv",4,3,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"ccv",1,3,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"ccv",2,3,tmp,tmp2,tmp3)
                elif(tt == "LCH"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing LCH Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"lch",4,3,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"lch",1,3,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"lch",2,3,tmp,tmp2,tmp3)
                elif(tt == "SASI"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing SASI Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"sasi",4,2,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"sasi",0,0,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"sasi",2,2,tmp,tmp2,tmp3)
                elif(tt == "LAS"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing LAS Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"las",4,2,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"las",0,0,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"las",2,2,tmp,tmp2,tmp3)
                elif(tt == "UNSER"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing UNSER Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"unser",4,2,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"unser",0,0,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"unser",2,2,tmp,tmp2,tmp3)
                elif(tt == "EOAC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing EOAC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"eoac",4,2,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"eoac",0,0,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"eoac",2,2,tmp,tmp2,tmp3)
                elif(tt == "SPYTEC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Processing SPYTEC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    buildTrainingTestFiles(dtst,"spytec",4,2,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"spytec",0,0,tmp,tmp2,tmp3)
                    buildTrainingTestFiles(dtst,"spytec",2,2,tmp,tmp2,tmp3)
                else:
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>> Image Descriptor %s not available! <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n " %tt)
        except ValueError:
            print("Image Descriptor needs to be a valid string")

if __name__ == "__main__":
    main()



