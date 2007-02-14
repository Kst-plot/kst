
include_directories( ${CMAKE_SOURCE_DIR}/kst/kst/datasources/healpix ${CMAKE_SOURCE_DIR}/kst/kst ${CMAKE_SOURCE_DIR}/kst/tests ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(testhealpix_SRCS testhealpix.cpp )

kde4_automoc(${testhealpix_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testhealpix ${testhealpix_SRCS})

target_link_libraries(testhealpix  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### install files ###############




#original Makefile.am contents follow:

#
#check_PROGRAMS=testhealpix
#
#METASOURCES=AUTO
#
#INCLUDES=-I$(top_srcdir)/kst/kst/datasources/healpix -I$(top_srcdir)/kst/kst -I$(top_srcdir)/kst/tests $(all_includes)
#
#AM_LDFLAGS = $(QT_LDFLAGS) $(KDE_LDFLAGS) $(X_LDFLAGS) $(KDE_RPATH)
#LDADD = $(LIB_KDECORE) ../../kst/libkstmath.la ../../kst/libkst.la ../../kst/libkstnonguiapp.la ../../kst/datasources/healpix/kstdata_healpix.la
#testhealpix_SOURCES=testhealpix.cpp
#
