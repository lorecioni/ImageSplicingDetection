'''
Created on 03 ott 2016

@author: lorenzocioni
'''

import argparse
import faceSplicingDetection
import sys
import os
import loadDatasets

__version__ = 0.1
__date__ = '2016-09-12'
__updated__ = '2016-11-14'

def main():
    print('ImageSplicingDetection v.' + str(__version__))
    print('Creation date: ' + __date__ + ', last update: ' + __updated__)
    parser = argparse.ArgumentParser()

    parser.add_argument("--train", help="train the model for further splicing detection", dest='train', action='store_true')
    parser.add_argument("--detect", help="detect splice over an image", dest='detect', action='store_true')
    parser.add_argument("--cross-validate", help="cross-validate the dataset", dest='cross_validation', action='store_true')
    parser.add_argument("--extract-single-features", help="extract feature vector for a specific image", dest='extract_single_features', action='store_true')
    parser.add_argument("--dataset", help="the path of the dataset folder containing all the training images")
    parser.add_argument("--labels", help="the path of labels txt file, a list of labels (1, 0) comma separated")
    parser.add_argument("--no-extract-features", help="no extract all training images features", dest='extract_features', action='store_false')
    parser.add_argument("--no-extract-maps", help="no extract all training images features", dest='extract_maps', action='store_false')
    parser.add_argument("--img", help="the path of the suspicious image")
    parser.add_argument("--depth", help="the depth of the recursive method (default 3)")
    parser.add_argument("--heat-map", help="display the heat map between GGE and IIC maps", dest='heat_map', action='store_true')
    parser.add_argument("--verbose", help="display all messages", dest='verbose', action='store_true')

    #Setting defaults
    parser.set_defaults(heat_map = False)
    parser.set_defaults(verbose = False)
    parser.set_defaults(train = False)
    parser.set_defaults(detect = False)
    parser.set_defaults(extract_maps = True)
    parser.set_defaults(extract_features = True)
    parser.set_defaults(cross_validation = False)
    parser.set_defaults(extract_single_features = False)
    parser.set_defaults(evaluate_eucl_distances = False)
    parser.set_defaults(depth = 3)

    args = parser.parse_args()

    if len(sys.argv) < 2:
        parser.print_help()
        sys.exit(1)

    detector = faceSplicingDetection.FaceSplicingDetection(args.extract_maps, 
                                                           args.extract_features,
                                                           args.cross_validation,
                                                           args.verbose,
                                                           args.heat_map)

    if args.train:
        #Training the model
        images, labels = loadDatasets.load(args.dataset, args.labels)
        if len(images) > 0:
            detector.train(images, labels)

    elif args.detect:
        #Detecting splice over a selected image
        if len(args.img) > 0:
            detector.detectSplice(args.img, args.heat_map, args.depth)
        else:
            print('No image selected for splicing detection. Must specify the --img argument.')

    elif args.extract_single_features:
        #Extract feature vector for a selected image
        if len(args.img) > 0:
            print('Extracting image feature vector froma single image')
            detector.extractFeatures(args.img)
        else:
            print('No image selected for splicing detection. Must specify the --img argument.')

if __name__ == '__main__':
    main()
