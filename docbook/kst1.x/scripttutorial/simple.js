// A very simple KstScript demo: plot column 1 of a file vs line number
// open the file "demodata.dat"
file = new DataSource("demodata.dat");

// grab the window
var w = Kst.windows[0];

// add a plot to the window
var p = new Plot(w);

// create the X vector which is the line number of the file
var x = new DataVector(file,"INDEX");

// create the Y vector, which is column 1
var y = new DataVector(file,"1");

// create a curve from x and y
var c = new Curve(x,y);

// put the curve in the plot
p.curves.append(c);

