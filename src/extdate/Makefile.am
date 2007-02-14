INCLUDES= -I$(top_srcdir)/kst/kst $(all_includes)

lib_LTLIBRARIES = libkstextdate.la

libkstextdate_la_SOURCES = kstextdatetime.cpp kstextcalendarsystem.cpp kstextcalendarsystemgregorian.cpp kstextdatetbl.cpp kstextdatepicker.cpp kstextdatetimeedit.cpp kstextdatewidget.cpp

include_HEADERS = kstextdatetime.h kstextcalendarsystem.h kstextcalendarsystemgregorian.h kstextdatetbl.h kstextdatepicker.h kstextdatetimeedit.h kstextdatewidget.h

libkstextdate_la_LDFLAGS = $(all_libraries) -no-undefined -version-info 3:0:2
libkstextdate_la_LIBADD = $(LIB_KDEUI)

METASOURCES = AUTO

