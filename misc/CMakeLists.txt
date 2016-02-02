# install runtime files

if(WIN32)
	if (kst_qt5)
		install(FILES
			${QT_BINARY_DIR}/Qt5Core.dll
			${QT_BINARY_DIR}/Qt5Gui.dll
			${QT_BINARY_DIR}/Qt5Widgets.dll
			${QT_BINARY_DIR}/Qt5Xml.dll
			${QT_BINARY_DIR}/Qt5Network.dll				
			${QT_BINARY_DIR}/Qt5PrintSupport.dll
                        ${QT_BINARY_DIR}/Qt5Svg.dll
                        DESTINATION bin
			CONFIGURATIONS Release)
		install(FILES
			${QT_BINARY_DIR}/../plugins/platforms/qminimal.dll
			${QT_BINARY_DIR}/../plugins/platforms/qwindows.dll			
			DESTINATION bin/platforms
			CONFIGURATIONS Release)
		install(FILES
			${QT_BINARY_DIR}/../plugins/printsupport/windowsprintersupport.dll		
			DESTINATION bin/printsupport
			CONFIGURATIONS Release)
		install(FILES
			${QT_BINARY_DIR}/../plugins/imageformats/qgif.dll
			${QT_BINARY_DIR}/../plugins/imageformats/qjpeg.dll				
                        ${QT_BINARY_DIR}/../plugins/imageformats/qsvg.dll
                        DESTINATION bin/imageformats
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
        set(runtime)
        if(kst_cross)
            execute_process(COMMAND ${CMAKE_CXX_COMPILER} -print-libgcc-file-name OUTPUT_VARIABLE GCC_LIBGCC OUTPUT_STRIP_TRAILING_WHITESPACE)
            list(APPEND runtime /usr/${kst_cross}/lib/libwinpthread-1.dll)
        else()
            set(GCC_LIBGCC ${CMAKE_CXX_COMPILER})
        endif()
        get_filename_component(MINGW_RUNTIME_PATH ${GCC_LIBGCC} PATH)        
        macro(add_runtime_dll _DLL)
            file(GLOB GCC_RUNTIME ${MINGW_RUNTIME_PATH}/${_DLL})
            list(APPEND runtime ${GCC_RUNTIME})
        endmacro()
        add_runtime_dll(libgcc*.dll)
        add_runtime_dll(libstd*.dll)
        add_runtime_dll(libwin*.dll)
        if(NOT runtime)
            message(FATAL_ERROR "No mingw runtime found in ${MINGW_RUNTIME_PATH}")
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

