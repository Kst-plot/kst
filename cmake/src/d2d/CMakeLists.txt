kst_init(d2d)

kst_files_find(src/d2d)

kst_include_directories(kstcore kstmath)

kst_add_executable()

kst_link(kstcore kstmath)

kst_install_executable()
