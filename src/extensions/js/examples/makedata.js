
var f = new DataSource("vector.txt");
var i = new DataVector(f, "INDEX");
var d = new DataVector(f, "2");
i.tagName = "INDEX";
d.tagName = "2";
var e = new Equation("sin(x)", "INDEX");
e.tagName = "myEquation";
var c = new Curve("INDEX", e.yVector);
c.tagName = "myCurve";
var w = new Window;
var p = new Plot(w);
p.curves.append(c);
p.topLabel = "JavaScript Demo"
p.xLabel = "Index"
p.yLabel = "sin(x)"

