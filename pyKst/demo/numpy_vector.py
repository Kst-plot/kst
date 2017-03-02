#!/usr/bin/python2.7
import pykst as kst
import numpy as np

client=kst.Client("NumpyVector")

# create a pair of numpy arrays
x = np.linspace( -10, 10, 1000)
y = np.sin(x)

# copy the numpy arrays into kst and plot them
V1 = client.new_editable_vector(x, name="X")
V2 = client.new_editable_vector(y, name="sin(X)")
c1 = client.new_curve(V1, V2)
p1 = client.new_plot()
p1.add(c1)

V3 = client.new_generated_vector(-10*180/3.1415926, 10*180/3.1415926, 100)
c2 = client.new_curve(V3, V2)
p2 = client.new_plot()
p2.add(c2)


# print out the name of every vector.
vectors = client.get_vector_list()
print "----- Vectors: ----------"
for vector in vectors:
  print vector.name()

# print out the name of ediable vectors.
vectors = client.get_editable_vector_list()
print "----- Editable Vectors: ----------"
for vector in vectors:
  print vector.name()

# print out the name of generated vectors.
vectors = client.get_generated_vector_list()
print "----- Generated Vectors: ----------"
for vector in vectors:
  print vector.name()
  
# print out every scalar name and value
scalars = client.get_scalar_list()
print "----- Scalars: ----------"
for scalar in scalars:
  print scalar.name(), scalar.value()
  

  
