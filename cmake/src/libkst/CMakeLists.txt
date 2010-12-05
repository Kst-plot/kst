kst_init(kstcore)

kst_files_ignore(stdinsource timezones)

if(WIN32 OR APPLE )
	kst_files_ignore(sysinfo psversion)
endif()

kst_files_find(src/libkst)

kst_dont_merge(sysinfo.c psversion.c)

kst_add_library(SHARED)

