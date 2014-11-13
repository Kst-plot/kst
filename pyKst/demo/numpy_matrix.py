#!/usr/bin/python
import pykst as kst
from numpy import *
#from PyQt4 import QtCore, QtNetwork, QtGui

def mandelbrot( h,w, maxit=10 ):
  '''Returns an image of the Mandelbrot fractal of size (h,w).
  '''
  y,x = ogrid[ -1.4:1.4:h*1j, -2:0.8:w*1j ]
  c = x+y*1j
  z = c
  divtime = maxit + zeros(z.shape, dtype=float64)

  for i in xrange(maxit):
    z  = z**2 + c
    diverge = z*conj(z) > 2**2            # who is diverging
    div_now = diverge & (divtime==maxit)  # who is diverging now
    divtime[div_now] = i                  # note when
    z[diverge] = 2.0                        # avoid diverging too much

  return divtime

client=kst.Client("numpy_matrix_demo")
np = mandelbrot(1000,1000)

M = client.new_editable_matrix(np)
I = client.new_image(M)
P = client.new_plot()
P.add(I)

