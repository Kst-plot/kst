
var line = readLine();
while ( line != null ) {
      line = line.replace( /^ */g, '' );      
      line = line.replace( / *$/g, '' );      

      if ( !Factory.isSupported( line ) )
         print( line + ' MISSING\n' );

      line = readLine();
}
