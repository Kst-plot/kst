
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KJSEMBED_SUBDIR)")
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkstapp ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${CMAKE_SOURCE_DIR}/kst/src/extdate ${CMAKE_SOURCE_DIR}/kst/src/widgets ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstextension_js_PART_SRCS 
   $(JSEXTERN) 
   js.cpp 
   jsiface_impl.cpp 
   kstbinding.cpp 
   bind_object.cpp 
   bind_objectcollection.cpp 
   bind_vector.cpp 
   bind_kst.cpp 
   bind_scalar.cpp 
   bind_string.cpp 
   bind_datasource.cpp 
   bind_datavector.cpp 
   bind_equation.cpp 
   bind_powerspectrum.cpp 
   bind_curve.cpp 
   bind_point.cpp 
   bind_window.cpp 
   bind_plot.cpp 
   bind_axis.cpp 
   bind_timeinterpretation.cpp 
   bind_collection.cpp 
   bind_plotcollection.cpp 
   bind_curvecollection.cpp 
   bind_windowcollection.cpp 
   bind_vectorcollection.cpp 
   bind_scalarcollection.cpp 
   bind_stringcollection.cpp 
   bind_datasourcecollection.cpp 
   bind_histogram.cpp 
   bind_matrix.cpp 
   bind_datamatrix.cpp 
   bind_debug.cpp 
   bind_debuglog.cpp 
   bind_debuglogentry.cpp 
   bind_histogramcollection.cpp 
   bind_equationcollection.cpp 
   bind_powerspectrumcollection.cpp 
   bind_matrixcollection.cpp 
   bind_dataobject.cpp 
   bind_dataobjectcollection.cpp 
   bind_colorsequence.cpp 
   bind_extension.cpp 
   bind_extensioncollection.cpp 
   bind_document.cpp 
   bind_plugin.cpp 
   bind_plugincollection.cpp 
   bind_pluginmanager.cpp 
   bind_pluginmodule.cpp 
   bind_pluginmodulecollection.cpp 
   bind_pluginio.cpp 
   bind_pluginiocollection.cpp 
   bind_viewobject.cpp 
   bind_borderedviewobject.cpp 
   bind_viewobjectcollection.cpp 
   bind_size.cpp 
   bind_box.cpp 
   bind_label.cpp 
   bind_legend.cpp 
   bind_ellipse.cpp 
   bind_picture.cpp 
   bind_arrow.cpp 
   bind_line.cpp )

kde4_automoc(${kstextension_js_PART_SRCS})

#kde4_add_dcop_skels(kstextension_js_PART_SRCS MESSAGE(STATUS "DCOP stuff was remove into kde4, convert it to dbus please.kde4_add_dcop_skels macro doesn't exist now. We keep it just to remember to port to dbus")
jsiface.h )

kde4_add_plugin(kstextension_js ${kstextension_js_PART_SRCS})

target_link_libraries(kstextension_js  ${KDE4_KDECORE_LIBS} kstapp kjsembed kstapp kjsembed )

install(TARGETS kstextension_js DESTINATION ${PLUGIN_INSTALL_DIR} )


########### next target ###############

set(kstcmd_SRCS kstcmd.cpp )

kde4_automoc(${kstcmd_SRCS})

kde4_add_executable(kstcmd ${kstcmd_SRCS})

