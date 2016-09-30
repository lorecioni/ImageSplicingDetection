import numpy as np
import sys
import os
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


def svmTraining(dataset,descriptor,space,channel, illuminant="IIC", trainingFolds=[1,2,3,4,5]):
    nameSpace,nameChannel = sc.getSpaceChannelName(space,channel)
    tt = descriptor.upper()

    # Loading Training Data
    fd=""
    for i in trainingFolds:
        fd = fd+str(i)+"-"
    fd = fd[:-1]
    outfile = "../training-test-files/" + tt + "-" + illuminant + "-" + nameSpace + "-" + nameChannel + "/" + dataset + "-SVM-training-folds-" + fd
    ft,lb = readTrainingTestFiles(outfile)
    trainingMatrixF = np.array(ft)
    trainingMatrixL = np.array(lb)

    #Calculate the weight of each class
    sp = lb.count('1')
    sn = lb.count('-1')
    weightFake = 2*(sp/(sp+sn))
    weightNormal = 2-weightFake

    #Scale Train Features
    #trainingMatrixFScaled = preprocessing.scale(trainingMatrixF)

    #Scale features between [-1,1]
    max_abs_scaler = preprocessing.MaxAbsScaler()
    trainingMatrixFScaled = max_abs_scaler.fit_transform(trainingMatrixF)

    # Make grid search for best set of parameters
    #Cs = np.logspace(-6, -1, 10)

    #svc = svm.SVC(kernel='rbf',class_weight={'1':weightNormal,'-1':weightFake})
    svc = svm.SVC()

    #clf = GridSearchCV(svc,dict(C=Cs),n_jobs=-1,param_grid={'C': [1, 10, 100, 1000], 'gamma': [0.001, 0.0001], 'kernel': ['rbf']})
    clf = GridSearchCV(svc,n_jobs=-1,param_grid={'C': list(range(1,1000,10)), 'gamma': np.arange(0.0001, 0.001,0.001), 'kernel': ['rbf'], 'class_weight':[{'1':weightNormal,'-1':weightFake}]})

    clf.fit(trainingMatrixFScaled, trainingMatrixL)
    npath = "../models/" + tt + "-" + illuminant + "-" + nameSpace + "-" + nameChannel + "/"
    if not os.path.exists(npath):
        os.makedirs(npath)
    modelName = npath + "model-" + dataset + "-" + tt + "-" + illuminant + "-" + nameSpace + "-" + nameChannel + ".pkl"
    joblib.dump(clf,modelName)

