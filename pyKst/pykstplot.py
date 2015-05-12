#!/usr/bin/python2.7
import pykst as kst
import numpy as _np

_current_plot = None
_client = None
_subplots = {}

class _CurveInfo:
    def __init__(self):
        self.reset()
        self.line_width = 1
        self.line_style = ""
        self.color = "auto"
        self.marker_size=0
        self.label = None


    def reset(self):
        self.x = None
        self.y = None
        self.f = None

def _add_curve_to_plot(P, C):
    global _client

    colors = {'b': "blue",   'r':"red",    'g':"green", 'c':"cyan",
              'm':"magenta", 'y':"yellow", 'k':"black", 'w':"white"}
    points = {'.':(3,3),  ',':(13,12), 'v':(9,10),  '^':(10,10), 's':(6,7),
              '*':(8,9), '+':(7,10),  'D':(12,10), 'x':(0,9), 'o':(3,10)}

    V1 = _client.new_editable_vector(C.x, name="X")
    V2 = _client.new_editable_vector(C.y, name="Y")
    c1 = _client.new_curve(V1, V2)

    if isinstance(C.f, basestring):
        if '--' in C.f:
            c1.set_line_style(1)
        elif '-.' in C.f:
            c1.set_line_style(3)
        elif ':' in C.f:
            c1.set_line_style(2)

        for color_key in colors.keys():
            if color_key in C.f:
                c1.set_color(colors[color_key])

        for point_key in points.keys():
            if point_key in C.f:
                c1.set_has_lines(False)
                c1.set_has_points(True)
                c1.set_point_type(points[point_key][0])
                c1.set_point_size(points[point_key][1])

    c1.set_line_width(C.line_width)
    if C.color != "auto":
        c1.set_color(C.color)

    if (C.line_style == "-"):
        c1.set_line_style(0)
    elif (C.line_style == "--"):
        c1.set_line_style(1)
    elif (C.line_style == ":"):
        c1.set_line_style(2)
    elif (C.line_style == "-."):
        c1.set_line_style(3)

    if (C.marker_size > 0):
        c1.set_point_size(C.marker_size)

    if (C.label is not None):
        c1.set_name(C.label)

    P.add(c1)

def semilogx(*args, **kwargs):
    """
    Add a curve to the current axis (or make a new one)
    with log scaling on the X axis.  All parameters are the same as :func:`plot()`.
    """
    plot(*args, **kwargs)
    _current_plot.set_log_x()

def semilogy(*args, **kwargs):
    """
    Add a curve to the current axis (or make a new one)
    with log scaling on the Y axis.  All parameters are the same as :func:`plot()`.
    """
    plot(*args, **kwargs)
    _current_plot.set_log_y()

def loglog(*args, **kwargs):
    """
    Add a curve to the current axis (or make a new one)
    with log-log scaling.  All parameters are the same as :func:`plot()`.
    """
    plot(*args, **kwargs)
    _current_plot.set_log_y()
    _current_plot.set_log_x()

def xlabel(s):
    """
    Set the *x* axis label of the current axis.
    """
    _current_plot.set_bottom_label(s)

def ylabel(s):
    """
    Set the *y* axis label of the current axis.
    """
    _current_plot.set_left_label(s)

def title(s):
    """
    Set the *top* axis label of the current axis.
    """
    _current_plot.set_top_label(s)

def show():
    """
    This method does nothing, but exists for compatibility
    with matplotlib scripts.
    """
    pass

def savefig(fname, **kwargs):
    """
    Export the kst session as a series of graphics files.  If there is
    only 1 tab, then the file will be called *fname*.  If there are multiple
    tabs, the files will be called, for example, fname_1.png, fname_2.png, etc.

    The plot will be have the same aspect ratio as the kst session, and, for
    pixel based formats, will be 1280 pixels wide.

    The format is determined by the *format* kwarg, or by the *fname* extension
    if *format* is not defined.

    All formats supported by kst are supported, including, among many others, png,
    jpg, eps, svg, and pdf.
    """
    if "format" in kwargs:
        format = kwargs["format"]
    else:
        format = None

    _client.export_graphics_file(fname, format, 1280, 1024, 0)

def figure():
    """
    Creates a new tab in kst.
    """

    global _client
    global _current_plot
    global _subplots

    _client.new_tab()
    _current_plot = None
    _subplots = {}


