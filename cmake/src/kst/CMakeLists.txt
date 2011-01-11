kst_init(kst)

kst_files_ignore(main-vld)

kst_files_find(src/kst)

if(MSVC)
	set(rc_file ${kst_dir}/src/kst/kst_icon.rc)
endif()

kst_add_resources(src/images/images.qrc)

kst_include_directories(kstapp)

kst_find_info_files(KstCMakeFiles ${kst_dir}/cmake/*)
kst_find_info_files(KstCMakeFiles ${kst_dir}/cmake/modules/*)

kst_add_info_files(KstInfoFiles 
	${kst_dir}/INSTALL
	${kst_dir}/NEWS
	${kst_dir}/README)

add_executable(kst ${kst_win32} ${kst_kst_sources} ${rc_file} ${kst_kst_info_files})

kst_link(kstcore kstmath kstapp kstwidgets ${QT_QTOPENGL_LIBRARY} ${QT_QTSVG_LIBRARY} ${kst_qtmain_library})


install(TARGETS kst 
	RUNTIME DESTINATION bin
	BUNDLE DESTINATION bin)
	
if(APPLE)
	#TODO does not work
	set(bdir ${CMAKE_INSTALL_PREFIX}/bin ${CMAKE_INSTALL_PREFIX}/lib ${CMAKE_INSTALL_PREFIX}/plugin)
	set(blib ${CMAKE_INSTALL_PREFIX}/lib/libkstcore.dylib ${CMAKE_INSTALL_PREFIX}/lib/libkstapp.dylib ${CMAKE_INSTALL_PREFIX}/lib/libkstwidgets.dylib ${CMAKE_INSTALL_PREFIX}/lib/libkstmath.dylib)
	install(CODE "include(BundleUtilities) fixup_bundle(kst \"${blib}\" \"${bdir}\")" COMPONENT RUNTIME)
endif()

if(WIN32)
	install(FILES
		${QT_BINARY_DIR}/QtCore4.dll
		${QT_BINARY_DIR}/QtGui4.dll
		${QT_BINARY_DIR}/QtXml4.dll
		${QT_BINARY_DIR}/QtOpenGL4.dll
		${QT_BINARY_DIR}/QtSvg4.dll
		DESTINATION bin
		CONFIGURATIONS Release)
	install(FILES
		${QT_BINARY_DIR}/QtCored4.dll
		${QT_BINARY_DIR}/QtGuid4.dll
		${QT_BINARY_DIR}/QtXmld4.dll
		${QT_BINARY_DIR}/QtOpenGLd4.dll
		${QT_BINARY_DIR}/QtSvgd4.dll
		DESTINATION bin
		CONFIGURATIONS Debug)
endif()

if(MINGW)
	get_filename_component(MINGW_BIN_PATH ${CMAKE_CXX_COMPILER} PATH)
	install(FILES 
		${MINGW_BIN_PATH}/libgcc_s_dw2-1.dll
		${MINGW_BIN_PATH}/mingwm10.dll
		DESTINATION bin)
endif()


