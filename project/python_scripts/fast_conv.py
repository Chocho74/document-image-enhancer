# -*- coding: utf-8 -*-
"""
Created on Fri Dec  2 16:02:57 2022

@author: Eric
"""
import matplotlib.pyplot as plt
import numpy as np
import cv2

img = cv2.imread("in.ppm")
height,width,_=np.shape(img)
test_img=np.reshape(img[:,:,::-1],(height,width*3))
typ='P3'
height_widht=str(width) + " " + str(height)
RGB="255"

with open('input.ppm', 'w') as f:
    f.write(typ)
    f.write('\n')
    f.write(height_widht)
    f.write('\n')
    f.write(RGB)
    f.write('\n')
    for x in range(height):
        for y in range(width*3):
            l=len(str(test_img[x,y]))
            f.write(str(test_img[x,y]).ljust(4))
        f.write('\n')