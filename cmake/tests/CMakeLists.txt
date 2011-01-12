project(ksttests)

file(GLOB test_headers ${kst_dir}/tests/*.h)

kst_include_directories(kstcore kstmath)

add_definitions(-DKST_USE_QTEST_MAIN)

foreach(header ${test_headers})
	get_filename_component(filename ${header} NAME_WE)
	string(REPLACE test test_ testname ${filename})
	kst_init(${testname})
	set(moc_file)
	qt4_wrap_cpp(moc_file ${header} OPTIONS "-nw")
	kst_add_executable(${kst_dir}/tests/${filename}.cpp ${header} ${moc_file})
	kst_link(kstcore kstmath ${QT_QTTEST_LIBRARY})
	add_test(NAME ${testname} COMMAND ${testname})
	message(STATUS "Adding test: ${testname}")
endforeach()

message(STATUS)

# like done with qmake
#add_subdirectory(allinone)



