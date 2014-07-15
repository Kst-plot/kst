#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("TestPlugins")

v1 = client.new_generated_vector(0, 10, 1000)
v4 = client.new_generated_vector(0, 20, 10000)

e1 = client.new_equation(v1, "x^2")
e1.set_x(v4)
v2 = e1.Y()
v3 = e1.X()
print v2.name(), v2.value(3), v3.value(3)


c1 = client.new_curve(e1.X(), e1.Y())
c1.set_color("blue")

p1 = client.new_plot((0.5,0.25), (1,0.5))
p1.add(c1)

psd1 = client.new_spectrum(e1.X())
c2 = client.new_curve(psd1.X(), psd1.Y())
c2.set_color("green")

p2 = client.new_plot((0.5,0.75), (1,0.5))
#p2.add(c2)

print "average?", psd1.interleaved_average()

h1 = client.new_histogram(v2)

c3 = client.new_curve(h1.X(), h1.Y())
c3.set_color("black")
c3.set_has_bars(True)
c3.set_has_lines(False)
c3.set_bar_fill_color("red")
p2.add(c3)

h1.change(v2, 0, 400, 10)

print h1.bin_min(), h1.bin_max(), h1.n_bins(), h1.auto_bin()

