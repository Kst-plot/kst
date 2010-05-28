
project( libsrc )


set(SOURCES 
	${TOP_DIR}/libsrc/attr.c
	${TOP_DIR}/libsrc/ncx.c
	${TOP_DIR}/libsrc/putget.c
	${TOP_DIR}/libsrc/dim.c 
	${TOP_DIR}/libsrc/error.c
	${TOP_DIR}/libsrc/libvers.c
	${TOP_DIR}/libsrc/nc.c
	${TOP_DIR}/libsrc/string.c
	${TOP_DIR}/libsrc/v1hpg.c
	${TOP_DIR}/libsrc/var.c
	${TOP_DIR}/libsrc/utf8proc.c
	${TOP_DIR}/libsrc/posixio.c
)

set(HEADERS
	${TOP_DIR}/libsrc/fbits.h
	${TOP_DIR}/libsrc/nc.h
	${TOP_DIR}/libsrc/ncio.h
	${TOP_DIR}/libsrc/ncx.h
	${TOP_DIR}/libsrc/netcdf.h
	${TOP_DIR}/libsrc/onstack.h
	${TOP_DIR}/libsrc/rnd.h
	${TOP_DIR}/libsrc/netcdf3.h
	${TOP_DIR}/libsrc/utf8proc.h
	${TOP_DIR}/libsrc/utf8proc_data.h
	${TOP_DIR}/libsrc/rename.h
)

include_directories(${TOP_DIR}/libsrc)

add_library(netcdf STATIC ${SOURCES} ${HEADERS})


install(FILES ${TOP_DIR}/libsrc/netcdf.h DESTINATION ${NETCDF_INSTALL_DIR}/include)
install(TARGETS netcdf DESTINATION ${NETCDF_INSTALL_DIR}/lib)

