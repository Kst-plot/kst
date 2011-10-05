.. toctree::
   :maxdepth: 2

Welcome to pykst's documentation!
=================================
PyKst is a python interface to kst. With PyKst, scripts can control and share data with a kst session.

Clients
*******************
To interact with a kst session, you must create a client. To open an interface with the last kst session started, or if none open, a new one, try::

	import pykst as kst
	client = kst.Client()

.. automodule:: pykst
   :members: Client
   :exclude-members: getArray, getList, send

Primitives
**********
Scalars, Vectors, Matracies, and Strings (Primitives) can be used in your scripts to share data with kst. All methods are pass-by-value: for example, if you get a value of a scalar and change it, the data inside kst is not changed. You need to explicitly call setValue(). You can get an instance of an existing primitive through the ExistingScalar, ExistingVector, ExistingMatrix, and ExistingString classes. These classes also provide a class method getList() which returns a list of all instances of that type.

Scalars
-------
.. autoclass:: DataSourceScalar
   :members:
   :inherited-members:
   :exclude-members: setValue

.. autoclass:: DataVectorScalar
   :members:
   :inherited-members:
   :exclude-members: setValue

.. autoclass:: GeneratedScalar
   :members:
   :inherited-members:

.. autoclass:: ExistingScalar
   :members:
   :inherited-members:

Vectors
-------
.. autoclass:: DataVector
   :members:
   :inherited-members:
   :exclude-members: blank, zero, setNewAndShift, setName, resize, newSync

.. autoclass:: GeneratedVector
   :members:
   :inherited-members:
   :exclude-members: changeFrames, numFrames, startFrame, doSkip, doAve, skip, reload_, samplesPerFrame, fileLength, readToEOF, countFromEOF, descriptionTip, isValid

.. autoclass:: EditableVector
   :members:
   :inherited-members:
   :exclude-members: changeFrames, numFrames, startFrame, doSkip, doAve, skip, reload_, samplesPerFrame, fileLength, readToEOF, countFromEOF, descriptionTip, isValid

.. autoclass:: ExistingVector
   :members:
   :inherited-members:

Matricies
---------
Support for matrices is fairly limited, but you can get data into kst with an EditableMatrix. 

.. autoclass:: DataMatrix
   :members:
   :inherited-members:

.. autoclass:: ExistingMatrix
   :members:
   :inherited-members:

.. autoclass:: EditableMatrix
   :members:
   :inherited-members:

Strings
-------
.. autoclass:: DataSourceString
   :members:
   :inherited-members:
   :exclude-members: setValue 

.. autoclass:: GeneratedString
   :members:
   :inherited-members:

.. autoclass:: ExistingString
   :members:
   :inherited-members:

Relations
*********
Relations are objects which can be added to a plot.

Curves
------
.. autoclass:: NewCurve 
   :members:
   :inherited-members:

.. autoclass:: ExistingCurve
   :members:
   :inherited-members:

Equations
---------
An equation is not a relation, but creates a curve which is a relation.

.. autoclass:: NewEquation
   :members:
   :inherited-members:

.. autoclass:: ExistingEquation
   :members:
   :inherited-members:

Images
------
An image is a representation of a matrix

.. autoclass:: ColorImage
   :members:
   :inherited-members:

.. autoclass:: ContourImage
   :members:
   :inherited-members:

.. autoclass:: ColorContourImage
   :members:
   :inherited-members:

.. autoclass:: ExistingImage
   :members:
   :inherited-members:

Other data objects...
**********************
Histograms, power spectrums and spectrograms will hopefully be implemented in a later release!

Annotations
***********
Annotations along with interactive items (see the next section) can be used to turn kst into a graphing calculator, a tetris client, and maybe even a web browser...

.. autoclass:: Label
   :members:
   :inherited-members:

.. autoclass:: Box
   :members:
   :inherited-members:

.. autoclass:: Circle
   :members:
   :inherited-members:

.. autoclass:: Ellipse
   :members:
   :inherited-members:

.. autoclass:: Line
   :members:
   :inherited-members:

.. autoclass:: Arrow
   :members:
   :inherited-members:

.. autoclass:: Picture
   :members:
   :inherited-members:

.. autoclass:: SVG
   :members:
   :inherited-members:

.. autoclass:: Plot
   :members:
   :inherited-members:

Interactive items
*****************
Interactive items are controls which are part of a kst view and use QtNetwork.QLocalSocket to notify the script when the user has interacted with them. They can only be created through scripts.

.. autoclass:: Button
   :members:
   :inherited-members:

.. autoclass:: LineEdit
   :members:
   :inherited-members:
