import sip
sip.setapi('QString', 1)
import sys
import math
import os
import ctypes
from time import sleep
from PyQt4 import QtCore, QtNetwork
from numpy import *
from pykstpp import * 

def b2str(val):
  if isinstance(val, bool):
    return "True" if val else "False"
  else:
    return str(val)

class Client:
  """ This class is an interface to a running kst session. Every convenience class inside pykst accepts an instance of Client which it
  uses to interact with a kst session. In addition, it holds functions which effect the entire kst session.
      
  If serverName is specified, creates a connection to either a running kst session with serverName, or if none exists, a new one.
  If serverName is not specified, creates a connection to either the kst session with the name "kstScript", or if none exists, a new one. """
  
  def __init__(self,serverName="kstScript"):
    self.ls=QtNetwork.QLocalSocket()
    self.ls.connectToServer(serverName)
    self.ls.waitForConnected(300)
    self.serverName=serverName
    if self.ls.state()==QtNetwork.QLocalSocket.UnconnectedState:
      os.system("kst2 --serverName="+str(serverName)+"&")
      while self.ls.state()==QtNetwork.QLocalSocket.UnconnectedState:
        self.ls.connectToServer(serverName)
        self.ls.waitForConnected(300)
        self.serverName=serverName
    
  def send(self,command):
    """ Sends a command to kst and returns a response. You should never use this directly, as there is no guarantee that the internal command
        list kst uses won't change. Instead use the convenience classes included with pykst. """
    ba=QtCore.QByteArray(b2str(command))
    self.ls.write(ba)
    self.ls.flush()
    self.ls.waitForReadyRead(300000)
    x=self.ls.readAll()
    return x
    
  def send_si(self, handle, command):
    self.send(b2str("beginEdit("+handle.toAscii()+")"))
    x = self.send(command)
    self.send(b2str("endEdit()"))
    return x

  def clear(self):
    """ Equivalent to file->close from the menubar inside kst.  Clears all objects from kst."""
    self.send("clear()")
  def screenBack(self):
    """ Equivalent to "Range>Back One Screen" from the menubar inside kst. """
    self.send("screenBack()")
  def screenForward(self):
    """ Equivalent to "Range>Forward One Screen" from the menubar inside kst. """
    self.send("screenForward()")
  def countFromEnd(self):
    """ Equivalent to "Range>Count From End" from the menubar inside kst. """
    self.send("countFromEnd()")
  def readToEnd(self):
    """ Equivalent to "Range>Read To End" from the menubar inside kst. """
    self.send("readToEnd()")
  def setPaused(self):
    """ Equivalent to checking "Range>Pause" from the menubar inside kst if "Range>Pause" is unchecked, otherwise no action. """
    self.send("setPaused()")
  def unsetPaused(self):
    """ Equivalent to unchecking "Range>Pause" from the menubar inside kst if "Range>Pause" is checked, otherwise no action. """
    self.send("unsetPaused()")
  def tabCount(self):
    """ Get the number of tabs open in the current document. """
    return self.send("tabCount()")
  def newTab(self):
    """ Create a new tab in the current document and switch to it. """
    return self.send("newTab()")
  def setTab(self,tab):
    """ Set the index of the current tab. It must be greater or equal to 0 and less than tabCount(). """
    self.send("setTab("+b2str(tab)+")")


class NamedObject:
    """ This is a class which some convenience classes within pykst use. You should not use it directly."""
    def __init__(self,client):
      self.client=client
    def setName(self,name):
      self.client.send_si(self.handle, b2str("setName("+b2str(name)+")"))
    def name(self):
      return self.client.send_si(self.handle, "name()")


