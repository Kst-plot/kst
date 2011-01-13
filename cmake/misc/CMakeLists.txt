
if(WIN32)
	set(color_folder    colors)
	set(tutorial_folder tutorial)
	set(font_folder fonts)
else()
	set(color_folder    share/config/colors)
	set(tutorial_folder share/apps/kst/tutorial)
	SET(font_folder     share/apps/kst/fonts)
endif()


# install data files
if(UNIX)
	#configure_file(${kst_dir}/misc/tutorial/gyrodata.dat.gz ${CMAKE_BINARY_DIR} COPYONLY)
	#add_cusom_command(PRE_BUILD OUTPUT gyrodata.dat COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/gyrodata.dat.sh ${CMAKE_SOURCE_DIR})
	#ADD_CUSTOM_TARGET("gyrodata" ALL DEPENDS gyrodata.dat)
endif()
file(GLOB dat_files ${kst_dir}/misc/tutorial/*.dat)
install(FILES ${dat_files} DESTINATION ${tutorial_folder})


# install colors
file(GLOB idl_files ${kst_dir}/misc/IDL*)
install(FILES ${idl_files} DESTINATION ${color_folder})


#install fonts
if(UNIX)
	file(GLOB fonts ${kst_dir}/misc/fonts/*.gz)
	install(FILES ${fonts} DESTINATION ${font_folder})
endif()

