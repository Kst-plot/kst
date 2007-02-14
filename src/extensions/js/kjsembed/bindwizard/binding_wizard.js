#!/usr/bin/env kjscmd

//
// Script for automatically generating bindings to C++ classes.
//

plugin_name = '';
header_dir = '';
output_dir = '';

wiz = Factory.loadui( 'binding_wizard.ui' );

classview = wiz.child('classes');
classes = new Array();
page = 0;
output = new Array();

imp = new BindingWizard();

function BindingWizard()
{
   //
   // Update the GUI view from the classview list.
   //
   this.update_classes = function() {
      classview.clear();
      for ( var i=0 ; i < classes.length ; i++ ) {
         classview.insertItem( 'Yes', classes[i] );
      }
   }

   //
   // Selects all the classes.
   //
   this.select_all = function() {
      classview.selectAll( true );
   }

   //
   // Populate list views
   //
   this.choose_classes = function( view, name ) {

      sax = new Object();
      sax.chars = '';

      sax.endElement = function( namespace, localname, qualifiedname ) {
	  if ( localname == 'class' ) {
	      classes[ classes.length ] = this.chars;
	  }
	  this.chars = '';
	  return true;
      }

      sax.characters = function( chars ) {
         this.chars = chars;
         return true;
      }

      try {
         saxLoadFile( sax, name );
      }
      catch( theErr )
      {
         println(theErr);
      }
   }

   //
   // Creates the binding for the named class.
   //
   this.bind_class = function( clazz ) {

       var out = "<b>Binding Class '" + clazz + "'...</b><pre>\n";

       var outfile = output_dir + '/' + clazz.toLowerCase() + '_imp.h';
       var cmd = 'xsltproc -o ' + outfile + ' doxygen2imp_h.xsl xml/class' + clazz + '.xml';

       out += cmd + '\n';
       out += shell( cmd );
       output[ output.length ] = outfile;

       outfile = output_dir + '/' + clazz.toLowerCase() + '_imp.cpp';
       var cmd2 = 'xsltproc -o ' + outfile + ' doxygen2imp_cpp.xsl xml/class' + clazz + '.xml';

       out += cmd2 + '\n';
       out += shell( cmd2 );

       out += '</pre>';

       return out;
   }

   this.create_makefile = function() {
      
      var sources = output.join(' ');
      sources = sources.replace( /\w*\//g, '' );

      var make = System.readFile( 'plugin_Makefile.in' );
      make = make.replace( /__PLUGIN_NAME__/g, plugin_name );
      make = make.replace( /__PLUGIN_NAME_LC__/g, plugin_name.toLowerCase() );
      make = make.replace( /__PLUGIN_SOURCES__/g, sources );

      System.writeFile( output_dir + '/' + 'Makefile.am', make );
   }

   //
   // Called when the wizard has all the information it needs. This method
   // binds the classes the user specified.
   //
   this.invoke = function() {

      wiz.child('next').enabled = false;
      wiz.child('output').clear();

      for ( var i=0 ; i < classes.length ; i++ ) {
         wiz.child('output').append( this.bind_class( classes[i] ) );
      }

      wiz.child('next').enabled = true;
   }

   this.browse_input = function() {
       res = StdDialog.getExistingDirectory( '.' );
       wiz.child('header_dir').text = res;
   }

   this.browse_output = function() {
       res = StdDialog.getExistingDirectory( '.' );
       wiz.child('output_dir').text = res;
   }

   //
   // Lists the classes available for binding. In future, this will let you
   // control which classes are processed.
   //
   this.list_classes = function() {

      wiz.child('next').enabled = false;

      // Create class index
      var clazz = shell( 'xsltproc index_classes.xsl xml/index.xml' );
      System.writeFile( 'classes.xml', clazz );

      // Offer class selection
      var classview = wiz.child('classes');
      classview.selectionMode = 2 // Extended mode;
      classview.resizeMode = 2;

      this.choose_classes( classview, 'classes.xml' );
      this.update_classes();

      wiz.connect( wiz.child('select_all'), 'clicked()', this, 'select_all' );
      wiz.child('next').enabled = true;
   }

   //
   // Create the doxygen config
   //
   this.analyse = function() {

      wiz.child('next').enabled = false;

      led = wiz.child('analyse_led');
      led.color = 'orange';

      plugin_name = wiz.child('plugin_name').text;
      header_dir  = wiz.child('header_dir').text;
      output_dir  = wiz.child('output_dir').text;

      // Create Doxygen config
      var doxy = System.readFile( 'Doxyfile.in' );

      doxy = doxy.replace( /__PLUGIN_NAME__/, plugin_name );
      doxy = doxy.replace( /__HEADER_DIR__/, header_dir );

      System.writeFile( 'Doxyfile', doxy );

      // Run doxygen
      shell( 'doxygen' );

      wiz.child('analyse_led').color = 'green';
      wiz.child('analyse_status').text = 'Done';

      wiz.child('next').enabled = true;
   }

   this.view_results = function() {

      this.create_makefile();

      var lv = wiz.child('bindings');

      for ( var i=0 ; i < output.length ; i++ ) {
         lv.insertItem( output[i] );
      }

      wiz.child('finish').enabled = true;
   }

   //
   // Called to setup the display of the next page.
   //
   this.do_next = function() {

       try {
	   page++;

	   if ( page == 1 ) {
	       this.analyse();
	   }
	   else if ( page == 2 ) {
	       this.list_classes();
	   }
	   else if ( page == 3 ) {
	       this.invoke();
	   }
	   else if ( page == 4 ) {
	       this.view_results();
	   }
       }
       catch(err) {
	   warn( 'Error: ' + err );
       }
   }

   this.do_back = function() {

       page--;

   }

   wiz.connect( wiz.child('next'), 'clicked()', this, 'do_next' );
   wiz.connect( wiz.child('back'), 'clicked()', this, 'do_back' );

   //
   // Connnect the first page
   //
   wiz.connect( wiz.child('browse_header'), 'clicked()', this, 'browse_input' );
   wiz.connect( wiz.child('browse_output' ), 'clicked()', this, 'browse_output' );
}

wiz.show();
application.exec();
