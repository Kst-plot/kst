
add_subdirectory( pics )

include_directories( ${CMAKE_BINARY_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/extdate ${CMAKE_SOURCE_DIR}/kst/src/widgets ${CMAKE_BINARY_DIR}/kst/src/widgets ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_BINARY_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstapp ${CMAKE_BINARY_DIR}/kst/src/libkstapp ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kst_SRCS main.cpp )

kde4_automoc(${kst_SRCS})

kde4_add_executable(kstexe ${kst_SRCS})

target_link_libraries(kstexe kstapp )

set_target_properties(kstexe PROPERTIES OUTPUT_NAME kst)

install(TARGETS kstexe DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install( FILES  kst.desktop DESTINATION ${XDG_APPS_DIR} )
install( FILES  kstui.rc DESTINATION ${DATA_INSTALL_DIR}/kst )
install( FILES  kstplugin.desktop kstdatasourceplugin.desktop kstdataobjectplugin.desktop kstbasicplugin.desktop kstfilter.desktop kstextension.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR}/kst )
install( FILES  kstautosave11.upd kstrcmisc11.upd DESTINATION ${DATA_INSTALL_DIR}/kconf_update )
install( FILES  x-kst.desktop DESTINATION ${MIME_INSTALL_DIR}/application )

kde4_install_icons( ${ICON_INSTALL_DIR} )



#original Makefile.am contents follow:

#
##AM_CXXFLAGS = -Wold-style-cast -Wredundant-decls -Wunused-macros -Wfloat-equal -Wno-multichar
#
#SUBDIRS=. pics
#
#bin_PROGRAMS = kst
#
#kst_SOURCES = main.cpp
#kst_LDADD = ../libkstapp/libkstapp.la
#kst_LDFLAGS = -no-undefined $(KDE_RPATH) $(all_libraries)
#
#KDE_ICON = kst
#
#services_DATA = kstplugin.desktop kstdatasourceplugin.desktop kstdataobjectplugin.desktop kstbasicplugin.desktop kstfilter.desktop kstextension.desktop
#servicesdir = $(kde_servicetypesdir)/kst
#
#apps_DATA = kst.desktop
#appsdir = $(kde_appsdir)/Applications/Sciences
#
#mimedir = $(kde_mimedir)/application
#mime_DATA = x-kst.desktop
#
## set the include path for X, qt and KDE
#INCLUDES=-I$(top_srcdir)/kst/src/extdate -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath -I../libkst -I$(top_srcdir)/kst/src/widgets -I../widgets -I$(top_srcdir)/kst/src/libkstapp -I../libkstapp -I$(top_srcdir) $(KMDI_INCLUDES) $(all_includes)
#
#rcdir = $(kde_datadir)/kst
#rc_DATA = kstui.rc
#
#messages: rc.cpp
#	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
#	if test -n "$$LIST"; then \
#	 $(XGETTEXT) $$LIST -o $(podir)/kst.pot; \
#	fi
#
#METASOURCES=AUTO
#
#update_DATA = kstautosave11.upd kstrcmisc11.upd
#updatedir = $(kde_datadir)/kconf_update
#
