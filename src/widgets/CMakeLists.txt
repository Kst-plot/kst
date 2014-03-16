kst_init(${kst_binary_name} widgets)

if(kst_qt5)
    kst_files_ignore(widgets)
endif()

if(QNX)
    kst_files_ignore(widgets)
endif()

kst_files_find(src/widgets)

kst_include_directories(core app math)

kst_add_library(SHARED)

kst_link(${libcore} ${libmath})
