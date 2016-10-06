'''
Created on 03 ott 2016

@author: lorenzocioni
'''

import argparse
import splicingDetection
import sys
import os

__version__ = 0.1
__date__ = '2016-09-28'
__updated__ = '2016-10-06'

def main():
    print('ImageSplicingDetection v.' + str(__version__))
    print('Creation date: ' + __date__ + ', last update: ' + __updated__)
    parser = argparse.ArgumentParser()

    parser.add_argument("--train", help="train the model for further splicing detection", dest='train', action='store_true')
    parser.add_argument("--dataset", help="the path of the dataset folder containing all the training images")
    parser.add_argument("--labels", help="the path of labels txt file, a list of labels (1, 0) comma separated")
    parser.add_argument("--detect", help="detect splice over an image", dest='detect', action='store_true')
    parser.add_argument("--img", help="the path of the suspicious image")
    parser.add_argument("--heat-map", help="display the heat map between GGE and IIC maps", dest='heat_map', action='store_true')
    parser.add_argument("--verbose", help="display all messages", dest='verbose', action='store_true')

    #Setting defaults
    parser.set_defaults(heat_map = False)
    parser.set_defaults(verbose = False)
    parser.set_defaults(train = False)
    parser.set_defaults(detect = False)

    args = parser.parse_args()
    
    if len(sys.argv) < 2:
        parser.print_help()
        sys.exit(1)
    
    if args.train:
        #Training the model
        images = []
        #Retrieving file list
        files = os.listdir(args.dataset)
        for i in files:
            try:
                if not i.startswith('.'):
                    images.append(args.dataset + "/" + i)
            except:
                print("Erro ao processar imagem")
                
        #Retrieving labels     
        in_file = open(args.labels, "r")
        text = in_file.read()
        labels = [int(s.strip()) for s in text.splitlines()]
        in_file.close()

        splicingDetection.train(images, labels, args.verbose)
        
    elif args.detect:
        #Detecting splice over a selected image
        if len(args.img) > 0:
            splicingDetection.detectSplice(args.img, args.heat_map, args.verbose)
        else:
            print('No image selected for splicing detection. Must specify the --img argument.')

if __name__ == '__main__':
    main()
