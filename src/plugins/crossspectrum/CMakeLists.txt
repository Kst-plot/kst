
include_directories( ${CMAKE_SOURCE_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/widgets ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstapp ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstobject_crossspectrum_PART_SRCS crosspowerspectrum.cpp crossspectrumdialog_i.cpp fftsg_h.c )

kde4_automoc(${kstobject_crossspectrum_PART_SRCS})

kde4_add_ui_files(kstobject_crossspectrum_PART_SRCS crossspectrumdialogwidget.ui )

kde4_add_plugin(kstobject_crossspectrum ${kstobject_crossspectrum_PART_SRCS})

target_link_libraries(kstobject_crossspectrum  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstobject_crossspectrum DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstobject_crossspectrum.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/widgets -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstapp -I$(top_srcdir)/kst/src/libkstmath -I$(top_srcdir)/kst/src/extdate -I$(top_builddir)/kst -I$(top_builddir)/kst/src/widgets -I$(top_builddir)/kst/src/libkst -I$(top_builddir)/kst/src/libkstapp -I$(top_builddir)/kst/src/libkstmath -I$(top_builddir)/kst/src/extdate $(all_includes)
#
#kde_module_LTLIBRARIES=kstobject_crossspectrum.la
#
#kstobject_crossspectrum_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstobject_crossspectrum_la_SOURCES=crosspowerspectrum.cpp crossspectrumdialog_i.cpp crossspectrumdialogwidget.ui fftsg_h.c
#
#services_DATA=kstobject_crossspectrum.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#METASOURCES=AUTO
