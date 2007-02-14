INCLUDES = -I$(top_srcdir)/kst/kst/libkstkmdi/res -I$(top_srcdir) -I$(top_srcdir)/kst/kst/libkstkmdi $(all_includes)

noinst_LTLIBRARIES = libkstkmdi.la
libkstkmdi_la_LDFLAGS = $(all_libraries) -version-info 1:0
libkstkmdi_la_LIBADD  = $(LIB_KUTILS)

libkstkmdi_la_SOURCES = kmdichildarea.cpp kmdichildfrm.cpp \
	kmdichildfrmcaption.cpp kmdichildview.cpp kmdimainfrm.cpp \
	kmditaskbar.cpp kmdidockcontainer.cpp kmditoolviewaccessor.cpp \
	kmdiguiclient.cpp kmdidocumentviewtabwidget.cpp \
	kmdifocuslist.cpp kdockwidget.cpp kdockwidget_private.cpp \
	ktabbar.cpp ktabwidget.cpp kmultitabbar.cpp
			

METASOURCES = AUTO

