kst_init(kstmath)

kst_files_find(src/libkstmath)

kst_include_directories(kstcore)

kst_add_library(SHARED)

kst_link(kstcore)