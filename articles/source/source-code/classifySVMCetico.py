import numpy as np
from sklearn import svm
from sklearn.externals import joblib
from sklearn import preprocessing
from sklearn.grid_search import GridSearchCV
import getSpaceChannelName as sc

def readTrainingTestFiles(outfile):
    ofid = open(outfile,'rt')
    ofid.seek(0)
    lines = ofid.readlines()
    ofid.close()
    features = []
    labels = []
    for i in lines:
        label = 0
        tmp = i[:-3].split(" ")
        row = []
        for j in tmp:
            if (label != 0):
                tmp2 = j.split(":")
                row.append(tmp2[1])
            else:
                label = j
                labels.append(j)
        features.append(row)
    return(features,labels)


def svmTestBySample(imgName,descriptor,space,channel,illuminant="IIC"):
    nameSpace,nameChannel = sc.getSpaceChannelName(space,channel)
    tt = descriptor.upper()
    outfile = "../extracted-feature-vectors/" + tt + "-" + illuminant + "-" + nameSpace + "-" + nameChannel + "/fv-" + imgName[:-4] + ".txt"
    ft,lb = readTrainingTestFiles(outfile)
    testMatrixF = np.array(ft)
    testMatrixL = np.array(lb)

    #Scale Train Features
    #testMatrixFScaled = preprocessing.scale(testMatrixF)

    #Scale features between [-1,1]
    max_abs_scaler = preprocessing.MaxAbsScaler()
    testMatrixFScaled = max_abs_scaler.fit_transform(testMatrixF)

    npath = "../models/" + tt + "-" + illuminant + "-" + nameSpace + "-" + nameChannel + "/"
    modelName = npath + "model-DSO-" + tt + "-" + illuminant + "-" + nameSpace + "-" + nameChannel + ".pkl"
    clf = joblib.load(modelName)
    outLabels = clf.predict(testMatrixFScaled)
    scores = clf.score(testMatrixFScaled,testMatrixL)
    return(outLabels,scores)
    #print(outLabels,scores)



def svmTestByImage(imgName,descriptor,space,channel,illuminant="IIC"):
    nameSpace,nameChannel = sc.getSpaceChannelName(space,channel)
    tt = descriptor.upper()
    outfile = "../extracted-feature-vectors/" + tt + "-" + illuminant + "-" + nameSpace + "-" + nameChannel + "/fv-" + imgName[:-4] + ".txt"
    outLabels,scores = svmTestBySample(imgName,descriptor,space,channel,illuminant)
    labelsDefault,imageLables = readTrainingTestFiles(outfile)
    imageClass = 1
    for i in outLabels:
        if i == '-1':
            imageClass = -1
    return imageClass


def fullClassification(imgName):
    classifiersFake = []
    classifiersNormal = []
    listOfParams = [("acc",4,3,"IIC"), ("acc",1,3,"IIC"), ("acc",2,3,"IIC"), ("acc",4,3,"GGE"), ("acc",1,3,"GGE"), ("acc",2,3,"GGE"), ("ccv",4,3,"IIC"), ("ccv",1,3,"IIC"), ("ccv",2,3,"IIC"), ("ccv",4,3,"GGE"), ("ccv",1,3,"GGE"), ("ccv",2,3,"GGE"), ("bic",4,3,"IIC"), ("bic",1,3,"IIC"), ("bic",2,3,"IIC"), ("bic",4,3,"GGE"), ("bic",1,3,"GGE"), ("bic",2,3,"GGE"), ("lch",4,3,"IIC"), ("lch",1,3,"IIC"), ("lch",2,3,"IIC"), ("lch",4,3,"GGE"), ("lch",1,3,"GGE"), ("lch",2,3,"GGE"),("sasi",4,2,"IIC"), ("sasi",0,0,"IIC"), ("sasi",2,2,"IIC"), ("sasi",4,2,"GGE"), ("sasi",0,0,"GGE"), ("sasi",2,2,"GGE"), ("las",4,2,"IIC"), ("las",0,0,"IIC"), ("las",2,2,"IIC"), ("las",4,2,"GGE"), ("las",0,0,"GGE"), ("las",2,2,"GGE"), ("unser",4,2,"IIC"), ("unser",0,0,"IIC"), ("unser",2,2,"IIC"), ("unser",4,2,"GGE"), ("unser",0,0,"GGE"), ("unser",2,2,"GGE"), ("eoac",4,2,"IIC"), ("eoac",0,0,"IIC"), ("eoac",2,2,"IIC"), ("eoac",4,2,"GGE"), ("eoac",0,0,"GGE"), ("eoac",2,2,"GGE"), ("spytec",4,2,"IIC"), ("spytec",0,0,"IIC"), ("spytec",2,2,"IIC"), ("spytec",4,2,"GGE"), ("spytec",0,0,"GGE"), ("spytec",2,2,"GGE")]
    outClassification = []
    finalClass = "FAKE"
    votesNormal = 0
    votesFake = 0
    for i in listOfParams:
        desc,space,channel,illumi = i
        classPredic = svmTestByImage(imgName,desc,space,channel,illumi)
        outClassification.append((desc,space,channel,illumi,classPredic))
        if (classPredic == 1):
            votesNormal += 1
        else:
            votesFake += 1
    if (votesNormal > votesFake):
        finalClass = "NORMAL"


    #return (outClassification,votesNormal,votesFake,finalClass)
    print("Votos Normal: %d\nVotos Fake: %d\nClassificacao Final: %s" %(votesNormal,votesFake,finalClass))



fullClassification("splicing-87.png")