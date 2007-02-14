
/*
 This script creates a watcher on a scalar to determine when its value matches
 user-specified criteria.
*/
function ScalarTrigger() {
	// This code actually creates the Watcher object for a scalar.  The
	// Scalar's trigger() signal is connected to the watch_void method of
	// the object that is created.
	this.Watcher = function(name, comparator, value) {
		this.scalar = data.scalar(name);
		this.watch_void = function() {
			if (this.scalar && eval(Math.abs(this.scalar.value()).toString() + this.comp + this.value)) {
				if (!this.triggered) {
					// Display a messagebox
					alert("Triggered!  Value is: " + this.scalar.value());
					this.triggered = true;
				}
			} else {
				this.triggered = false;
			}
		};

		this.triggered = false;
		this.comp = comparator;
		this.value = value;

		if (this.scalar) {
			this.scalar.connect(this.scalar, 'trigger()', this, 'watch_void');
		}
	}


	this.watches = Array();

	// Slot that creates a new Watcher on a scalar
        this.newWatcher_void = function() {
		var dlg = this.dialog;
		var s = dlg.getElementById('scalars');
		if (s.currentItem == -1) {
			alert("You must select a scalar to apply the trigger to.");
			return;
		}

		var w = new this.Watcher(s.currentText, dlg.getElementById('comparator').currentText, dlg.getElementById('value').text);
		this.watches[this.watches.length] = w;
		s.clearSelection();
	};

	// Load the Designer-generated .ui file
	this.dialog = Factory.loadui("scalartriggerdialog.ui");
	// Connect any signals we need
	var button = this.dialog.getElementById('ok');
	button.connect(button, 'clicked()', this, 'newWatcher_void');

	// Create a function to show the dialog
	this.show = function() {
		var list = this.dialog.getElementById('scalars');
		list.clear();
		for (var i = 0; i < data.scalars().length; ++i) {
			list.insertItem(data.scalars()[i]);
		}
		this.dialog.show();
		this.dialog.raise();
	}

	// And a function to hide the dialog
	this.hide = function() {
		this.dialog.hide();
	}

	// Load the menu definition (GUI)
	var gui = KstUIMerge.loadGUI('scalartrigger.rc');
	// Add our action to the action collection of the GUI
	this.action = new KAction(gui.actionCollection(), 'kst_scalar_trigger');
	this.action.text = "Scalar Triggers";
	this.action.connect(this.action, 'activated()', this, 'show');
	// Merge our GUI into Kst
	gui.merge();
}

// Register the script with kst
KstScriptRegistry.addScript("Scalar Trigger", "ScalarTrigger");

