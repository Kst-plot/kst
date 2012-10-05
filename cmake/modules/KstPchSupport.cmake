

# Not supported officially my cmake
# but there is a cmake ticket with examples
# http://www.vtk.org/Bug/view.php?id=1260



# use this macro before <add_library>
#
# _header : header to make a .gch
# _sources: the variable name (do not use ${..}) which contains a
#           a list of sources (a.cpp b.cpp c.cpp ...)
#           This macro will append a header file to it, then this 
#           src_list can be used in <add_library>
#
# Now a .gch file should be generated and gcc should use it.
#       (add -Winvalid-pch to the cpp flags to verify)
#
# make clean should delete the pch file
#
# example : kst_add_pch_rule(pch.h source_list_name SHARED)

macro(kst_add_pch_rule  _header _sources _lib_type)

	if(CMAKE_COMPILER_IS_GNUCC)
		# first we have to find all compiler arguments
		get_directory_property(_definitions COMPILE_DEFINITIONS)
		foreach (_it ${_definitions})
			list(APPEND _args "-D${_it}")
		endforeach()
		
		list(APPEND _args ${CMAKE_CXX_FLAGS})
		if(CMAKE_BUILD_TYPE MATCHES Release)
			list(APPEND _args ${CMAKE_CXX_FLAGS_RELEASE})
			get_directory_property(_definitions_type COMPILE_DEFINITIONS_RELEASE)
		else()
			list(APPEND _args ${CMAKE_CXX_FLAGS_DEBUG})
			get_directory_property(_definitions_type COMPILE_DEFINITIONS_DEBUG)
		endif()
		if(${_lib_type} MATCHES SHARED)
			list(APPEND _args ${CMAKE_SHARED_LIBRARY_CXX_FLAGS})
		endif()
		if(_definitions_type)
			list(APPEND _args -D${_definitions_type})
		endif()
		#message(STATUS "pch: ${_args}")
		
		get_directory_property(DIRINC INCLUDE_DIRECTORIES)
		foreach (_inc ${DIRINC})
			LIST(APPEND _args "-I" ${_inc})
		endforeach()
		
		set(_gch_filename "${_header}.gch")
		list(APPEND ${_sources} ${_gch_filename})
		list(APPEND _args -c ${_header} -o ${_gch_filename})
		
		separate_arguments(_args)
		
		# now build the pch with the compiler arguments 
		add_custom_command(OUTPUT ${_gch_filename}
			COMMAND ${CMAKE_COMMAND} -E remove ${_gch_filename}
			COMMAND ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_ARG1} ${_args}
			DEPENDS ${_header})
			
		# all other files should use the pch
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winvalid-pch -include ${_header}")
	
	else()
		
		file(WRITE ${_header}.tmp "#include \"${_header}\"\n")
		execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_header}.tmp ${_header}.cpp)
		
		if(MSVC_IDE)
			set(pch_file "${_header}.\$(ConfigurationName).pch")
		else()
			set(pch_file ${_header}.pch)
		endif()
		
		set_source_files_properties(${_header}.cpp PROPERTIES COMPILE_FLAGS "/Yc\"${_header}\" /Fp${pch_file}"
		                                                      OBJECT_OUTPUTS ${pch_file})

		# Bug in cmake: next line also compile .c files with pchs
		#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /FI${_header} /Yu${_header} /Fp${pch_file}")
		foreach(it ${${_sources}})
			get_filename_component(ext ${it} EXT)
			if(ext STREQUAL .c)
			else()
				set_source_files_properties(${it} PROPERTIES COMPILE_FLAGS "/FI${_header} /Yu${_header} /Fp${pch_file}"
				                                             OBJECT_DEPENDS ${pch_file})
			endif()
		endforeach()
			
		list(APPEND ${_sources} ${_header} ${_header}.cpp) 
	endif()
		
endmacro()


