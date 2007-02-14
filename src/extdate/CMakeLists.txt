
FIND_PACKAGE(KDE4 REQUIRED)
include_directories( ${CMAKE_SOURCE_DIR}/kst/kst ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstextdate_LIB_SRCS 
   kstextdatetime.cpp 
   kstextcalendarsystem.cpp 
   kstextcalendarsystemgregorian.cpp 
   kstextdatetbl.cpp 
   kstextdatepicker.cpp 
   kstextdatetimeedit.cpp 
   kstextdatewidget.cpp )

kde4_automoc(${kstextdate_LIB_SRCS})

kde4_add_library(kstextdate SHARED ${kstextdate_LIB_SRCS})

target_link_libraries(kstextdate  ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} )

set_target_properties(kstextdate PROPERTIES VERSION 1.2.0 SOVERSION 1 )
install(TARGETS kstextdate DESTINATION ${LIB_INSTALL_DIR} )


########### install files ###############

install( FILES  kstextdatetime.h kstextcalendarsystem.h kstextcalendarsystemgregorian.h kstextdatetbl.h kstextdatepicker.h kstextdatetimeedit.h kstextdatewidget.h DESTINATION ${INCLUDE_INSTALL_DIR} )



#original Makefile.am contents follow:

#INCLUDES= -I$(top_srcdir)/kst/kst $(all_includes)
#
#lib_LTLIBRARIES = libkstextdate.la
#
#libkstextdate_la_SOURCES = kstextdatetime.cpp kstextcalendarsystem.cpp kstextcalendarsystemgregorian.cpp kstextdatetbl.cpp kstextdatepicker.cpp kstextdatetimeedit.cpp kstextdatewidget.cpp
#
#include_HEADERS = kstextdatetime.h kstextcalendarsystem.h kstextcalendarsystemgregorian.h kstextdatetbl.h kstextdatepicker.h kstextdatetimeedit.h kstextdatewidget.h
#
#libkstextdate_la_LDFLAGS = $(all_libraries) -no-undefined -version-info 3:0:2
#libkstextdate_la_LIBADD = $(LIB_KDEUI)
#
#METASOURCES = AUTO
#
