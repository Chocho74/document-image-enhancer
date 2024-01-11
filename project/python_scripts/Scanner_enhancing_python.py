# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import matplotlib.pyplot as plt
import numpy as np
import cv2
from scipy.signal import convolve2d
import time
from scipy.interpolate import interp1d

def moving_norm_scipy(array,size):
    kernel=np.ones((size,size))/(size**2)
    
    normed_array = convolve2d(array, kernel, mode='same', boundary='symm')
    normed_array = array/normed_array
    
    return normed_array
    
def moving_norm(array,size):
    
    x = np.array([0,0.2,0.5,1,1.2,1.5,1000])
    y = np.array([0,0.1,0.4,1.1,1.2,1.3,1.3])
    adjustment_func= interp1d(x, y) 

    rows,columns=np.shape(array)
    h_size=int((size-1)/2)       

    normed_array=np.copy(array).astype(float)
    
    big_array = np.empty((2*h_size+rows,2*h_size+columns))
    big_array[:] = np.nan
    big_array[h_size:h_size+rows,h_size:h_size+columns]=array

    for y in range (h_size,rows+h_size):
        for x in range (h_size,columns+h_size):
            temp_mean=np.nanmean(big_array[y-(h_size):y+(h_size+2),x-(h_size):x+(h_size+2)])
            normed_array[y-h_size,x-h_size]=adjustment_func(big_array[y,x]/temp_mean)

    return normed_array


def adaptive_mean_filter(array,size=21,factor=1,brightness=1.2):
    
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

def adaptive_thresh(array,sections):
    
    x = np.array([0,18,60,70,77,100])
    y = np.array([1.1,1.05,0.98,0.96,0.92,0.86])
    f = interp1d(x, y)
    
    rows,columns=np.shape(array)
    widht_sect=np.linspace(0,columns,sections+1,endpoint=True).astype(int)
    height_sect=np.linspace(0,rows,sections+1,endpoint=True).astype(int)
    
    thresh_array=np.copy(array).astype(float)
    
    for ro in range(sections):
        for co in range(sections):
            a=plt.hist(array[height_sect[ro]:height_sect[ro+1],widht_sect[co]:widht_sect[co+1]].flatten(),bins=100)[0]
            index=np.where(a==np.max(a))[0][0]
            thresh=f(index)
            thresh_array[height_sect[ro]:height_sect[ro+1],widht_sect[co]:widht_sect[co+1]][array[height_sect[ro]:height_sect[ro+1],widht_sect[co]:widht_sect[co+1]]<thresh]=0
            thresh_array[height_sect[ro]:height_sect[ro+1],widht_sect[co]:widht_sect[co+1]][array[height_sect[ro]:height_sect[ro+1],widht_sect[co]:widht_sect[co+1]]>=thresh]=1

    return thresh_array


plt.rcParams['figure.dpi'] = 300

img = cv2.imread("input_img.ppm")
img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)#[:400,:500]
plt.imshow(img)
plt.show()
start = time.time()

move_normed_scipy= moving_norm(img,15)
# plt.title('41')
# plt.imshow(move_normed)
# plt.show()
# print(np.max(move_normed))
# print(np.min(move_normed))

intermediat=time.time()

# move_normed_scipy=moving_norm_scipy(img,15)
# #move_normed_scipy*= 256/np.max(move_normed_scipy)
plt.title("moved 21")
plt.imshow(move_normed_scipy)
plt.show()
plt.hist(move_normed_scipy.flatten(),bins=80)
plt.show()

mean_filtered_img=adaptive_mean_filter(move_normed_scipy,size=13,factor=0.005,brightness=1.2)
plt.title("mean 17")
plt.imshow(mean_filtered_img,cmap='gray')
plt.show()
plt.hist(mean_filtered_img.flatten(),bins=80)
plt.show()

thresh=1.00
copy=np.copy(mean_filtered_img)
copy[mean_filtered_img<thresh]=0
copy[mean_filtered_img>=thresh]=1

#fig=plt.figure(figsize=(10,12),dpi=200)
plt.imshow(copy,cmap='gray')
plt.show()
# filtered_image = ex4_adaptive_median_filter(copy, 15)
# plt.imshow(move_normed_scipy)
# plt.show()

thresh_img=adaptive_thresh(mean_filtered_img,7)
plt.imshow(thresh_img,cmap='gray')
plt.show()

end=time.time()
print('time moved norm: ', intermediat-start)
print('time moved norm scipy: ', end-intermediat)





