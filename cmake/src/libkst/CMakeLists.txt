kst_init(kstcore)

kst_files_find(src/libkst)

kst_files_remove(stdinsource timezones)

if(WIN32 OR APPLE )
	kst_files_remove(sysinfo psversion)
endif()

kst_add_library(SHARED)

