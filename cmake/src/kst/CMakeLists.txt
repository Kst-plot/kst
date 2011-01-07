kst_init(kst)

kst_files_ignore(main-vld)

kst_files_find(src/kst)

if(MSVC)
	set(rc_file ${KST_DIR}/src/kst/kst_icon.rc)
endif()

kst_add_resources(src/images/images.qrc)

kst_include_directories(kstapp)

kst_find_info_files(KstCMakeFiles ${KST_DIR}/cmake/*)
kst_find_info_files(KstCMakeFiles ${KST_DIR}/cmake/modules/*)

kst_add_info_files(KstInfoFiles 
	${KST_DIR}/INSTALL
	${KST_DIR}/NEWS
	${KST_DIR}/README)

add_executable(kst ${kst_win32} ${kst_kst_sources} ${rc_file} ${kst_kst_info_files})

kst_link(kstcore kstmath kstapp kstwidgets ${QT_QTOPENGL_LIBRARY} ${QT_QTSVG_LIBRARY} ${kst_qtmain_library})

install(TARGETS kst RUNTIME DESTINATION bin)

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


