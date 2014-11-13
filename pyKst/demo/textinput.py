#!/usr/bin/python

# demonstrate buttons and line inputs:
# plot an equation the user inputs

import sip
sip.setapi('QString', 1)
import pykst as kst
import sys
import random
from PyQt4 import QtCore, QtNetwork, QtGui

class KsNspire:
  text=""
  def __init__(self,client):
    self.client=client
    self.s=QtNetwork.QLocalSocket()
    self.s.readyRead.connect(self.create)
    self.s2=QtNetwork.QLocalSocket()
    self.s2.readyRead.connect(self.changeValue)
    self.l=kst.LineEdit(client,"",self.s2,0.47,0.975,0.93,0.025)
    self.b=kst.Button(client,"Go!",self.s,0.97,0.975,0.05,0.025)
    self.plot=client.new_plot((0.5,0.4885),(0.9,0.8))
    self.genVec=client.new_generated_vector(-100,100,1000)
    
  def create(self):
    eq = client.new_equation(self.genVec, self.text)
    c = client.new_curve(eq.x(), eq.y())
    self.plot.add(c)

  def changeValue(self):
    strx=QtCore.QString(self.s2.read(8000))
    if strx.contains("valueSet:"):
      strx.remove("valueSet:")
      self.text=str(strx)

client=kst.Client()
app=QtGui.QApplication(sys.argv)
m=KsNspire(client)

app.exec_()