def plot(*args, **kwargs):
    """
    Plot lines and/or markers to a kst window.  *args* is a variable length
    argument, allowing for multiple *x*, *y* pairs with an
    optional format string.  For example, each of the following is
    legal::

        plot(x, y)        # plot x and y using default line style and color
        plot(x, y, 'bo')  # plot x and y using blue circle markers
        plot(y)           # plot y using x as index array 0..N-1
        plot(y, 'r+')     # ditto, but with red plusses

    An arbitrary number of *x*, *y*, *fmt* groups can be
    specified, as in::

        a.plot(x1, y1, 'g^', x2, y2, 'g-')

    By default, each line is assigned a different color in kst.

    The following format string characters are accepted to control
    the line style or marker:

        ================    ===============================
        character           description
        ================    ===============================
        ``'-'``             solid line style
        ``'--'``            dashed line style
        ``'-.'``            dash-dot line style
        ``':'``             dotted line style
        ``'.'``             point marker
        ``','``             pixel marker
        ``'o'``             circle marker
        ``'v'``             triangle_down marker
        ``'^'``             triangle_up marker
        ``'s'``             square marker
        ``'*'``             star marker
        ``'+'``             plus marker
        ``'x'``             x marker
        ``'D'``             diamond marker
        ================    ===============================


    The following color abbreviations are supported:

        ==========  ========
        character   color
        ==========  ========
        'b'         blue
        'g'         green
        'r'         red
        'c'         cyan
        'm'         magenta
        'y'         yellow
        'k'         black
        'w'         white
        ==========  ========

    Line styles and colors are combined in a single format string, as in
    ``'bo'`` for blue circles.

    The *kwargs* can be used to set the color, the line width, the line type,
    and the legend label.  You can specify colors using full names (``'green'``),
    or hex strings (``'#008000'``).

    Some examples::

        plot([1,2,3], [1,2,3], 'go-', label='line 1', linewidth=2)
        plot([1,2,3], [1,4,9], 'rs',  label='line 2')
        axis([0, 4, 0, 10])
        legend()

    If you make multiple lines with one plot command, the kwargs
    apply to all those lines, e.g.::

        plot(x1, y1, x2, y2, linewidth=2)

    Both lines will have a width of 2.

    You do not need to use format strings, which are just
    abbreviations.  All of the line properties can be controlled
    by keyword arguments.  For example, you can set the color,
    marker, linestyle, and markercolor with::

        plot(x, y, color='green', linestyle='dashed', marker='o',
        color='blue', markersize=12).

    Supported kwargs are::

        color
        label
        linestyle
        linewidth
        markersize

    """
    global _current_plot
    global _client

    if _client is None:
        _client=kst.Client()

    if _current_plot is None:
      _current_plot = _client.new_plot()


    C = _CurveInfo()

    if "linewidth" in kwargs:
        C.line_width = kwargs["linewidth"]
    if "color" in kwargs:
        C.color = kwargs["color"]
    if "linestyle" in kwargs:
        C.line_style = kwargs["linestyle"]
    if "markersize" in kwargs:
        C.marker_size = kwargs["markersize"]
    if "label" in kwargs:
        C.label = kwargs["label"]

    for arg in args:
        if isinstance(arg, basestring):
            C.f = arg
            if (C.y is None) & (isinstance(C.x, _np.ndarray)):
                C.y = C.x
                C.x = linspace(0, C.y.size-1, C.y.size)
            if (isinstance(C.x, _np.ndarray)):
                _add_curve_to_plot(_current_plot, C)
            C.reset()
        else:
            if (isinstance(C.y, _np.ndarray)):
                _add_curve_to_plot(_current_plot, C)
                C.reset()
            if isinstance(C.x, _np.ndarray):
                C.y = _np.asanyarray(arg, dtype=_np.float64)
            else:
                C.x = _np.asanyarray(arg, dtype=_np.float64)

    if (C.y is None) & (isinstance(C.x, _np.ndarray)):
        C.y = C.x
        C.x = _np.asanyarray([0,1,2], dtype=_np.float64)
    if (isinstance(C.x, _np.ndarray)):
        _add_curve_to_plot(_current_plot, C)

def subplot(*args, **kwargs):
    """
    Return a subplot axes positioned by the given grid definition.

    Typical call signature::

      subplot(nrows, ncols, plot_number)

    Where *nrows* and *ncols* are used to notionally split the figure
    into ``nrows * ncols`` sub-axes, and *plot_number* is used to identify
    the particular subplot that this function is to create within the notional
    grid. *plot_number* starts at 1, increments across rows first and has a
    maximum of ``nrows * ncols``.

    In the case when *nrows*, *ncols* and *plot_number* are all less than 10,
    a convenience exists, such that the a 3 digit number can be given instead,
    where the hundreds represent *nrows*, the tens represent *ncols* and the
    units represent *plot_number*. For instance::

      subplot(211)

    produces a subaxes in a figure which represents the top plot (i.e. the
    first) in a 2 row by 1 column notional grid (no grid actually exists,
    but conceptually this is how the returned subplot has been positioned).

    .. note::

       unlike *matplotlib.pyplot.subplot()*, creating a new subplot with a position
       which is entirely inside a pre-existing axes will not delete the previous
       plot.

    Keyword arguments:

      *axisbg*:
        The background color of the subplot, which can be any valid
        color specifier.


    """

    global _current_plot
    global _client
    global _subplots

    w = 0
    h = 0
    x = 0
    y = 0
    n = 0

    if (len(args) == 1):
        h = args[0]/100
        w = (args[0]%100)/10
        n = args[0]%10
    elif (len(args) == 3):
        h = args[0]
        w = args[1]
        n = args[2]
    else:
        w = h = n = 1

    x = (n-1)%w
    y = (n-1)/w

    serial = y + x*100 + h*10000 + w*1000000

    #print args[0], w,h,x,y, serial

    size = (1.0/w, 1.0/h)
    pos = (x/float(w)+0.5/w,y/float(h)+0.5/h)
    #print pos, size

    if serial in _subplots:
        _current_plot = _subplots[serial]
    else:
        if _client is None:
            _client=kst.Client()

        _current_plot = _client.new_plot(pos, size)
        _subplots[serial] = _current_plot

    if "axisbg" in kwargs:
        _current_plot.set_fill_color(kwargs["axisbg"])


