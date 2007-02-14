
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(toigen_SRCS toigen.c )

kde4_automoc(${toigen_SRCS})

kde4_add_executable(toigen ${toigen_SRCS})

target_link_libraries(toigen  ${KDE4_KDECORE_LIBS} m dl pthread )

install(TARGETS toigen DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############




#original Makefile.am contents follow:

#bin_PROGRAMS = toigen
#
#INCLUDES=$(all_includes) $(PIOINCLUDES)
#toigen_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#toigen_LDADD = $(PIOLIBS) -lm -ldl -lpthread
#toigen_SOURCES = toigen.c
#
