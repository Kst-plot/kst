#!/usr/bin/python2.7
import pykst as kst
from numpy  import *
import time

client=kst.Client("VectorIO")

t0 = time.clock()

# create a pair of numpy arrays
x = linspace( 0, 50, 500000)
y = sin(x)

t1 = time.clock()

# copy the numpy arrays into kst and plot them
V1 = client.new_editable_vector(x)
V2 = client.new_editable_vector(y)
c1 = client.new_curve(V1, V2)
p1 = client.new_plot()
p1.add(c1)

t2 = time.clock()

# copy numpy array back into python.
A = V2.get_numpy_array()

t3 = time.clock()

# manipulate the array in python, and plot it in kst
A = A*A
V3 = client.new_editable_vector(A)
c2 = client.new_curve(V1, V3)
p1.add(c2)

# manipulate it again, and replace it in kst
A = A/2
V2.load(A)

print "creation of numpy arrays took", t1 - t0, "s"
print "copying onto kst and plotting took", t2-t1, "s"
print "copying from kst into python took:", t3-t2, "s"

