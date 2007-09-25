/*
Test bind_debug
*/

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


assertNoReturn("Debug.clear()");
assertNoReturn("Debug.notice('Kst Test')");
assertNoReturn("Debug.warning('Kst Test')");
assertNoReturn("Debug.error('Kst Test')");
assertNoReturn("Debug.debug('Kst Test')");
assert("Debug.log[0].text == 'Kst Test'");
assert("Debug.log[1].text == 'Kst Test'");
assert("Debug.log[2].text == 'Kst Test'");
assert("Debug.log[3].text == 'Kst Test'");
assert("Debug.log[0].level == 'N'");
assert("Debug.log[1].level == 'W'");
assert("Debug.log[2].level == 'E'");
assert("Debug.log[3].level == 'D'");
assert("Debug.log.text.length > 60");

if (errors > 0) {
	alert("" + errors + " failed testcases.");
} else {
	alert("All tests passed.");
}
