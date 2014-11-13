.. toctree::
   :maxdepth: 2

PyKst documentation
=================================
PyKst is a python interface to kst. With PyKst, scripts can control and share data with a kst session.

The following minimal example plots sin(x) from -10 to 10::

      import pykst as kst

      client=kst.Client("TestSinX")

      v1 = client.new_generated_vector(-10, 10, 1000)
      e1 = client.new_equation(v1, "sin(x)")
      c1 = client.new_curve(e1.x(), e1.y())
      p1 = client.new_plot()
      p1.add(c1)



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
