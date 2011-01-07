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

add_executable(kst ${kst_kst_sources} ${rc_file} ${kst_kst_info_files})

kst_link(kstcore kstmath kstapp kstwidgets ${QT_QTOPENGL_LIBRARY} ${QT_QTSVG_LIBRARY})