class String(NamedObject) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly. """
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def value(self) :
    """ Returns the string. """
    return self.client.send_si(self.handle, "value()")

class GeneratedString(String) :
  """ This class represents a string you would create via "Create>String>Generate" from the menubar inside kst.

  The parameters of this function mirror the parameters within "Create>String>Generate".

  To import the string Hello World into kst::

    import pykst as kst
    client=kst.Client()
    s=kst.GeneratedString(client,"Hello World")"""
  def __init__(self,client,string,name="") :
    String.__init__(self,client)

    self.client.send("newGeneratedString()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setValue(string)
    self.setName(name)

  def setValue(self,val):
    self.client.send_si(self.handle, b2str("setValue("+b2str(val)+")"))

class DataSourceString(String) :
  """ This class represents a string you would create via "Create>String>Read from Data Source" from the menubar inside kst."""

  def __init__(self,client,filename,field,name="") :
    String.__init__(self,client)
    self.client.send("newDataString()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.change(filename, field)

  def change(self,filename,field):
    self.client.send_si(self.handle, b2str("change("+b2str(filename)+","+b2str(field)+")"))


class Scalar(NamedObject) :
  """ This is a class which some convenience classes within pykst use. You should not use it directly. """
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def value(self) :
    """ Returns the scalar. """
    return self.client.send_si(self.handle, "value()")

class GeneratedScalar(Scalar) :
  """ This class represents a scalar you would create via "Create>Scalar>Generate" from the menubar inside kst.

  The parameters of this function mirror the parameters within "Create>Scalar>Generate".

  To import the scalar of value 42 into kst::

    import pykst as kst
    client=kst.Client()
    s=kst.GeneratedScalar(client,42)"""
  def __init__(self,client,value,name="") :
    Scalar.__init__(self,client)

    self.client.send("newGeneratedScalar()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setValue(value)
    self.setName(name)

  def setValue(self,val):
    self.client.send_si(self.handle, b2str("setValue("+b2str(val)+")"))


class DataSourceScalar(Scalar) :
  """ This class represents a scalar you would create via "Create>Scalar>Read from Data Source" from the menubar inside kst."""

  def __init__(self,client,filename,field,name="") :
    Scalar.__init__(self,client)
    self.client.send("newDataScalar()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.change(filename, field)

  def change(self,filename,field):
    self.client.send_si(self.handle, b2str("change("+b2str(filename)+","+b2str(field)+")"))

  def file(self) :
    """ Returns the the data source file name. """
    return self.client.send_si(self.handle, "file()")

  def field(self) :
    """ Returns the field. """
    return self.client.send_si(self.handle, "field()")


class VectorScalar(Scalar) :
  """ This class represents a scalar you would create via "Create>Scalar>Read from vector" from the menubar inside kst.
      frame = -1 to read from the end of the file."""

  def __init__(self,client,filename,field,frame,name="") :
    Scalar.__init__(self,client)
    self.client.send("newVectorScalar()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.change(filename, field, frame)

  def change(self,filename,field,frame):
    self.client.send_si(self.handle, b2str("change("+b2str(filename)+","+b2str(field)+","+b2str(frame)+")"))

  def file(self) :
    """ Returns the the data source file name. """
    return self.client.send_si(self.handle, "file()")

  def field(self) :
    """ Returns the field. """
    return self.client.send_si(self.handle, "field()")

  def frame(self) :
    """ Returns the fame. """
    return self.client.send_si(self.handle, "frame()")

class Vector(NamedObject):
  """ This is a class which some convenience classes within pykst use. You should not use it directly.

  "handle" is a descriptive or short name of a scalar created inside kst or through a script. """
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def value(self,index):
    """  Returns element i of this vector. """
    return self.client.send_si(self.handle, "value("+b2str(index)+")")

  def length(self):
    """  Returns the number of samples in the vector. """
    return self.client.send_si(self.handle, "length()")

  def min(self):
    """  Returns the minimum value in the vector. """
    return self.client.send_si(self.handle, "min()")

  def mean(self):
    """  Returns the mean of the vector. """
    return self.client.send_si(self.handle, "mean()")

  def max(self):
    """  Returns the maximum value in the vector. """
    return self.client.send_si(self.handle, "max()")

  def descriptionTip(self):
    """  Returns a string describing the vector """
    return self.client.send_si(self.handle, "descriptionTip()")

class DataVector(Vector):
  """ This class represents a vector you would create via "Create>Vector>Read from Data Source" from the menubar inside kst.

  TODO: implement the configure widget and implement edit functions.

  The parameters of this function mirror the parameters within "Create>Vector>Read from Data Source".

  start is start, or if from end, -1. drange is range, or if to end, -1. start and drange cannot both be -1, obviously.

  skip is 0 for no skip or the number of samples to read per frame.

  To create a vector from '/foo.bar' with field 'foo' from index 3 to index 10 skipping every other sample without a boxcar filter::

    import pykst as kst
    client = kst.Client()
    v = kst.DataVector(client,"/foo.bar","foo",False,True,False,3,10,2,False) """
  def __init__(self,client,filename,field,start=0,NFrames=-1,skip=0,boxcarFirst=False,name="") :
    Vector.__init__(self,client)
    self.client.send("newDataVector()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.change(filename, field, start,NFrames,skip,boxcarFirst)

  def change(self,filename,field,start,NFrames,skip,boxcarFirst):
    self.client.send_si(self.handle, b2str("change("+b2str(filename)+","+b2str(field)+","+b2str(start)+","+b2str(NFrames)+","+b2str(skip)+","+b2str(boxcarFirst)+")"))

  def field(self):
    """  Returns the fieldname. """
    return self.client.send_si(self.handle, "field()")

  def filename(self):
    """  Returns the filename. """
    return self.client.send_si(self.handle, "filename()")

  def start(self):
    """  Returns the index of first frame in the vector.  -1 means count from end. """
    return self.client.send_si(self.handle, "start()")

  def NFrames(self):
    """  Returns the number of frames to be read. -1 means read to end. """
    return self.client.send_si(self.handle, "NFrames()")

  def skip(self):
    """  Returns number of frames to be skipped between samples read. """
    return self.client.send_si(self.handle, "skip()")

  def boxcarFirst(self):
    """  True if boxcar filtering has been applied before skipping. """
    return self.client.send_si(self.handle, "boxcarFirst()")

class GeneratedVector(Vector):
  """ This class represents a vector you would create via "Create>Vector>Generate" from the menubar inside kst.

  X0 is the first value in the vector.  X1 is the last value.  N is the number of samples between
  X0 and X1.

  To create the vector {0, 0.2, 0.4, 0.6, 0.8, 1.0}::

    import pykst as kst
    client = kst.Client()
    v = kst.GeneratedVector(client, 0, 1, 6) """
  def __init__(self,client,X0,X1,N) :
    Vector.__init__(self,client)
    self.client.send("newGeneratedVector()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.change(X0, X1, N)

  def change(self,X0, X1, N):
    self.client.send_si(self.handle, b2str("change("+b2str(X0)+","+b2str(X1)+","+b2str(N)+")"))


class Matrix(NamedObject):
  """ This is a class which some convenience classes within pykst use. You should not use it directly.

  "handle" is a descriptive or short name of a scalar created inside kst or through a script. """
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def value(self,i_x, i_y):
    """  Returns element (i_x, i_y} of this matrix. """
    return self.client.send_si(self.handle, "value("+b2str(i_x)+","+b2str(i_y)+")")

  def length(self):
    """  Returns the number of elements in the matrix. """
    return self.client.send_si(self.handle, "length()")

  def min(self):
    """  Returns the minimum value in the matrix. """
    return self.client.send_si(self.handle, "min()")

  def mean(self):
    """  Returns the mean of the matrix. """
    return self.client.send_si(self.handle, "mean()")

  def max(self):
    """  Returns the maximum value in the matrix. """
    return self.client.send_si(self.handle, "max()")

  def width(self):
    """  Returns the X dimension of the matrix. """
    return self.client.send_si(self.handle, "width()")

  def height(self):
    """  Returns the Y dimension of the matrix. """
    return self.client.send_si(self.handle, "height()")

  def dX(self):
    """  Returns the X spacing of the matrix, for when the matrix is used in an image. """
    return self.client.send_si(self.handle, "dX()")

  def dY(self):
    """  Returns the Y spacing of the matrix, for when the matrix is used in an image. """
    return self.client.send_si(self.handle, "dY()")

  def minX(self):
    """  Returns the minimum X location of the matrix, for when the matrix is used in an image. """
    return self.client.send_si(self.handle, "minX()")

  def minY(self):
    """  Returns the minimum X location of the matrix, for when the matrix is used in an image. """
    return self.client.send_si(self.handle, "minX()")

  def descriptionTip(self):
    """  Returns a string describing the vector """
    return self.client.send_si(self.handle, "descriptionTip()")

class DataMatrix(Matrix):
  """ This class represents a matrix you would create via "Create>Vector>Read from Data Source" from the menubar inside kst.

  TODO: implement the configure widget and implement edit functions.

  The parameters of this function mirror the parameters within "Create>Matrix>Read from Data Source".

  startX is start, or if count from end, -1. nX is the number of x steps, or if to end, -1.
  startX and xSteps cannot both be -1, obviously.

  minX, and stepX provide hints to images for setting the X and Y ranges.

  To create a matrix from '/foo.png' with field '1'

    import pykst as kst
    client = kst.Client()
    v = kst.DataMatrix(client,"/foo.png","1") """
  def __init__(self,client,filename,field,startX=0,startY=0,nX=-1,nY=-1,minX=0, minY=0, dX=1, dY=1,name="") :
    Matrix.__init__(self,client)
    self.client.send("newDataMatrix()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.change(filename,field,startX,startY,nX,nY,minX,minY,dX,dY)

  def change(self,filename,field,startX=0,startY=0,nX=-1,nY=-1,minX=0, minY=0, dX=1, dY=1):
    self.client.send_si(self.handle, "change("+b2str(filename)+","+b2str(field)+","+b2str(startX)+","+b2str(startY)+","+
    b2str(nX)+","+b2str(nY)+","+b2str(minX)+","+b2str(minY)+","+b2str(dX)+","+b2str(dY)+")")

  def field(self):
    """  Returns the fieldname. """
    return self.client.send_si(self.handle, "field()")

  def filename(self):
    """  Returns the filename. """
    return self.client.send_si(self.handle, "filename()")

  def startX(self):
    """  Returns the X index of the matrix in the file """
    return self.client.send_si(self.handle, "startX()")

  def startY(self):
    """  Returns the Y index of the matrix in the file """
    return self.client.send_si(self.handle, "startX()")


class ViewItem(NamedObject):
  """ This is a class which some convenience classes within pykst use. You should not use it directly."""
  def __init__(self,client):
    self.client=client
    
  def setHMargin(self,margin):
    """ This is equivalent to setting Apperance>Layout>Horizontal Margin within a view item dialog in kst. """
    #self.client.send(b2str("#setHMargin("+self.handle.toAscii()+","+b2str(margin)+")"))
    self.client.send_si(self.handle, b2str("setLayoutHorizontalMargin("+b2str(margin)+")"))

  def setVMargin(self,margin):
    """ This is equivalent to setting Apperance>Layout>Vertical Margin within a view item dialog in kst. """
    #self.client.send(b2str("#setVMargin("+self.handle.toAscii()+","+b2str(margin)+")"))
    self.client.send_si(self.handle, b2str("setLayoutVerticalMargin("+b2str(margin)+")"))

  def setHSpace(self,space):
    """ This is equivalent to setting Apperance>Layout>Horizontal Spacing within a view item dialog in kst. """
    #self.client.send(b2str("#setHSpace("+self.handle.toAscii()+","+b2str(space)+")"))
    self.client.send_si(self.handle, b2str("setLayoutHorizontalSpacing("+b2str(space)+")"))

  def setVSpace(self,space):
    """ This is equivalent to setting Apperance>Layout>Vertical Spacing within a view item dialog in kst. """
    #self.client.send(b2str("#setVSpace("+self.handle.toAscii()+","+b2str(space)+")"))
    self.client.send_si(self.handle, b2str("setLayoutVerticalSpacing("+b2str(space)+")"))

  def setFillColor(self,color):
    """ This is equivalent to setting Apperance>Fill>Color within a view item dialog in kst. Not all view items
    support fill color. Colors are given by a name such as 'red' or a hex number such as '#FF0000'. """
    self.client.send_si(self.handle, b2str("setFillColor("+b2str(color)+")"))
    
  def setFillStyle(self,style):
    """ This is equivalent to setting the index of Apperance>Fill>Style within a view item dialog in kst.
    Not all view items support fill styles. 0 is NoBrush, 1 is SolidPattern, 2 is Dense1Pattern, 3 is Dense2Pattern,
    4 is Dense3Pattern, 5 is Dense4Pattern, 6 is Dense5Pattern, 7 is Dense6Pattern, 8 is Dense7Pattern
    9 is HorPattern, 11 is VerPattern, 12 is CrossPattern, 13 is BDiagPattern and 14 is FDiagPattern."""
    self.client.send_si(self.handle, b2str("setIndexOfFillStyle("+b2str(style)+")"))

  def setStrokeStyle(self,style):
    """ This is equivalent to setting the index of Apperance>Stroke>Style within a view item dialog in kst. Not all view items
    support stroke styles. 0 is SolidLine, 1 is DashLine, 2 is DotLine, 3 is DashDotLine, 4 is DashDotDotLine, and 5 is
    CustomDashLine"""
    self.client.send_si(self.handle, b2str("setIndexOfStrokeStyle("+b2str(style)+")"))

  def setStrokeWidth(self,width):
    """ This is equivalent to setting Apperance>Stroke>Width within a view item dialog in kst. Not all view items
    support stroke width."""
    self.client.send_si(self.handle, b2str("setStrokeWidth("+b2str(width)+")"))

  def setStrokeBrushColor(self,color):
    """ This is equivalent to setting Apperance>Stroke>Brush Color within a view item dialog in kst. Not all view items
    support fill color. Colors are given by a name such as 'red' or a hex number such as '#FF0000'. """
    self.client.send_si(self.handle, b2str("setStrokeBrushColor("+b2str(color)+")"))

  def setStrokeBrushStyle(self,style):
    """ This is equivalent to setting the index of Apperance>Stroke>Brush Style within a view item dialog in kst.
    Not all view items support stroke brush styles. 0 is NoBrush, 1 is SolidPattern, 2 is Dense1Pattern, 3 is Dense2Pattern,
    4 is Dense3Pattern, 5 is Dense4Pattern, 6 is Dense5Pattern, 7 is Dense6Pattern, 8 is Dense7Pattern
    9 is HorPattern, 11 is VerPattern, 12 is CrossPattern, 13 is BDiagPattern and 14 is FDiagPattern."""
    self.client.send_si(self.handle, b2str("setIndexOfStrokeBrushStyle("+b2str(style)+")"))

  def setStrokeJoinStyle(self,style):
    """ This is equivalent to setting the index of Apperance>Stroke>Join Style within a view item dialog in kst.
    Not all view items support stroke join styles. 0 is MiterJoin, 1 is BevelJoin, 2 is RoundJoin,
    and 3 is SvgMiterJoin."""
    self.client.send_si(self.handle, b2str("setIndexOfStrokeJoinStyle("+b2str(style)+")"))

  def setStrokeCapStyle(self,style):
    """ This is equivalent to setting the index of Apperance>Stroke>Cap Style within a view item dialog in kst.
    Not all view items support stroke cap styles. 0 is FlatCap, 1 is SquareCap, and 2 is RoundCap"""
    self.client.send_si(self.handle, b2str("setIndexOfStrokeCapStyle("+b2str(style)+")"))

  def setFixedAspectRatio(self, fixed=True):
    """ This is equivalent to checking Dimensions>Fix aspect ratio within a view item dialog in kst.
    The behaviour of this is undefined in view items which always have fixed aspect ratios (e.g., circles)"""
    if fixed == True:
      self.client.send_si(self.handle, b2str("checkFixAspectRatio()"))
    else:
      self.client.send_si(self.handle, b2str("uncheckFixAspectRatio()"))

  def setPosX(self,pos):
    """ This is equivalent to setting Dimensions>Position within a view item dialog in kst.
    Positions refer to the center of an object and x-values range from 0.0 (left) to 1.0 (right)"""
    self.client.send_si(self.handle, b2str("setPosX("+b2str(pos)+")"))

  def setPosY(self,pos):
    """ This is equivalent to setting Dimensions>Position within a view item dialog in kst.
    Positions refer to the center of an object and y-values range from 0.0(top) to 1.0(bottom)"""
    self.client.send_si(self.handle, b2str("setPosY("+b2str(pos)+")"))

  def setSizeX(self,size):
    """ This is equivalent to setting Dimensions>Size within a view item dialog. In cases
    where the aspect ratio is fixed, this changes both sizes (x and y)"""
    #self.client.send(b2str("#setSizeX("+self.handle+","+b2str(size)+")"))
    self.client.send_si(self.handle, b2str("setGeoX("+b2str(size)+")"))

  def setSizeY(self,size):
    """ This is equivalent to setting Dimensions>Size within a view item dialog. In cases
    where the aspect ratio is fixed, this behaviour of this function is undefined."""
    #self.client.send(b2str("#setSizeY("+self.handle+","+b2str(size)+")"))
    self.client.send_si(self.handle, b2str("setGeoY("+b2str(size)+")"))

  def setRotation(self,rot):
    """ This is equivalent to setting Dimensions>Rotation within a view item dialog."""
    #self.client.send(b2str("#setRot("+self.handle+","+b2str(rot)+")"))
    self.client.send_si(self.handle, b2str("setRotation("+b2str(rot)+")"))

  def remove(self):
    """ This removes the object from Kst. You should be careful when using this, because any handles you have to this object will
        still exist and be invalid. Additionally, in Kst, ViewItems are not actually deleted, only hidden, causing even more
        problems. """
    self.client.send("eliminate("+self.handle.toAscii()+")")

class ExistingViewItem(ViewItem) :
  def  __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle

# LABELS ######################################################################
class Label(ViewItem) :
  """ This class represents a label you would create via "Create>Annotations>Label" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the label edit dialog which can be acessed by right clicking on
  a label and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'".
  
  See the kstOwl example."""
  def __init__(self,client,text,fontSize=12,bold=False,italic=False,fontColor="black",fontFamily="Serif",posX=0.1,posY=0.1,rot=0):
    ViewItem.__init__(self,client)
    self.client.send("newLabel()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setText(text)
    self.setLabelFontSize(fontSize)
    self.setPosX(posX)
    self.setPosY(posY)
    self.setFixedAspectRatio(True)
    self.setRotation(rot)
    self.setFontColor(fontColor)
    self.setFontFamily(fontFamily)

    self.setFontBold(bold)
    self.setFontItalic(italic)

  def setText(self,text):
    """ Set text. It may be faster to insert strings within labels (e.g.,"[String (X1)]") and simply modify strings. """
    self.client.send_si(self.handle, b2str("setLabel("+b2str(text)+")"))

  def setLabelFontSize(self,size):
    """ This does not actually represent point size but is relative to the size of the window. """
    self.client.send_si(self.handle, b2str("setFontSize("+b2str(size)+")"))

  def setFontBold(self, bold = True):
    """ . . . """
    if bold == True:
      self.client.send_si(self.handle, b2str("checkLabelBold()"))
    else:
      self.client.send_si(self.handle, b2str("uncheckLabelBold()"))

  def setFontItalic(self, italic = True):
    """ . . . """
    if italic == True:
      self.client.send_si(self.handle, b2str("checkLabelItalic()"))
    else:
      self.client.send_si(self.handle, b2str("uncheckLabelItalic()"))

  def setFontColor(self,color):
    """ Colors are given by a name such as 'red' or a hex number such as '#FF0000' """
    self.client.send_si(self.handle, b2str("setLabelColor("+b2str(color)+")"))

  def setFontFamily(self,family):
    """ . . . """
    self.client.send_si(self.handle, b2str("setFontFamily("+b2str(family)+")"))


class ExistingLabel(Label):
  def  __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getLabelList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingLabel(client,y))
    return ret
  




class Box(ViewItem) :
  """ This class represents a box you would create via "Create>Annotations>Box" from the menubar inside kst.

  The parameters of this function mirror the parameters within the box edit dialog which can be acessed by right clicking on
  a box and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'".
  See the kstOwl example."""
  def __init__(self,client,fixAspect=False,posX=0.1,posY=0.1,sizeX=0.1,sizeY=0.1,rot=0,fillColor="white",fillStyle=1,strokeStyle=1,strokeWidth=1,strokeBrushColor="black",strokeBrushStyle=1,strokeJoinStyle=1,strokeCapStyle=1) :
    ViewItem.__init__(self,client)

    self.client.send("newBox()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(sizeX)
    self.setSizeY(sizeY)
    if fixAspect==True:
        self.setFixedAspectRatio(True)
    else:
        self.setFixedAspectRatio(False)
    self.setRotation(rot)

    self.setStrokeBrushColor(strokeBrushColor)
    self.setFillColor(fillColor)
    self.setFillStyle(fillStyle)
    self.setStrokeStyle(strokeStyle)
    self.setStrokeWidth(strokeWidth)
    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeBrushStyle(strokeBrushStyle)
    self.setStrokeJoinStyle(strokeJoinStyle)
    self.setStrokeCapStyle(strokeCapStyle)

  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getBoxList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret





class Circle(ViewItem) :
  """ This class represents a circle you would create via "Create>Annotations>Circle" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the circle edit dialog which can be acessed by right clicking on
  a circle and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'".
  See the bonjourMonde example"""
  def __init__(self,client,posX=0.1,posY=0.1,size=0.1,fillColor="white",fillStyle=1,strokeStyle=1,strokeWidth=1,strokeBrushColor="grey",strokeBrushStyle=1,strokeJoinStyle=1,strokeCapStyle=1) :
    ViewItem.__init__(self,client)

    self.client.send("newCircle()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(size)

    self.setStrokeBrushColor(strokeBrushColor)
    self.setFillColor(fillColor)
    self.setFillStyle(fillStyle)
    self.setStrokeStyle(strokeStyle)
    self.setStrokeWidth(strokeWidth)
    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeBrushStyle(strokeBrushStyle)
    self.setStrokeJoinStyle(strokeJoinStyle)
    self.setStrokeCapStyle(strokeCapStyle)

  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getCircleList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret
    




class Ellipse(ViewItem) :
  """ This class represents an ellipse you would create via "Create>Annotations>Ellipse" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the ellipse edit dialog which can be acessed by right clicking on
  an ellipse and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'"."""
  def __init__(self,client,fixAspect=False,posX=0.1,posY=0.1,sizeX=0.1,sizeY=0.1,rot=0,fillColor="white",fillStyle=1,strokeStyle=1,strokeWidth=1,strokeBrushColor="black",strokeBrushStyle=1,strokeJoinStyle=1,strokeCapStyle=1) :
    ViewItem.__init__(self,client)
    self.client.send("newEllipse()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(sizeX)
    self.setSizeY(sizeY)
    if fixAspect==True:
        self.setFixedAspectRatio(True)
    else:
        self.setFixedAspectRatio(False)
    self.setRotation(rot)

    self.setStrokeBrushColor(strokeBrushColor)
    self.setFillColor(fillColor)
    self.setFillStyle(fillStyle)
    self.setStrokeStyle(strokeStyle)
    self.setStrokeWidth(strokeWidth)
    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeBrushStyle(strokeBrushStyle)
    self.setStrokeJoinStyle(strokeJoinStyle)
    self.setStrokeCapStyle(strokeCapStyle)

  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getEllipseList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret





class Line(ViewItem) :
  """ This class represents a line you would create via "Create>Annotations>Line" from the menubar inside kst.

  Colors are given by a name such as 'red' or a hex number such as '#FF0000'"."""
  def __init__(self,client,posX=0.1,posY=0.1,length=0.1,rot=0,strokeStyle=1,strokeWidth=1,strokeBrushColor="black",strokeBrushStyle=1,strokeJoinStyle=1,strokeCapStyle=1) :
    ViewItem.__init__(self,client)
    self.client.send("newLine()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(length)
    self.setRotation(rot)

    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeStyle(strokeStyle)
    self.setStrokeWidth(strokeWidth)
    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeBrushStyle(strokeBrushStyle)
    self.setStrokeJoinStyle(strokeJoinStyle)
    self.setStrokeCapStyle(strokeCapStyle)

  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getCircleList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret



class Arrow(ViewItem) :
  """ This class represents an arrow you would create via "Create>Annotations>Arrow" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the arrow edit dialog which can be acessed by right clicking on
  a arrow and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'"."""
  def __init__(self,client,posX=0.1,posY=0.1,length=0.1, rot=0,arrowAtStart = False, arrowAtEnd = True, arrowSize = 12.0, strokeStyle=1,strokeWidth=1,strokeBrushColor="black",strokeBrushStyle=1,strokeJoinStyle=1,strokeCapStyle=1) :
    ViewItem.__init__(self,client)
    self.client.send("newArrow()")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(length)
    self.setRotation(rot)

    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeStyle(strokeStyle)
    self.setStrokeWidth(strokeWidth)
    self.setStrokeBrushColor(strokeBrushColor)
    self.setStrokeBrushStyle(strokeBrushStyle)
    self.setStrokeJoinStyle(strokeJoinStyle)
    self.setStrokeCapStyle(strokeCapStyle)
    self.setArrowAtStart(arrowAtStart)
    self.setArrowAtEnd(arrowAtEnd)
    self.setArrowSize(arrowSize)

  def setArrowAtStart(self, arrow=True) :
    """ Set whether an arrow head is shown at the start of the line """
    if arrow==True:
      self.client.send_si(self.handle, b2str("arrowAtStart(True)"))
    else:
      self.client.send_si(self.handle, b2str("arrowAtStart(False)"))

  def setArrowAtEnd(self, arrow=True) :
    """ Set whether an arrow head is shown at the end of the line """
    if arrow==True:
      self.client.send_si(self.handle, b2str("arrowAtEnd(True)"))
    else:
      self.client.send_si(self.handle, b2str("arrowAtEnd(False)"))

  def setArrowSize(self, arrowSize) :
    self.client.send_si(self.handle, b2str("arrowHeadScale("+b2str(arrowSize)+")"))

  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getArrowList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret
    
    
    
    
class Picture(ViewItem) :
  """ This class represents a picture you would create via "Create>Annotations>Picture" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the picture edit dialog which can be acessed by right clicking on
  a picture and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'".
  
  See the kstOwl example."""
  def __init__(self,client,filename,posX=0.1,posY=0.1,size=0.1,rot=0) :
    ViewItem.__init__(self,client)
    self.client.send("newPicture("+b2str(filename)+")")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setFixedAspectRatio(True)
    self.setSizeX(size)
    #fixme: aspect ratio is wrong.
    #self.setSizeY(size)
    self.setRotation(rot)

  def setPicture(self,pic):
    """ BUG: aspect ratio is not changed. There is no parellel for this function within the kst GUI. """
    self.client.send_si(self.handle, b2str("setPicture("+b2str(pic)+")"))

class ExistingPicture(Picture):
  def __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getPictureList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingPicture(client,y))
    return ret





class SVG(ViewItem) :
  """ This class represents a SVG you would create via "Create>Annotations>SVG" from the menubar inside kst.
  
  The parameters of this function mirror the parameters within the SVG edit dialog which can be acessed by right clicking on
  a SVG and selecting Edit. Colors are given by a name such as 'red' or a hex number such as '#FF0000'"."""
  def __init__(self,client,filename,posX=0.1,posY=0.1,size=0.1,rot=0) :
    ViewItem.__init__(self,client)
    self.client.send("newSvgItem("+b2str(filename)+")")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

    self.setPosX(posX)
    self.setPosY(posY)
    self.setSizeX(size)
    self.setSizeY(size)
    self.setFixedAspectRatio(True)
    self.setRotation(rot)


class ExistingSVG(SVG):
  def __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getSVGList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingSVG(client,y))
    return ret





