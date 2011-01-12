
kst_init(ksttests)

kst_files_find(tests)

kst_include_directories(kstcore kstmath)

remove_definitions(-DKST_USE_QTEST_MAIN)

kst_add_executable()

kst_link(kstcore kstmath ${QT_QTTEST_LIBRARY})

