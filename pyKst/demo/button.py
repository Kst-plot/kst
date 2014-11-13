#!/usr/bin/python

# pressing a button makes random circles appear.

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
    client.new_circle((random.random(),random.random()), random.random()/10.0,
                      "#"+str(random.randint(0,9))+str(random.randint(0,9))+str(random.randint(0,9)))
                      
client=kst.Client("testbutton")
app=QtGui.QApplication(sys.argv)
m=Magic(client)

s=QtNetwork.QLocalSocket()
s.readyRead.connect(m.test)
b=kst.Button(client,"Click Here",s,0.5,0.5,0.2,0.1)
app.exec_()
