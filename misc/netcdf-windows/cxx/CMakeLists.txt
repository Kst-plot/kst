
project( cxx )


set(SOURCES 
	${TOP_DIR}/cxx/netcdf.cpp 
	${TOP_DIR}/cxx/ncvalues.cpp
)

set(HEADERS 
	${TOP_DIR}/cxx/netcdfcpp.h
	${TOP_DIR}/cxx/netcdf.hh 
	${TOP_DIR}/cxx/ncvalues.h
)

include_directories(
	${TOP_DIR}/cxx
	${TOP_DIR}/libsrc
)

add_library(netcdf_c++ STATIC ${SOURCES} ${HEADERS})

target_link_libraries(netcdf_c++ netcdf)

install(FILES ${HEADERS} DESTINATION include)
install(TARGETS netcdf_c++ DESTINATION lib)

