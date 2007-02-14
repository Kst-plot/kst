
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstXBench_SRCS tw.cpp main.cpp )

kde4_automoc(${kstXBench_SRCS})

if(KDE4_BUILD_TESTS)

kde4_add_executable(kstXBench ${kstXBench_SRCS})

target_link_libraries(kstXBench  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### install files ###############




#original Makefile.am contents follow:

#
#check_PROGRAMS=kstXBench
#
#METASOURCES=AUTO
#
#INCLUDES=$(all_includes)
#
#AM_LDFLAGS = $(QT_LDFLAGS) $(KDE_LDFLAGS) $(X_LDFLAGS) $(KDE_RPATH)
#LDADD = $(LIB_QT)
#kstXBench_SOURCES=tw.cpp main.cpp
#
