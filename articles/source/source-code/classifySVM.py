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
        tmp = i[:-2].split(" ")
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


def readTrainingTestFilesImages(outfile):
    ofid = open(outfile,'rt')
    ofid.seek(0)
    lines = ofid.readlines()
    ofid.close()
    images = []
    labels = []
    for i in lines:
        tmp = i.split(" ")
        labels.append(tmp[0])
        images.append(tmp[len(tmp)-2])
    return(labels,images)

def svmTestBySample(dataset,descriptor,space,channel,illuminant="IIC", testFolds=[5]):
    nameSpace,nameChannel = sc.getSpaceChannelName(space,channel)
    tt = descriptor.upper()
    # Loading Test Data
    fd=""
    for i in testFolds:
        fd = fd+str(i)+"-"
    fd = fd[:-1]
    outfile = "../training-test-files/" + tt + "-" + illuminant + "-" + nameSpace + "-" + nameChannel + "/" + dataset + "-SVM-test-folds-" + fd
    ft,lb = readTrainingTestFiles(outfile)
    testMatrixF = np.array(ft)
    testMatrixL = np.array(lb)

    #Scale Train Features
    #testMatrixFScaled = preprocessing.scale(testMatrixF)

    #Scale features between [-1,1]
    max_abs_scaler = preprocessing.MaxAbsScaler()
    testMatrixFScaled = max_abs_scaler.fit_transform(testMatrixF)

    npath = "../models/" + tt + "-" + illuminant + "-" + nameSpace + "-" + nameChannel + "/"
    modelName = npath + "model-" + dataset + "-" + tt + "-" + illuminant + "-" + nameSpace + "-" + nameChannel + ".pkl"
    clf = joblib.load(modelName)
    outLabels = clf.predict(testMatrixFScaled)
    scores = clf.score(testMatrixFScaled,testMatrixL)
    return(outLabels,scores)
    #print(outLabels,scores)



def svmTestByImage(dataset,descriptor,space,channel,illuminant="IIC", testFolds=[5]):
    nameSpace,nameChannel = sc.getSpaceChannelName(space,channel)
    tt = descriptor.upper()
    # Loading Test Data
    fd=""
    for i in testFolds:
        fd = fd+str(i)+"-"
    fd = fd[:-1]
    outfile = "../training-test-files/" + tt + "-" + illuminant + "-" + nameSpace + "-" + nameChannel + "/" + dataset + "-test-folds-" + fd
    labelsDefault,imagesLabels = readTrainingTestFilesImages(outfile)
    outLabels,scores = svmTestBySample(dataset,descriptor,space,channel,illuminant, testFolds)
    lastImage = imagesLabels[0]
    classImg = outLabels[0]
    correctClassImg = labelsDefault[0]
    cont = 1
    finalLabelsPredicted = []
    finalLabelsCorrect = []
    while cont < len(imagesLabels):
        if (lastImage != imagesLabels[cont]):
            finalLabelsPredicted.append((lastImage,classImg))
            finalLabelsCorrect.append(correctClassImg)
            lastImage = imagesLabels[cont]
            classImg = outLabels[cont]
            correctClassImg = labelsDefault[cont]
        else:
            if (outLabels[cont] == -1):
                classImg = -1
        cont+=1
    finalLabelsPredicted.append((lastImage,classImg))
    finalLabelsCorrect.append(correctClassImg)

    fakeImg = 0
    fakeCorrect = 0
    normalImg = 0
    normalCorrect = 0

    cont = 0
    while cont < len(finalLabelsPredicted):
        t1,t2 = finalLabelsPredicted[cont]
        t3 = finalLabelsCorrect[cont]
        if (t3 == '1'):
            normalImg +=1
            if (t2 == t3):
                normalCorrect += 1
        elif(t3 == '-1'):
            fakeImg += 1
            if (t2 == t3):
                fakeCorrect += 1
        cont+=1

    #print('Accuracy Normal Images: %.2f' %(normalCorrect/normalImg))
    #print('Accuracy Fake Images: %.2f' %(fakeCorrect/fakeImg))
    #print('Accuracy Final: %.2f' %((normalCorrect+fakeCorrect)/(normalImg+fakeImg)))




svmTestByImage("DSO","BIC",4,3,"IIC", [5])