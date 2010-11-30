
set(BZIP2_C bzip.c)
set(DEBUG_C debug.c)
set(GZIP_C gzip.c)
set(LZMA_C lzma.c)
set(LEGACY_C legacy.c)
set(SLIM_C slim.c)

GD_FILES(gd src)

if(MSVC)
	set(gdsrc 
		#${GETDATA_DIR}/src/add.c
		#${GETDATA_DIR}/src/ascii.c
		${GETDATA_DIR}/src/${BZIP2_C}
		#${GETDATA_DIR}/src/close.c
		#${GETDATA_DIR}/src/common.c
		#${GETDATA_DIR}/src/compat.c
		#${GETDATA_DIR}/src/constant.c
		${GETDATA_DIR}/src/${DEBUG_C}
		#${GETDATA_DIR}/src/del.c
		#${GETDATA_DIR}/src/encoding.c
		#${GETDATA_DIR}/src/endian.c
		#${GETDATA_DIR}/src/entry.c
		${GETDATA_DIR}/src/errors.c #config.h needed
		#${GETDATA_DIR}/src/field_list.c
		#${GETDATA_DIR}/src/flimits.c
		#${GETDATA_DIR}/src/flush.c
		#${GETDATA_DIR}/src/fragment.c
		#${GETDATA_DIR}/src/getdata.c
		#${GETDATA_DIR}/src/globals.c 
		#${GETDATA_DIR}/src/${GZIP_C}
		#${GETDATA_DIR}/src/index.c
		#${GETDATA_DIR}/src/include.c
		${GETDATA_DIR}/src/${LEGACY_C}
		${GETDATA_DIR}/src/${LZMA_C}
		#${GETDATA_DIR}/src/meta_list.c
		#${GETDATA_DIR}/src/mod.c
		#${GETDATA_DIR}/src/move.c
		#${GETDATA_DIR}/src/name.c
		#${GETDATA_DIR}/src/native.c
		#${GETDATA_DIR}/src/nfields.c
		#${GETDATA_DIR}/src/nframes.c
		#${GETDATA_DIR}/src/nmeta.c
		#${GETDATA_DIR}/src/open.c
		#${GETDATA_DIR}/src/parse.c
		#${GETDATA_DIR}/src/protect.c
		#${GETDATA_DIR}/src/putdata.c 
		#${GETDATA_DIR}/src/raw.c 
		${GETDATA_DIR}/src/${SLIM_C}
		#${GETDATA_DIR}/src/spf.c
		#${GETDATA_DIR}/src/string.c
		#${GETDATA_DIR}/src/types.c
		${GETDATA_DIR}/src/${GETDATA_LEGACY_H}
		${GETDATA_DIR}/src/internal.h		
	)
	
	# MSVC doesn't support C99, try as C++
	#set_source_files_properties(${gd_sources} PROPERTIES LANGUAGE CXX)
	list(REMOVE_ITEM gd_sources ${gdsrc})
endif()

if(MINGW)
	list(REMOVE_ITEM gd_sources
		${GETDATA_DIR}/src/${BZIP2_C}
		${GETDATA_DIR}/src/${DEBUG_C}
		${GETDATA_DIR}/src/${GZIP_C}
		${GETDATA_DIR}/src/${LZMA_C}
		${GETDATA_DIR}/src/${LEGACY_C}
		${GETDATA_DIR}/src/${SLIM_C})
endif()


add_library(getdata STATIC ${gd_sources} ${gd_headers} ${CMAKE_BINARY_DIR}/getdata.h)



install(FILES ${CMAKE_BINARY_DIR}/getdata.h DESTINATION include)
install(TARGETS getdata DESTINATION lib)


