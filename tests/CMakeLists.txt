
add_subdirectory( kstXbench )

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(HEALPIX_DIR)")
include_directories( ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${CMAKE_SOURCE_DIR}/kst/src/extdate ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(testeqparser_SRCS testeqparser.cpp )

kde4_automoc(${testeqparser_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testeqparser ${testeqparser_SRCS})

target_link_libraries(testeqparser  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(testhistogram_SRCS testhistogram.cpp )

kde4_automoc(${testhistogram_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testhistogram ${testhistogram_SRCS})

target_link_libraries(testhistogram  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(testscalars_SRCS testscalars.cpp )

kde4_automoc(${testscalars_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testscalars ${testscalars_SRCS})

target_link_libraries(testscalars  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(testlabelparser_SRCS testlabelparser.cpp )

kde4_automoc(${testlabelparser_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testlabelparser ${testlabelparser_SRCS})

target_link_libraries(testlabelparser  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(testvector_SRCS testvector.cpp )

kde4_automoc(${testvector_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testvector ${testvector_SRCS})

target_link_libraries(testvector  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(testrvector_SRCS testrvector.cpp )

kde4_automoc(${testrvector_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testrvector ${testrvector_SRCS})

target_link_libraries(testrvector  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(testmatrix_SRCS testmatrix.cpp )

kde4_automoc(${testmatrix_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testmatrix ${testmatrix_SRCS})

target_link_libraries(testmatrix  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(testamatrix_SRCS testamatrix.cpp )

kde4_automoc(${testamatrix_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testamatrix ${testamatrix_SRCS})

target_link_libraries(testamatrix  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(testcsd_SRCS testcsd.cpp )

kde4_automoc(${testcsd_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testcsd ${testcsd_SRCS})

target_link_libraries(testcsd  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(testpsd_SRCS testpsd.cpp )

kde4_automoc(${testpsd_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testpsd ${testpsd_SRCS})

target_link_libraries(testpsd  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(testsmatrix_SRCS testsmatrix.cpp )

kde4_automoc(${testsmatrix_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(testsmatrix ${testsmatrix_SRCS})

target_link_libraries(testsmatrix  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### install files ###############




#original Makefile.am contents follow:

#
#if include_fitsio_datasource
#HEALPIX_DIR=healpix
#endif
#
#SUBDIRS=. kstXbench $(HEALPIX_DIR)
#
#check_PROGRAMS=testeqparser testhistogram testscalars testlabelparser \
#		testvector testrvector \
#		testmatrix testamatrix testcsd testpsd testsmatrix
#
#METASOURCES=AUTO
#
#INCLUDES=-I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath -I$(top_srcdir)/kst/src/extdate $(all_includes)
#
#AM_LDFLAGS = $(QT_LDFLAGS) $(KDE_LDFLAGS) $(X_LDFLAGS) $(KDE_RPATH)
#LDADD = $(LIB_KDECORE) ../src/libkstmath/libkstmath.la ../src/libkst/libkst.la
#testeqparser_SOURCES=testeqparser.cpp
#testhistogram_SOURCES=testhistogram.cpp
#testscalars_SOURCES=testscalars.cpp
#testlabelparser_SOURCES=testlabelparser.cpp
#testvector_SOURCES=testvector.cpp
#testrvector_SOURCES=testrvector.cpp
#testmatrix_SOURCES=testmatrix.cpp
#testamatrix_SOURCES=testamatrix.cpp
#testcsd_SOURCES=testcsd.cpp
#testpsd_SOURCES=testpsd.cpp
#testsmatrix_SOURCES=testsmatrix.cpp
