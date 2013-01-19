
# copied from FindGsl.cmake

if(NOT MATIO_INCLUDEDIR)

if(NOT kst_cross)
	include(FindPkgConfig)
	pkg_check_modules(PKGMATIO QUIET matio)
endif()

if(NOT PKGMATIO_LIBRARIES)
	set(PKGMATIO_LIBRARIES matio)
	if (UNIX)
		set(PKGMATIO_LIBRARIES ${PKGMATIO_LIBRARIES})
	endif()
endif()

set(MATIO_INCLUDEDIR MATIO_INCLUDEDIR-NOTFOUND CACHE STRING "" FORCE)
find_path(MATIO_INCLUDEDIR matio.h
	HINTS
	ENV MATIO_DIR
	PATH_SUFFIXES include
	PATHS ${kst_3rdparty_dir} ${PKGMATIO_INCLUDEDIR})

set(MATIO_LIBRARY_LIST)
if(kst_3rdparty_dir)
    list(APPEND PKGMATIO_LIBRARIES zlibstatic)
endif()
foreach(it ${PKGMATIO_LIBRARIES})
	set(lib lib-NOTFOUND CACHE STRING "" FORCE)
	FIND_LIBRARY(lib ${it} 
		HINTS
		ENV MATIO_DIR
		PATH_SUFFIXES lib
		PATHS ${kst_3rdparty_dir} ${PKGMATIO_LIBRARY_DIRS})
	list(APPEND MATIO_LIBRARY_LIST ${lib})
endforeach()
set(MATIO_LIBRARIES ${MATIO_LIBRARY_LIST} CACHE STRING "" FORCE)

endif()


if(MATIO_INCLUDEDIR AND MATIO_LIBRARIES)
	set(MATIO_INCLUDE_DIR ${MATIO_INCLUDEDIR} ${MATIO_INCLUDEDIR}/..)
	set(matio 1)
	message(STATUS "Found matio (for Matlab .mat support):")
	message(STATUS "     includes : ${MATIO_INCLUDE_DIR}")
	message(STATUS "     libraries: ${MATIO_LIBRARIES}")
else()
	message(STATUS "Not found: matio, set MATIO_DIR")
endif()

message(STATUS "")

