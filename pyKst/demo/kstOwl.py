#!/bin/python
import sip
sip.setapi('QString', 1)
import pykst as kst
import sys
import urllib.request as urllib2
import json
import math
import xml.dom.minidom
import ctypes
from PyQt4 import QtCore, QtNetwork, QtGui

class KstOwl(QtCore.QObject) :
  labels=[]
  def __init__(self,client) :
    super(KstOwl, self).__init__(None)
    self.client=client
    self.ctimer = QtCore.QTimer()
    QtCore.QObject.connect(self.ctimer, QtCore.SIGNAL("timeout()"), self.update)
    self.ctimer.start(30)
    for i in range(1000):
      self.labels.append(0)
    
    css = urllib2.urlopen("http://hen.astro.utoronto.ca/cgi-bin/owl-cgi?blast2010:css")
    x=QtCore.QString(css.read()).split("\n")
    i=0
    bg = list()
    fg = list()

    for i in range(0,x.count()):
      if((i+1)%3==1):
        y=x[i]
        y.remove(0,y.indexOf("color:")+6)
        z=y
        y.remove(y.indexOf(";"),99999)
        bg.append(y)
        z.remove(0,z.indexOf(":")+1)
        z.remove(z.indexOf(";")+1,99999)
        fg.append(y)
    
    response = urllib2.urlopen("http://hen.astro.utoronto.ca/cgi-bin/owl-cgi?blast2010:layout")
    dom=xml.dom.minidom.parseString(response.read())
    for n in dom.getElementsByTagName("Owl"):
      for nn in n.getElementsByTagName("top"):
        top=nn.firstChild.toxml()
      for nn in n.getElementsByTagName("left"):
        left=nn.firstChild.toxml()
      for nn in n.getElementsByTagName("width"):
        width=nn.firstChild.toxml()
      for nn in n.getElementsByTagName("height"):
        height=nn.firstChild.toxml() 
      rt=(float(top)+float(height)/2.0)/1200.0
      rl=(float(left)+float(width)/2.0)/1200.0
      rx=(float(width))/1200.0
      ry=(float(height))/1200.0
      self.owl=kst.Picture(client,"/home/joshua/owl0.png",rl,rt,math.sqrt(rx*rx+ry*ry)/2)
    for n in dom.getElementsByTagName("PBox"):
      for nn in n.getElementsByTagName("top"):
        top=nn.firstChild.toxml()
      for nn in n.getElementsByTagName("left"):
        left=nn.firstChild.toxml()
      for nn in n.getElementsByTagName("width"):
        width=nn.firstChild.toxml()
      for nn in n.getElementsByTagName("height"):
        height=nn.firstChild.toxml()
      for nn in n.getElementsByTagName("title"):
        title=nn.firstChild.toxml()
      for nn in n.getElementsByTagName("boxStyle"):
        style=nn.firstChild.toxml()
      rt=(float(top)+float(height)/2.0)/1200.0
      rl=(float(left)+float(width)/2.0)/1200.0
      rx=(float(width))/1200.0
      ry=(float(height))/1200.0
      box=kst.Box(client,"false",rl,rt,rx,ry,0,bg[int(style)],1,1,1,"black",1,1,1)
      label=kst.Label(client,title,6,"false","false",fg[int(style)],"Serif",(float(left)+5.0)/1200.0,(float(top)+float(4)/2.0)/1200.0,0,3,3,0,0)
      pos=14
      maxl=0
      c=0
      ids=list()
      for nn in n.getElementsByTagName("PDataItem"):
        c+=1 #c++
        for nnn in nn.getElementsByTagName("capStyle"):
          style=nnn.firstChild.toxml()
        for nnn in nn.getElementsByTagName("caption"):
          caption=nnn.firstChild.toxml()
        for nnn in nn.getElementsByTagName("dataID"):
          ids.append(int(nnn.firstChild.toxml()))
        label=kst.Label(client,caption,6,"false","false",fg[int(style)],"Serif",(float(left)+5.0)/1200.0,(float(top)+pos+float(8)/2.0)/1200.0,0,3,3,0,0)
        pos+=14
        x=QtGui.QFontMetrics(QtGui.QFont("Serif",8)).width(caption)
        maxl=x if x>maxl else maxl
      i=0
      pos=14
      for i in range(0,c):
        self.labels[ids[i]]=kst.GeneratedString(client,"Loading","owl"+str(ids[i]))
        kst.Label(client,"[owl"+str(ids[i])+"]",6,"false","false","Black","Serif",(float(left)+5.0+maxl)/1200.0,(float(top)+pos+float(8)/2.0)/1200.0,0,3,3,0,0)
        pos+=14

  def update(self):
    response = urllib2.urlopen("http://hen.astro.utoronto.ca/cgi-bin/owl-cgi?blast2010:data")
    objects=json.loads(str(response.read().decode('utf8')))
    self.owl.setPic(QtCore.QString("/home/joshua/owl"+str(objects["owlData"]["owlStage"])+".png"))
    for o in objects["owlData"]["Obj"]:
      x=QtCore.QString(str(o))
      if x.contains("{\'i\': "):
        y=QtCore.QString(str(x))
        y.remove(0,6)
        y.remove(y.indexOf(","),99999)
        z=QtCore.QString(str(x))
        z.remove(0,z.lastIndexOf(": ")+3)
        z.chop(2)
        if z.contains("src")==False:
          self.labels[int(str(y))].setValue(z)

client=kst.Client()
app=QtGui.QApplication(sys.argv)
owl=KstOwl(client)
app.exec_()
