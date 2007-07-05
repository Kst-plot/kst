// callsubs.js
// demonstrate calling subroutines

loadScript('subroutines.js');

file = new DataSource("demodata.dat");

var p = plotField("1", 0, -1, file);
addField(p, "2", 0, -1, file);
addField(p, "3", 0, -1, file);

