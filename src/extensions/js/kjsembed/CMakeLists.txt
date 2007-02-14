
add_subdirectory( bindings )
add_subdirectory( builtins )
add_subdirectory( qtbindings )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############

install( FILES  DESTINATION ${INCLUDE_INSTALL_DIR}/kjsembed )



#original Makefile.am contents follow:

## -*- makefile -*-
#AM_CXXFLAGS = -frtti
#
## Make sure the include path includes Qt's uilib
#INCLUDES = -I$(QTDIR)/tools/designer/uilib -I$(srcdir)/.. -I$(srcdir)/../../../libkst $(all_includes)
#
#INCLUDES += -DQT_CLEAN_NAMESPACE -DQT_NO_ASCII_CAST
#
#SUBDIRS = bindings builtins qtbindings .
#
#noinst_LTLIBRARIES = libkjsembed.la
#
#libkjsembed_la_METASOURCES = AUTO
#libkjsembed_la_SOURCES =	jsconsolewidget.cpp \
#				kjsembedpart.cpp \
#				jsobjectproxy.cpp \
#				slotutils.cpp \
#				jsproxy.cpp \
#				slotproxy.cpp \
#				jsbinding.cpp \
#				jssecuritypolicy.cpp \
#				xmlactionclient.cpp \
#				jsfactory.cpp \
#				kjsembedpart_imp.cpp \
#				jsobjectproxy_imp.cpp \
#				customobject_imp.cpp \
#				jsvalueproxy_imp.cpp \
#				jsvalueproxy.cpp \
#				jsobjecteventproxy.cpp \
#				jseventmapper.cpp \
#				jsbuiltin.cpp \
#				jsproxy_imp.cpp \
#				jsfactory_imp.cpp \
#				jsbuiltin_imp.cpp \
#				jsbuiltinproxy.cpp \
#				jsopaqueproxy.cpp \
#				jsopaqueproxy_imp.cpp \
#				jseventutils.cpp \
#				jsbindingplugin.cpp \
#				ksimpleprocess.cpp \
#				global.cpp
#
#kjsembedincludedir = $(includedir)/kjsembed
#noinst_HEADERS =	jsconsolewidget.h \
#				kjsembedpart.h \
#				jsobjectproxy.h \
#				slotutils.h \
#				jsproxy.h \
#				slotproxy.h \
#				jsbinding.h \
#				jssecuritypolicy.h \
#				xmlactionclient.h \
#				jsfactory.h \
#				kjsembedpart_imp.h \
#				jsobjectproxy_imp.h \
#				customobject_imp.h \
#				jsvalueproxy_imp.h \
#				jsvalueproxy.h \
#				jsobjecteventproxy.h \
#				jseventmapper.h \
#				jsbuiltin.h \
#				jsproxy_imp.h \
#				jsfactory_imp.h \
#				jsbuiltin_imp.h \
#				jsopaqueproxy.h \
#				jsopaqueproxy_imp.h \
#				jseventutils.h \
#				jsbindingplugin.h \
#				global.h
#
#libkjsembed_la_LDFLAGS = -no-undefined -version-info 1:0 $(all_libraries) $(KDE_RPATH)
#
#libkjsembed_la_LIBADD = bindings/libkjsembedbindings.la \
#			builtins/libkjsembedbuiltins.la \
#			qtbindings/libkjsembedqtbindings.la \
#			-lqui \
#			$(LIB_KHTML) -lkjs $(LIBSOCKET)
#
