
if include_linux
PROCPS_COPY=sysinfo.c psversion.c
endif

lib_LTLIBRARIES = libkst.la

libkst_la_SOURCES = \
	ksdebug.cpp \
	kstobject.cpp \
	kstdatasource.cpp \
	stdinsource.cpp \
	kstextension.cpp \
	kstdatacollection.cpp \
	kstdebug.cpp \
	rwlock.cpp \
	kstmath.cpp \
	kstdateparser.cpp \
	ksttimezones.cpp \
	kstscalar.cpp $(PROCPS_COPY) \
	kststring.cpp \
	kstmatrix.cpp \
	kstrmatrix.cpp \
	kstsmatrix.cpp \
	kstamatrix.cpp \
	kstvector.cpp \
	kstsvector.cpp \
	kstavector.cpp \
	kstrvector.cpp \
	kstvectordefaults.cpp \
	kstmatrixdefaults.cpp \
	kstprimitive.cpp \
	defaultprimitivenames.cpp \
	kstwaitcondition_unix.cpp

libkst_la_LDFLAGS = -version-info @KST_LIBKST_VERSION@ -no-undefined $(all_libraries)
libkst_la_LIBADD = $(LIB_QT) $(LIB_KIO) $(LIB_KDECORE) ../extdate/libkstextdate.la

include_HEADERS = kstdatasource.h kstobject.h kst_export.h kstsharedptr.h rwlock.h kstdateparser.h kstwaitcondition.h kstobjectcollection.h

INCLUDES=-I$(top_srcdir)/kst/src/extdate -I$(top_srcdir) $(all_includes)

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/kst.pot; \
	fi

METASOURCES=AUTO

kstdebug.lo: kstrevision.h

kstrevision.h:
	@if (which svn >/dev/null 2>&1); then \
	if (svn info $(srcdir) >/dev/null 2>&1); then \
	REV=`svn info $(srcdir) | grep ^Revision | cut -b 11-`; \
	else REV="dist"; fi; else REV="dist"; fi; \
	echo "#ifndef KSTREVISION" >kstrevision.h.tmp; \
	echo "#define KSTREVISION \""$REV"\"" >>kstrevision.h.tmp; \
	echo "#endif" >>kstrevision.h.tmp; \
	if (diff -N kstrevision.h.tmp kstrevision.h >/dev/null 2>&1); \
	then rm kstrevision.h.tmp; else mv kstrevision.h.tmp kstrevision.h; fi

noinst_HEADERS = kstdataplugin.h
