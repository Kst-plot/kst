
add_subdirectory( gaussian )
add_subdirectory( gaussian_weighted )
add_subdirectory( lorentzian )
add_subdirectory( lorentzian_weighted )
add_subdirectory( exponential )
add_subdirectory( exponential_weighted )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############




#original Makefile.am contents follow:

#SUBDIRS=gaussian gaussian_weighted lorentzian lorentzian_weighted exponential exponential_weighted
#
