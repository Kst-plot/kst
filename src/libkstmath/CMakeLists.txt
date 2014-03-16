kst_init(${kst_binary_name} math)

kst_files_find(src/libkstmath)

kst_dont_merge(eparse.cpp escan.cpp fftsg_h.c)

kst_include_directories(core math)

kst_add_library(SHARED)

kst_link(${libcore})
