

w = new Window;

la = new Label(w); // a simple label
la.text = "My test label";
la.position = new Point(383, 34);

lb = new Label(w);  // Show some kst-latex
lb.text = "a label with math: \\Sigma^{x+5}_5";
lb.position = new Point(100, 34);

lc = new Label(w); // show scalar substitution works
lc.text = "A label with a scalar: [CONST_PI]";
lc.position = new Point(100, 154);
lc.font = "verdana"; // show font changing works
lc.fontSize = 26;  // show font size changing
lc.color = "red"; // show color changing
lc.backgroundColor = "#08324b"; // show background color changing

ld = new Label(w);
ld.text = "[CONST_PI]";  // show scalar substitution works with no text
ld.position = new Point(100, 204);
ld.fontSize = 10;
ld.justification = 2 | (2 << 8); // show justification right+bottom

le = new Label(w);
le.text = "[=2*sqrt(2)/sin(sqrt(2))^2]";  // show equations work
le.position = new Point(100, 234);

lf = new Label(w);
lf.text = "[=[CONST_PI]]"; // show nesting works
lf.position = new Point(100, 274);
lf.transparent = false;

lg = new Label(w);  // Complex height calculations
lg.text = "\\Sigma^{(x+5)^2}_{5+1} + \\Pi^{\\Sigma^{i-j}_{i+j}}_{x = 0} + 5";
lg.position = new Point(100, 314);
lg.justification = 3 << 8;
lg.fontSize = 38;
lg.transparent = false;

lh = new Label(w);
lh.text = "\\Sigma^{(x+5)^2}_{5+1} + \\Pi^{\\Sigma^{i-j}_{i+j}}_{x = 0} + 5";
lh.position = new Point(400, 394);
lh.fontSize = 32;
lh.justification = 3 | (3 << 8);
lh.rotation = 45;
lh.adjustSizeForText();

li = new Label(w);
li.position = new Point(50, 394);
li.text = "This\\tcontains\\ttabs\\t.";
li.transparent = false;

lj = new Label(w);
lj.position = new Point(50, 494);
lj.text = "This\\ncontains\\ncrs\\n.";
lj.transparent = false;

