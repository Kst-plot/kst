.. toctree::
   :maxdepth: 2

PyKst documentation
=================================
PyKst is a python interface to kst. With PyKst, scripts can control and share data with a kst session.

Installation
************
You will want the version of pykst.py that goes with your version of kst2.
Until packaging (and the API) are settled, this may mean compiling kst2 from source.

pykst.py can use either PySide or PyQT4.  Make sure one is installed, and edit the begining of pykst.py
accordingly (make sure the one you want to use is enabled, and the other is commented out.)

Then run setup.py to install things properly.  In linux this is::

      sudo python2.7 setup.py install

PyKst depends on python2.7 or greater, and modern versions of NumPy and SciPy.

Examples
********
PyKst can be used control kst, as one would with the GUI.  The following (minimal) example tells kst to
plot sin(x) from -10 to 10. The results are identical to having used create->equation from within kst::

      import pykst as kst

      # start a kst session with the arbitrary name "TestSinX"
      client=kst.Client("TestSinX")

      # inside kst, create the x vector and the equation
      v1 = client.new_generated_vector(-10, 10, 1000)
      e1 = client.new_equation(v1, "sin(x)")

      # inside kst, create a curve, a plot, and add the curve to the plot
      c1 = client.new_curve(e1.x(), e1.y())
      p1 = client.new_plot()
      p1.add(c1)

kst can be also be used to plot numpy arrays, as in this example::

      #!/usr/bin/python2.7
      import pykst as kst
      import numpy as np

      # create a pair of numpy arrays
      x = np.linspace( -10, 10, 1000)
      y = np.sin(x)

      # start a kst session with the arbitrary name "NumpyVector"
      client=kst.Client("NumpyVector")

      # copy the numpy arrays into kst
      V1 = client.new_editable_vector(x, name="X") # the name is for the label
      V2 = client.new_editable_vector(y, name="sin(X)") # the name is for the label

      # inside kst, create a curve, a plot, and add the curve to the plot
      c1 = client.new_curve(V1, V2)
      p1 = client.new_plot()
      p1.add(c1)

Alternativly, one can use a (tiny) subset of matplotlib.pyplot called pykstplot.
This interface is conceptually incompatible with the native interface, and is described
at the end of this document.  As an example::

      #!/usr/bin/python2.7
      import pykstplot as plt
      #import matplotlib.pyplot as plt
      import numpy as np

      x = np.linspace( -10, 10, 100)
      y = np.sin(x)
      z = np.cos(x)

      plt.subplot(221)
      plt.plot(x,y*y, linewidth=2, color = "green", linestyle="-.", label="greenline")
      plt.subplot(122)
      plt.plot(x,y,"k.")
      plt.subplot(223)
      plt.plot(x,z,"m*", markersize=6, color="blue")
      plt.subplot(221, axisbg="lightblue")
      plt.plot(x,z)
      plt.xlabel("X axis")
      plt.ylabel("Y axis")
      plt.title("Title")

      plt.figure()
      plt.plot([1,3,7,15])

      plt.show()

      #plt.savefig("pltdemo.eps")

"

Clients
*******************
.. automodule:: pykst
   :members: Client
   :exclude-members: getArray, getList, send

Primitives
**********
Scalars, Vectors, Matrices, and Strings (Primitives) can be used in 
your scripts to share data with kst. All methods are pass-by-value: 
for example, if you get a value of a scalar and change it, the data 
inside kst is not changed. You need to explicitly call setValue(). 

Scalars
-------
.. autoclass:: DataSourceScalar
   :members:
   :inherited-members:

.. autoclass:: VectorScalar
   :members:
   :inherited-members:

.. autoclass:: GeneratedScalar
   :members:
   :inherited-members:


Vectors
-------
.. autoclass:: DataVector
   :members:
   :inherited-members:

.. autoclass:: GeneratedVector
   :members:
   :inherited-members:

.. autoclass:: EditableVector
   :members:
   :inherited-members:


Matrices
---------
.. autoclass:: DataMatrix
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

Data Objects
************
Data Objects are objects which transform primitives into other
primitives.

Equations
---------
.. autoclass:: Equation
   :members:
   :inherited-members:

Spectra
-------
.. autoclass:: Spectrum
   :members:
   :inherited-members:

Histograms
----------
.. autoclass:: Histogram
   :members:
   :inherited-members:

Linear Fit
----------
.. autoclass:: LinearFit
   :members:
   :inherited-members:

Polynomial Fit
--------------
.. autoclass:: PolynomialFit
   :members:
   :inherited-members:

Relations
*********
Relations are objects which can be added to a plot.

Curves
------
.. autoclass:: Curve 
   :members:
   :inherited-members:

Images
------
.. autoclass:: Image 
   :members:
   :inherited-members:

Annotations
***********
Annotations along with interactive items (see the next section) can be used to turn kst into a graphing calculator, a tetris client, and maybe even a web browser...

.. autoclass:: Label
   :members:
   :inherited-members:
   :exclude-members: set_fill_color, set_fill_style, set_stroke_style, set_stroke_width, set_stroke_brush_color, set_stroke_brush_style, set_stroke_join_style, set_stroke_cap_style, set_fixed_aspect_ratio, set_size
.. autoclass:: Legend
   :members:
   :inherited-members:
   :exclude-members: set_fixed_aspect_ratio, set_size
.. autoclass:: Box
   :members:
   :inherited-members:

.. autoclass:: Circle
   :members:
   :inherited-members:
   :exclude-members: set_stroke_join_style, set_stroke_cap_style, set_fixed_aspect_ratio, set_size

.. autoclass:: Ellipse
   :members:
   :inherited-members:
   :exclude-members: set_stroke_join_style, set_stroke_cap_style

.. autoclass:: Line
   :members:
   :inherited-members:
   :exclude-members: set_fill_color, set_fill_style, set_stroke_join_style, set_fixed_aspect_ratio, set_size

.. autoclass:: Arrow
   :members:
   :inherited-members:
   :exclude-members: set_fill_color, set_fill_style, set_stroke_join_style, set_fixed_aspect_ratio, set_size

.. autoclass:: Picture
   :members:
   :inherited-members:
   :exclude-members: set_fill_color, set_fill_style, set_stroke_style, set_stroke_width, set_stroke_brush_color, set_stroke_brush_style, set_stroke_join_style, set_stroke_cap_style

.. autoclass:: SVG
   :members:
   :inherited-members:
   :exclude-members: set_fill_color, set_fill_style, set_stroke_style, set_stroke_width, set_stroke_brush_color, set_stroke_brush_style, set_stroke_join_style, set_stroke_cap_style

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

Pykstplot
*********
pykstplot re-implements a tiny subset of matplotlib.pyplot.  It is included by importing pykstplot, 
and is conceptually incompatible with pykst.

.. automodule:: pykstplot
   :members:
