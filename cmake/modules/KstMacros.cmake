macro(kst_init name)
	set(kst_name ${name})
endmacro()


macro(kst_files_find folder)
	set(_folder ${KST_DIR}/${folder})
	file(GLOB _sources     ${_folder}/*.c) 
	file(GLOB _sources_cpp ${_folder}/*.cpp)
	file(GLOB _headers     ${_folder}/*.h)
	file(GLOB _ui_files    ${_folder}/*.ui)
	set(_mocs)
	qt4_wrap_cpp(_mocs ${_headers} OPTIONS "-nw")
	set(_uis)
	qt4_wrap_ui(_uis ${_ui_files})
	set(kst_${kst_name}_sources ${_sources} ${_sources_cpp} ${_mocs} ${_uis})
	set(kst_${kst_name}_headers ${_headers})
	set(kst_${kst_name}_folder  ${_folder})
	set(kst_${kst_name}_includes ${_folder} ${CMAKE_CURRENT_BINARY_DIR} CACHE STRING "Include dir for ${kst_name}" FORCE)
	source_group("Ui" FILES ${_mocs} ${_ui_files})
	source_group("Generated" FILES ${_mocs} ${_uis})
endmacro()


macro(kst_add_library type)
	include_directories(${kst_${kst_name}_folder} ${CMAKE_CURRENT_BINARY_DIR})
	string(TOUPPER BUILD_${kst_name} _build_macro)
	add_definitions(-D${_build_macro})
	add_library(${kst_name} ${type} ${kst_${kst_name}_sources} ${kst_${kst_name}_headers})
	target_link_libraries(${kst_name} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTXML_LIBRARY})
endmacro()


macro(kst_init_plugin dir)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugin)
	include_directories(${CMAKE_BINARY_DIR}/${dir})
	kst_include_directories(kstcore kstmath kstwidgets)
	set(kst_plugin_dir ${dir})
endmacro()


macro(kst_add_plugin folder name)
	set(_name kstplugin_${folder}_${name})
	string(REPLACE . _  _name ${_name})
	string(REPLACE / _  _name ${_name})
	string(REPLACE __ _ _name ${_name})
	string(REPLACE __ _ _name ${_name})
	kst_init(${_name})
	kst_files_find(${kst_plugin_dir}/${folder}/${name})
	add_library(${kst_name} SHARED ${kst_${kst_name}_sources} ${kst_${kst_name}_headers})
	kst_link(kstcore kstmath kstwidgets)
endmacro()


macro(kst_include_directories)
	foreach(_it ${ARGV})
		include_directories(${kst_${_it}_includes})
	endforeach()
endmacro()


macro(kst_link type)
	target_link_libraries(${kst_name} ${ARGV})
endmacro()


macro(kst_files_remove)	
	foreach(_item ${ARGN})
		set(_file ${kst_${kst_name}_folder}/${_item})
		set(_moc)
		qt4_wrap_cpp(_moc ${_file}.h)
		list(REMOVE_ITEM kst_${kst_name}_sources ${_file}.cpp ${_file}.c ${_moc})
		#list(REMOVE_ITEM kst_${kst_name}_headers ${_file}.h)
	endforeach()
endmacro()


macro(kst_add_resources filepath)
	qt4_add_resources(_rcc ${KST_DIR}/${filepath})
	set(kst_${kst_name}_sources ${kst_${kst_name}_sources} ${_rcc})
endmacro()

		