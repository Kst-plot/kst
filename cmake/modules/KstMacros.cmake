include(MergedFilesBuild)
include(KstPchSupport)


macro(kst_dbg)
	foreach(it ${ARGN})
		message(STATUS "dgb: ${it} = ${${it}}")
	endforeach()
endmacro()


macro(kst_init name)
	set(kst_name ${name})
	set(kst_${kst_name}_dont_merge)
	set(kst_${kst_name}_ignore)
	set(kst_${kst_name}_info_files)
endmacro()


macro(kst_revision_project_name name)
	set(kst_revision_project ${name})
endmacro()


macro(kst_revision_add_dependency)
	if(kst_revision_project)
		add_dependencies(${kst_name} ${kst_revision_project})
	endif()
endmacro()


macro(kst_files_find folder)
	set(_folder ${kst_dir}/${folder})
	file(GLOB _sources     ${_folder}/*.c) 
	file(GLOB _sources_cpp ${_folder}/*.cpp)
	file(GLOB _headers     ${_folder}/*.h)
	file(GLOB _ui_files    ${_folder}/*.ui)
	set(kst_${kst_name}_folder  ${_folder})
	kst_files_remove(_sources     ${kst_${kst_name}_ignore})
	kst_files_remove(_sources_cpp ${kst_${kst_name}_ignore})
	kst_files_remove(_headers     ${kst_${kst_name}_ignore})
	set(_mocs)
	qt4_wrap_cpp(_mocs ${_headers} OPTIONS "-nw")
	set(_uis)
	qt4_wrap_ui(_uis ${_ui_files})
	set(kst_${kst_name}_sources_not_generated ${_sources} ${_sources_cpp})
	set(kst_${kst_name}_sources ${_sources} ${_sources_cpp} ${_mocs} ${_uis})
	set(kst_${kst_name}_headers ${_headers})
	set(kst_${kst_name}_mocs    ${_mocs})
	set(kst_${kst_name}_ui_files ${_ui_files})
	set(kst_${kst_name}_uis     ${_uis})
	set(kst_${kst_name}_includes ${_folder} ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "Include dir for ${kst_name}" FORCE)
	source_group("Ui" FILES ${_mocs} ${_ui_files})
	source_group("Generated" FILES ${_mocs} ${_uis})
endmacro()


macro(kst_dont_merge)
	set(kst_${kst_name}_dont_merge)
	foreach(_it ${ARGV})
		set(kst_${kst_name}_dont_merge ${kst_${kst_name}_dont_merge} ${kst_${kst_name}_folder}/${_it})
	endforeach()
endmacro()


macro(kst_set_target_properties)
	set_property(TARGET ${kst_name} PROPERTY DEBUG_POSTFIX ${kst_debug_postfix})
	set_target_properties(${kst_name} PROPERTIES VERSION ${kst_version} SOVERSION 2)
endmacro()


macro(kst_add_executable)
	include_directories(${kst_${kst_name}_folder} ${CMAKE_CURRENT_BINARY_DIR})
	add_executable(${kst_name} ${ARGN} ${kst_${kst_name}_sources} ${kst_${kst_name}_headers} ${kst_${kst_name}_info_files})
	target_link_libraries(${kst_name} ${kst_qtmain_library})
	kst_set_target_properties()
	kst_revision_add_dependency()
	kst_flat_source_group(${kst_${kst_name}_headers} ${kst_${kst_name}_sources_not_generated})
endmacro()


macro(kst_install_executable)
	install(TARGETS ${kst_name} 
		RUNTIME DESTINATION bin
		BUNDLE DESTINATION .)
endmacro()


macro(kst_add_library type)
	include_directories(${kst_${kst_name}_folder} ${CMAKE_CURRENT_BINARY_DIR})
	string(TOUPPER BUILD_${kst_name} _build_macro)
	add_definitions(-D${_build_macro})
	if(kst_pch)
		set(pch ${kst_name}_pch)
		configure_file(${kst_${kst_name}_folder}/${pch} ${CMAKE_CURRENT_BINARY_DIR}/${pch}.h COPYONLY)
		#add_definitions(-DKST_PCH_RECURSIVE) # only some percents faster
		kst_add_pch_rule(${CMAKE_CURRENT_BINARY_DIR}/${pch}.h kst_${kst_name}_sources ${type})
	endif()
	if(kst_merge_files)
		if(kst_merge_rebuild)
			set(merged_files_rebuild 1)
		endif()
		kst_files_remove(kst_${kst_name}_sources ${kst_${kst_name}_dont_merge})
		kst_files_remove(kst_${kst_name}_dont_merge ${kst_${kst_name}_ignore})
		kst_files_remove(kst_${kst_name}_sources_not_generated ${kst_${kst_name}_dont_merge})
		merged_files_build(merged kst_${kst_name}_sources)
		set(depends_moc_uic ${kst_${kst_name}_headers} ${kst_${kst_name}_uis} ${kst_${kst_name}_ui_files} ${kst_${kst_name}_mocs})
		set_source_files_properties(merged_const.cpp   PROPERTIES OBJECT_DEPENDS "${depends_moc_uic}")
		set_source_files_properties(merged_touched.cpp PROPERTIES OBJECT_DEPENDS "${depends_moc_uic}")
		add_library(${kst_name} ${type} ${merged_files} ${kst_${kst_name}_dont_merge} ${kst_${kst_name}_headers} ${kst_${kst_name}_uis} ${kst_${kst_name}_sources_not_generated} ${svnversion_h})
		kst_add_info_files(Headers/Sources ${kst_${kst_name}_sources_not_generated})		
	else()
		add_library(${kst_name} ${type} ${kst_${kst_name}_sources} ${kst_${kst_name}_headers} ${svnversion_h})
	endif()
	kst_set_target_properties()
	kst_flat_source_group(${kst_${kst_name}_headers} ${kst_${kst_name}_sources_not_generated})
	kst_revision_add_dependency()
	if(WIN32)
		if(NOT ${type} STREQUAL "STATIC")
			install(TARGETS ${kst_name} RUNTIME DESTINATION bin)
		endif()
	elseif(NOT APPLE)
		install(TARGETS ${kst_name} DESTINATION lib)
	endif()
endmacro()


macro(kst_init_plugin dir)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)
	if(APPLE AND NOT CMAKE_GENERATOR STREQUAL Xcode)
	    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${kst_binary_name}.app/Contents/plugins)
	else()
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins)
	endif()
	include_directories(${CMAKE_BINARY_DIR}/${dir})
	kst_include_directories(kstcore kstmath kstwidgets)
	set(kst_plugin_dir ${dir})
	set(kst_plugin_prefix ${ARGN})
endmacro()


macro(kst_add_plugin folder name)
	set(_name kst_${kst_plugin_prefix}_${folder}_${name})
	string(REPLACE . _  _name ${_name})
	string(REPLACE / _  _name ${_name})
	string(REPLACE __ _ _name ${_name})
	string(REPLACE __ _ _name ${_name})
	kst_init(${_name})
	kst_files_find(${kst_plugin_dir}/${folder}/${name})
	add_library(${kst_name} MODULE ${kst_${kst_name}_sources} ${kst_${kst_name}_headers})
	kst_link(kstcore kstmath kstwidgets)
	if(NOT APPLE)
		install(TARGETS ${kst_name} LIBRARY DESTINATION plugins)
		kst_find_install_desktop_file(${kst_plugin_dir}/${folder}/${name})
	endif()
	add_dependencies(${kst_binary_name} ${kst_name})
	kst_flat_source_group(${kst_${kst_name}_headers} ${kst_${kst_name}_sources_not_generated})
	if(kst_verbose)
	  message(STATUS "Building plugin ${kst_name}")
	endif()
endmacro()


macro(kst_include_directories)
	foreach(_it ${ARGV})
		include_directories(${kst_${_it}_includes})
	endforeach()
endmacro()


macro(kst_find_install_desktop_file folder)
	if(UNIX)
		file(GLOB _desktop_file ${kst_dir}/${folder}/*.desktop)
		install(FILES  ${_desktop_file} DESTINATION ${kst_install_plugin_desktop_file_path})
	endif()
endmacro()


macro(kst_link)
	target_link_libraries(${kst_name} ${ARGV}
		${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY} ${QT_QTOPENGL_LIBRARY} ${QT_QTSVG_LIBRARY})
endmacro()


macro(kst_files_ignore)
	set(kst_${kst_name}_ignore ${kst_${kst_name}_ignore} ${ARGV})
endmacro()


macro(kst_files_remove list)
	foreach(_item ${ARGN})
		set(_file ${kst_${kst_name}_folder}/${_item})
		if(${list})
			list(REMOVE_ITEM ${list} ${_item} ${_file} ${_file}.cpp ${_file}.c ${_file}.h)
		endif()
	endforeach()
endmacro()


macro(kst_add_files)
	set(kst_${kst_name}_sources ${kst_${kst_name}_sources} ${ARGN})
endmacro()


macro(kst_add_resources filepath)
	qt4_add_resources(_rcc ${kst_dir}/${filepath})
	kst_add_files(${_rcc})
endmacro()


macro(kst_option_init)
	set(kst_options)
endmacro()


macro(kst_option _name _description _default _sys)
	set(_msg OFF)
	if(${_sys} MATCHES "gcc")
		set(_system CMAKE_COMPILER_IS_GNUCXX)
	elseif(${_sys} MATCHES "win")
		set(_system WIN32)
	else()
		set(_system ${_sys})
	endif()
	if(${_system} MATCHES "all")
		option(kst_${_name} ${_description} ${_default})
		set(_msg ON)
	else()
		if(${${_system}})
			option(kst_${_name} ${_description} ${_default})
			set(_msg ON)
		endif()
	endif()
	list(APPEND kst_options kst_${_name})
	set(kst_${_name}_description ${_description})
	set(kst_${_name}_show_message ${_msg})
endmacro()


macro(kst_option_list_all)
	foreach(_option ${kst_options})
		if(${${_option}_show_message})
			string(SUBSTRING "${_option}                            " 0 25 _var)
			if(${_option})
				set(_isset ON)
			else()
				set(_isset OFF)
			endif()
			string(SUBSTRING "${_isset}     " 0 4 _val)
			message(STATUS "${_var}= ${_val}   : ${${_option}_description}")
		endif()
	endforeach()
endmacro()


macro(kst_add_info_files group)
	foreach(_it ${ARGN})
		if(NOT IS_DIRECTORY ${_it})
			get_filename_component(name ${_it} NAME)
			if(NOT ${_it} MATCHES "^/\\\\..*$;~$")
				set_source_files_properties(${_it} PROPERTIES HEADER_FILE_ONLY TRUE)
				set(kst_${group}_info_files ${kst_${group}_info_files} ${_it})
			endif()
		endif()
	endforeach()
	source_group(${group} FILES ${kst_${group}_info_files})
	set(kst_${kst_name}_info_files ${kst_${kst_name}_info_files} ${kst_${group}_info_files})
endmacro()


macro(kst_find_info_files group files)
	file(GLOB _filelist ${files})
	kst_add_info_files(${group} ${_filelist})
endmacro()


macro(kst_flat_source_group)
	source_group("Source Files" Files)
	source_group("Header Files" Files)
	source_group("CMakeLists" FILES CMakeLists.txt)
	source_group("Headers/Sources" FILES ${ARGN})
endmacro()


