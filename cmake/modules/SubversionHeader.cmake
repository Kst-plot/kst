
macro(SubversionHeader source_dir target_name header_file modified_str)
	# a custom target that is always built
	# creates ${header_file} using SubversionGenerator.cmake script
	add_custom_target(${target_name} ALL 
		COMMAND ${CMAKE_COMMAND} 
			-Dsource_dir="${source_dir}"
			-Dheader_file="${header_file}"
			-Dmodified_str="${modified_str}"
			-P ${CMAKE_CURRENT_SOURCE_DIR}/modules/SubversionGenerator.cmake)

	# ${header_file} is a generated file
	set_source_files_properties(${header_file}
			PROPERTIES
			GENERATED TRUE
			HEADER_FILE_ONLY TRUE)
endmacro()

