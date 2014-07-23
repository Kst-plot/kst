#!/usr/bin/python2.7
import pykst as kst
from numpy  import *
from PySide import *

client=kst.Client("TestMatrix")
m1=client.new_data_matrix("/home/cbn/programs/kst/tests/fitsimage_testcase/test.fits",
                          field="flux")


i1 = client.new_image(m1)

p1 = client.new_plot((0.25, 0.5), (0.5, 1.0))
p1.add(i1)

print m1.min(), m1.max(), m1.mean(), m1.width(), m1.height()

I = identity(5)

m2 = client.new_editable_matrix(I)
i2 = client.new_image(m2)
p2 = client.new_plot((0.75, 0.5), (0.5, 1.0))
p2.add(i2)

npM = m2.get_numpy_array()
print npM

