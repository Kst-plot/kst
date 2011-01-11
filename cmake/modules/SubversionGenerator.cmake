
find_package(Subversion)

if(SUBVERSION_FOUND)
	# extract revision
	Subversion_WC_INFO(${source_dir} src)
	set(_revision "${src_WC_REVISION}")

	# check for modifications
	execute_process(
		COMMAND ${Subversion_SVN_EXECUTABLE} status ${source_dir}
		OUTPUT_VARIABLE src_WC_STATUS
		ERROR_VARIABLE Subversion_src_info_error
		RESULT_VARIABLE Subversion_src_info_result
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	# problems with multiple lines,
	# http://www.mail-archive.com/cmake@cmake.org/msg07254.html
	string(REGEX REPLACE "\r?\n" ";" lines "${src_WC_STATUS}")

	set(_modified)
	foreach(line ${lines})
		string(REGEX MATCH "^(.*\n)?M ([^\n]+).*" _found "${line}")
		if(_found)
			message(STATUS "Modified file: ${_found}")
			set(_modified ${_modified} _found)
		endif()
	endforeach()

	if(_modified)
		set(_revision "${_revision}${modified_str}")
	endif()

	# write a file with the SVN_REVISION define
	file(WRITE ${header_file}.tmp "#define SVN_REVISION \"${_revision}\"\n")
else()
	file(WRITE ${header_file}.tmp "#define SVN_REVISION \"unknown\"\n")
endif()

# copy the file to the final header only if 
# the version changes reduces needless rebuilds
execute_process(COMMAND ${CMAKE_COMMAND}
		-E copy_if_different
		${header_file}.tmp ${header_file})