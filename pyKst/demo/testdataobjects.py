#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("TestPlugins")

v1 = client.new_generated_vector(0, 10, 10)
v4 = client.new_generated_vector(0, 20, 100)

e1 = client.new_equation(v1, "x^2")
e1.set_x(v4)
v2 = e1.Y()
v3 = e1.X()
print v2.name(), v2.value(3), v3.value(3)
