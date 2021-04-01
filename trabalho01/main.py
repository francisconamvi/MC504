from ctypes import *
import numpy as np
import numpy.ctypeslib as ctl
import sys

import matplotlib.pyplot as plt
import numpy as np

import os
import glob

lib_path = './main_c.so'
hdl = CDLL(lib_path)

M,N = list(map(int,input().split()))
a, b, c, d = list(map(int,input().split()))
S = int(input())

###creating h
h = M*[N*[0]]
h = np.array(h, dtype=np.intc)
h_ = (h.__array_interface__['data'][0] 
    + np.arange(h.shape[0])*h.strides[0]).astype(np.uintp)

###creating h
h2 = M*[N*[0]]
h2 = np.array(h2, dtype=np.intc)
h2_ = (h2.__array_interface__['data'][0] 
    + np.arange(h2.shape[0])*h2.strides[0]).astype(np.uintp)

###function that inicialize h
inicialize_ = hdl.inicialize_h
inicialize_.argtypes = [ctl.ndpointer(dtype=np.uintp, ndim=1, flags='C'), c_int, c_int,
    c_int,c_int,c_int, c_int]
inicialize_(h_,M,N,a,b,c,d)
print("Step 0")

myimg = plt.imshow(h, cmap='jet', interpolation='nearest')
plt.colorbar()

files = glob.glob('./img/*')
for f in files:
    os.remove(f)
plt.savefig("img/0")
plt.pause(0.001)

###step funcion
next_step_ = hdl.next_step
next_step_.argtypes = [ctl.ndpointer(dtype=np.uintp, ndim=1, flags='C'),
                       ctl.ndpointer(dtype=np.uintp, ndim=1, flags='C'),
                       c_int, c_int]

###copy matrix funcion
copy_matrix_ = hdl.copy_matrix
copy_matrix_.argtypes = [ctl.ndpointer(dtype=np.uintp, ndim=1, flags='C'),
                       ctl.ndpointer(dtype=np.uintp, ndim=1, flags='C'),
                       c_int, c_int]

for s in range(1,S):
    print("Step %d" %(s))
    next_step_(h_, h2_, M, N)
    copy_matrix_(h2_,h_,M,N)
    # print(h)

    myimg.set_data(h)
    plt.draw()
    plt.savefig("img/%d" %s)
    plt.pause(0.001)