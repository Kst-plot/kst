kst_init(${kst_binary_name} widgets)

kst_files_find(src/widgets)

kst_include_directories(core app math)

kst_add_library(SHARED)

kst_link(${libcore} ${libmath})