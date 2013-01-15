

kst_init_plugin(src/datasources datasource)

if(MSVC OR MINGW)
	add_definitions(-DKST_USE_KST_ATOF)
endif()

include_directories(${kst_dir}/src/datasources/ascii)

kst_add_plugin_lib(. ascii)
set_target_properties(kst2_datasource_ascii_lib PROPERTIES COMPILE_DEFINITIONS KST_SMALL_PRREALLOC)
kst_init(test_asciisource "")
kst_add_test(${kst_dir}/tests/datasources/ascii/asciifilebuffertest.cpp)
kst_link(kst2_datasource_ascii_lib ${libcore} ${libmath} ${libwidgets})

kst_init(test_asciiatof "")
kst_add_test(${kst_dir}/tests/datasources/ascii/asciiatoftest.cpp)
kst_link(kst2_datasource_ascii_lib ${libcore} ${libmath} ${libwidgets})

kst_init(asciifilegenerator "")
kst_add_files(${kst_dir}/tests/datasources/ascii/asciifilegenerator.cpp)
kst_add_executable()
kst_link(${QT_QTCORE_LIBRARY})
