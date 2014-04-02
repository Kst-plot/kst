#!/usr/bin/python2.7
import pykst as kst
from datetime import datetime
from numpy import *
from PyQt4 import QtCore, QtNetwork, QtGui
from time import sleep

client=kst.Client("DataObjects2")

#I1 = kst.Picture(client, "/home/cbn/cropped.jpeg",0.7, 0.3, 0.3, 0)

P1=kst.Plot(client,0.25,0.25,0.5,0.5,0, "P1")
P1.setXRange(-10.0,10.0)
P1.setGlobalFont("courier",False,False)
P1.setName("Fred the Plot")

#C1 = kst.Circle(client, 0.7,0.7,0.1, strokeBrushColor="red")
B1 = kst.Box(client, "false", 0.1, 0.1, 0.1, 0.1, fillColor="pink")
#B1.setFixedAspectRatio(False)
E1 = kst.Ellipse(client, "false", 0.1, 0.7, 0.1, 0.1, 45)
A1 = kst.Arrow(client, 0.1, 0.5, 0.1, 45, False, True, 18)
#L2 = kst.Line(client, 0.1, 0.5, 0.1, 90)
#L3 = kst.Line(client, 0.1, 0.5, 0.1, 45)
#L4 = kst.Line(client, 0.1, 0.5, 0.1, -45)
#I1 = kst.Picture(client, "/home/cbn/cropped.jpeg",0.7, 0.3, 0.3, 0)
#I1.setPicture("/home/cbn/netterfield.jpg")
#S1 = kst.SVG(client, "/home/cbn/drawing.svg",0.9, 0.2, 0.2, 0)
Label = kst.Label(client, "Label Here", 16, fontFamily="courier", posX=0.3,posY=0.7)
Label.setFontItalic(True)

print P1.name()
