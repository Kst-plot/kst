kst_init(d2asc)

kst_files_find(src/d2asc)

kst_include_directories(core math)

kst_add_executable()

kst_link(${libcore} ${libmath})

kst_install_executable()





