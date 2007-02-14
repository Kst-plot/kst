
add_subdirectory( elog )
add_subdirectory( pulse )

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KJSE_SUBDIR)")
include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############




#original Makefile.am contents follow:

#KJSE_SUBDIR=js
##if include_rana
##RANA_SUBDIR=rana
##endif
#SUBDIRS=elog $(KJSE_SUBDIR) pulse
