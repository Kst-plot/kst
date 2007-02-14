
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstplugineditor_SRCS main.cpp )

kde4_automoc(${kstplugineditor_SRCS})

kde4_add_ui_files(kstplugineditor_SRCS plugineditor.ui )

kde4_add_executable(kstplugineditor ${kstplugineditor_SRCS})

target_link_libraries(kstplugineditor  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstplugineditor DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############




#original Makefile.am contents follow:

#INCLUDES= $(all_includes)
#
#bin_PROGRAMS = kstplugineditor
#kstplugineditor_SOURCES = plugineditor.ui main.cpp
#kstplugineditor_LDFLAGS = $(all_libraries) $(KDE_RPATH)
#kstplugineditor_LDADD = $(LIB_KDECORE)
#METASOURCES = AUTO
#
#messages: rc.cpp
#	$(XGETTEXT) *.cpp -o $(podir)/kstplugineditor.pot
