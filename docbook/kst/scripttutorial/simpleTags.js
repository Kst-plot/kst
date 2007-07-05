// very simple KstScript demo: plot a vector collumn from a file
file = new DataSource("demodata.dat")

var w = Kst.windows[0];
var p = new Plot(w);

var x = new DataVector(file,"INDEX");
x.tagName = "xv"

var y = new DataVector(file,"1");
y.tagName = "yv"

var c = new Curve("xv","yv");
c.tagName = "c"

p.curves.append("c");

