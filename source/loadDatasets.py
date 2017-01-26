'''
Created on 14 nov 2016

@author: lorenzocioni
'''

import os
import config
import cv2
import utils
import re
import illuminantMaps

def load():
    name = config.dataset
    images = []
    labels = []
    
    if name == 'DSO-1' or name == 'DSI-1':
        print('Loading DSO-1 dataset') 
        #Retrieving file list
        files = os.listdir(config.imagesFolder)
        for i in files:
            filename = utils.getFilename(i)
            try:
                img = config.imagesFolder + i
                if os.path.isfile(img) and not i.startswith('.'):
                    images.append(img)
                    #Retrieving label
                    labelFile = open(config.labelsFolder + filename + '.txt', 'r')
                    imageLabels = [re.split(r'\t+', s.strip()) for s in labelFile.readlines()]
                    labelFile.close()
                    labels.append(imageLabels)
            except:
                print("Error on processing image: " + i)
                
        print('Loaded DSO-1 dataset') 
    
    elif name == 'COLUMBIA':
        print('Loading COLUMBIA dataset') 
        originals = os.listdir(config.imagesFolder + '/4cam_auth')
        spliced = os.listdir(config.imagesFolder + '/4cam_splc')
        
        padding = 50
        
        index = 0;
        for i in spliced:
            try:         
                img = config.imagesFolder + "/4cam_splc/" + i
                if os.path.isfile(img) and not i.startswith('.'):
                    filename = i.split('/')
                    filename = filename[len(filename) - 1]
                    filename = filename[:-4]               
                    
                    mask = cv2.imread(config.imagesFolder + '/4cam_splc/edgemask/' + filename + '_edgemask.jpg', cv2.IMREAD_GRAYSCALE)
                    _, mask = cv2.threshold(mask, 100, 255, cv2.THRESH_BINARY)
                    mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5)))
                    mask = cv2.dilate(mask, cv2.getStructuringElement(cv2.MORPH_ELLIPSE,(5,5)), iterations = 1)
                    _, contours, _ = cv2.findContours(mask.copy(), cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
                    x, y, width, height = cv2.boundingRect(contours[0])
                                        
                    img_splc = cv2.imread(img)
                    img_orig = cv2.imread(config.imagesFolder + '/4cam_auth/' + originals[index])
                    
                    if img_splc is not None and img_orig is not None:
                        
                        filename_orig = originals[index].split('/')
                        filename_orig = filename_orig[len(filename_orig) - 1]
                        filename_orig = filename_orig[:-4]   
                        
                        #print ('Processing image ' + str(index))
                        
                        # 1. Extracting GGE and IIC illuminant maps
                        #print('\t-Segmenting image')
                        #illuminantMaps.prepareImageIlluminants(config.imagesFolder + '/4cam_auth/' + originals[index], config.seg_sigma, config.seg_k, config.seg_min_size, config.min_intensity, config.max_intensity, False)
                        #illuminantMaps.prepareImageIlluminants(img, config.seg_sigma, config.seg_k, config.seg_min_size, config.min_intensity, config.max_intensity, False)

                        # 1.2 Extracting GGE illuminant map
                        #print('\t-Extracting GGE map')
                        #illuminantMaps.extractGGEMap(config.imagesFolder + '/4cam_auth/' + originals[index], filename_orig + "_segmented.png", config.gge_sigma, config.gge_n, config.gge_p, False)
                        #illuminantMaps.extractGGEMap(img, filename + "_segmented.png", config.gge_sigma, config.gge_n, config.gge_p, False)

                        # 1.3 Extracting IIC illuminant map
                        #print('\t-Extracting IIC map')
                        #illuminantMaps.extractIICMap(config.imagesFolder + '/4cam_auth/' + originals[index], filename_orig + "_segmented" + config.maps_out_suffix + ".png", False)
                        #illuminantMaps.extractIICMap(img, filename + "_segmented" + config.maps_out_suffix + ".png", False)

                        rows, cols, _ = img_orig.shape
                        
                        padding_y = padding
                        padding_x = padding
                        if y + height + padding > rows:
                            padding_y = 0
                        if x + width + padding > cols:
                            padding_x = 0
                            
                        roi = img_orig[y:y + height + padding_y, x:x + width + padding_x]
                        path = config.imagesFolder + '/roi_original/original_' + str(index) + '.png'
                        path_gge = config.maps_folder + '/original_' + str(index) + '_gge_map.png'
                        path_iic = config.maps_folder + '/original_' + str(index) + '_iic_map.png'
                        
                        #gge_img_orig = cv2.imread(config.imagesFolder + '/maps/' + filename_orig + "_gge_map" + config.maps_out_suffix + ".png")
                        #iic_img_orig = cv2.imread(config.imagesFolder + '/maps/' + filename_orig + "_iic_map" + config.maps_out_suffix + ".png")
                        #gge_img_splc = cv2.imread(config.imagesFolder + '/maps/' + filename + "_gge_map" + config.maps_out_suffix + ".png")
                        #iic_img_splc = cv2.imread(config.imagesFolder + '/maps/' + filename + "_iic_map" + config.maps_out_suffix + ".png")
                        
                        r, c, _ = roi.shape
                        if r > 0 and c > 0 :
                            #roi_gge_orig = gge_img_orig[y:y + height + padding_y, x:x + width + padding_x]
                            #roi_iic_orig = iic_img_orig[y:y + height + padding_y, x:x + width + padding_x]
                            #cv2.imwrite(path, roi)
                            #cv2.imwrite(path_gge, roi_gge_orig)
                            #cv2.imwrite(path_iic, roi_iic_orig)
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
                        path = config.imagesFolder + '/roi_spliced/spliced_' + str(index) + '.png'
                        path_gge = config.maps_folder + '/spliced_' + str(index) + '_gge_map.png'
                        path_iic = config.maps_folder + '/spliced_' + str(index) + '_iic_map.png'
                        
                        r, c, _ = roi.shape
                        if r > 0 and c > 0 :
                            #roi_gge_splc = gge_img_splc[y:y + height + padding_y, x:x + width + padding_x]
                            #roi_iic_splc = iic_img_splc[y:y + height + padding_y, x:x + width + padding_x]
                            #cv2.imwrite(path, roi)
                            #cv2.imwrite(path_gge, roi_gge_splc)
                            #cv2.imwrite(path_iic, roi_iic_splc)
                            images.append(path)
                            labels.append(1)


                        #print('Removing whole maps')
                        #os.remove(config.imagesFolder + '/maps/' + filename_orig + "_gge_map" + config.maps_out_suffix + ".png")
                        #os.remove(config.imagesFolder + '/maps/' + filename_orig + "_iic_map" + config.maps_out_suffix + ".png")
                        #os.remove(config.imagesFolder + '/maps/' + filename + "_gge_map" + config.maps_out_suffix + ".png")
                        #os.remove(config.imagesFolder + '/maps/' + filename + "_iic_map" + config.maps_out_suffix + ".png")

            except:
                print("Error on processing image")
                index = index + 1
        
            index = index + 1   
        
        print('Loaded COLUMBIA dataset')   
    
    return (images, labels)