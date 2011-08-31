#!/bin/python
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
    self.plot=kst.Plot(client,0.5,0.4885,0.9,0.8,0,"KsNspirePLOT")
    self.genVec=kst.GeneratedVector(self.client,-100,100,1000)
    
  def create(self):
    kst.NewEquation(self.client,self.text,"["+self.genVec.handle+"]",True,"black","0","1",True,False,"","",False,"","",False,"","",False,"KsNspirePLOT",False,False,"","","")
    
  def changeValue(self):
    strx=QtCore.QString(self.s2.read(8000))
    if strx.contains("valueSet:"):
      strx.remove("valueSet:")
      self.text=str(strx)

client=kst.Client()
app=QtGui.QApplication(sys.argv)
m=KsNspire(client)

app.exec_()
