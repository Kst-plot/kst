
if (w) {
	delete w;
}

function Watcher(name) {
	this.scalar = data.scalar(name);
	this.watch_void = function() {
		if (this.scalar && Math.abs(this.scalar.value()) > 0.1) {
			if (!this.triggered) {
				alert("Triggered!  Value is: " + this.scalar.value());
				this.triggered = true;
			}
		} else {
			this.triggered = false;
		}
	};

	this.triggered = false;

	if (this.scalar) {
		this.scalar.connect(this.scalar, 'trigger()', this, 'watch_void');
	}
}


var w = new Watcher("V2-2-Mean");


