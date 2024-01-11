# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import matplotlib.pyplot as plt
import numpy as np
import cv2
import time

def histogram(image):
    histo = [0] * 256
    for i in image.flatten():
        histo[i]+=1
    return histo

def cumulative_histogram(image):
    histo = histogram(image)
    Hkg=[]
    for g in range(0,256):
        Hk=0
        for i in range(0,g):
            Hk+=histo[i]
        Hkg.append(Hk)
    cumulative_histo = [(i /Hkg[-1])*np.max(image) for i in Hkg]

    return cumulative_histo

def histogram_linearization(image, cumulative_histo):
    r,c=np.shape(image)
    linearized_image=np.zeros_like(image,dtype=int)
    for y in range(r):
        for x in range(c):
            linearized_image[y,x]=cumulative_histo[image[y,x]]
    return linearized_image
    
def moving_norm(array,size):
    
    ''' this function get the image to process as the first paramter and the size of the convolution kernel  size 
        as the second parameter. Make sure that the kernal size is odd. A size of 15 is recommended '''
        
    rows,columns=np.shape(array)
    h_size=int((size-1)/2)       

    normed_array=np.copy(array).astype(float)
    
    big_array = np.empty((2*h_size+rows,2*h_size+columns))
    big_array[:] = np.nan
    big_array[h_size:h_size+rows,h_size:h_size+columns]=array

    for y in range (h_size,rows+h_size):
        for x in range (h_size,columns+h_size):
            temp_mean=np.nanmean(big_array[y-(h_size):y+(h_size+2),x-(h_size):x+(h_size+2)])
            normed_array[y-h_size,x-h_size]=big_array[y,x]/temp_mean

    return normed_array


def adaptive_mean_filter(array,size=21,factor=1,brightness=1.2):
    ''' This filter is applied on the image which is processed. This filter calculates 
        the local variance of an image section and blurrs the image in reference to the variance.
        The first parameter is the the image.the second is the area size at which the local variance is calculated.
        The parameter factor is used for the decision when the variance is high enough that the local image section is detected as an edge
        or low that the image sections is detected as background.
        The Parameter brightness increases the pixel values of the background.
        This Function is computaional very expansive'''
    
    global_var=np.var(array)*factor
    rows,columns=np.shape(array)
    h_size=int((size-1)/2)       

    filtered_array=np.copy(array).astype(float)
    
    big_array = np.empty((2*h_size+rows,2*h_size+columns))
    big_array[:] = np.nan
    big_array[h_size:h_size+rows,h_size:h_size+columns]=array

    for y in range (h_size,rows+h_size):
        for x in range (h_size,columns+h_size):
            local_var=np.nanvar(big_array[y-(h_size):y+(h_size+2),x-(h_size):x+(h_size+2)])
            local_mean=np.nanmean(big_array[y-(h_size):y+(h_size+2),x-(h_size):x+(h_size+2)])
            
            filtered_array[y-h_size,x-h_size]=big_array[y,x] - ((global_var/local_var) *(big_array[y,x]-local_mean*brightness))

    return filtered_array


def adaptive_thresh(array,original_img,sections=100,m=0.5,n=0.79,f=0.0014):
    '''This function decides fpr every pixel if it is a letter or background and return a boolean array.
        The first parameter is the array which should be processed. The second Parameter is the original image which isnt processed at all.
        The parameter sections determines in how many parts the whole image is divided. A number of largest image dimension / 13 is recommended.
        The parameters m and n determines the gradient and the crosspoint of the abscissa for the linar function where the x-value is the variance of the image section.
        The paramter f is the factor with wich the squar root of maximum brightness of each section is multiplied and which also contribute in the Threshold formula.'''
    
    image_cumulative_histogram = cumulative_histogram(original_img)
    linearized_image = histogram_linearization(original_img, image_cumulative_histogram)
    
    rows,columns=np.shape(array)
    widht_sect=np.linspace(0,columns,sections+1,endpoint=True).astype(int)
    height_sect=np.linspace(0,rows,sections+1,endpoint=True).astype(int)
    
    thresh_array=np.copy(array).astype(float)
    
    for ro in range(0,sections):
        for co in range(0,sections):
            
            var=np.var(array[height_sect[ro]:height_sect[ro+1],widht_sect[co]:widht_sect[co+1]])
            b_max=np.max(linearized_image[height_sect[ro]:height_sect[ro+1],widht_sect[co]:widht_sect[co+1]])
            thresh=m*var+n+(b_max**(0.5))*f
            thresh_array[height_sect[ro]:height_sect[ro+1],widht_sect[co]:widht_sect[co+1]][array[height_sect[ro]:height_sect[ro+1],widht_sect[co]:widht_sect[co+1]]<thresh]=0
            thresh_array[height_sect[ro]:height_sect[ro+1],widht_sect[co]:widht_sect[co+1]][array[height_sect[ro]:height_sect[ro+1],widht_sect[co]:widht_sect[co+1]]>=thresh]=1
        

    return thresh_array




plt.rcParams['figure.dpi'] = 300

start = time.time()

img = cv2.imread(r"G:\Meine Ablage\Studium\Synchronisation\9. Semester\Algorithm Engineering_all\algorithm-engineering\project\input_img.ppm")
img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)#[:400,:500]
# plt.imshow(img)
# plt.show()
p1 = time.time()

normalized_img= moving_norm(img,15)

p2=time.time()

# plt.title("moved 21")
# plt.imshow(normalized_img)
# plt.show()
# plt.hist(normalized_img.flatten(),bins=80)
# plt.show()

#adaptive_mean_filter is not recommeded to use right now!
#mean_filtered_img=adaptive_mean_filter(normalized_img,size=13,factor=0.005,brightness=1.2)

thresh_img=adaptive_thresh(normalized_img,img,100,0.5,0.84,0.01)
p3=time.time()


plt.imshow(thresh_img,cmap='gray')

plt.savefig('python_output_img.png', dpi=200)

end=time.time()

print('Load ppm in python into vector array: ', 1000*round(p1-start,2), 'ms')
print('time moved norm: ', 1000*round(p2-p1,2),'ms')
print('apply adaptive threshold: ', 1000*round(p3-p2,2),'ms')
print('create png file: ', 1000*round(end-p3,2),'ms')
print('Whole programm: ', 1000*round(end-start,2),'ms')





