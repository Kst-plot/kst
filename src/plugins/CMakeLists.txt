
add_subdirectory( bin )
add_subdirectory( linefit )
add_subdirectory( periodogram )
add_subdirectory( phase )
add_subdirectory( statistics )
add_subdirectory( chop )
add_subdirectory( crossspectrum )
add_subdirectory( syncbin )
add_subdirectory( effective_bandwidth )
add_subdirectory( despike )
add_subdirectory( cumulative_sum )
add_subdirectory( differentiation )
add_subdirectory( shift )
add_subdirectory( discretizing_filters )

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(NOISE_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(FITS_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(FITSNONLINEAR_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(INTERPOLATIONS_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(PASS_FILTERS_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(CORRELATION_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(CONVOLUTION_SUBDIR)")
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############




#original Makefile.am contents follow:

#if include_noise_plugins
#NOISE_SUBDIR=noise_addition
#endif
#if include_fits_plugins
#FITS_SUBDIR=fits
#endif
#if include_fitsnonlinear_plugins
#FITSNONLINEAR_SUBDIR=fits_nonlinear
#endif
#if include_interpolations_plugins
#INTERPOLATIONS_SUBDIR=interpolations
#endif
#if include_pass_filters_plugins
#PASS_FILTERS_SUBDIR=pass_filters
#endif
#if include_convolution_plugins
#CONVOLUTION_SUBDIR=convolution
#endif
#if include_correlation_plugins
#CORRELATION_SUBDIR=correlation
#endif
#
#SUBDIRS=bin linefit periodogram phase statistics chop crossspectrum syncbin effective_bandwidth despike $(NOISE_SUBDIR) $(FITS_SUBDIR) $(FITSNONLINEAR_SUBDIR) $(INTERPOLATIONS_SUBDIR) $(PASS_FILTERS_SUBDIR) $(CORRELATION_SUBDIR) $(CONVOLUTION_SUBDIR) cumulative_sum differentiation shift discretizing_filters
#
