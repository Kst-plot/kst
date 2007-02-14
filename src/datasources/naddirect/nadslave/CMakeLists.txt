
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kio_nad_PART_SRCS nadslave.cpp )

kde4_automoc(${kio_nad_PART_SRCS})

kde4_add_plugin(kio_nad ${kio_nad_PART_SRCS})

target_link_libraries(kio_nad  ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} )

install(TARGETS kio_nad DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  nad.protocol DESTINATION ${SERVICES_INSTALL_DIR} )
install( FILES  x-piolib.desktop x-dirfile.desktop DESTINATION ${MIME_INSTALL_DIR}/application )

kde4_install_icons( ${ICON_INSTALL_DIR} )



#original Makefile.am contents follow:

#INCLUDES = $(all_includes) -I$(src_dir)/..
#kde_module_LTLIBRARIES = kio_nad.la
#
#kio_nad_la_SOURCES = nadslave.cpp
#kio_nad_la_LIBADD = $(LIB_KIO)
#kio_nad_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN) 
#noinst_HEADERS = nadslave.h
#
#METASOURCES = AUTO
#
#kdelnkdir = $(kde_servicesdir)
#kdelnk_DATA = nad.protocol
#
#KDE_ICON = piolib dirfile
#
#mimedir = $(kde_mimedir)/application
#mime_DATA = x-piolib.desktop x-dirfile.desktop
#
