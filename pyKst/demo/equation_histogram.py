#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("EqHist")

v1 = client.new_generated_vector(0, 10, 1000)
v4 = client.new_generated_vector(0, 20, 10000)

e1 = client.new_equation(v1, "x^2")
e1.set_x(v4)
v2 = e1.y()
v3 = e1.x()
print "vector 2  name: ", v2.name() 
print "vector 2 Value at 4th element: ", v2.value(3)

print "vector 1 type: ", v1.type_str()

c1 = client.new_curve(e1.x(), e1.y())
c1.set_color("blue")

p1 = client.new_plot((0.5,0.25), (1,0.5))
p1.add(c1)

e2 = client.new_equation(v1, "x^2.1")
e2.set_x(v4)
c12 = client.new_curve(e2.x(), e2.y())
p1.add(c12)

psd1 = client.new_spectrum(e1.x())
c2 = client.new_curve(psd1.x(), psd1.y())
c2.set_color("green")

p2 = client.new_plot((0.5,0.75), (1,0.5))

h1 = client.new_histogram(v2)

c3 = client.new_curve(h1.x(), h1.y())
c3.set_color("black")
c3.set_has_bars(True)
c3.set_has_lines(False)
c3.set_bar_fill_color("red")
p2.add(c3)

h1.change(v2, 0, 400, 10)

L1 = client.new_legend(p1)
L2 = client.new_legend(p2)

L1.set_font_size(18)
L2.set_font_color("red")

print "histogram 1 range: ", h1.bin_min(), h1.bin_max()
print "histogram 1 num bins: ", h1.num_bins()