class Plot(ViewItem) :
  """ This class represents a Plot you would create via "Create>Annotations>Plot" from the menubar inside kst.
  TODO: a lot.
  
  The parameters of this function mirror a few of the parameters within the plot edit dialog which can be acessed by right clicking on
  a plot and selecting Edit. To add curves and images to plots, use functions inside this class which don't exist yet or use the Curve/Equation/Image
  conb2structors. See the ksNspire example. """
  def __init__(self,client,posX=0.1,posY=0.1,sizeX=0.1,sizeY=0.1,rot=0,name="") :
    ViewItem.__init__(self,client)
    self.client.send("newPlot()")
    self.client.send("setPosX("+b2str(posX)+")")
    self.client.send("setPosY("+b2str(posY)+")")
    self.client.send("setGeoX("+b2str(sizeX)+")")
    self.client.send("setGeoY("+b2str(sizeY)+")")
    self.client.send("setRotation("+b2str(rot)+")")
    self.client.send("setName("+b2str(name)+")")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)

  def setXRange(self,x0 = 0.0, x1 = 10.0) :
    """ Set X zoom range from x0 to x1 """
    self.client.send_si(self.handle, b2str("setXRange("+b2str(x0)+","+b2str(x1)+")"))

  def setYRange(self, y0 = 0.0, y1 = 10.0) :
    """ Set Y zoom range from y0 to y1 """
    self.client.send_si(self.handle, b2str("setYRange("+b2str(y0)+","+b2str(y1)+")"))

  def setXAuto(self) :
    """ Set X zoom range to autoscale """
    self.client.send_si(self.handle,b2str("setXAuto()"))

  def setYAuto(self) :
    """ Set Y zoom range to autoscale """
    self.client.send_si(self.handle, b2str("setPlotYAuto()"))

  def setXAutoBorder(self) :
    """ Set X zoom range to autoscale with a small border """
    self.client.send_si(self.handle, b2str("setPlotXAutoBorder()"))

  def setYAutoBorder(self) :
    """ Set Y zoom range to autoscale with a small border """
    self.client.send_si(self.handle,b2str("setYAutoBorder()"))

  def setXNoSpike(self) :
    """ Set X zoom range to spike insensitive autoscale """
    self.client.send_si(self.handle,b2str("setXNoSpike()"))

  def setYNoSpike(self) :
    """ Set Y zoom range to spike insensitive autoscale """
    self.client.send_si(self.handle,b2str("setYNoSpike()"))

  def setXAC(self, r=0.2) :
    """ Set X zoom range to fixed range, centered around the mean.  Similar to AC coupling on an oscilloscope. """
    self.client.send_si(self.handle,b2str("setXAC("+b2str(r)+")"))

  def setYAC(self, r=0.2) :
    """ Set Y zoom range to fixed range, centered around the mean.  Similar to AC coupling on an oscilloscope. """
    self.client.send_si(self.handle,b2str("setYAC("+b2str(r)+")"))

  def setGlobalFont(self, family="", bold=False, italic=False) :
    """ Set the global plot font.  By default, the axis labels all use this, unless they have been set to use their own.
        If the parameter 'family' is empty, the font family will be unchanged.
        The font will be bold if parameter 'bold' is set to 'bold' or 'True'.
        The font will be italic if parameter 'italic' is set to 'italic' or 'True'."""
    self.client.send_si(self.handle,b2str("setGlobalFont("+family+","+b2str(bold)+","+b2str(italic)+")"))

  def setTopLabel(self, label="") :
    """ Set the plot top label """
    self.client.send_si(self.handle,b2str("setTopLabel("+label+")"))

  def setBottomLabel(self, label="") :
    """ Set the plot bottom label """
    self.client.send_si(self.handle,b2str("setBottomLabel("+label+")"))

  def setLeftLabel(self, label="") :
    """ Set the plot left label """
    self.client.send_si(self.handle,b2str("setLeftLabel("+label+")"))

  def setRightLabel(self, label="") :
    """ Set the plot right label """
    self.client.send_si(self.handle,b2str("setRightLabel("+label+")"))

  def setTopLabelAuto(self) :
    """ Set the top label to auto generated. """
    self.client.send_si(self.handle,b2str("setTopLabelAuto()"))
  def setBottomLabelAuto(self) :
    """ Set the bottom label to auto generated. """
    self.client.send_si(self.handle,b2str("setBottomLabelAuto()"))

  def setLeftLabelAuto(self) :
    """ Set the left label to auto generated. """
    self.client.send_si(self.handle,b2str("setLeftLabelAuto()"))

  def setRightLabelAuto(self) :
    """ Set the right label to auto generated. """
    self.client.send_si(self.handle,b2str("setRightLabelAuto()"))

  def normalizeXtoY(self) :
    """ Adjust the X zoom range so X and Y have the same scale per unit (square pixels) """
    self.client.send_si(self.handle, b2str("normalizeXtoY()"))

  def setLogX(self) :
    """ Set X axis to log mode. """
    self.client.send_si(self.handle, b2str("setLogX()"))

  def setLogY(self) :
    """ Set X axis to log mode. """
    self.client.send_si(self.handle, b2str("setLogY()"))

  def setYAxisReversed(self, reversed=True) :
    """ set the Y axis to decreasing from bottom to top. """
    if reversed == True:
      self.client.send_si(self.handle, b2str("setYAxisReversed()"))
    else:
      self.client.send_si(self.handle, b2str("setYAxisNotReversed()"))

  def setXAxisReversed(self, reversed=True) :
    """ set the X axis to decreasing from left to right. """
    if reversed == True:
      self.client.send_si(self.handle, b2str("setXAxisReversed()"))
    else:
      self.client.send_si(self.handle, b2str("setXAxisNotReversed()"))


