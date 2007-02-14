
add_subdirectory( linear )
add_subdirectory( cspline )
add_subdirectory( cspline_periodic )
add_subdirectory( akima )
add_subdirectory( akima_periodic )
add_subdirectory( polynomial )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############




#original Makefile.am contents follow:

#SUBDIRS=linear cspline cspline_periodic akima akima_periodic polynomial
