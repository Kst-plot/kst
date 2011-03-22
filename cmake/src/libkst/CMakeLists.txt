kst_init(${kst_binary_name} core)

kst_files_ignore(stdinsource timezones)

if(WIN32 OR APPLE )
	kst_files_ignore(sysinfo psversion)
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lrt")
endif()

kst_files_find(src/libkst)

kst_dont_merge(sysinfo.c psversion.c)

kst_add_library(SHARED)

kst_link()
