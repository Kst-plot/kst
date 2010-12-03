project(GetDataTests)

GD_FILES(gd test)

set(tests_ignored
	bzip_get bzip_get_get bzip_move_from bzip_nframes bzip_put
	gzip_get gzip_get_get gzip_move_from gzip_nframes gzip_put
	lzma_get lzma_nframes
	legacy_get legacy_get_put legacy_get_rofs legacy_nframes legacy_nonexistent legacy_put legacy_spf
	slim_get slim_nframes slim_put
	xz_get xz_nframes
	)
	
if(MSVC)
	set(tests_ignored 
		${tests_ignored}
		endian_alter endian_get endian_move
		get64 get_uint64 get_endian64 get_int64
		include_cb include_pc
		index_domain index_range
		move_data_endian
		nframes64
		open_cb_abort open_cb_cont open_cb_ignore open_cb_invalid open_cb_rescan
		put64 put_endian64
		)
endif()


if(MINGW)
	set(tests_ignored
		${tests_ignored}
		add_crecip89 
		alter_crecip89
		endian_alter endian_get endian_move
		header_complex
		madd_crecip89
		move_data_endian
		)
endif()

foreach(_item ${tests_ignored})
	list(REMOVE_ITEM gd_sources ${GD_DIR}/test/${_item}.c)
endforeach()

#set(gd_sources ${GD_DIR}/test/close.c ${GD_DIR}/test/alter_multiply.c)

add_definitions(-D__TEST__=\"${CMAKE_BINARY_DIR}/\")

foreach(_test ${gd_sources})
	get_filename_component(testname ${_test} NAME_WE)
	set(testname test_${testname})
	add_executable(${testname} ${_test})
	target_link_libraries(${testname} getdata)
	add_test(${testname} ${testname})
endforeach()



