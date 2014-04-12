import sip
sip.setapi('QString', 1)
import sys
import math
import os
import ctypes
from time import sleep
#from PyQt4 import QtCore, QtNetwork
from PySide import QtCore, QtNetwork
from numpy import *
#from pykstpp import * 

def b2str(val):
  if isinstance(val, bool):
    return "True" if val else "False"
  else:
    return str(val)

class Client:
  """ An interface to a running kst session. 

  Every class inside pykst accepts an instance of Client which it 
  uses to interact with a kst session. Alternatively, the classes are 
  accessable from the Client.  In addition, it holds functions which 
  effect the entire kst session.
      
  If serverName is specified, creates a connection to either a running 
  kst session with serverName, or if none exists, a new one.
  If serverName is not specified, creates a connection to either the 
  kst session with the name ``kstScript``, or if none exists, a new one. 
  """
  
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
    """ Sends a command to kst and returns a response. 
    
    You should never use 
    this directly, as there is no guarantee that the internal command
    list kst uses won't change. Instead use the convenience classes 
    included with pykst. 
    """
    self.ls.write(command)
    self.ls.flush()
    self.ls.waitForReadyRead(300000)
    x=self.ls.readAll()
    return x
    
  def send_si(self, handle, command):
    self.send(b2str("beginEdit("+handle+")"))
    x = self.send(command)
    self.send(b2str("endEdit()"))
    return x

  def clear(self):
    """ Clears all objects from kst.
    
    Equivalent to file->close from the menubar inside kst.  
    """
    self.send("clear()")
    
  def screen_back(self):
    """ Equivalent to "Range>Back One Screen" from the menubar inside kst. """
    self.send("screenBack()")
    
  def screen_forward(self):
    """ Equivalent to "Range>Forward One Screen" from the menubar inside kst. """
    self.send("screenForward()")
    
  def count_from_end(self):
    """ Equivalent to "Range>Count From End" from the menubar inside kst. """
    self.send("countFromEnd()")
    
  def read_to_end(self):
    """ Equivalent to "Range>Read To End" from the menubar inside kst. """
    self.send("readToEnd()")
    
  def set_paused(self):
    """ Equivalent to checking "Range>Pause" from the menubar inside kst."""
    self.send("setPaused()")
    
  def unset_paused(self):
    """ Equivalent to unchecking "Range>Pause" from the menubar inside kst."""
    self.send("unsetPaused()")
    
  def tab_count(self):
    """ Get the number of tabs open in the current document. """
    return self.send("tabCount()")
  
  def new_tab(self):
    """ Create a new tab in the current document and switch to it. """
    return self.send("newTab()")
  
  def set_tab(self,tab):
    """ Set the index of the current tab. 
    
    tab must be greater or equal to 0 and less than tabCount(). 
    """
    self.send("setTab("+b2str(tab)+")")
  
  def NewGeneratedString(self, string, name=""):
    """ Create a new generated string in kst.
    
    See :class:`GeneratedString`
    """
    return GeneratedString(self, string, name)

  def GeneratedString(self, name):
    """ Returns a generated string from kst given its name.
    
    See :class:`GeneratedString`
    """
    return GeneratedString(self, "", name, new=False)
  
  def NewDataSourceString(self, filename, field, name=""):
    """ Create a New Data Source String in kst.
    
    See :class:`DataSourceString`
    """
    return DataSourceString(self, filename, field, name)

  def DataSourceString(self, name):
    """ Returns a datasource string from kst given its name.
    
    See :class:`DataSourceString`
    """
    return DataSourceString(self, "", "", name, new=False)
  
  def NewGeneratedScalar(self, value, name=""):
    """ Create a New Generated Scalar in kst.
    
    See :class:`GeneratedScalar`
    """
    return GeneratedScalar(self, value, name)

  def GeneratedScalar(self, name):
    """ Returns a Generated Scalar from kst given its name.
    
    See :class:`GeneratedScalar`
    """
    return GeneratedScalar(self, "", name, new=False)

  def NewDataSourceScalar(self, filename, field, name=""):
    """ Create a New DataSource Scalar in kst.
    
    See :class:`DataSourceScalar`
    """
    return DataSourceScalar(self, filename, field, name)

  def DataSourceScalar(self, name):
    """ Returns a DataSource Scalar from kst given its name.
    
    See :class:`DataSourceScalar`
    """
    return DataSourceScalar(self, "", "", name, new=False)

  def NewVectorScalar(self, filename, field, frame=-1, name=""):
    """ Create a New VectorScalar in kst.
    
    See :class:`VectorScalar`
    """
    return VectorScalar(self, filename, field, frame, name)

  def VectorScalar(self, name):
    """ Returns a VectorScalar from kst given its name.
    
    See :class:`VectorScalar`
    """
    return VectorScalar(self, "", "", 0, name, new=False)

  def NewDataVector(self, filename, field, start=0, NFrames=-1,
                 skip=0, boxcarFirst=False, name="") :
    """ Create a New DataVector in kst.
    
    See :class:`DataVector`
    """
    return DataVector(self, filename, field, start, NFrames,
                      skip, boxcarFirst, name)
  
  def DataVector(self, name):
    """ Returns a DataVector from kst given its name.
    
    See :class:`DataVector`
    """
    return DataVector(self, "", "", name=name, new=False)

  def NewGeneratedVector(self, X0, X1, N, name=""):
    """ Create a New GeneratedVector in kst.
    
    See :class:`GeneratedVector`
    """
    return GeneratedVector(self, X0, X1, N, name)

  def GeneratedVector(self, name):
    """ Returns a GeneratedVector from kst given its name.
    
    See :class:`GeneratedVector`
    """
    return GeneratedVector(self, 0, 0, 0, name, new=False)
  
  def NewDataMatrix(self, filename, field, startX=0, startY=0, nX=-1, nY=-1, 
                 minX=0, minY=0, dX=1, dY=1,name="") :
    """ Create a New DataMatrix in kst.
    
    See :class:`DataMatrix`
    """
    return DataMatrix(self, filename, field, startX, startY, nX, nY, 
                 minX, minY, dX, dY,name)

  def DataMatrix(self, name):
    """ Returns a DataMatrix from kst given its name.
    
    See :class:`DataMatrix`
    """
    return DataMatrix(self, "", "", name=name, new=False)

  def NewCurve(self, xVector, yVector, name=""):
    """ Create a New Curve in kst.
    
    See :class:`Curve`
    """
    return Curve(self, xVector, yVector, name)

  def Curve(self, name):
    """ Returns a Curve from kst given its name.
    
    See :class:`Curve`
    """
    return Curve(self, "", "", name, new=False)

  def NewLabel(self, text, pos=(0.5,0.5), rot=0, fontSize=12, 
            bold=False, italic=False, fontColor="black", 
            fontFamily="Serif", name="") :
    """ Create a New Label in kst.
    
    See :class:`Label`
    """
    return Label(self, text, pos, rot, fontSize, bold, italic, 
                 fontColor, fontFamily, name)

  def Label(self, name):
    """ Returns a Label from kst given its name.
    
    See :class:`Label`
    """
    return Label(self, "", name=name, new=False)
  
  def NewBox(self, pos=(0.1,0.1), size=(0.1,0.1), rot=0, 
          fillColor="white", fillStyle=1, strokeStyle=1, strokeWidth=1,
          strokeBrushColor="black", strokeBrushStyle=1, 
          strokeJoinStyle=1, strokeCapStyle=1, fixAspect=False, name="") :
    """ Create a New Box in kst.
    
    See :class:`Box`
    """
    return Box(self, pos, size, rot, fillColor, fillStyle, strokeStyle, 
               strokeWidth, strokeBrushColor, strokeBrushStyle, 
               strokeJoinStyle, strokeCapStyle, fixAspect, name)

  def Box(self, name):
    """ Returns a Box from kst given its name.
    
    See :class:`Box`
    """
    return Box(self, name=name, new=False)
  
  def NewCircle(self, pos=(0.1, 0.1), diameter=0.1,
             fillColor="white",fillStyle=1,strokeStyle=1,
             strokeWidth=1,strokeBrushColor="grey",strokeBrushStyle=1, name="") :
    """ Create a New Circle in kst.
    
    See :class:`Circle`
    """
    return Circle(self, pos, diameter, fillColor, fillStyle, strokeStyle, 
                  strokeWidth, strokeBrushColor, strokeBrushStyle, name)
  
  def Circle(self, name):
    """ Returns a Circle from kst given its name.
    
    See :class:`Circle`
    """
    return Circle(self, name=name, new=False)
  
  def NewEllipse(self,pos=(0.1,0.1), size=(0.1,0.1),
               rot=0, fillColor="white", fillStyle=1, strokeStyle=1,
               strokeWidth=1, strokeBrushColor="black", strokeBrushStyle=1,
               fixAspect=False, name="") :
    """ Create a New Ellipse in kst.
    
    See :class:`Ellipse`
    """
    return Ellipse(self,pos, size, rot, fillColor, fillStyle, strokeStyle, 
                   strokeWidth, strokeBrushColor, strokeBrushStyle, 
                   fixAspect, name)

  def Ellipse(self, name):
    """ Returns a  from kst given its name.
    
    See :class:`Ellipse`
    """
    return Ellipse(self, name=name, new=False)
  
  def NewLine(self,pos=(0.1,0.1),length=0.1,rot=0,
           strokeStyle=1,strokeWidth=1,strokeBrushColor="black",
           strokeBrushStyle=1,strokeCapStyle=1, name="") :
    """ Create a New Line in kst.
    
    See :class:`Line`
    """
    return Line(self,pos, length, rot, strokeStyle, strokeWidth, 
                strokeBrushColor, strokeBrushStyle, strokeCapStyle, name)
  
  def Line(self, name):
    """ Returns a Line from kst given its name.
    
    See :class:`Line`
    """
    return Line(self, name=name, new=False)

  def NewArrow(self,pos=(0.1,0.1), length=0.1, rot=0, 
            arrowAtStart = False, arrowAtEnd = True, arrowSize = 12.0, 
            strokeStyle=1, strokeWidth=1, strokeBrushColor="black",
            strokeBrushStyle=1, strokeCapStyle=1, name="") :
    """ Create a New Arrow in kst.
    
    See :class:`Arrow`
    """
    return Arrow(self,pos, length, rot, arrowAtStart, arrowAtEnd, arrowSize, 
          strokeStyle, strokeWidth, strokeBrushColor, strokeBrushStyle, 
          strokeCapStyle, name)
    
  def Arrow(self, name):
    """ Returns a Arrow from kst given its name.
    
    See :class:`Arrow`
    """
    return Arrow(self, name=name, new=False)
    
  def NewPicture(self,filename,pos=(0.1,0.1), width=0.1,rot=0, name="") :
    """ Create a New Picture in kst.
    
    See :class:`Picture`
    """
    return Picture(self,filename, pos, width, rot, name)
  
  def Picture(self, name):
    """ Returns a Picture from kst given its name.
    
    See :class:`Picture`
    """
    return Picture(self, "", name = name, new=False)
  
  def NewSVG(self, filename, pos=(0.1,0.1), width=0.1, rot=0, name="") :
    """ Create a New SVG in kst.
    
    See :class:`SVG`
    """
    return SVG(self, filename, pos, width, rot, name)
  
  def SVG(self, name):
    """ Returns a SVG from kst given its name.
    
    See :class:`SVG`
    """
    return SVG(self, "", name = name, new=False)

  def NewPlot(self,pos=(0.1,0.1),size=(0.1,0.1),rot=0,
           fillColor="white", fillStyle=1, strokeStyle=1, strokeWidth=1,
           strokeBrushColor="black", strokeBrushStyle=1, 
           strokeJoinStyle=1, strokeCapStyle=1, fixAspect=False, name="") :
    """ Create a New Plot in kst.
    
    See :class:`Plot`
    """
    return Plot(self, pos, size, rot, fillColor, fillStyle, strokeStyle, 
                strokeWidth, strokeBrushColor, strokeBrushStyle, 
                strokeJoinStyle, strokeCapStyle, fixAspect, name)
  
  def Plot(self, name):
    """ Returns a Plot from kst given its name.
    
    See :class:`Plot`
    """
    return Plot(self, name = name, new=False)

    
