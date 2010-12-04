kst_init(kstwidgets)

kst_files_find(src/widgets)

kst_include_directories(kstcore kstapp kstmath)

kst_add_library(SHARED)

kst_link(kstcore kstmath)