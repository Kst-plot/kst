

kst_init_plugin(src/datasources datasource)

if(MSVC OR MINGW)
	add_definitions(-DKST_USE_KST_ATOF)
endif()


kst_add_plugin_lib(. ascii)
#set_target_properties(kst2_datasource_ascii_lib PROPERTIES COMPILE_DEFINITIONS KST_SMALL_PRREALLOC)
kst_init(test_asciisource "")
include_directories(${kst_dir}/src/datasources/ascii)
kst_add_test(${kst_dir}/tests/datasources/ascii/asciifilebuffertest.cpp)
kst_link(kst2_datasource_ascii_lib ${libcore} ${libmath} ${libwidgets})
