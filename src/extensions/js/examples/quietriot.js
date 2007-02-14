
function QuietRiot() {
	this.doIt = function() {
		for (w = 0; w < Kst.windows.length; ++w) {
			win = Kst.windows[w];
			for (p = 0; p < win.plots.length; ++p) {
				plot = win.plots[p];
				plot.labels.top.rotation = (plot.labels.top.rotation + 10) % 360;
			}
		}
	}

	this.timer = new QTimer(this);
	this.timer.connect(this.timer, 'timeout()', this, 'doIt');

	this.timer.start(200);
}

KstScriptRegistry.addScript("Quiet Riot", "QuietRiot");