class ExistingPlot(Plot):
  def __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=QtCore.QString(client.send("getPlotList()"))
    ret=[]
    while x.contains('['):
      y=QtCore.QString(x)
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingPlot(client,y))
    return ret





class Button(ViewItem) :
  """ This represents a button inside a View. When the button is pressed, it sends a message via a socket.
  
  socket is a QtNetwork.QLocalSocket that is not connected to anything. The message "clicked" will be sent
  when the button is pressed. See the bonjourMonde example. """
  def __init__(self,client,text,socket,posX=0.1,posY=0.1,sizeX=0.1,sizeY=0.1,rot=0) :
    ViewItem.__init__(self,client)
    self.client.send("newButton()")
    self.client.send("setPosX("+b2str(posX)+")")
    self.client.send("setPosY("+b2str(posY)+")")
    self.client.send("setGeoX("+b2str(sizeX)+")")
    self.client.send("setGeoY("+b2str(sizeY)+")")
    self.client.send("setText("+b2str(text)+")")
    self.client.send("setRotation("+b2str(rot)+")")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    socket.connectToServer(client.serverName)
    socket.waitForConnected(300)
    socket.write(b2str("attachTo("+self.handle+")"))
    
  def setText(self,text):
    """ Sets the text of the button. """
    self.client.send("beginEdit("+self.handle+")")
    self.client.send("setText("+b2str(text)+")")
    self.client.send("endEdit()")





