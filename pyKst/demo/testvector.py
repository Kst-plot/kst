#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("TestVectors")
V1=client.new_data_vector("/home/cbn/programs/KDE/kst_tutorial/gyrodata.dat",
                          field="INDEX",
                          start=0,NFrames=1000)

V2=client.new_data_vector("/home/cbn/programs/KDE/kst_tutorial/gyrodata.dat",
                          field="Column 2",
                          start=0,NFrames=1000)

c1 = client.new_curve(V1, V2)

p1 = client.new_plot((0.5, 0.5), (1.0, 1.0))
p1.add(c1)

print V2.length()

print client.get_scalar_list()