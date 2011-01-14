add_subdirectory(libkst)
add_subdirectory(libkstmath)
add_subdirectory(widgets)
add_subdirectory(libkstapp)
add_subdirectory(kst)

add_subdirectory(datasources)

if(kst_dataobject)
	add_subdirectory(plugins)
endif()

add_subdirectory(d2asc)
#add_subdirectory(d2d)


