# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(kst main.cpp)
target_link_libraries(kst
   Kst6App
   Kst6Math
   Kst6Widgets
   Kst6Core
)
install(TARGETS kst RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

set_target_properties(kst PROPERTIES INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}/kst")

if(UNIX AND NOT APPLE)
	set(man ${CMAKE_BINARY_DIR}/${kst_binary_name}.1)
	configure_file(${kst_dir}/src/kst/kst.1.txt ${man})
	set_source_files_properties(${man} PROPERTIES GENERATED TRUE)
	install(FILES ${man} DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)

	set(k ${kst_dir}/src/kst)
#	install(FILES ${k}/kstplugin.desktop
#	              ${k}/kstdatasourceplugin.desktop
#	              ${k}/kstdataobjectplugin.desktop
#	              ${k}/kstbasicplugin.desktop
#	              ${k}/kstfilter.desktop
#	              ${k}/kstextension.desktop
#	                                 DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/servicetypes/kst)
    install(FILES ${k}/x-kst.xml      DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/mime/packages)

    install(FILES org.kde.kst.metainfo.xml DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/metainfo)
    install(FILES org.kde.kst.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
    install(FILES ${kst_dir}/src/images/32x32/kst.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/apps)
    install(FILES ${kst_dir}/src/images/48x48/kst.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/apps)
    install(FILES ${kst_dir}/src/images/64x64/kst.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/apps)
    install(FILES ${kst_dir}/src/images/128x128/kst.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/128x128/apps)
    install(FILES ${kst_dir}/src/images/svg_icons/kst.svg DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps)

    install(FILES ${kst_dir}/src/images/32x32/application-x-kst.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/32x32/mimetypes)
    install(FILES ${kst_dir}/src/images/48x48/application-x-kst.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/48x48/mimetypes)
    install(FILES ${kst_dir}/src/images/64x64/application-x-kst.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/64x64/mimetypes)
    install(FILES ${kst_dir}/src/images/128x128/application-x-kst.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/128x128/mimetypes)
    install(FILES ${kst_dir}/src/images/svg_icons/application-x-kst.svg DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/mimetypes)
endif()

