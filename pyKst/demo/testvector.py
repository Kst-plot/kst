#!/usr/bin/python2.7
import pykst as kst
from datetime import datetime
from numpy import *
from PyQt4 import QtCore, QtNetwork, QtGui
from time import sleep

client=kst.Client("DataObjects2")
dataVectorGY1=kst.DataVector(client,"/home/cbn/programs/KDE/kst_tutorial/gyrodata.dat",field="Column 1",changeDetection=False,timeInterval=True,dontUpdate=False,start=0,drange=1000,skip=0)
