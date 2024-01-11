# -*- coding: utf-8 -*-
"""
Created on Sat Mar 11 20:46:19 2023

@author: Eric Günl
"""

import matplotlib.pyplot as plt
import numpy as np


x=np.arange(0,100,0.1)

def func(x,n,m):
    return x*m+n

y=func(x,100,-0.7)

t=[200,600]
for i in t:
    for o in range(30):
        y[i+o]=y[i]-7


def moving_norm(f,size):
    
    normed_array=np.copy(f).astype(float)
    le=f.shape[0]
    pad=int((size-1)/2)
    big_arr=np.empty(le+2*pad)
    big_arr[:]=np.nan
    
    big_arr[pad:le+pad]=f
    
    for i in range(pad,pad+le):
        temp_mean=np.nanmean(big_arr[i-pad:i+pad+2])
        normed_array[i-pad]=big_arr[i]/temp_mean
        
    return normed_array

plt.rcParams['figure.dpi'] = 300

fig, (ax0, ax1) = plt.subplots(nrows=2, ncols=1, sharex=True,
                                    figsize=(12, 8))

ax0.set_title("original data")
ax0.plot(x,y,label='original data')
ax0.grid()
ax0.set_ylim([0,110])

ax1.set_title("data after moving norm")
ax1.plot(x,moving_norm(y,150),label='data after moving norm')
ax1.set_ylim([0.5,1.5])
ax1.grid()

#plt.savefig("image to show effect of moving norm.png", dpi=250)