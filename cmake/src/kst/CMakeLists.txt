kst_init(${kst_binary_name} "")

kst_files_ignore(main-vld)

kst_files_find(src/kst)

if(WIN32)
	if(MINGW)
	    add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/kst_icon.o
		                   COMMAND windres.exe -I${CMAKE_CURRENT_BINARY_DIR}
		                   -i${kst_dir}/src/kst/kst_icon.rc
		                   -o ${CMAKE_CURRENT_BINARY_DIR}/kst_icon.o)
		set(rc_file ${CMAKE_CURRENT_BINARY_DIR}/kst_icon.o)
	else()
		set(rc_file ${kst_dir}/src/kst/kst_icon.rc)
	endif()
endif()

kst_add_resources(src/images/images.qrc)

kst_find_info_files(KstCMakeFiles ${kst_dir}/cmake/*)
kst_find_info_files(KstCMakeFiles ${kst_dir}/cmake/modules/*)

kst_add_info_files(KstInfoFiles 
	${kst_dir}/INSTALL
	${kst_dir}/NEWS
	${kst_dir}/README)

kst_include_directories(app)

kst_add_executable(${kst_win32} ${rc_file})

kst_link(${libcore} ${libmath} ${libapp} ${libwidgets})

kst_install_executable()

if(UNIX AND NOT APPLE)
	set(man ${CMAKE_BINARY_DIR}/${kst_binary_name}.1)
	configure_file(${kst_dir}/src/kst/kst.1.txt ${man})
	install(CODE "execute_process(COMMAND gzip ${man})")
	set_source_files_properties(${man}.gz PROPERTIES GENERATED TRUE) 
	install(FILES ${man}.gz DESTINATION share/man/man1)
	
	set(k ${kst_dir}/src/kst)
#	install(FILES ${k}/kstplugin.desktop
#	              ${k}/kstdatasourceplugin.desktop
#	              ${k}/kstdataobjectplugin.desktop
#	              ${k}/kstbasicplugin.desktop
#	              ${k}/kstfilter.desktop
#	              ${k}/kstextension.desktop
#	                                 DESTINATION share/servicetypes/kst)
	install(FILES ${k}/kst2.desktop   DESTINATION share/applnk/Graphics)
	install(FILES ${k}/x-kst2.desktop DESTINATION share/mimelink/application)
	install(FILES ${k}/kst2.desktop   DESTINATION share/applications)

#       icons
        install(FILES ${kst_dir}/src/images/32x32/kst.png DESTINATION share/icons/hicolor/32x32/apps)
        install(FILES ${kst_dir}/src/images/48x48/kst.png DESTINATION share/icons/hicolor/48x48/apps)
        install(FILES ${kst_dir}/src/images/64x64/kst.png DESTINATION share/icons/hicolor/64x64/apps)
        install(FILES ${kst_dir}/src/images/128x128/kst.png DESTINATION share/icons/hicolor/128x128/apps)
        install(FILES ${kst_dir}/src/images/svg_icons/kst.svg DESTINATION share/icons/hicolor/scalable/apps)

        install(FILES ${kst_dir}/src/images/32x32/application-x-kst.png DESTINATION share/icons/hicolor/32x32/apps)
        install(FILES ${kst_dir}/src/images/48x48/application-x-kst.png DESTINATION share/icons/hicolor/48x48/apps)
        install(FILES ${kst_dir}/src/images/64x64/application-x-kst.png DESTINATION share/icons/hicolor/64x64/apps)
        install(FILES ${kst_dir}/src/images/128x128/application-x-kst.png DESTINATION share/icons/hicolor/128x128/apps)
        install(FILES ${kst_dir}/src/images/svg_icons/application-x-kst.svg DESTINATION share/icons/hicolor/scalable/apps)

endif()
