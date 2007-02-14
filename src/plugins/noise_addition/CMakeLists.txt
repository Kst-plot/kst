
include_directories( ${CMAKE_SOURCE_DIR}/kst ${CMAKE_SOURCE_DIR}/kst/src/libkst ${CMAKE_SOURCE_DIR}/kst/src/libkstmath ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kstobject_noise_addition_PART_SRCS noise_addition.cpp )

kde4_automoc(${kstobject_noise_addition_PART_SRCS})

kde4_add_plugin(kstobject_noise_addition ${kstobject_noise_addition_PART_SRCS})

target_link_libraries(kstobject_noise_addition  ${KDE4_KDECORE_LIBS} )

install(TARGETS kstobject_noise_addition DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES  kstobject_noise_addition.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kst )



#original Makefile.am contents follow:

#INCLUDES=-I$(top_srcdir)/kst -I$(top_srcdir)/kst/src/libkst -I$(top_srcdir)/kst/src/libkstmath $(all_includes)
#
#kde_module_LTLIBRARIES=kstobject_noise_addition.la
#
#kstobject_noise_addition_la_LDFLAGS=$(all_libraries) -module -avoid-version
#kstobject_noise_addition_la_SOURCES=noise_addition.cpp
#kstobject_noise_addition_la_LIBADD = $(GSL_LIBS)
#
#services_DATA=kstobject_noise_addition.desktop
#servicesdir=$(kde_servicesdir)/kst
#
#METASOURCES=AUTO
