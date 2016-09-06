#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("TestVectors")
V1=client.new_data_vector("/home/cbn/programs/KDE/kst_tutorial/gyrodata.dat",
                          field="INDEX",
                          start=0,num_frames=1000)

V2=client.new_data_vector("/home/cbn/programs/KDE/kst_tutorial/gyrodata.dat",
                          field="Column 2",
                          start=0,num_frames=1000)

c1 = client.new_curve(V1, V2)
p1 = client.new_plot(font_size = 12)
p1.add(c1)

V1.change_frames(1000, 500, 0, False)
V2.change_frames(1000, 500, 0, False)
