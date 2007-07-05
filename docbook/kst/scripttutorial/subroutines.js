// subroutines.js
// plot a field from a datasource in a previously created plot
function addField(plot, field, f0, n, datasource) {
  var x1 = new DataVector(datasource, "INDEX", f0, n);
  x1.tagName = "INDEX"
  var y1 = new DataVector(datasource, field, f0, n);
  y1.tagName=field
  var c1 = new Curve(x1, y1);
  c1.tagName = field
  plot.curves.append(c1)
  plot.topLabel =  plot.topLabel + field + " "
}

// create a plot and add a field curve to it.
function plotField(field, f0, n, datasource) {
  var p1 = new Plot(Kst.windows[0]);
  p1.topLabel = "File: " + datasource.fileName + "   Fields: "
  addField(p1,field, f0, n, datasource);
  p1.xAxis.label = "Index"
  return p1
}