class LineEdit(ViewItem) :
  """ This represents a line edit inside a View. When the lineedit's value is changed, it sends a message via a socket.
  
  socket is a QtNetwork.QLocalSocket that is not connected to anything. The message "valueSet:VAL" where VAL is some text
  will be sent when the text is changed. See the ksNspire example. """
  def __init__(self,client,text,socket,posX=0.1,posY=0.1,sizeX=0.1,sizeY=0.1,rot=0) :
    ViewItem.__init__(self,client)
    self.client.send("newLineEdit()")
    self.client.send("setPosX("+b2str(posX)+")")
    self.client.send("setPosY("+b2str(posY)+")")
    self.client.send("setGeoX("+b2str(sizeX)+")")
    self.client.send("setGeoY("+b2str(sizeY)+")")
    self.client.send("setText("+b2str(text)+")")
    self.client.send("setRotation("+b2str(rot)+")")
    self.handle=QtCore.QString(self.client.send("endEdit()"))
    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    socket.connectToServer(b2str(client.serverName))
    socket.waitForConnected(300)
    socket.write(b2str("attachTo("+self.handle+")"))
    
  def setText(self,text):
    """ Sets the text of the line edit. """
    self.client.send("beginEdit("+self.handle+")")
    self.client.send("setText("+b2str(text)+")")
    self.client.send("endEdit()")
