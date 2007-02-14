
add_subdirectory( linear_unweighted )
add_subdirectory( linear_weighted )
add_subdirectory( gradient_unweighted )
add_subdirectory( gradient_weighted )
add_subdirectory( polynomial_weighted )
add_subdirectory( polynomial_unweighted )
add_subdirectory( sinusoid_weighted )
add_subdirectory( sinusoid_unweighted )

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############




#original Makefile.am contents follow:

#SUBDIRS=linear_unweighted linear_weighted gradient_unweighted gradient_weighted polynomial_weighted polynomial_unweighted sinusoid_weighted sinusoid_unweighted
#
