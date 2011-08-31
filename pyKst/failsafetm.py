import sip
sip.setapi('QString', 1)
import pykst as kst
c=kst.Client()
v=kst.EditableVector(c)
v.setFromList([1,2,3,4,5,6,7,8,9,10])
for i in range(0,10):
  print(v.value(i))
