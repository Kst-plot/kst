
set(files_ignored
	${GD_DIR}/src/bzip.c
	${GD_DIR}/src/debug.c
	${GD_DIR}/src/gzip.c
	${GD_DIR}/src/lzma.c
	${GD_DIR}/src/legacy.c
	${GD_DIR}/src/slim.c
	)

GD_FILES(gd src)

if(MSVC)
	set(gd_sources 
		${gd_sources}
		${GD_DIR}/src/msvc/dirent.h
		${GD_DIR}/src/msvc/dirent.c
		${GD_DIR}/src/msvc/dirname.c
		${GD_DIR}/src/msvc/libgen.h
		${GD_DIR}/src/msvc/unistd.h
		)
endif()


list(REMOVE_ITEM gd_sources ${files_ignored})

add_library(getdata STATIC ${gd_sources} ${gd_headers} ${CMAKE_BINARY_DIR}/getdata.h)


install(FILES ${CMAKE_BINARY_DIR}/getdata.h DESTINATION include)
install(TARGETS getdata DESTINATION lib)



