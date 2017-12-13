#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("RenameDemo")
V1=client.new_data_vector("/home/cbn/programs/KDE/kst_tutorial/gyrodata.dat",
                          field="INDEX",
                          start=0,num_frames=1000)

V2=client.new_data_vector("/home/cbn/programs/KDE/kst_tutorial/gyrodata.dat",
                          field="Column 2",
                          start=0,num_frames=1000)

c1 = client.new_curve(V1, V2)

p1 = client.new_plot(font_size = 12)

p1.add(c1)

print "-------------- Scalar list -------------"
print client.get_scalar_list()

print "-------------- Vector list -------------"
vectors = client.get_vector_list()
print vectors
print "----------"

# change the name of the vector made from field "Column 1"
for Vname in vectors:
  Vr = client.data_vector(Vname.name())
  if Vr.field() == "Column 2":
    print "Changing name of ", Vr.name(), " to A Gyro" 
    Vr.set_name("A Gyro")
    
