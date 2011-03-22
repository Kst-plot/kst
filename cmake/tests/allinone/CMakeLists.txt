
kst_init(${kst_binary_name} tests)

kst_files_find(tests)

kst_include_directories(core math)

remove_definitions(-DKST_USE_QTEST_MAIN)

kst_add_executable()

kst_link(${libcore} ${libmath} ${QT_QTTEST_LIBRARY})

