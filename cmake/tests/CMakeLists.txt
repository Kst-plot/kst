project(ksttests)

file(GLOB test_headers ${kst_dir}/tests/*.h)

list(REMOVE_ITEM test_headers ${kst_dir}/tests/ksttest.h)

kst_include_directories(kstcore kstmath)

add_definitions(-DKST_USE_QTEST_MAIN -DKST_SRC_DIR=${kst_dir})


foreach(header ${test_headers})
	get_filename_component(filename ${header} NAME_WE)
	string(REPLACE test test_ testname ${filename})
	if(kst_verbose)
		message(STATUS "Building test ${testname}")
	endif()

	kst_init(${testname})

	set(moc_file)
	qt4_wrap_cpp(moc_file ${header} OPTIONS "-nw")

	kst_add_executable(
		${kst_dir}/tests/ksttest.h
		${kst_dir}/tests/${filename}.cpp
		${header} ${moc_file})

	kst_link(kstcore kstmath ${QT_QTTEST_LIBRARY})

	add_test(NAME ${testname} COMMAND ${testname})

	# Some file grouping in Studio
	kst_flat_source_group()
	source_group(Generated FILES ${moc_file})
endforeach()

message(STATUS)
message(STATUS "Tests added, to run them call 'make test'")
message(STATUS)

# like done with qmake
#add_subdirectory(allinone)



