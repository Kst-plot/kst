#!/usr/bin/python
import pykst as kst
from datetime import datetime
from numpy import *
from PyQt4 import QtCore, QtNetwork, QtGui
from time import sleep

client=kst.Client()
dt=datetime.now()
vec=kst.GeneratedVector(client,0,1000,100000,"Alfred")
print("Took "+str((datetime.now()-dt))+" to generate an 100000-sample vector.")
dt=datetime.now()
pyar=vec.getNumPyArray()
print("Took "+str((datetime.now()-dt))+" to import an 100000-sample vector.")
dt=datetime.now()
e=kst.EditableVector(client)
e.setFromList(pyar)
print("Took "+str((datetime.now()-dt))+" to export an 100000-sample vector.")
client.plot(vec,e)
