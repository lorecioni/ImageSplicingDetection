'''
Created on 14 nov 2016

@author: lorenzocioni
'''

import os
import config
import cv2

def load(imagesPath, labelsPath):
    name = config.dataset
    images = []
    labels = []
    
    if name == 'DSO-1':
        print('Loaded DSO-1 dataset') 
        #Retrieving file list
        files = os.listdir(imagesPath)
        for i in files:
            try:
                img = imagesPath + "/" + i
                if os.path.isfile(img) and not i.startswith('.'):
                    images.append(img)
            except:
                print("Error on processing image")
                
        #Retrieving labels     
        in_file = open(labelsPath, "r")
        text = in_file.read()
        labels = [int(s.strip()) for s in text.splitlines()]
        in_file.close()
        print('Loaded DSO-1 dataset') 
    
    elif name == 'COLUMBIA':
        print('Loading COLUMBIA dataset') 
        originals = os.listdir(imagesPath + '/4cam_auth')
        spliced = os.listdir(imagesPath + '/4cam_splc')
        
        padding = 50
        
        index = 0;
        for i in spliced:
            try:         
                img = imagesPath + "/4cam_splc/" + i
                if os.path.isfile(img) and not i.startswith('.'):
                    filename = i.split('/')
                    filename = filename[len(filename) - 1]
                    filename = filename[:-4]               
                    
                    mask = cv2.imread(imagesPath + '/4cam_splc/edgemask/' + filename + '_edgemask.jpg', cv2.IMREAD_GRAYSCALE)
                    _, mask = cv2.threshold(mask, 100, 255, cv2.THRESH_BINARY)
                    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5)))
                    mask = cv2.dilate(mask, cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5)), iterations = 1)
                    _, contours, _ = cv2.findContours(mask.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
                    x, y, width, height = cv2.boundingRect(contours[0])
                                        
                    img_splc = cv2.imread(img)
                    img_orig = cv2.imread(imagesPath + '/4cam_auth/' + originals[index])
                    
                    if img_splc is not None and img_orig is not None:

                        rows, cols, _ = img_orig.shape
                        
                        padding_y = padding
                        padding_x = padding
                        if y + height + padding > rows:
                            padding_y = 0
                        if x + width + padding > cols:
                            padding_x = 0
                            
                        roi = img_orig[y:y + height + padding_y, x:x + width + padding_x]
                        path = imagesPath + '/roi_original/original_' + str(index) + '.png'
                        
                        r, c, _ = roi.shape
                        if r > 0 and c > 0 :
                            cv2.imwrite(path, roi)
                            images.append(path)
                            labels.append(0)
                        
                        rows, cols, _ = img_splc.shape
                        padding_y = padding
                        padding_x = padding
                        if y + height + padding > rows:
                            padding_y = 0
                        if x + width + padding > cols:
                            padding_x = 0
                            
                        roi = img_splc[y:y + height + padding_y, x:x + width + padding_x]
                        path = imagesPath + '/roi_spliced/spliced_' + str(index) + '.png'
                        
                        r, c, _ = roi.shape
                        if r > 0 and c > 0 :
                            cv2.imwrite(path, roi)
                            images.append(path)
                            labels.append(1)

            except:
                print("Error on processing image")
        
            index = index + 1   
        
    print('Loaded COLUMBIA dataset')   
    return (images, labels)