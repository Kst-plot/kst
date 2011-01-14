kst_init(${kst_binary_name})

kst_files_ignore(main-vld)

kst_files_find(src/kst)

if(MSVC)
	set(rc_file ${kst_dir}/src/kst/kst_icon.rc)
endif()

kst_add_resources(src/images/images.qrc)

kst_find_info_files(KstCMakeFiles ${kst_dir}/cmake/*)
kst_find_info_files(KstCMakeFiles ${kst_dir}/cmake/modules/*)

kst_add_info_files(KstInfoFiles 
	${kst_dir}/INSTALL
	${kst_dir}/NEWS
	${kst_dir}/README)

kst_include_directories(kstapp)

kst_add_executable(${kst_win32} ${rc_file})

kst_link(kstcore kstmath kstapp kstwidgets)

kst_install_executable()

if(UNIX)
	set(k ${kst_dir}/src/kst)
	install(FILES ${k}/kstplugin.desktop
	              ${k}/kstdatasourceplugin.desktop
	              ${k}/kstdataobjectplugin.desktop
	              ${k}/kstbasicplugin.desktop
	              ${k}/kstfilter.desktop
	              ${k}/kstextension.desktop
	                                 DESTINATION share/servicetypes/kst)
	install(FILES ${k}/kst.desktop   DESTINATION share/applnk/Graphics)
	install(FILES ${k}/kstui.rc      DESTINATION share/apps/kst)
	install(FILES ${k}/x-kst.desktop DESTINATION share/mimelink/application)
	install(FILES ${k}/kst.desktop   DESTINATION share/applications)
endif()
