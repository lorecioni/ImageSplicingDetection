'''
Created on 03 ott 2016

@author: lorenzocioni
'''

import argparse
import splicingDetection

__version__ = 0.1
__date__ = '2016-09-28'
__updated__ = '2016-10-05'

def main():
    print('ImageSplicingDetection v.' + str(__version__))
    print('Creation date: ' + __date__ + ', last update: ' + __updated__)
    parser = argparse.ArgumentParser()
    parser.add_argument("--img", help="the path of the suspicious image")
    parser.add_argument("--heat-map", help="display the heat map between GGE and IIC maps", dest='heat_map', action='store_true')
    parser.add_argument("--verbose", help="display all messages", dest='verbose', action='store_true')
    parser.set_defaults(heat_map = False)
    parser.set_defaults(verbose = False)
    args = parser.parse_args()
    splicingDetection.detectSplice(args.img, args.heat_map, args.verbose)

if __name__ == '__main__':
    main()
