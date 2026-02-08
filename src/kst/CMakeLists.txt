# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(kst main.cpp)
target_link_libraries(kst
   Kst6App
   Kst6Math
   Kst6Widgets
   Kst6Core
)
install(TARGETS kst RUNTIME DESTINATION bin)

set_target_properties(kst PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/kst")

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
    install(FILES ${k}/x-kst.xml      DESTINATION share/mime/packages)

    install(FILES org.kde.kst.metainfo.xml DESTINATION share/metainfo)
    install(FILES org.kde.kst.desktop DESTINATION share/applications)
    install(FILES ${kst_dir}/src/images/32x32/kst.png DESTINATION share/icons/hicolor/32x32/apps)
    install(FILES ${kst_dir}/src/images/48x48/kst.png DESTINATION share/icons/hicolor/48x48/apps)
    install(FILES ${kst_dir}/src/images/64x64/kst.png DESTINATION share/icons/hicolor/64x64/apps)
    install(FILES ${kst_dir}/src/images/128x128/kst.png DESTINATION share/icons/hicolor/128x128/apps)
    install(FILES ${kst_dir}/src/images/svg_icons/kst.svg DESTINATION share/icons/hicolor/scalable/apps)

    install(FILES ${kst_dir}/src/images/32x32/application-x-kst.png DESTINATION share/icons/hicolor/32x32/mimetypes)
    install(FILES ${kst_dir}/src/images/48x48/application-x-kst.png DESTINATION share/icons/hicolor/48x48/mimetypes)
    install(FILES ${kst_dir}/src/images/64x64/application-x-kst.png DESTINATION share/icons/hicolor/64x64/mimetypes)
    install(FILES ${kst_dir}/src/images/128x128/application-x-kst.png DESTINATION share/icons/hicolor/128x128/mimetypes)
    install(FILES ${kst_dir}/src/images/svg_icons/application-x-kst.svg DESTINATION share/icons/hicolor/scalable/mimetypes)
endif()

