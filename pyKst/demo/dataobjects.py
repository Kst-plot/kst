#!/usr/bin/python2.7
import pykst as kst
from datetime import datetime
from numpy import *
from PyQt4 import QtCore, QtNetwork, QtGui
from time import sleep

client=kst.Client("DataObjects")

P1=kst.Plot(client,0.25,0.25,0.5,0.5,"P1")
P2=kst.Plot(client,0.75,0.25,0.5,0.5,"P2")
P3=kst.Plot(client,0.25,0.75,0.5,0.5,"P3")
P4=kst.Plot(client,0.75,0.75,0.5,0.5,"P4")

dataVectorIndex=kst.DataVector(client,"/data/blast2003",field="INDEX",changeDetection=False,timeInterval=True,dontUpdate=False,start=0,drange=1000)
dataVectorGY1=kst.DataVector(client,"/data/blast2003",field="GYRO1",changeDetection=False,timeInterval=True,dontUpdate=False,start=0,drange=1000)
GYCurve=kst.NewCurve(client,dataVectorIndex, dataVectorGY1, curvecolor="blue", curveweight=1, placeinexistingplot=P1)

GYEquation=kst.NewEquation(client, "x^2", dataVectorIndex)
GYEqCurve = kst.NewCurve(client,GYEquation.X(), GYEquation.Y(), curvecolor="black", curveweight=1, placeinexistingplot=P2)

GYSpectrum=kst.NewSpectrum(client, dataVectorGY1, length = 12, vectorUnits="^o/s", rateUnits="Hz", rate=100.16)
GYSpecCurve = kst.NewCurve(client,GYSpectrum.X(), GYSpectrum.Y(), curvecolor="green", curveweight=1, placeinexistingplot=P3)

GYHist=kst.NewHistogram(client, dataVectorGY1, rtAutoBin=True)
GYHistCurve = kst.NewCurve(client,GYHist.X(), GYHist.Y(), curvecolor="black", curveweight=1, uselines=False, usebargraph=True,bargraphfill="green",placeinexistingplot=P4)

LFit=kst.NewExponentialFit(client, dataVectorIndex, dataVectorGY1, dataVectorGY1)
GYFitCurve = kst.NewCurve(client,dataVectorIndex, LFit.Fit(), curvecolor="black", curveweight=1, placeinexistingplot=P1)

Fit=kst.NewGaussianFit(client, GYHist.X(), GYHist.Y(), GYHist.Y())
GYFitCurve = kst.NewCurve(client,GYHist.X(), Fit.Fit(), curvecolor="black", curveweight=1, placeinexistingplot=P4)

#, curvecolor="red", curveweight=1, placeinexistingplot=P1.handle)
