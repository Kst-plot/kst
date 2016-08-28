#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("viewitems")


P1=client.new_plot((0.25,0.25),(0.5,0.5),0)
P1.set_x_range(-10.0,10.0)
P1.set_global_font("courier",12,False,False)

C1 = client.new_circle((0.9,0.3),0.1, stroke_width = 2, stroke_brush_color="red")
C1.set_fill_color("Green")

B1 = client.new_box((0.9,0.9), (0.1,0.1), fill_color="pink")

E1 = client.new_ellipse((0.1, 0.7), (0.1, 0.1), 45, fill_color="blue")

A1 = client.new_arrow((0.1, 0.5), 0.1, 45, False, True, 18)
A1.set_stroke_style(3)

L1 = client.new_line((0.1, 0.5), 0.1, 15, stroke_width=4, stroke_brush_color="green")
L1.set_stroke_style(2)

Label = client.new_label("Label Here", (0.7,0.7), 0, 16, font_family="courier")
Label.set_font_italic(True)

Label2 = client.new_label("Sub Label Here", (0.25,0.25), 0, 16, font_family="courier")
Label2.set_font_bold(True)
Label2.update_parent()

