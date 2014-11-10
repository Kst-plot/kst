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

p1 = client.new_plot()

p1.add(c1)

print V2.length()

print client.get_scalar_list()

print "--------------"
vectors = client.get_vector_list()
print vectors

print vectors[1]

for Vname in vectors:
  Vr = client.data_vector(Vname)
  if Vr.field() == "Column 2":
    Vr.set_name("A Gyro")
    


