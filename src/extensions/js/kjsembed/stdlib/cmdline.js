#!/usr/bin/env kjscmd

function list(obj)
{
    println( obj );

    for ( var i in obj ) {
	try {
	    tp = typeof obj[i];

	    count = 20 - i.length;
	    spc = '';
	    for ( var s=0; s < count; s++ )
		spc = spc + ' ';

	    println( '   ' + i + spc + tp );
	}
	catch(err) {
	    println( '   ' + i + spc + 'Error, ' + err );
	}
    }
}

function help()
{
    println();
    println( 'KJSCmd ' + part.versionString + ' Prompt' );
    println( '======='        + '===' +       '=======' );
    println();
    println( "Type 'quit' to exit, and 'help' for this text. Anything else" );
    println( 'is evaluated by the KJSCmd engine and the result displayed.' );
    println();
}

function cmd_prompt()
{
    System.stdout.print( 'kjscmd> ' );
    return System.stdin.readLine();
}

//
// Main
//

println( "KJSCmd " + part.versionString + " (type 'help' for more information)" );
done = false;

do {

    line = cmd_prompt();

    if ( line == 'quit' ) {
	done = true;
    }
    else if ( line == 'help' ) {
	help();
    }
    else if ( line == '' ) {
	// DO NOTHING
    }
    else if ( line == null ) {
	done = true;
    }
    else {
	try {
	    output = eval( line );
	    
	    if ( typeof(output) != 'undefined' )
		println( output );
	}
	catch(err) {
	    println( 'Error: ' + err );
	}
    }

} while ( !done );

System.exit(0);
