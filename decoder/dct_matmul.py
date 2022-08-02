# -*- coding: utf-8 -*-

import numpy as np, arrayfire as af, matplotlib.pyplot as pl
from scipy.fftpack import dct, idct
from time import time

def dct_mat(size):
	coord = af.iota(1,size,tile_dims=(size,1))
	coeffs = np.sqrt(2./size) * af.cos(np.pi*(2.*coord+1.)*af.transpose(coord)/2./size)
	coeffs[0,:] = np.sqrt(1./size)
	coord=None
	return coeffs

def dct_mm(arr,inverse=False):
	N, M = arr.dims()
	Cm = dct_mat(M)
	Cn = dct_mat(N)
	if inverse:
		out = af.matmul(af.matmulTN(Cn,arr),Cm)
	else:
		out = af.matmulNT(af.matmul(Cn,arr),Cm)
	Cm=None; Cn=None
	return out

M = 2000
N = 2500
k = 10

ah = np.random.rand(N,M).astype(np.float32)
ad = af.to_array(ah)

b = dct_mm(ad,inverse=False); af.device.sync()
t0 = time()
for i in range(k):
	b = dct_mm(ad,inverse=False)
af.device.sync()
t1 = time()-t0
af.device.device_gc()
print('af:\t%.3fs'%(t1/k))

c = dct(dct(ah, axis=0, norm='ortho'), axis=1, norm='ortho')
t0 = time()
for i in range(k):
	c = dct(dct(ah, axis=0, norm='ortho'), axis=1, norm='ortho')
t2 = time()-t0
print('scipy:\t%.3fs'%(t2/k))

print( 'norm:\t%.3f with %i elements'%(np.linalg.norm(b.__array__()-c),M*N) )
#pl.imshow(b.__array__()-c)
#pl.colorbar()
#pl.show()
