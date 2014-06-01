#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("TestPlugins")

v1 = client.new_generated_vector(0, 10, 10)

e1 = client.new_equation(v1, "x^2")
