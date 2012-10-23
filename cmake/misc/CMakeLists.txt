# install runtime files

if(WIN32)
	if (kst_qt5)
		install(FILES
			${QT_BINARY_DIR}/QtCore5.dll
			${QT_BINARY_DIR}/QtGui5.dll
			${QT_BINARY_DIR}/QtWidgets5.dll
			${QT_BINARY_DIR}/QtXml5.dll
			${QT_BINARY_DIR}/QtNetwork5.dll				
			${QT_BINARY_DIR}/QtPrintSupport5.dll
			DESTINATION bin
			CONFIGURATIONS Release)
		install(FILES
			${QT_BINARY_DIR}/../plugins/platforms/qminimal5.dll
			${QT_BINARY_DIR}/../plugins/platforms/windows5.dll			
			DESTINATION bin/platforms
			CONFIGURATIONS Release)
	else()
		install(FILES
			${QT_BINARY_DIR}/QtCore4.dll
			${QT_BINARY_DIR}/QtGui4.dll
			${QT_BINARY_DIR}/QtXml4.dll
			${QT_BINARY_DIR}/QtOpenGL4.dll
			${QT_BINARY_DIR}/QtSvg4.dll
			${QT_BINARY_DIR}/QtNetwork4.dll
			DESTINATION bin
			CONFIGURATIONS Release)
		install(FILES
			${QT_BINARY_DIR}/QtCored4.dll
			${QT_BINARY_DIR}/QtGuid4.dll
			${QT_BINARY_DIR}/QtXmld4.dll
			${QT_BINARY_DIR}/QtOpenGLd4.dll
			${QT_BINARY_DIR}/QtSvgd4.dll
			${QT_BINARY_DIR}/QtNetworkd4.dll
			DESTINATION bin
			CONFIGURATIONS Debug)
	endif()
endif()

if(MINGW)
	get_filename_component(MINGW_BIN_PATH ${CMAKE_CXX_COMPILER} PATH)
	if(kst_cross)
		# TODO hard coded paths
		get_filename_component(mingw_name ${kst_cross} NAME)
		set(MINGW_BIN_PATH ${MINGW_BIN_PATH}/../${mingw_name}/lib)
	endif()
	if(EXISTS ${MINGW_BIN_PATH}/libgcc_s_sjlj-1.dll)
		list(APPEND runtime ${MINGW_BIN_PATH}/libgcc_s_sjlj-1.dll)
	elseif(EXISTS ${MINGW_BIN_PATH}/libgcc_s_dw2-1.dll)
		list(APPEND runtime ${MINGW_BIN_PATH}/libgcc_s_dw2-1.dll)
	endif()
	if(EXISTS ${MINGW_BIN_PATH}/libstdc++-6.dll)
		list(APPEND runtime ${MINGW_BIN_PATH}/libstdc++-6.dll)
	endif()
	if(NOT runtime)
		message(FATAL_ERROR "No mingw runtime found in ${MINGW_BIN_PATH}")
	endif()
	install(FILES ${runtime} DESTINATION bin)
endif()


if(WIN32)
	set(color_folder    colors)
	set(tutorial_folder tutorial)
	set(font_folder fonts)
else()
	set(color_folder    share/config/colors)
	set(tutorial_folder share/apps/kst/tutorial)
	SET(font_folder     share/apps/kst/fonts)
endif()


# none of these are needed for now, so don't install.
# cbn April 8, 2011
# install data files (TODO: decide on how we want to ship tutorial/demo data)
#if(UNIX)
#	if(NOT gyrodata)
#		configure_file(${kst_dir}/sample_data/GYRODATA+free-width+whitespace+unixEOL+dot-decimal-separator.txt.gz 
#		${CMAKE_BINARY_DIR}/gyrodata.dat.gz COPYONLY)
#		execute_process(WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND gunzip gyrodata.dat.gz)
#		set(gyrodata ${CMAKE_BINARY_DIR}/gyrodata.dat CACHE STRING "gyrodata decompressed")
#	endif()
#endif()
#file(GLOB dat_files ${kst_dir}/misc/tutorial/*.dat)
#install(FILES ${dat_files} ${gyrodata} DESTINATION ${tutorial_folder})


# install colors (TODO: support reading color pallets from disk)
#file(GLOB idl_files ${kst_dir}/misc/IDL*)
#install(FILES ${idl_files} DESTINATION ${color_folder})

