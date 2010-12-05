kst_init(kstmath)

kst_files_find(src/libkstmath)

kst_dont_merge(eparse.cpp escan.cpp fftsg_h.c)

kst_include_directories(kstcore)

kst_add_library(SHARED)

kst_link(kstcore)