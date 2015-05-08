#!/usr/bin/python2.7
import pykstplot as plt
#import matplotlib.pyplot as plt
from numpy  import *

# create a pair of numpy arrays
x = linspace( -10, 10, 100)
y = sin(x)
z = cos(x)

plt.plot(x,y,"k.",x,z,"m*")
plt.plot(x,y*y,"r-")
plt.show()