target_link_libraries(kstcmd  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstcmd DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install( FILES  kstextension_js.rc DESTINATION ${DATA_INSTALL_DIR}/kst )
install( FILES  kstextension_js.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#if include_kjse_internal
#KJSEMBED_SUBDIR=kjsembed
#endif
#
#SUBDIRS=$(KJSEMBED_SUBDIR) .
#
#if include_kjse_internal
#KJSEINCLUDES=-I$(srcdir)/kjsembed
#else
#JSEXTERN=js_extern.cpp
#endif
#INCLUDES=-I$(top_srcdir)/kst/src/libkstapp -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath -I$(top_srcdir)/kst/src/extdate -I$(top_builddir)/kst/src/libkst -I$(top_builddir)/kst/src/libkstapp -I$(top_srcdir)/kst/src/widgets -I$(top_srcdir) $(KMDI_INCLUDES) $(KJSEINCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES=kstextension_js.la
#if include_kstcmd
#bin_PROGRAMS=kstcmd
#
#kstcmd_SOURCES=kstcmd.cpp
#kstcmd_LDADD=$(LIB_KDECORE) $(LIBREADLINE)
#kstcmd_LDFLAGS=$(all_libraries)
#endif
#
#kstextension_js_la_LDFLAGS=$(all_libraries) -module -avoid-version
#if include_kjse_internal
#kstextension_js_la_LIBADD=../../libkstapp/libkstapp.la kjsembed/libkjsembed.la
#else
#kstextension_js_la_LIBADD=../../libkstapp/libkstapp.la -lkjsembed
#endif
#kstextension_js_la_SOURCES= $(JSEXTERN) \
#			    js.cpp \
#			    jsiface.skel \
#			    jsiface_impl.cpp \
#			    kstbinding.cpp \
#			    bind_object.cpp \
#			    bind_objectcollection.cpp \
#			    bind_vector.cpp \
#			    bind_kst.cpp \
#			    bind_scalar.cpp \
#			    bind_string.cpp \
#			    bind_datasource.cpp \
#			    bind_datavector.cpp \
#			    bind_equation.cpp \
#			    bind_powerspectrum.cpp \
#			    bind_curve.cpp \
#			    bind_point.cpp \
#			    bind_window.cpp \
#			    bind_plot.cpp \
#			    bind_axis.cpp \
#			    bind_timeinterpretation.cpp \
#			    bind_collection.cpp \
#			    bind_plotcollection.cpp \
#			    bind_curvecollection.cpp \
#			    bind_windowcollection.cpp \
#			    bind_vectorcollection.cpp \
#			    bind_scalarcollection.cpp \
#			    bind_stringcollection.cpp \
#			    bind_datasourcecollection.cpp \
#			    bind_histogram.cpp \
#			    bind_matrix.cpp \
#			    bind_datamatrix.cpp \
#			    bind_debug.cpp \
#			    bind_debuglog.cpp \
#			    bind_debuglogentry.cpp \
#			    bind_histogramcollection.cpp \
#			    bind_equationcollection.cpp \
#			    bind_powerspectrumcollection.cpp \
#			    bind_matrixcollection.cpp \
#			    bind_dataobject.cpp \
#			    bind_dataobjectcollection.cpp \
#			    bind_colorsequence.cpp \
#			    bind_extension.cpp \
#			    bind_extensioncollection.cpp \
#			    bind_document.cpp \
#			    bind_plugin.cpp \
#			    bind_plugincollection.cpp \
#			    bind_pluginmanager.cpp \
#			    bind_pluginmodule.cpp \
#			    bind_pluginmodulecollection.cpp \
#			    bind_pluginio.cpp \
#			    bind_pluginiocollection.cpp \
#			    bind_viewobject.cpp \
#			    bind_borderedviewobject.cpp \
#			    bind_viewobjectcollection.cpp \
#			    bind_size.cpp \
#			    bind_box.cpp \
#			    bind_label.cpp \
#			    bind_legend.cpp \
#			    bind_ellipse.cpp \
#			    bind_picture.cpp \
#			    bind_arrow.cpp \
#			    bind_line.cpp
#
##			    js_dataobject.cpp \
##			    bind_cubicbezier.cpp \
##			    bind_jsdataobject.cpp
#
#METASOURCES=AUTO
#
#services_DATA=kstextension_js.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#rcdir=$(kde_datadir)/kst
#rc_DATA=kstextension_js.rc
#
#apidox:
#	xsltproc --stringparam srcdir "$(srcdir)" "$(srcdir)/jsdocs.xsl" "$(srcdir)/classindex.xml" >builddocs.sh
#	chmod +x builddocs.sh
#	./builddocs.sh
#
#
