
add_subdirectory( ascii )
add_subdirectory( qimagesource )
add_subdirectory( dirfile )
add_subdirectory( frame )
add_subdirectory( indirect )

#add_subdirectory( cdf )
#add_subdirectory( netcdf )
#add_subdirectory( libfitstools )
#add_subdirectory( healpix )
#add_subdirectory( scuba2 )

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(PIOLIB_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(LFIIO_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(WMAP_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(FITSIMAGE_SUBDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(NAD_SUBDIR)")
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


