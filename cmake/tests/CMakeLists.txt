project(ksttests)

kst_init(tests)
kst_files_find(tests)

kst_include_directories(kstcore kstmath)
add_definitions(-DKST_USE_QTEST_MAIN)

foreach(header ${kst_tests_headers})
	get_filename_component(testname ${header} NAME_WE)
	kst_init(${testname})
	set(moc_file)
	qt4_wrap_cpp(moc_file ${header} OPTIONS "-nw")
	kst_add_executable(${kst_dir}/tests/${testname}.cpp ${header} ${moc_file})
	kst_link(kstcore kstmath ${QT_QTTEST_LIBRARY})
	add_test(NAME ${testname} COMMAND ${testname})
	message(STATUS "Adding test: ${testname}")
endforeach()

# like done with qmake
#add_subdirectory(allinone)



