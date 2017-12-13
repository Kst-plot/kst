#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("viewitems")


P1=client.new_plot((0.25,0.25),(0.5,0.5),0)
P1.set_x_range(-10.0,10.0)
P1.set_global_font("courier",12,False,False)

C1 = client.new_circle((0.9,0.3),0.1, stroke_width = 2, stroke_brush_color="red")
C1.set_fill_color("Green")

C1.set_diameter(0.05)

B1 = client.new_box((0.9,0.9), (0.1,0.1), fill_color="pink")

E1 = client.new_ellipse((0.1, 0.7), (0.1, 0.1), 45, fill_color="blue")

A1 = client.new_arrow((0.1, 0.5), (0.2, 0.8), False, True, 18)
A1.set_stroke_style(3)
#A1.set_endpoints((0.2, 0.7), (0.05, 0.8))

L1 = client.new_line((0.20, 0.20), (0.30, 0.30), stroke_width=2, stroke_brush_color="green")
L1.set_stroke_style(2)
L1.set_parent_auto()
L1.set_lock_pos_to_data(True)
L1.set_endpoints((-5,-0.05), (5, 0.05))

Label = client.new_label("Label Here", (0.7,0.7), 0, 16, font_family="courier")
Label.set_font_italic(True)

Label2 = client.new_label("Sub Label Here", (0.25,0.25), 0, 16, font_family="courier")
Label2.set_font_bold(True)
Label2.set_parent_auto()

P1.set_pos((0.5,0.5))
P1.set_fixed_aspect_ratio(True)

print Label2.position(), Label2.dimensions()

Label2.set_lock_pos_to_data(True)
Label2.set_pos((5,0))

B1.set_pos((0.4, 0.4))

B1.set_parent_auto()

B1.set_lock_pos_to_data(True)
B1.set_pos((-5, 0.05))
