#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("DataObjects2")


P1=client.new_plot((0.25,0.25),(0.5,0.5),0)
P2 = client.plot(P1.handle)
P2.set_x_range(-10.0,10.0)
P2.set_global_font("courier",False,False)
P2.set_name("Fred the plot")

C1 = client.new_circle((0.9,0.3),0.1, strokeBrushColor="red")
C2 = client.circle(C1.handle)
C2.set_fill_color("Green")

B1 = client.new_box((0.9,0.9), (0.1,0.1), fillColor="pink")
B2 = client.box(B1.handle)
B2.set_fill_color("Green")

E1 = client.new_ellipse((0.1, 0.7), (0.1, 0.1), 45, fillColor="blue")
E2 = client.ellipse(E1.handle)
E2.set_fill_color("Green")

A1 = client.new_arrow((0.1, 0.5), 0.1, 45, False, True, 18)
A2 = client.arrow(A1.handle)
A2.set_stroke_style(3)

L1 = client.new_line((0.1, 0.5), 0.1, 15, strokeWidth=4, strokeBrushColor="green")
L2 = client.line(L1.handle)
L2.set_stroke_style(2)

I1 = client.new_picture("/home/cbn/Desktop/from_deck1_cropped.jpg",(0.4, 0.6), 0.3, 0)
I2 = client.picture(I1.handle)
I2.set_width(0.1)

S1 = client.new_SVG("/home/cbn/notes/blastbus/layout.svg", (0.6, 0.3), 0.2, 0)
S2 = client.SVG(S1.handle)
S2.set_width(0.1)

Label = client.new_label("Label Here", (0.7,0.7), 0, 16, fontFamily="courier")
Label.set_font_italic(True)
Label2 = client.label(Label.handle)
Label2.set_text("changed label text")


