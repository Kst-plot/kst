INCLUDES= -I$(srcdir)/../.. -I$(srcdir)/.. -I$(top_srcdir)/kjsembed -I$(srcdir)/../../../../libkst $(all_includes)
KDE_CXXFLAGS = -DQT_NO_ASCII_CAST -DQT_CLEAN_NAMESPACE -Wno-unused

noinst_LTLIBRARIES = libkjsembedbuiltins.la

libkjsembedbuiltins_la_SOURCES =	stdaction_imp.cpp \
					resources.cpp \
					stddialog_imp.cpp \
					saxhandler.cpp \
					textstream_imp.cpp \
					qdir_imp.cpp \
					qfile_imp.cpp

libkjsembedbuiltins_la_LDFLAGS = $(all_libraries) $(KDE_RPATH)
libkjsembedbuiltins_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KPARTS) \
				$(LIB_KFILE) -lkjs $(LIBSOCKET)

libkjsembedbuiltins_la_METASOURCES = AUTO