class NamedObject:
    """ Convenience class. You should not use it directly."""
    def __init__(self,client):
      self.client=client
      
    def set_name(self,name):
      """ Set the name of the object inside kst. """
      self.client.send_si(self.handle, b2str("setName("+b2str(name)+")"))
      
    def name(self):
      """ Returns the name of the object from inside kst. """
      return self.client.send_si(self.handle, "name()")


class String(NamedObject) :
  """ Convenience class. You should not use it directly."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def value(self) :
    """ Returns the string. """
    return self.client.send_si(self.handle, "value()")

class GeneratedString(String) :
  """ A string constant inside kst.
  
  This class represents a string you would create via 
  "Create>String>Generated" from the menubar inside kst.

  :param string: The value of the string.

  To import the string "Hello World" into kst::

    import pykst as kst
    client=kst.Client()
    s=client.GeneratedString("Hello World")
    
  """
  def __init__(self,client,string,name="", new=True) :
    String.__init__(self,client)

    if (new == True):
      self.client.send("newGeneratedString()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.set_value(string)
      self.set_name(name)
    else:
      self.handle = name

  def set_value(self,val):
    """ set the value of the string inside kst. """
    self.client.send_si(self.handle, b2str("setValue("+b2str(val)+")"))

class DataSourceString(String) :
  """ A string read from a data source inside kst.
  
  This class represents a string you would create via 
  "Create>String>Read from Data Source" from the menubar inside kst.
  
  :param filename: The name of the file/data source to read the string from.
  :param field: the name of the field in the data source.

  To read "File path" from the data source "tmp.dat" into kst::
  
    import pykst as kst
    client=kst.Client()
    s=client.DataSourceString("tmp.dat", "File Path")
    
  """
  def __init__(self,client,filename,field,name="", new=True) :
    String.__init__(self,client)
    
    if (new == True):
      self.client.send("newDataString()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)
      self.change(filename, field)
    else:
      self.handle = name

  def change(self,filename,field):
    """ Change a DataSource String.
    
    Change the file and field of a DataSourceString in kst.
    
    :param filename: The name of the file/data source to read the string from.
    :param field: the name of the field in the data source.
    """
    self.client.send_si(self.handle, b2str("change("+b2str(filename)+","+b2str(field)+")"))


class Scalar(NamedObject) :
  """ Convenience class. You should not use it directly."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def value(self) :
    """ Returns the scalar. """
    return self.client.send_si(self.handle, "value()")

