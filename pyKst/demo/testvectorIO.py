#!/usr/bin/python2.7
import pykst as kst
from numpy  import *
from PySide import *
import time
import tempfile

client=kst.Client("TestVectorIO")

t0 = time.clock()

x = linspace( 0, 50, 500000)
y = sin(x)

t1 = time.clock()

print t1 - t0

V1 = client.new_editable_vector(x)
V2 = client.new_editable_vector(y)

c1 = client.new_curve(V1, V2)

p1 = client.new_plot((0.5, 0.5), (1.0, 1.0))
p1.add(c1)



t2 = time.clock()

print t2-t1


A = V2.get_numpy_array()

A = A*A

t3 = time.clock()

V3 = client.new_editable_vector(A)

c2 = client.new_curve(V1, V3)
p1.add(c2)

t4 = time.clock()

print t4-t3

V2.load(A)
