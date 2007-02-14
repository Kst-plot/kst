
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/extdate ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(d2asc_SRCS d2asc.cpp )

kde4_automoc(${d2asc_SRCS})

kde4_add_executable(d2asc ${d2asc_SRCS})

target_link_libraries(d2asc  ${KDE4_KDECORE_LIBS} kstmath )

install(TARGETS d2asc DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############




#original Makefile.am contents follow:

#bin_PROGRAMS = d2asc
#
## set the include path for X, qt and KDE
#INCLUDES=-I$(top_srcdir)/kst/src/extdate -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath -I../libkst -I$(top_srcdir) $(KMDI_INCLUDES) $(all_includes)
#
#d2asc_LDFLAGS = $(all_libraries) $(KDE_RPATH) -no-undefined
#d2asc_LDADD = ../libkstmath/libkstmath.la
#d2asc_SOURCES = d2asc.cpp
#
#
