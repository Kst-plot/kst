

w = new Window;
p = new Picture(w);
b = new Box(w);
e = new Ellipse(w);
l = new Line(w);

b.size = new Size(120, 57);
b.position = new Point(30, 22);
b.color = "red"
b.borderWidth = 3;

p.size = new Size(284, 261);
p.position = new Point(73, 113);
p.load("http://www.kde.org/media/images/kde_gear_64.png");
p.borderWidth = 3;
p.borderColor = "blue";

e.size = new Size(342, 400);
e.position = new Point(441, 66);
e.borderWidth = 2;
e.borderColor = "red";
e.color = "blue";

l.from = new Point(73, 283);
l.to = new Point(401, 459);
l.width = 3;
l.color = "#10ff10";

la = new Label(w);
la.text = "My test label";
la.position = new Point(283, 34);

a = new Arrow(w);
a.from = new Point(283, 346);
a.to = new Point(472, 535);
a.width = 3;

