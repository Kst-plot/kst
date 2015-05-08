#!/usr/bin/python2.7
import pykst as kst
from numpy  import *
from copy import deepcopy

__current_plot__ = None

class __CurveInfo__:
    x = None
    y = None
    f = None

    def reset(self):
        self.x = None
        self.y = None
        self.f = None

def __add_curve_to_plot__(client, P, C):
    colors = {'b': "blue",   'r':"red",    'g':"green", 'c':"cyan",
              'm':"magenta", 'y':"yellow", 'k':"black", 'w':"white"}
    points = {'.':(3,3),  ',':(13,12), 'v':(9,10),  '^':(10,10), 's':(6,7),
              '*':(8,9), '+':(7,10),  'D':(12,10), 'x':(0,9), 'o':(3,10)}

    V1 = client.new_editable_vector(C.x, name="X")
    V2 = client.new_editable_vector(C.y, name="Y")
    c1 = client.new_curve(V1, V2)

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

    P.add(c1)

def show():
    global __current_plot__
    __current_plot__ = None

def plot(*args):
    global __current_plot__

    client=kst.Client()

    if __current_plot__ is None:
      __current_plot__ = client.new_plot()


    C = __CurveInfo__()

    for arg in args:
        if isinstance(arg, basestring):
            C.f = arg
            if (C.y is None) & (type(C.x) is ndarray):
                C.y = C.x
                C.x = linspace(0, C.y.size-1, C.y.size)
            if (type(C.x) is ndarray):
                __add_curve_to_plot__(client, __current_plot__, C)
            C.reset()
        else:
            if (type(C.y) is ndarray):
                __add_curve_to_plot__(client, __current_plot__, C)
                C.reset()
            if type(C.x) is ndarray:
                C.y = asarray(arg, dtype=float64)
            else:
                C.x = asarray(arg, dtype=float64)

    if (C.y is None) & (type(C.x) is ndarray):
        C.y = C.x
        C.x = asarray([0,1,2], dtype=float64)
    if (type(C.x) is ndarray):
        __add_curve_to_plot__(client, __current_plot__, C)

