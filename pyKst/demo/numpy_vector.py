#!/usr/bin/python2.7
import pykst as kst
from numpy  import *

client=kst.Client("NumpyVector")

# create a pair of numpy arrays
x = linspace( -10, 10, 1000)
y = sin(x)

# copy the numpy arrays into kst and plot them
V1 = client.new_editable_vector(x, name="X")
V2 = client.new_editable_vector(y, name="sin(X)")
c1 = client.new_curve(V1, V2)
p1 = client.new_plot()
p1.add(c1)
