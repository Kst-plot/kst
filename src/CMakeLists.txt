
#add_subdirectory( extdate )
add_subdirectory( libkst )
add_subdirectory( libkstmath )
add_subdirectory( datasources )
#add_subdirectory( widgets )
#add_subdirectory( libkstapp )
#add_subdirectory( kst )
#add_subdirectory( d2asc )
#add_subdirectory( d2d )
#add_subdirectory( plugins )
#add_subdirectory( extensions )

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KMDI_SUBDIR)")
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

