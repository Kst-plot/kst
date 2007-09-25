/*
Test bind_kst
*/

// Can't resetInterpreter because it nukes the script, of course...
//try { Kst.resetInterpreter(); } catch(e) { alert("Error: " + e.name); }

var errors = 0;
function assertNoReturn(x) {
	try {
		eval(x);
	} catch (e) {
		alert("Error: " + e.name + "\nLast test was: " + x);
		++errors;
	}
}


function assert(x) {
	try {
		var xrc = eval(x);
		if (!xrc) {
			alert("Failed: " + x);
			++errors;
		}
	} catch (e) {
		alert("Error: " + e.name + "\nLast test was: " + x);
		++errors;
	}
}

assertNoReturn("Kst.purge()");

assert("Kst.vectors.length == 0");
assert("Kst.scalars.length >= 0"); // the built-ins
assert("Kst.strings.length == 0");
assert("Kst.windows.length <= 1");
assert("Kst.dataSources.length == 0");

var v = new Vector;
v.tagName = "My Vector";
assert("Kst.vectors.length == 1");
assert("var j = Kst.vectors[0]; j.tagName = 'My Vector'");
assert("Kst.vectors[0].tagName = 'My Vector'");

var sc = Kst.scalars.length;
var s = new Scalar;
s.tagName = "My Scalar";
assert("Kst.scalars.length == " + (sc + 1));
assert("var j = Kst.scalars[0]; j.tagName = 'My Scalar'");
assert("Kst.scalars[" + sc + "].tagName = 'My Scalar'");



if (errors > 0) {
	alert("" + errors + " failed testcases.");
} else {
	alert("All tests passed.");
}
