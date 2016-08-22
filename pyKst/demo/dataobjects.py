#!/usr/bin/python2.7

# Demonstrate curves, data vectors, equations, spectra, and histograms

import pykst as kst

client=kst.Client("DataObjects")

client.hide_window()

# create a 2x2 grid of plots
P1=client.new_plot(font_size = 12);
P2=client.new_plot(font_size = 12);
P3=client.new_plot(font_size = 12);
P4=client.new_plot(font_size = 12);
client.cleanup_layout(2)

# plot a curve made from data vectors
dv1=client.new_data_vector("./demodata.dat",
                          field="INDEX",
                          start=0,num_frames=2000)
dv2=client.new_data_vector("./demodata.dat",
                          field="Column 2",
                          start=0,num_frames=2000)
c1 = client.new_curve(dv1, dv2)
c1.set_color("red")
c1.set_line_width(2)
P1.add(c1)


# plot x^2 from -1 to 1
gv1 = client.new_generated_vector(-1.0, 1.0, 100)
eq1 = client.new_equation(gv1, "x^2")
c2 = client.new_curve(eq1.x(), eq1.y())
c2.set_color("green")
c2.set_line_width(2)
P2.add(c2)

# plot the spectrum of vector dv2
s1 = client.new_spectrum(dv2, 
                         sample_rate = 60.0,
                         interleaved_average = True,
                         fft_length = 9)
c3 = client.new_curve(s1.x(), s1.y())
c3.set_color("blue")
c3.set_line_width(2)
P3.add(c3)
P3.set_log_x()
P3.set_log_y()

#plot a histogram of dv1
h1 = client.new_histogram(dv2)

c4 = client.new_curve(h1.x(), h1.y())
c4.set_color("black")
c4.set_line_width(1)
c4.set_bar_fill_color("dark green")
c4.set_has_bars()
c4.set_has_lines(False)
P4.add(c4)

client.show_window()
