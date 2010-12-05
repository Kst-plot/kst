kst_init(kstapp)

kst_files_find(src/libkstapp)

kst_include_directories(kstcore kstmath kstwidgets)

kst_add_library(STATIC)

kst_link(kstcore kstmath kstwidgets)