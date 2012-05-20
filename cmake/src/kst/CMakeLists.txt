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

kst_add_resources(src/images/icons.qrc)
kst_add_resources(src/libkstapp/script.qrc)

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


if(UNIX AND kst_deploy)
	set(app "${CMAKE_INSTALL_PREFIX}/bin/${kst_binary_name}")
	set(dirs ${QT_LIBRARY_DIR} ${kst_build_dir}/lib)
	install(CODE "
		include(DeployQt4)
		fixup_qt4_executable(${app} \"\" \"\" \"${dirs}\")"
		COMPONENT RUNTIME)

	set(kst_renamed kst${kst_version})
	set(start_script ${CMAKE_BINARY_DIR}/kst2_start.sh)
	configure_file(${kst_dir}/cmake/src/kst/kst2.sh.cmake ${start_script} @ONLY)
	file(WRITE ${CMAKE_BINARY_DIR}/delpoy.sh "#!/bin/bash
	mv ${CMAKE_INSTALL_PREFIX}/bin/*.so* ${CMAKE_INSTALL_PREFIX}/lib
	mv ${CMAKE_INSTALL_PREFIX}/bin/${kst_binary_name} ${CMAKE_INSTALL_PREFIX}/bin/${kst_renamed}
	cp ${start_script} ${CMAKE_INSTALL_PREFIX}/bin/${kst_binary_name}
	")
	install(CODE "
		execute_process(COMMAND bash ${CMAKE_BINARY_DIR}/delpoy.sh) "
		COMPONENT RUNTIME)
endif()


