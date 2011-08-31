#!/bin/python
import sip
sip.setapi('QString', 1)
import pykst as kst
import sys
import random
from PyQt4 import QtCore, QtNetwork, QtGui

class Magic:
  def __init__(self,client):
    self.client=client
  def test(self):
    self.client=client
    random.seed()
    kst.Circle(self.client,random.random(),random.random(),random.random()/10.0,random.random()*180,"#"+str(random.randint(0,9))+str(random.randint(0,9))+str(random.randint(0,9)),1,1,4,"#"+str(random.randint(0,9))+str(random.randint(0,9))+str(random.randint(0,9)))

client=kst.Client()
app=QtGui.QApplication(sys.argv)
m=Magic(client)

s=QtNetwork.QLocalSocket()
s.readyRead.connect(m.test)
b=kst.Button(client,"Hello World!",s,0.5,0.5,0.2,0.1)
app.exec_()
