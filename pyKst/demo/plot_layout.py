#!/usr/bin/python2.7
import pykst as kst

client=kst.Client("PlotLayoutDemo")

#autolayout in tab 1
p1 = client.new_plot(font_size = 12)

client.new_plot(font_size = 12)
client.new_plot(font_size = 12)
client.new_plot(font_size = 12)
client.new_plot(font_size = 12)
client.new_plot(font_size = 12)
client.new_plot(font_size = 12)
client.new_plot(font_size = 12)
client.new_plot(font_size = 12)
 
client.cleanup_layout(3)

client.set_tab_text("First")

p1.set_global_font(family = "Courier", font_size = 6)

# manual grid
client.new_tab()
client.set_tab_text("Second")

for x in [.125, 0.375, 0.625, 0.875]:
  for y in [.125, 0.375, 0.625, 0.875]:
    client.new_plot((x,y),(0.25,0.25))

