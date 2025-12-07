# SPDX-FileCopyrightText: 2025 Carl Schwan <carl@carlschwan.eu>
# SPDX-License-Identifier: BSD-2-Clause

add_executable(kst main.cpp)
target_link_libraries(kst
   Kst6App
   Kst6Math
   Kst6Widgets
   Kst6Core
)

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
    install(FILES ${k}/x-kst.xml      DESTINATION ${KDE_INSTALL_MIMEDIR})

    install(FILES org.kde.kst.metainfo.xml DESTINATION ${KDE_INSTALL_APPDIR})
    install(FILES org.kde.kst.desktop DESTINATION ${KDE_INSTALL_METAINFODIR})
    install(FILES ${kst_dir}/src/images/32x32/kst.png DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/32x32/apps)
    install(FILES ${kst_dir}/src/images/48x48/kst.png DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/48x48/apps)
    install(FILES ${kst_dir}/src/images/64x64/kst.png DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/64x64/apps)
    install(FILES ${kst_dir}/src/images/128x128/kst.png DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/128x128/apps)
    install(FILES ${kst_dir}/src/images/svg_icons/kst.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps)

    install(FILES ${kst_dir}/src/images/32x32/application-x-kst.png DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/32x32/mimetypes)
    install(FILES ${kst_dir}/src/images/48x48/application-x-kst.png DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/48x48/mimetypes)
    install(FILES ${kst_dir}/src/images/64x64/application-x-kst.png DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/64x64/mimetypes)
    install(FILES ${kst_dir}/src/images/128x128/application-x-kst.png DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/128x128/mimetypes)
    install(FILES ${kst_dir}/src/images/svg_icons/application-x-kst.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/mimetypes)
endif()