def main():
    if len(sys.argv) < 3:
        print("Provide correct number of arguments - svmTraining.py <dataset> <descriptorName> <training set - default \"1,2,3,4,5\">")
        return
    else:
        try:
            tt = sys.argv[2].upper()
            dtst = sys.argv[1]
            if (len(sys.argv) == 3):
                if(tt == "ACC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using ACC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"acc",4,3,"IIC")
                    svmTraining(dtst,"acc",1,3,"IIC")
                    svmTraining(dtst,"acc",2,3,"IIC")
                    svmTraining(dtst,"acc",4,3,"GGE")
                    svmTraining(dtst,"acc",1,3,"GGE")
                    svmTraining(dtst,"acc",2,3,"GGE")
                elif(tt == "BIC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using BIC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"bic",4,3,"IIC")
                    svmTraining(dtst,"bic",1,3,"IIC")
                    svmTraining(dtst,"bic",2,3,"IIC")
                    svmTraining(dtst,"bic",4,3,"GGE")
                    svmTraining(dtst,"bic",1,3,"GGE")
                    svmTraining(dtst,"bic",2,3,"GGE")
                elif(tt == "CCV"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using CCV Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"ccv",4,3,"IIC")
                    svmTraining(dtst,"ccv",1,3,"IIC")
                    svmTraining(dtst,"ccv",2,3,"IIC")
                    svmTraining(dtst,"ccv",4,3,"GGE")
                    svmTraining(dtst,"ccv",1,3,"GGE")
                    svmTraining(dtst,"ccv",2,3,"GGE")
                elif(tt == "LCH"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using LCH Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"lch",4,3,"IIC")
                    svmTraining(dtst,"lch",1,3,"IIC")
                    svmTraining(dtst,"lch",2,3,"IIC")
                    svmTraining(dtst,"lch",4,3,"GGE")
                    svmTraining(dtst,"lch",1,3,"GGE")
                    svmTraining(dtst,"lch",2,3,"GGE")
                elif(tt == "SASI"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using SASI Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"sasi",4,2,"IIC")
                    svmTraining(dtst,"sasi",0,0,"IIC")
                    svmTraining(dtst,"sasi",2,2,"IIC")
                    svmTraining(dtst,"sasi",4,2,"GGE")
                    svmTraining(dtst,"sasi",0,0,"GGE")
                    svmTraining(dtst,"sasi",2,2,"GGE")
                elif(tt == "LAS"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using LAS Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"las",4,2,"IIC")
                    svmTraining(dtst,"las",0,0,"IIC")
                    svmTraining(dtst,"las",2,2,"IIC")
                    svmTraining(dtst,"las",4,2,"GGE")
                    svmTraining(dtst,"las",0,0,"GGE")
                    svmTraining(dtst,"las",2,2,"GGE")
                elif(tt == "UNSER"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using UNSER Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"unser",4,2,"IIC")
                    svmTraining(dtst,"unser",0,0,"IIC")
                    svmTraining(dtst,"unser",2,2,"IIC")
                    svmTraining(dtst,"unser",4,2,"GGE")
                    svmTraining(dtst,"unser",0,0,"GGE")
                    svmTraining(dtst,"unser",2,2,"GGE")
                elif(tt == "EOAC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using EOAC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"eoac",4,2,"IIC")
                    svmTraining(dtst,"eoac",0,0,"IIC")
                    svmTraining(dtst,"eoac",2,2,"IIC")
                    svmTraining(dtst,"eoac",4,2,"GGE")
                    svmTraining(dtst,"eoac",0,0,"GGE")
                    svmTraining(dtst,"eoac",2,2,"GGE")
                elif(tt == "SPYTEC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using SPYTEC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"spytec",4,2,"IIC")
                    svmTraining(dtst,"spytec",0,0,"IIC")
                    svmTraining(dtst,"spytec",2,2,"IIC")
                    svmTraining(dtst,"spytec",4,2,"GGE")
                    svmTraining(dtst,"spytec",0,0,"GGE")
                    svmTraining(dtst,"spytec",2,2,"GGE")
                else:
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>> Image Descriptor %s not available! <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n " %tt)
            elif(len(sys.argv) == 4):
                tmp = sys.argv[3].split(",")
                if(tt == "ACC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using ACC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"acc",4,3,"IIC",tmp)
                    svmTraining(dtst,"acc",1,3,"IIC",tmp)
                    svmTraining(dtst,"acc",2,3,"IIC",tmp)
                    svmTraining(dtst,"acc",4,3,"GGE",tmp)
                    svmTraining(dtst,"acc",1,3,"GGE",tmp)
                    svmTraining(dtst,"acc",2,3,"GGE",tmp)
                elif(tt == "BIC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using BIC Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"bic",4,3,"IIC",tmp)
                    svmTraining(dtst,"bic",1,3,"IIC",tmp)
                    svmTraining(dtst,"bic",2,3,"IIC",tmp)
                    svmTraining(dtst,"bic",4,3,"GGE",tmp)
                    svmTraining(dtst,"bic",1,3,"GGE",tmp)
                    svmTraining(dtst,"bic",2,3,"GGE",tmp)
                elif(tt == "CCV"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using CCV Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"ccv",4,3,"IIC",tmp)
                    svmTraining(dtst,"ccv",1,3,"IIC",tmp)
                    svmTraining(dtst,"ccv",2,3,"IIC",tmp)
                    svmTraining(dtst,"ccv",4,3,"GGE",tmp)
                    svmTraining(dtst,"ccv",1,3,"GGE",tmp)
                    svmTraining(dtst,"ccv",2,3,"GGE",tmp)
                elif(tt == "LCH"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using LCH Color Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"lch",4,3,"IIC",tmp)
                    svmTraining(dtst,"lch",1,3,"IIC",tmp)
                    svmTraining(dtst,"lch",2,3,"IIC",tmp)
                    svmTraining(dtst,"lch",4,3,"GGE",tmp)
                    svmTraining(dtst,"lch",1,3,"GGE",tmp)
                    svmTraining(dtst,"lch",2,3,"GGE",tmp)
                elif(tt == "SASI"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using SASI Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"sasi",4,2,"IIC",tmp)
                    svmTraining(dtst,"sasi",0,0,"IIC",tmp)
                    svmTraining(dtst,"sasi",2,2,"IIC",tmp)
                    svmTraining(dtst,"sasi",4,2,"GGE",tmp)
                    svmTraining(dtst,"sasi",0,0,"GGE",tmp)
                    svmTraining(dtst,"sasi",2,2,"GGE",tmp)
                elif(tt == "LAS"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using LAS Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"las",4,2,"IIC",tmp)
                    svmTraining(dtst,"las",0,0,"IIC",tmp)
                    svmTraining(dtst,"las",2,2,"IIC",tmp)
                    svmTraining(dtst,"las",4,2,"GGE",tmp)
                    svmTraining(dtst,"las",0,0,"GGE",tmp)
                    svmTraining(dtst,"las",2,2,"GGE",tmp)
                elif(tt == "UNSER"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using UNSER Texture Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"unser",4,2,"IIC",tmp)
                    svmTraining(dtst,"unser",0,0,"IIC",tmp)
                    svmTraining(dtst,"unser",2,2,"IIC",tmp)
                    svmTraining(dtst,"unser",4,2,"GGE",tmp)
                    svmTraining(dtst,"unser",0,0,"GGE",tmp)
                    svmTraining(dtst,"unser",2,2,"GGE",tmp)
                elif(tt == "EOAC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using EOAC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"eoac",4,2,"IIC",tmp)
                    svmTraining(dtst,"eoac",0,0,"IIC",tmp)
                    svmTraining(dtst,"eoac",2,2,"IIC",tmp)
                    svmTraining(dtst,"eoac",4,2,"GGE",tmp)
                    svmTraining(dtst,"eoac",0,0,"GGE",tmp)
                    svmTraining(dtst,"eoac",2,2,"GGE",tmp)
                elif(tt == "SPYTEC"):
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>> Training an SVM Classifier Using SPYTEC Shape Descriptor <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n")
                    svmTraining(dtst,"spytec",4,2,"IIC",tmp)
                    svmTraining(dtst,"spytec",0,0,"IIC",tmp)
                    svmTraining(dtst,"spytec",2,2,"IIC",tmp)
                    svmTraining(dtst,"spytec",4,2,"GGE",tmp)
                    svmTraining(dtst,"spytec",0,0,"GGE",tmp)
                    svmTraining(dtst,"spytec",2,2,"GGE",tmp)
                else:
                    print("\n\n>>>>>>>>>>>>>>>>>>>>>>>>>> Image Descriptor %s not available! <<<<<<<<<<<<<<<<<<<<<<<<<<<<< \n\n " %tt)
        except ValueError:
            print("Image Descriptor needs to be a valid string")

if __name__ == "__main__":
    main()