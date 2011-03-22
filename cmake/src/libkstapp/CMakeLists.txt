kst_init(${kst_binary_name} app)

kst_files_find(src/libkstapp)

kst_include_directories(core math widgets)

set(AUTHORS ${kst_dir}/AUTHORS)
set_source_files_properties(${AUTHORS} PROPERTIES GENERATED TRUE HEADER_FILE_ONLY TRUE)
file(READ ${AUTHORS} data)
string(REPLACE "\n" ";" kst_authors ${data})
file(WRITE ${CMAKE_BINARY_DIR}/authors.h.tmp "static const char* kst_authors = \"${kst_authors}\";\n")

execute_process(COMMAND ${CMAKE_COMMAND}
		-E copy_if_different
		${CMAKE_BINARY_DIR}/authors.h.tmp ${CMAKE_BINARY_DIR}/authors.h)
		
kst_add_library(STATIC ${CMAKE_BINARY_DIR}/authors.h ${AUTHORS})

kst_link(${libcore} ${libmath} ${libwidgets})