class GeneratedScalar(Scalar) :
  """ A scalar constant inside kst.
 
  This class represents a scalar you would create via 
  "Create>Scalar>Generate" from the menubar inside kst.

  :param value: the value to assign to the scalar constant.
  
  To import the scalar of value 42 into kst::

    import pykst as kst
    client=kst.Client()
    s=client.GeneratedScalar(42)
    
  """
  def __init__(self, client, value, name="", new=True) :
    Scalar.__init__(self,client)

    if (new == True):
      self.client.send("newGeneratedScalar()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.set_value(value)
      self.set_name(name)
    else:
      self.handle = name      

  def set_value(self,val):
    """ set the value of the string inside kst. """
    self.client.send_si(self.handle, b2str("setValue("+b2str(val)+")"))


class DataSourceScalar(Scalar) :
  """ A scalar read from a data source inside kst.
  
  This class represents a scalar you would create via 
  "Create>Scalar>Read from Data Source" from the menubar inside kst.
 
  :param filename: The name of the file/data source to read the scalar from.
  :param field: the name of the field in the data source.
 
  To read "CONST1" from the data source "tmp.dat" into kst::
  
    import pykst as kst
    client=kst.Client()
    x=client.DataSourceScalar("tmp.dat", "CONST1")
    
  """
  def __init__(self,client,filename,field,name="", new=True) :
    Scalar.__init__(self,client)
    
    if (new == True):
      self.client.send("newDataScalar()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.change(filename, field)
    else:
      self.handle = name      

  def change(self,filename,field):
    """ Change a DataSource Scalar.
    
    Change the file and field of a DataSourceScalar in kst.
    
    :param filename: The name of the file/data source to read the scalar from.
    :param field: the name of the field in the data source.
    """
    self.client.send_si(self.handle, "change("+filename+","+field+")")

  def file(self) :
    """ Returns the the data source file name. """
    return self.client.send_si(self.handle, "file()")

  def field(self) :
    """ Returns the field. """
    return self.client.send_si(self.handle, "field()")


class VectorScalar(Scalar) :
  """ A scalar in kst read from a vector from a data source.
  
  This class represents a scalar you would create via 
  "Create>Scalar>Read from vector" from the menubar inside kst.
  
  :param filename: The name of the file/data source to read the scalar from.
  :param field: the name of the vector in the data source.  
  :param frame: which frame of the vector to read the scalar from. 
                frame = -1 (the default) reads from the end of the file.
  
  To read the last value of the vector INDEX from the file "tmp.dat"
  into kst::

    import pykst as kst
    client=kst.Client()
    x=client.VectorScalar("tmp.dat", "INDEX", -1)

  """
  def __init__(self, client, filename, field, frame=-1, name="", new=True) :
    Scalar.__init__(self,client)

    if (new == True):    
      self.client.send("newVectorScalar()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.change(filename, field, frame)
    else:
      self.handle = name      

  def change(self,filename,field,frame):
    """ Change a Vector Scalar in kst.
    
    Change the file, field and frame of a VectorScalar in kst.
    
    :param filename: The name of the file/data source to read the scalar from.
    :param field: the name of the vector in the data source.  
    :param frame: which frame of the vector to read the scalar from.   
                  frame = -1 reads from the end of the file.
    """
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
  """ Convenience class. You should not use it directly."""
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

  def description_tip(self):
    """  Returns a string describing the vector """
    return self.client.send_si(self.handle, "descriptionTip()")

class DataVector(Vector):
  """ A vector in kst, read from a data source.
  
  This class represents a vector you would create via 
  "Create>Vector>Read from Data Source" from the menubar inside kst.

  The parameters of this function mirror the parameters within 
  "Create>Vector>Read from Data Source".

  :param filename: The name of the file/data source to read the scalar from.
  :param field: the name of the vector in the data source.  
  :param start: The starting index of the vector.  
                start = -1 for count from end.
  :param NFrames: The number of frames to read.  
                  NFrames = -1 for read to end.
  :param skip: The number of frames per sample read.  
               skip = 0 to read every sample.
  :param boxcarFirst: apply a boxcar filter before skiping.
  
  To create a vector from "tmp.dat" with field "INDEX" from 
  frame 3 to frame 10, reading a sample every other frame without 
  a boxcar filter::

    import pykst as kst
    client = kst.Client()
    v = client.DataVector("tmp.dat", "INDEX", 3, 10, 2, False) 
    
  """
  def __init__(self, client, filename, field, start=0, NFrames=-1,
               skip=0, boxcarFirst=False, name="", new=True) :
    Vector.__init__(self,client)

    if (new == True):
      self.client.send("newDataVector()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)
      self.change(filename, field, start, NFrames, skip, boxcarFirst)
    else:
      self.handle = name      

  def change(self, filename, field, start, NFrames, skip, boxcarFirst):
    """ Change the parameters of a data vector.
    
    :param filename: The name of the file/data source to read the scalar from.
    :param field: the name of the vector in the data source.  
    :param start: The starting index of the vector.  
                  start = -1 for count from end.
    :param NFrames: The number of frames to read.  
                    NFrames = -1 for read to end.
    :param skip: The number of frames per sample read.  
                skip = 0 to read every sample.
    :param boxcarFirst: apply a boxcar filter before skiping.
    
    """
    self.client.send_si(self.handle, "change("+filename+","+field+","
                        +b2str(start)+","+b2str(NFrames)+","+b2str(skip)
                        +","+b2str(boxcarFirst)+")")

  def field(self):
    """  Returns the fieldname. """
    return self.client.send_si(self.handle, "field()")

  def filename(self):
    """  Returns the filename. """
    return self.client.send_si(self.handle, "filename()")

  def start(self):
    """  Returns the index of first frame in the vector.  
    -1 means count from end. """
    return self.client.send_si(self.handle, "start()")

  def n_frames(self):
    """  Returns the number of frames to be read. -1 means read to end. """
    return self.client.send_si(self.handle, "NFrames()")

  def skip(self):
    """  Returns number of frames to be skipped between samples read. """
    return self.client.send_si(self.handle, "skip()")

  def boxcar_first(self):
    """  True if boxcar filtering has been applied before skipping. """
    return self.client.send_si(self.handle, "boxcarFirst()")

class GeneratedVector(Vector):
  """ Create a generated vector in kst.
  
  This class represents a vector you would create via 
  "Create>Vector>Generate" from the menubar inside kst.

  :param X0: The first value in the vector.
  :param X1: The last value in the vector.
  :param N: The number of evenly spaced values in the vector.

  To create the vector {0, 0.2, 0.4, 0.6, 0.8, 1.0}::

    import pykst as kst
    client = kst.Client()
    v = client.GeneratedVector(0, 1, 6) 
    
  """
  def __init__(self, client, X0, X1, N, name="", new=True) :
    Vector.__init__(self,client)

    if (new == True):
      self.client.send("newGeneratedVector()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.change(X0, X1, N)
      self.set_name(name)
    else:
      self.handle = name      

  def change(self,X0, X1, N):
    """ Change the parameters of a Generated Vector inside kst.
    
    :param X0: The first value in the vector.
    :param X1: The last value in the vector.
    :param N: The number of evenly spaced values in the vector.
    """
    self.client.send_si(self.handle, "change("+b2str(X0)+","+b2str(X1)+
                        ","+b2str(N)+")")


class Matrix(NamedObject):
  """ Convenience class. You should not use it directly."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def value(self,i_x, i_y):
    """  Returns element (i_x, i_y} of this matrix. """
    return self.client.send_si(self.handle, "value("+b2str(i_x)+
                               ","+b2str(i_y)+")")

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

  def dx(self):
    """  Returns the X spacing of the matrix, for when the matrix is used in an image. """
    return self.client.send_si(self.handle, "dX()")

  def dy(self):
    """  Returns the Y spacing of the matrix, for when the matrix is used in an image. """
    return self.client.send_si(self.handle, "dY()")

  def min_x(self):
    """  Returns the minimum X location of the matrix, for when the matrix is used in an image. """
    return self.client.send_si(self.handle, "minX()")

  def min_y(self):
    """  Returns the minimum X location of the matrix, for when the matrix is used in an image. """
    return self.client.send_si(self.handle, "minX()")

  def description_tip(self):
    """  Returns a string describing the vector """
    return self.client.send_si(self.handle, "descriptionTip()")

class DataMatrix(Matrix):
  """  Create a Data Matrix which reads from a data source inside kst.
  
  This class represents a matrix you would create via 
  "Create>Vector>Read from Data Source" from the menubar inside kst.
  The parameters of this function mirror the parameters within 
  "Create>Matrix>Read from Data Source".

  :param filename: The name of the file/data source to read the scalar from.
  :param field: the name of the vector in the data source.  
  :param startX/Y: the x/y index to start reading from. startX/Y = -1 
                   to count from the right/bottom.
  :param nX/Y: the number of columns/rows to read.  nX/Y = -1 to read
               to the end.
  :param minX/Y: Hint to Images of the coordinates corresponding to the
                 the left/bottom of the Matrix
  :param stepX/Y: Hint to Images of the spacing between points.

  To create a matrix from 'foo.png' with field '1'::
  
    import pykst as kst
    client = kst.Client()
    v = client.DataMatrix("foo.png", "1") 
    
  """
  def __init__(self,client,filename,field,startX=0,startY=0,nX=-1,nY=-1,
               minX=0, minY=0, dX=1, dY=1,name="", new=True) :
    Matrix.__init__(self,client)

    if (new == True):
      self.client.send("newDataMatrix()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.change(filename,field,startX,startY,nX,nY,minX,minY,dX,dY)
    else:
      self.handle = name      

  def change(self,filename,field,startX=0,startY=0,nX=-1,nY=-1,
             minX=0, minY=0, dX=1, dY=1):
    """ Change the parameters if a Data Matrix inside kst.
    
    :param filename: The name of the file/data source to read the scalar from.
    :param field: the name of the vector in the data source.  
    :param startX/Y: the x/y index to start reading from. startX/Y = -1 
                    to count from the right/bottom.
    :param nX/Y: the number of columns/rows to read.  nX/Y = -1 to read
                to the end.
    :param minX/Y: Hint to Images of the coordinates corresponding to the
                  the left/bottom of the Matrix
    :param stepX/Y: Hint to Images of the spacing between points.
    """    
    self.client.send_si(self.handle, "change("+b2str(filename)+","+
                        b2str(field)+","+b2str(startX)+","+
                        b2str(startY)+","+b2str(nX)+","+b2str(nY)+","+
                        b2str(minX)+","+b2str(minY)+","+b2str(dX)+","+
                        b2str(dY)+")")

  def field(self):
    """  Returns the fieldname. """
    return self.client.send_si(self.handle, "field()")

  def filename(self):
    """  Returns the filename. """
    return self.client.send_si(self.handle, "filename()")

  def start_x(self):
    """  Returns the X index of the matrix in the file """
    return self.client.send_si(self.handle, "startX()")

  def start_y(self):
    """  Returns the Y index of the matrix in the file """
    return self.client.send_si(self.handle, "startX()")

class Relation(NamedObject):
  """ Convenience class. You should not use it directly."""
  def __init__(self,client) :
    NamedObject.__init__(self,client)

  def max_x(self):
    """  Returns the max X value of the curve or image. """
    return self.client.send_si(self.handle, "maxX()")

  def min_x(self):
    """  Returns the min X value of the curve or image. """
    return self.client.send_si(self.handle, "minX()")

  def max_y(self):
    """  Returns the max Y value of the curve or image. """
    return self.client.send_si(self.handle, "maxY()")

  def min_y(self):
    """  Returns the min Y value of the curve or image. """
    return self.client.send_si(self.handle, "minY()")

  def show_edit_dialog(self):
    """  shows the edit dialog for the curve or image. """
    return self.client.send_si(self.handle, "showEditDialog()")

class Curve(Relation):
  """ A Curve inside kst.
  
  This class represents a string you would create via 
  "Create>Curve" from the menubar inside kst.  The parameters of this 
  function mirror the parameters within "Create>Curve".

  :param xVector: The vector which specifies the X coordinates of each point.
  :param xVector: The vector which specifies the Y coordinates of each point.
  
  Use the convenience function in client to create a curve in kst session 
  "client" of vectors v1 and v2::
  
    c1 = client.Curve(v1, v2)
  
  """
  def __init__(self,client, xVector, yVector, name="", new=True) :
    Relation.__init__(self,client)

    if (new == True):
      self.client.send("newCurve()")
      self.client.send("setXVector("+xVector.handle+")")
      self.client.send("setYVector("+yVector.handle+")")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)
      self.set_name(name)
    else:
      self.handle = name      

  def set_y_error(self,vector, vectorminus=0):
    """ Set the Y Error flags for the curve.  
    
    The error bars are symetric if vectorminus is not set. 
    """
    self.client.send("beginEdit("+self.handle+")")

    self.client.send("setYError("+vector.handle+")")
    if vectorminus != 0:
      self.client.send("setYMinusError("+vectorminus.handle+")")
    else:
      self.client.send("setYMinusError("+vector.handle+")")

    self.client.send("endEdit()")

  def set_x_error(self,vector, vectorminus=0):
    """ Set the X Error flags for the curve.  
    
    The error bars are symetric if vectorminus is not set.  
    """
    self.client.send("beginEdit("+self.handle+")")

    self.client.send("setXError("+vector.handle+")")
    if vectorminus != 0:
      self.client.send("setXMinusError("+vectorminus.handle+")")
    else:
      self.client.send("setXMinusError("+vector.handle+")")

    self.client.send("endEdit()")

  def set_color(self,color):
    """ Set the color of the points and lines.  
    
    Colors are given by a name such as ``red`` or a hex number such 
    as ``#FF0000``. 
    """
    self.client.send_si(self.handle, "setColor("+color+")")

  def set_head_color(self,color):
    """ Set the color of the Head marker, if any.  
    
    Colors are given by a name such as ``red`` or a hex number such 
    as ``#FF0000``. 
    """
    self.client.send_si(self.handle, "setHeadColor("+color+")")

  def set_bar_fill_color(self,color):
    """ Set the fill color of the histogram bars, if any.  
    
    Colors are given by a name such as ``red`` or a hex number such 
    as ``#FF0000``. 
    """
    self.client.send_si(self.handle, "setBarFillColor("+color+")")

  def set_has_points(self,has=True):
    """ Set whether individual points are drawn on the curve """
    if (has == True):
      self.client.send_si(self.handle, "setHasPoints(True)")
    else:
      self.client.send_si(self.handle, "setHasPoints(False)")

  def set_has_bars(self,has=True):
    """ Set whether histogram bars are drawn. """
    if (has == True):
      self.client.send_si(self.handle, "setHasBars(True)")
    else:
      self.client.send_si(self.handle, "setHasBars(False)")

  def set_has_lines(self,has=True):
    """ Set whether lines are drawn. """
    if (has == True):
      self.client.send_si(self.handle, "setHasLines(True)")
    else:
      self.client.send_si(self.handle, "setHasLines(False)")

  def set_has_head(self,has=True):
    """ Set whether a point at the head of the line is drawn """
    if (has == True):
      self.client.send_si(self.handle, "setHasHead(True)")
    else:
      self.client.send_si(self.handle, "setHasHead(False)")

  def set_line_width(self,x):
    """ Sets the width of the curve's line. """
    self.client.send_si(self.handle, "setLineWidth("+b2str(x)+")")

  def set_point_size(self,x):
    """ Sets the size of points, if they are drawn. """
    self.client.send_si(self.handle, "setPointSize("+b2str(x)+")")

  def set_point_density(self,density):
    """ Sets the point density.  
    
    When show_points is true, this option can be used to only show a
    subset of the points, for example, to use point types to discriminate
    between different curves..  This does not effect 'lines', where every
    point is always connected.
    
    density can be from 0 (all points) to 4. 
    """
    self.client.send_si(self.handle, "setPointDensity("+b2str(density)+")")

  def set_point_type(self,pointType):
    """ Sets the point type.  
    
    0 is an X, 1 is an open square, 2 is an open circle, 
    3 is a filled circle, 4 is a downward open triangle, 
    5 is an upward open triangle, 6 is a filled square, 
    7 is a plus, 8 is a asterix,
    9 is a downward filled triangle, 10 is an upward filled triangle, 
    11 is an open diamond, and 12 is a filled diamond.
    
    """
    self.client.send_si(self.handle, "setPointType("+b2str(pointType)+")")

  def set_head_type(self,x):
    """ Sets the head point type.  See set_point_type for details."""
    self.client.send_si(self.handle, "setHeadType("+b2str(x)+")")

  def set_line_style(self,lineStyle):
    """ Sets the line type.  
    
    0 is SolidLine, 1 is DashLine, 2 is DotLine, 3 is DashDotLine, 
    and 4 isDashDotDotLine,
    """
    self.client.send_si(self.handle, "setLineStyle("+b2str(lineStyle)+")")


  def color(self):
    """ Returns the curve color. """
    return self.client.send_si(self.handle, "color()")

  def head_color(self):
    """ Returns the curve head color. """
    return self.client.send_si(self.handle, "headColor()")

  def bar_fill_color(self):
    """ Returns the bar fill color. """
    return self.client.send_si(self.handle, "barFillColor()")

  def has_points(self):
    """ Returns True if the line has points. """
    return (self.client.send_si(self.handle, "hasPoints()")=="True")

  def has_lines(self):
    """ Returns True if the line has lines. """
    return (self.client.send_si(self.handle, "hasLines()")=="True")

  def has_bars(self):
    """ Returns True if the line has historgram bars. """
    return (self.client.send_si(self.handle, "hasBars()")=="True")

  def has_head(self):
    """ Returns True if the last point has a special marker. """
    return (self.client.send_si(self.handle, "hasHead()")=="True")

  def line_width(self):
    """ Returns the width of the line. """
    return self.client.send_si(self.handle, "lineWidth()")

  def point_size(self):
    """ Returns the size of the points. """
    return self.client.send_si(self.handle, "pointSize()")

  def point_type(self):
    """ Returns index of the point type.  See set_point_type. """
    return self.client.send_si(self.handle, "pointType()")

  def head_type(self):
    """ Returns index of the head point type.  See set_point_type. """
    return self.client.send_si(self.handle, "headType()")

  def line_style(self):
    """ Returns the index of the line style.  See set_line_style. """
    return self.client.send_si(self.handle, "lineStyle()")

  def point_density(self):
    """ Returns the density of points shown.  see set_point_density.  """
    return self.client.send_si(self.handle, "pointDensity()")


class ViewItem(NamedObject):
  """ Convenience class. You should not use it directly."""
  def __init__(self,client):
    self.client=client
    
  def set_h_margin(self,margin):
    self.client.send_si(self.handle, 
                        "setLayoutHorizontalMargin("+b2str(margin)+")")

  def set_v_margin(self,margin):
    self.client.send_si(self.handle, 
                        "setLayoutVerticalMargin("+b2str(margin)+")")

  def set_h_space(self,space):
    self.client.send_si(self.handle, 
                        "setLayoutHorizontalSpacing("+b2str(space)+")")

  def set_v_space(self,space):
    self.client.send_si(self.handle, 
                        "setLayoutVerticalSpacing("+b2str(space)+")")

  def set_fill_color(self,color):
    """ Set the fill/background color.
    
    Colors are given by a name such as ``red`` or a hex number such 
    as ``#FF0000``. 

    """
    self.client.send_si(self.handle, b2str("setFillColor("+b2str(color)+")"))
    
  def set_fill_style(self,style):
    """ Set the background fill style.
    
    This is equivalent to setting the index of Apperance>Fill>Style within 
    a view item dialog in kst.::
    
     0:  NoBrush          1:  SolidPattern
     2:  Dense1Pattern    3:  Dense2Pattern
     4:  Dense3Pattern    5:  Dense4Pattern
     6:  Dense5Pattern    7:  Dense6Pattern
     8:  Dense7Pattern    9:  HorPattern
     11: VerPattern       12: CrossPattern, 
     13: BDiagPattern     14: FDiagPattern.
    
    """
    self.client.send_si(self.handle, 
                        "setIndexOfFillStyle("+b2str(style)+")")

  def set_stroke_style(self,style):
    """ Set the stroke style of lines for the item.
    
    This is equivalent to setting the index of Apperance>Stroke>Style 
    within a view item dialog in kst::
    
     0: SolidLine       1: DashLine
     2: DotLine         3: DashDotLine 
     4: DashDotDotLine  5: CustomDashLine
     
    """
    self.client.send_si(self.handle, "setIndexOfStrokeStyle("+b2str(style)+")")

  def set_stroke_width(self,width):
    """ Set the width of lines for the item. """
    self.client.send_si(self.handle, "setStrokeWidth("+b2str(width)+")")

  def set_stroke_brush_color(self,color):
    """ Set the color for lines for the item.
    
    Colors are given by a name such as ``red`` or a hex number 
    such as ``#FF0000``. 
    """
    self.client.send_si(self.handle, "setStrokeBrushColor("+b2str(color)+")")

  def set_stroke_brush_style(self,style):
    """ Set the brush style for lines for the item.
    
    This is equivalent to setting the index of Apperance>Stroke>Brush Style 
    within a view item dialog in kst.  
    
    This sets the brush type for lines in the item, and not for the fill, 
    so values other than ``1`` (SolidPattern) only make sense for wide lines
    and are rarely used::
    
     0:  NoBrush          1:  SolidPattern
     2:  Dense1Pattern    3:  Dense2Pattern
     4:  Dense3Pattern    5:  Dense4Pattern
     6:  Dense5Pattern    7:  Dense6Pattern
     8:  Dense7Pattern    9:  HorPattern
     11: VerPattern       12: CrossPattern, 
     13: BDiagPattern     14: FDiagPattern.
    
    """
    self.client.send_si(self.handle, 
                        "setIndexOfStrokeBrushStyle("+b2str(style)+")")

  def set_stroke_join_style(self,style):
    """ Set the style by which lines are joined in the item.
    
    This is equivalent to setting the index of Apperance>Stroke>Join Style 
    within a view item dialog in kst.
    
    0 is MiterJoin, 1 is BevelJoin, 2 is RoundJoin,
    and 3 is SvgMiterJoin.
    """
    self.client.send_si(self.handle, 
                        "setIndexOfStrokeJoinStyle("+b2str(style)+")")

  def set_stroke_cap_style(self,style):
    """ Set the cap style for the ends of lines in the item.
    
    This is equivalent to setting the index of Apperance>Stroke>Cap Style 
    within a view item dialog in kst.
    
    0 is FlatCap, 1 is SquareCap, and 2 is RoundCap.
    """
    self.client.send_si(self.handle, 
                        "setIndexOfStrokeCapStyle("+b2str(style)+")")

  def set_fixed_aspect_ratio(self, fixed=True):
    """ if True, fix the aspect ratio of the item to its current value.
    
    This is equivalent to checking Dimensions>Fix aspect ratio within a 
    view item dialog in kst.
    """
    if fixed == True:
      self.client.send_si(self.handle, b2str("checkFixAspectRatio()"))
    else:
      self.client.send_si(self.handle, b2str("uncheckFixAspectRatio()"))

  def set_pos(self,pos):
    """ Set the center position of the item.
    
    :param pos: a 2 element tuple ``(x,y)`` specifying the position.  The Top Left is (0,0).
                The Bottom Right is (1,1)

    This is equivalent to setting Dimensions>Position within a view 
    item dialog in kst.    
    """
    x,y = pos
    
    self.client.send("beginEdit("+self.handle+")")
    self.client.send("setPosX("+b2str(x)+")")
    self.client.send("setPosY("+b2str(y)+")")
    self.client.send("endEdit()")

  def set_size(self,size):
    """ Set the size of the item.
    
    :param size: a 2 element tuple ``(w,h)`` specifying the size.

    Elements go from 0 to 1.  If the aspect ratio is fixed, then ``h`` 
    is ignored.

    This is equivalent to setting Dimensions>Position within a view 
    item dialog in kst.    
    
    """
    w,h = size
    self.client.send("beginEdit("+self.handle+")")
    self.client.send("setGeoX("+b2str(w)+")")
    self.client.send("setGeoY("+b2str(h)+")")
    self.client.send("endEdit()")

  def set_rotation(self,rot):
    """ Set the rotation of the item.
    
    This is equivalent to setting Dimensions>Rotation within a view item dialog.
    
    Scalars can be included by wrapping their names in ``[ ]``. eg ``[(X1)]``
    
    Labels support a subset of latex.
    """
    self.client.send_si(self.handle, b2str("setRotation("+b2str(rot)+")"))

  def remove(self):
    """ This removes the object from Kst. """
    self.client.send("eliminate("+self.handle+")")

# LABELS ######################################################################
class Label(ViewItem) :
  """ A floating label inside kst.

  :param text: the text of the label.  Supports scalars, equations, and a 
               LaTeX subset.
  :param pos: a 2 element tuple ``(x,y)`` specifying the position. 
              (0,0) is top left.  (1,1) is bottom right.
  :param rot: rotation of the label in degrees.
  :param fontSize: size of the label in points, when the printed at the
                   reference size.
  :param fontColor: Colors are given by a name such as ``red`` or a 
                    hex number such as ``#FF0000``. 
  :param fontFamily: The font family.  eg, TimeNewRoman.
  
  Scalars and scalar equations can be displayed live in labels. 
  When the scalar is updated, the label is updated.
  
  The format is::
    Scalar:         [scalarname]         eg [GYRO1:Mean(X4)]
    Vector Element: [vectorName[index]]  eg [GYRO1 (V2)[4]]
    Equation:       [=equation]          eg [=[GYRO1:Mean(X4)]/[GYRO1:Sigma (X4)]]
    
  Labels in kst support a derrivitive subset of LaTeX. For example, to display 
  the equation for the area of a circle, you could set the label to ``A=2\pir^2``. 
  Unlike LaTeX, it is not necessary to enter math mode using ``$``. Also, 
  unlike LaTeX, variables are not automatically displayed in italic font. 
  If desired, this must be done explicitly using ``\\textit{}``. 
  
  Greek letters: \\\\name or \\\\Name. eg: ``\\alpha``
  
  Other symbols: ``\\approx, \\cdot, \\ge, \\geq, \\inf, \\approx, \\cdot, 
  \\ge, \\geq, \\inf, \\int, \\le, \\leq, \\ne, \\n, \\partial, \\prod, \\pm, 
  \\sum, \\sqrt``
  
  Font effects: ``\\textcolor{colorname}{colored text}, \\textbf{bold text},
  \\textit{italicized text}, \\underline{underlined text}, 
  \\overline{overlined text}.``
  
  Other:``x^y``, ``x_y``, ``\\t``, ``\\n``, ``\\[``

  This class represents a label you would create via "Create>Annotations>Label" 
  from the menubar inside kst.  
  
  Use the convenience function in Client to create a label "Test Label" in kst::
  
    import pykst as kst
    client=kst.Client()
    L=client.Label("Test Label", (0.25, 0.25), fontSize=18)

  """
  def __init__(self,client, text, pos=(0.5,0.5), rot=0, fontSize=12, 
               bold=False, italic=False, fontColor="black", 
               fontFamily="Serif", name="", new=True) :
    ViewItem.__init__(self,client)

    if (new == True):
      self.client.send("newLabel()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.set_text(text)
      self.set_label_font_size(fontSize)
      self.set_pos(pos)
      self.set_fixed_aspect_ratio(True)
      self.set_rotation(rot)
      self.set_font_color(fontColor)
      self.set_font_family(fontFamily)

      self.set_font_bold(bold)
      self.set_font_italic(italic)
      self.set_name(name)
    else:
      self.handle = name      

  def set_text(self,text):
    """ Set text displayed by the label. 

    Scalars and scalar equations can be displayed live in labels. 
    When the scalar is updated, the label is updated.
    The format is::
    Scalar:         [scalarname]         eg [GYRO1:Mean(X4)]
    Vector Element: [vectorName[index]]  eg [GYRO1 (V2)[4]]
    Equation:       [=equation]          eg [=[GYRO1:Mean(X4)]/[GYRO1:Sigma (X4)]]
    
    Labels in kst support a derrivitive subset of LaTeX. For example, 
    to display the equation for the area of a circle, you could set the 
    label to ``A=2\pir^2``. Unlike LaTeX, it is not necessary to enter
    math mode using ``$``. Also, unlike LaTeX, variables are not 
    automatically displayed in italic font.  If desired, this must be done 
    explicitly using ``\\textit{}``. 
    
    Greek letters: \\\\name or \\\\Name. eg: ``\\alpha``
    
    Other symbols: ``\\approx, \\cdot, \\ge, \\geq, \\inf, \\approx, \\cdot, 
    \\ge, \\geq, \\inf, \\int, \\le, \\leq, \\ne, \\n, \\partial, \\prod, \\pm, 
    \\sum, \\sqrt``
    
    Font effects: ``\\textcolor{colorname}{colored text}, \\textbf{bold text},
    \\textit{italicized text}, \\underline{underlined text}, 
    \\overline{overlined text}.``
    
    Other:``x^y``, ``x_y``, ``\\t``, ``\\n``, ``\\[``    
    """
    self.client.send_si(self.handle, b2str("setLabel("+b2str(text)+")"))

  def set_label_font_size(self,size):
    """ size of the label in points, when the printed at the reference size."""
    self.client.send_si(self.handle, b2str("setFontSize("+b2str(size)+")"))

  def set_font_bold(self, bold = True):
    """ . . . """
    if bold == True:
      self.client.send_si(self.handle, b2str("checkLabelBold()"))
    else:
      self.client.send_si(self.handle, b2str("uncheckLabelBold()"))

  def set_font_italic(self, italic = True):
    """ . . . """
    if italic == True:
      self.client.send_si(self.handle, b2str("checkLabelItalic()"))
    else:
      self.client.send_si(self.handle, b2str("uncheckLabelItalic()"))

  def set_font_color(self,color):
    """ Colors are given by a name such as ``red`` or a hex number such 
    as ``#FF0000`` """
    self.client.send_si(self.handle, b2str("setLabelColor("+b2str(color)+")"))

  def set_font_family(self,family):
    """ set the font family.  eg, TimeNewRoman. """
    self.client.send_si(self.handle, b2str("setFontFamily("+b2str(family)+")"))


class ExistingLabel(Label):
  def  __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=client.send("getLabelList()")
    ret=[]
    while x.contains('['):
      y=x
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingLabel(client,y))
    return ret
  




class Box(ViewItem) :
  """ A floating box inside kst.

  :param pos: a 2 element tuple ``(x,y)`` specifying the position. 
              ``(0,0)`` is top left.  ``(1,1)`` is bottom right.
  :param size: a 2 element tuple ``(w,h)`` specifying the size.
              ``(1,1)`` is the size of the window.
  :param rotation: rotation of the label in degrees.
  :param fillColor: the background color.
  :param fillStyle: the background fill style.  See set_fill_style.
  :param strokeStyle: see set_stroke_style
  :param strokeWidth: the pen width for the box outline.
  :param strokeBrushColor: the box outline color
  :param strokeBrushStyle: see set_stroke_brush_style
  :param strokeJoinStyle: see set_stroke_join_style
  :param strokeCapStyle: see set_stroke_cap_style
  :param fixAspect: if true, the box will have a fixed aspect ratio.

  Colors are given by a name such as ``red`` or a hex number such 
  as ``#FF0000``. 
  
  This class represents a box you would create via "Create>Annotations>Box" 
  from the menubar inside kst.
  
  Use the convenience function in Client to create a box in kst::
  
    import pykst as kst
    client=kst.Client()
    ...
    B=client.Box((0.25, 0.25), (0.2, 0.1), fillColor="blue")
  
  """
  def __init__(self,client, pos=(0.1,0.1), size=(0.1,0.1), rot=0, 
               fillColor="white", fillStyle=1, strokeStyle=1, strokeWidth=1,
               strokeBrushColor="black", strokeBrushStyle=1, 
               strokeJoinStyle=1, strokeCapStyle=1, fixAspect=False,
               name="", new=True) :
    ViewItem.__init__(self,client)

    if (new == True):
      self.client.send("newBox()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.set_pos(pos)
      self.set_size(size)

      self.set_fixed_aspect_ratio(fixAspect)
      self.set_rotation(rot)

      self.set_stroke_brush_color(strokeBrushColor)
      self.set_fill_color(fillColor)
      self.set_fill_style(fillStyle)
      self.set_stroke_style(strokeStyle)
      self.set_stroke_width(strokeWidth)
      self.set_stroke_brush_color(strokeBrushColor)
      self.set_stroke_brush_style(strokeBrushStyle)
      self.set_stroke_join_style(strokeJoinStyle)
      self.set_stroke_cap_style(strokeCapStyle)
      self.set_name(name)
    else:
      self.handle = name      

  @classmethod
  def getList(cls,client):
    x=client.send("getBoxList()")
    ret=[]
    while x.contains('['):
      y=x
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret





class Circle(ViewItem) :
  """ A floating circle inside kst.

  :param pos: a 2 element tuple ``(x,y)`` specifying the position. 
              ``(0,0)`` is top left.  ``(1,1)`` is bottom right.
  :param diameter: the diameter of the circle.  1 is the width of the window.
  :param fillColor: the background color.
  :param fillStyle: the background fill style.  See set_fill_style.
  :param strokeStyle: see set_stroke_style
  :param strokeWidth: the pen width for the circle outline.
  :param strokeBrushColor: the circle outline color
  :param strokeBrushStyle: see set_stroke_brush_style
  
  Colors are given by a name such as ``red`` or a hex number such 
  as ``#FF0000``. 
  
  This class represents a circle you would create via 
  "Create>Annotations>Circle" from the menubar inside kst.

  Use the convenience function in Client to create a circle in kst::
  
    import pykst as kst
    client=kst.Client()
    ...
    Cr=client.Cirvle((0.5, 0.5), 0.2, fillColor="red")
  
  """
  def __init__(self,client,pos=(0.1, 0.1), diameter=0.1,
               fillColor="white",fillStyle=1,strokeStyle=1,
               strokeWidth=1,strokeBrushColor="grey",strokeBrushStyle=1, 
               name="", new=True) :
    ViewItem.__init__(self,client)

    if (new == True):
      self.client.send("newCircle()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.set_pos(pos)
      self.set_diameter(diameter)

      self.set_stroke_brush_color(strokeBrushColor)
      self.set_fill_color(fillColor)
      self.set_fill_style(fillStyle)
      self.set_stroke_style(strokeStyle)
      self.set_stroke_width(strokeWidth)
      self.set_stroke_brush_color(strokeBrushColor)
      self.set_stroke_brush_style(strokeBrushStyle)
      self.set_name(name)
    else:
      self.handle = name      

  @classmethod
  def getList(cls,client):
    x=client.send("getCircleList()")
    ret=[]
    while x.contains('['):
      y=x
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret
    
  def set_diameter(self,diameter):
    """ set the diamter of the circle.
    
    The width of the window is 1.0.
    """
    self.client.send_si(self.handle,"setGeoX("+b2str(diameter)+")")


class Ellipse(ViewItem) :
  """ A floating ellipse inside kst.

  :param pos: a 2 element tuple ``(x,y)`` specifying the position. 
              ``(0,0)`` is top left.  ``(1,1)`` is bottom right.
  :param size: a 2 element tuple ``(w,h)`` specifying the size.
              ``(1,1)`` is the size of the window.
  :param fillColor: the background color.
  :param fillStyle: the background fill style.  See set_fill_style.
  :param strokeStyle: see set_stroke_style
  :param strokeWidth: the pen width for the ellipse outline.
  :param strokeBrushColor: the ellipse outline color
  :param strokeBrushStyle: see set_stroke_brush_style
  
  Colors are given by a name such as ``red`` or a hex number such 
  as ``#FF0000``. 
  
  This class represents an ellipse you would create via 
  "Create>Annotations>Ellipse" from the menubar inside kst.

  Use the convenience function in Client to create an Ellipse in kst::
  
    import pykst as kst
    client=kst.Client()
    ...
    E=client.Ellipse((0.25, 0.25), (0.2, 0.1), fillColor="green")
  
  """
  def __init__(self,client,pos=(0.1,0.1), size=(0.1,0.1),
               rot=0, fillColor="white", fillStyle=1, strokeStyle=1,
               strokeWidth=1, strokeBrushColor="black", strokeBrushStyle=1,
               fixAspect=False, name="", new=True) :
    ViewItem.__init__(self,client)

    if (new == True):
      self.client.send("newEllipse()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.set_pos(pos)
      self.set_size(size)
      if fixAspect==True:
          self.set_fixed_aspect_ratio(True)
      else:
          self.set_fixed_aspect_ratio(False)

      self.set_rotation(rot)

      self.set_stroke_brush_color(strokeBrushColor)
      self.set_fill_color(fillColor)
      self.set_fill_style(fillStyle)
      self.set_stroke_style(strokeStyle)
      self.set_stroke_width(strokeWidth)
      self.set_stroke_brush_color(strokeBrushColor)
      self.set_stroke_brush_style(strokeBrushStyle)
      self.set_name(name)
    else:
      self.handle = name      

  @classmethod
  def getList(cls,client):
    x=client.send("getEllipseList()")
    ret=[]
    while x.contains('['):
      y=x
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret





class Line(ViewItem) :
  """ A floating line inside kst.

  :param pos: a 2 element tuple ``(x,y)`` specifying the position of the
              center of the line. 
              ``(0,0)`` is top left.  ``(1,1)`` is bottom right.
  :param length: The length of the line.  1 is the width of the window.
  :param rot: rotation of the line in degrees.
  :param strokeStyle: see set_stroke_style
  :param strokeWidth: the pen width for the ellipse outline.
  :param strokeBrushColor: the ellipse outline color
  :param strokeBrushStyle: see set_stroke_brush_style
  :param strokeCapStyle: see set_stroke_cap_style

  Colors are given by a name such as ``red`` or a hex number such 
  as ``#FF0000``. 
  
  This class represents a line you would create via "Create>Annotations>Line" from the menubar inside kst.

  Colors are given by a name such as ``red`` or a hex number such as ``#FF0000``".
  
  Use the convenience function in Client to create a line in kst::
  
    import pykst as kst
    client=kst.Client()
    ...
    Ln=client.Line((0.25, 0.25), 0.2, rot=15)

  """
  def __init__(self,client,pos=(0.1,0.1),length=0.1,rot=0,
               strokeStyle=1,strokeWidth=1,strokeBrushColor="black",
               strokeBrushStyle=1,strokeCapStyle=1, name="", new=True) :
    ViewItem.__init__(self,client)

    if (new == True):
      self.client.send("newLine()")
      self.handle=self.client.send("endEdit()")

      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.set_pos(pos)
      self.set_length(length)
      self.set_rotation(rot)

      self.set_stroke_brush_color(strokeBrushColor)
      self.set_stroke_style(strokeStyle)
      self.set_stroke_width(strokeWidth)
      self.set_stroke_brush_color(strokeBrushColor)
      self.set_stroke_brush_style(strokeBrushStyle)
      self.set_stroke_cap_style(strokeCapStyle)
      self.set_name(name)
    else:
      self.handle = name      

  @classmethod
  def getList(cls,client):
    x=client.send("getLineList()")
    ret=[]
    while x.contains('['):
      y=x
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret

  def set_length(self, length):
    """ set the length of the line.
    
    The width of the window is 1.0.
    """
    self.client.send_si(self.handle,"setGeoX("+b2str(length)+")")


class Arrow(ViewItem) :
  """ A floating arrow inside kst.

  :param pos: a 2 element tuple ``(x,y)`` specifying the position of the
              center of the line. 
              ``(0,0)`` is top left.  ``(1,1)`` is bottom right.
  :param length: The length of the line.  1 is the width of the window.
  :param rot: rotation of the line in degrees.
  :param arrowAtStart: if True, draw an arrow at the start of the line.
  :param arrowAtEnd: if True, draw an arrow at the end of the line.
  :param arrowSize: the size of the arrow.
  :param strokeStyle: see set_stroke_style.
  :param strokeWidth: the pen width for the ellipse outline.
  :param strokeBrushColor: the ellipse outline color
  :param strokeBrushStyle: see set_stroke_brush_style
  :param strokeCapStyle: see set_stroke_cap_style

  Colors are given by a name such as ``red`` or a hex number such 
  as ``#FF0000``. 
  
  This class represents an arrow you would create via 
  "Create>Annotations>Arrow" from the menubar inside kst.

  Use the convenience function in Client to create an arrow in kst::
  
    import pykst as kst
    client=kst.Client()
    ...
    Ln=client.Arrow((0.25, 0.25), 0.2, rot=15, arrowAtStart=True)
    
  """
  def __init__(self,client,pos=(0.1,0.1), length=0.1, rot=0, 
               arrowAtStart = False, arrowAtEnd = True, arrowSize = 12.0, 
               strokeStyle=1, strokeWidth=1, strokeBrushColor="black",
               strokeBrushStyle=1, strokeCapStyle=1, name="", new=True) :
    ViewItem.__init__(self,client)

    if (new == True):
      self.client.send("newArrow()")
      self.handle=self.client.send("endEdit()")
      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.set_pos(pos)
      self.set_length(length)
      self.set_rotation(rot)

      self.set_stroke_brush_color(strokeBrushColor)
      self.set_stroke_style(strokeStyle)
      self.set_stroke_width(strokeWidth)
      self.set_stroke_brush_color(strokeBrushColor)
      self.set_stroke_brush_style(strokeBrushStyle)
      self.set_stroke_cap_style(strokeCapStyle)
      self.set_arrow_at_start(arrowAtStart)
      self.set_arrow_at_end(arrowAtEnd)
      self.set_arrow_size(arrowSize)
      self.set_name(name)
    else:
      self.handle = name      

  def set_arrow_at_start(self, arrow=True) :
    """ Set whether an arrow head is shown at the start of the line """
    if arrow==True:
      self.client.send_si(self.handle, b2str("arrowAtStart(True)"))
    else:
      self.client.send_si(self.handle, b2str("arrowAtStart(False)"))

  def set_arrow_at_end(self, arrow=True) :
    """ Set whether an arrow head is shown at the end of the line """
    if arrow==True:
      self.client.send_si(self.handle, b2str("arrowAtEnd(True)"))
    else:
      self.client.send_si(self.handle, b2str("arrowAtEnd(False)"))

  def set_arrow_size(self, arrowSize) :
    self.client.send_si(self.handle, b2str("arrowHeadScale("+b2str(arrowSize)+")"))

  def set_length(self, length):
    """ set the length of the line.
    
    The width of the window is 1.0.
    """
    self.client.send_si(self.handle,"setGeoX("+b2str(length)+")")

  @classmethod
  def getList(cls,client):
    x=client.send("getArrowList()")
    ret=[]
    while x.contains('['):
      y=x
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingViewItem(client,y))
    return ret
    
    
    
    
class Picture(ViewItem) :
  """ A floating image inside kst.

  :param filename: the file which holds the image to be shown.
  :param pos: a 2 element tuple ``(x,y)`` specifying the position of the
              center of the picture. 
              ``(0,0)`` is top left.  ``(1,1)`` is bottom right.
  :param width: The width of the picture.  1 is the width of the window.
  :param rot: rotation of the picture in degrees.

  This class represents a picture you would create via 
  "Create>Annotations>Picture" from the menubar inside kst.
  
  Use the convenience function in Client to create a picture in kst::
  
    import pykst as kst
    client=kst.Client()
    ...
    pic=client.Picture("image.jpg", (0.25, 0.25), 0.2)

  BUG: the aspect ratio of the picture is wrong.
  """
  def __init__(self,client,filename,pos=(0.1,0.1), width=0.1,rot=0, 
               name="", new=True) :
    ViewItem.__init__(self,client)

    if (new == True):
      self.client.send("newPicture("+b2str(filename)+")")
      self.handle=self.client.send("endEdit()")

      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.set_pos(pos)
      self.set_width(width)
      self.set_fixed_aspect_ratio(True)
      self.set_rotation(rot)
      self.set_name(name)
    else:
      self.handle = name      

  def set_width(self, width):
    """ set the width of the picture.
    
    The width of the window is 1.0.
    """
    self.client.send_si(self.handle,"setGeoX("+b2str(width)+")")
  

  def setPicture(self,pic):
    """ BUG: aspect ratio is not changed. There is no parellel for this 
    function within the kst GUI. """
    self.client.send_si(self.handle, b2str("setPicture("+b2str(pic)+")"))
    
  @classmethod
  def getList(cls,client):
    x=client.send("getPictureList()")
    ret=[]
    while x.contains('['):
      y=x
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingPicture(client,y))
    return ret





class SVG(ViewItem) :
  """ A floating svg image inside kst.

  :param filename: the file which holds the svg image to be shown.
  :param pos: a 2 element tuple ``(x,y)`` specifying the position of the
              center of the picture. 
              ``(0,0)`` is top left.  ``(1,1)`` is bottom right.
  :param width: The width of the picture.  1 is the width of the window.
  :param rot: rotation of the picture in degrees.

  This class represents a picture you would create via 
  "Create>Annotations>SVG" from the menubar inside kst.
  
  Use the convenience function in Client to create an SVG picture in kst::
  
    import pykst as kst
    client=kst.Client()
    ...
    svg1=client.SVG("image.svg", (0.25, 0.25), 0.2)

  
  """
  def __init__(self,client,filename, pos=(0.1,0.1), width=0.1, rot=0, 
               name="", new=True) :
    ViewItem.__init__(self,client)

    if (new == True):
      self.client.send("newSvgItem("+b2str(filename)+")")
      self.handle=self.client.send("endEdit()")

      self.handle.remove(0,self.handle.indexOf("ing ")+4)

      self.set_pos(pos)
      self.set_width(width)
      self.set_fixed_aspect_ratio(True)
      self.set_rotation(rot)
      self.set_name(name)
    else:
      self.handle = name      

  def set_width(self, width):
    """ set the width of the picture.
    
    The width of the window is 1.0.
    """
    self.client.send_si(self.handle,"setGeoX("+b2str(width)+")")

  @classmethod
  def getList(cls,client):
    x=client.send("getSVGList()")
    ret=[]
    while x.contains('['):
      y=x
      y.remove(0,1)
      y.remove(y.indexOf(']'),9999999)
      x.remove(0,x.indexOf(']')+1)
      ret.append(ExistingSVG(client,y))
    return ret





class Plot(ViewItem) :
  """ A plot inside kst.

  :param pos: a 2 element tuple ``(x,y)`` specifying the position. 
              ``(0,0)`` is top left.  ``(1,1)`` is bottom right.
  :param size: a 2 element tuple ``(w,h)`` specifying the size.
              ``(1,1)`` is the size of the window.
  :param rotation: rotation of the label in degrees.
  :param fillColor: the background color.
  :param fillStyle: the background fill style.  See set_fill_style.
  :param strokeStyle: see set_stroke_style
  :param strokeWidth: the pen width for the plot outline.
  :param strokeBrushColor: the plot outline color
  :param strokeBrushStyle: see set_stroke_brush_style
  :param strokeJoinStyle: see set_stroke_join_style
  :param strokeCapStyle: see set_stroke_cap_style
  :param fixAspect: if true, the plot will have a fixed aspect ratio.

  Colors are given by a name such as ``red`` or a hex number such 
  as ``#FF0000``. 
  
  This class represents a Plot you would create via 
  "Create>Annotations>Plot" from the menubar inside kst.
  
  To create an plot in kst and plot a curve ``curve1``::
  
    import pykst as kst
    client=kst.Client()
    ...
    P1=client.Plot((0.25, 0.25), (0.5,0.5))
    P1.add(curve1)

  """
  def __init__(self,client,pos=(0.1,0.1),size=(0.1,0.1),rot=0,
               fillColor="white", fillStyle=1, strokeStyle=1, strokeWidth=1,
               strokeBrushColor="black", strokeBrushStyle=1, 
               strokeJoinStyle=1, strokeCapStyle=1, fixAspect=False,
               name="", new=True) :
    ViewItem.__init__(self,client)

    if (new == True):
      self.client.send("newPlot()")    
      self.handle=self.client.send("endEdit()")

      self.handle.remove(0,self.handle.indexOf("ing ")+4)
      self.set_pos(pos)
      self.set_size(size)

      self.set_fixed_aspect_ratio(fixAspect)
      self.set_rotation(rot)

      self.set_stroke_brush_color(strokeBrushColor)
      self.set_fill_color(fillColor)
      self.set_fill_style(fillStyle)
      self.set_stroke_style(strokeStyle)
      self.set_stroke_width(strokeWidth)
      self.set_stroke_brush_color(strokeBrushColor)
      self.set_stroke_brush_style(strokeBrushStyle)
      self.set_stroke_join_style(strokeJoinStyle)
      self.set_stroke_cap_style(strokeCapStyle)
      self.set_name(name)
    else:
      self.handle = name      


  def add(self, relation) :
    """ Add a curve or an image to the plot. """
    self.client.send_si(self.handle, "addRelation(" + relation.handle + ")")

  def set_x_range(self,x0 = 0.0, x1 = 10.0) :
    """ Set X zoom range from x0 to x1 """
    self.client.send_si(self.handle, "setXRange("+b2str(x0)+","+b2str(x1)+")")

  def set_y_range(self, y0 = 0.0, y1 = 10.0) :
    """ Set Y zoom range from y0 to y1 """
    self.client.send_si(self.handle, "setYRange("+b2str(y0)+","+b2str(y1)+")")

  def set_x_auto(self) :
    """ Set X zoom range to autoscale """
    self.client.send_si(self.handle,"setXAuto()")

  def set_y_auto(self) :
    """ Set Y zoom range to autoscale """
    self.client.send_si(self.handle, "setPlotYAuto()")

  def set_x_auto_border(self) :
    """ Set X zoom range to autoscale with a small border """
    self.client.send_si(self.handle, "setPlotXAutoBorder()")

  def set_y_auto_border(self) :
    """ Set Y zoom range to autoscale with a small border """
    self.client.send_si(self.handle, "setYAutoBorder()")

  def set_x_no_spike(self) :
    """ Set X zoom range to spike insensitive autoscale """
    self.client.send_si(self.handle, "setXNoSpike()")

  def set_y_no_spike(self) :
    """ Set Y zoom range to spike insensitive autoscale """
    self.client.send_si(self.handle, "setYNoSpike()")

  def set_x_ac(self, r=0.2) :
    """ Set X zoom range to fixed range, centered around the mean.  
    
    Similar to AC coupling on an oscilloscope. 
    """
    self.client.send_si(self.handle, "setXAC("+b2str(r)+")")

  def set_y_ac(self, r=0.2) :
    """ Set Y zoom range to fixed range, centered around the mean.  
    
    Similar to AC coupling on an oscilloscope. 
    """
    self.client.send_si(self.handle, "setYAC("+b2str(r)+")")

  def set_global_font(self, family="", bold=False, italic=False) :
    """ Set the global plot font.  
    
    By default, the axis labels all use this, unless they have been set 
    to use their own.
    
    If the parameter 'family' is empty, the font family will be unchanged.
    The font will be bold if parameter 'bold' is set to 'bold' or 'True'.
    The font will be italic if parameter 'italic' is set to 'italic' 
    or 'True'.
    """
    self.client.send_si(self.handle, "setGlobalFont("+family+","+
                        b2str(bold)+","+b2str(italic)+")")

  def set_top_label(self, label="") :
    """ Set the plot top label """
    self.client.send_si(self.handle, "setTopLabel("+label+")")

  def set_bottom_label(self, label="") :
    """ Set the plot bottom label """
    self.client.send_si(self.handle, "setBottomLabel("+label+")")

  def set_left_label(self, label="") :
    """ Set the plot left label """
    self.client.send_si(self.handle, "setLeftLabel("+label+")")

  def set_right_label(self, label="") :
    """ Set the plot right label """
    self.client.send_si(self.handle, "setRightLabel("+label+")")

  def set_top_label_auto(self) :
    """ Set the top label to auto generated. """
    self.client.send_si(self.handle, "setTopLabelAuto()")
    
  def set_bottom_label_auto(self) :
    """ Set the bottom label to auto generated. """
    self.client.send_si(self.handle, "setBottomLabelAuto()")

  def set_left_label_auto(self) :
    """ Set the left label to auto generated. """
    self.client.send_si(self.handle, "setLeftLabelAuto()")

  def set_right_label_auto(self) :
    """ Set the right label to auto generated. """
    self.client.send_si(self.handle, "setRightLabelAuto()")

  def normalize_x_to_y(self) :
    """ Adjust the X zoom range so X and Y have the same scale 
    per unit (square pixels) """
    self.client.send_si(self.handle,  "normalizeXtoY()")

  def set_log_x(self) :
    """ Set X axis to log mode. """
    self.client.send_si(self.handle,  "setLogX()")

  def set_log_y(self) :
    """ Set X axis to log mode. """
    self.client.send_si(self.handle,  "setLogY()")

  def set_y_axis_reversed(self, reversed=True) :
    """ set the Y axis to decreasing from bottom to top. """
    if reversed == True:
      self.client.send_si(self.handle,  "setYAxisReversed()")
    else:
      self.client.send_si(self.handle,  "setYAxisNotReversed()")

  def set_x_axis_reversed(self, reversed=True) :
    """ set the X axis to decreasing from left to right. """
    if reversed == True:
      self.client.send_si(self.handle,  "setXAxisReversed()")
    else:
      self.client.send_si(self.handle,  "setXAxisNotReversed()")


class ExistingPlot(Plot):
  def __init__(self,client,handle):
    ViewItem.__init__(self,client)
    self.handle=handle
    
  @classmethod
  def getList(cls,client):
    x=client.send("getPlotList()")
    ret=[]
    while x.contains('['):
      y=x
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
    self.handle=self.client.send("endEdit()")

    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    socket.connectToServer(client.serverName)
    socket.waitForConnected(300)
    socket.write(b2str("attachTo("+self.handle+")"))
    
  def set_text(self,text):
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
    self.handle=self.client.send("endEdit()")

    self.handle.remove(0,self.handle.indexOf("ing ")+4)
    socket.connectToServer(b2str(client.serverName))
    socket.waitForConnected(300)
    socket.write(b2str("attachTo("+self.handle+")"))
    
  def set_text(self,text):
    """ Sets the text of the line edit. """
    self.client.send("beginEdit("+self.handle+")")
    self.client.send("setText("+b2str(text)+")")
    self.client.send("endEdit()")
