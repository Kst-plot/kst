cmake_minimum_required(VERSION 2.6.4)

project( cfit )


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin) 
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/lib) 

if(NOT CFIT_DIR)
	set(CFIT_DIR ${CMAKE_SOURCE_DIR})
endif()

set(cfit_sources
	${CFIT_DIR}/buffers.c
	${CFIT_DIR}/cfileio.c
	${CFIT_DIR}/checksum.c
	${CFIT_DIR}/compress.c
	${CFIT_DIR}/drvrfile.c
	${CFIT_DIR}/drvrmem.c
	${CFIT_DIR}/editcol.c
	${CFIT_DIR}/edithdu.c
	${CFIT_DIR}/eval_f.c
	${CFIT_DIR}/eval_l.c
	${CFIT_DIR}/eval_y.c
	${CFIT_DIR}/f77_wrap1.c
	${CFIT_DIR}/f77_wrap2.c
	${CFIT_DIR}/f77_wrap3.c
	${CFIT_DIR}/f77_wrap4.c
	${CFIT_DIR}/fitscore.c
	${CFIT_DIR}/getcol.c
	${CFIT_DIR}/getcolb.c
	${CFIT_DIR}/getcold.c
	${CFIT_DIR}/getcole.c
	${CFIT_DIR}/getcoli.c
	${CFIT_DIR}/getcolj.c
	${CFIT_DIR}/getcolk.c
	${CFIT_DIR}/getcoll.c
	${CFIT_DIR}/getcols.c
	${CFIT_DIR}/getcolsb.c
	${CFIT_DIR}/getcolui.c
	${CFIT_DIR}/getcoluj.c
	${CFIT_DIR}/getcoluk.c
	${CFIT_DIR}/getkey.c
	${CFIT_DIR}/group.c
	${CFIT_DIR}/grparser.c
	${CFIT_DIR}/histo.c
	${CFIT_DIR}/iraffits.c
	${CFIT_DIR}/modkey.c
	${CFIT_DIR}/putcol.c
	${CFIT_DIR}/putcolb.c
	${CFIT_DIR}/putcold.c
	${CFIT_DIR}/putcole.c
	${CFIT_DIR}/putcoli.c
	${CFIT_DIR}/putcolj.c
	${CFIT_DIR}/putcolk.c
	${CFIT_DIR}/putcoll.c
	${CFIT_DIR}/putcols.c
	${CFIT_DIR}/putcolsb.c
	${CFIT_DIR}/putcolu.c
	${CFIT_DIR}/putcolui.c
	${CFIT_DIR}/putcoluj.c
	${CFIT_DIR}/putcoluk.c
	${CFIT_DIR}/putkey.c
	${CFIT_DIR}/region.c
	${CFIT_DIR}/scalnull.c
	${CFIT_DIR}/swapproc.c
	${CFIT_DIR}/wcssub.c
	${CFIT_DIR}/wcsutil.c
	${CFIT_DIR}/fits_hcompress.c
	${CFIT_DIR}/fits_hdecompress.c
	${CFIT_DIR}/imcompress.c
	${CFIT_DIR}/pliocomp.c
	${CFIT_DIR}/quantize.c
	${CFIT_DIR}/ricecomp.c
)

file(GLOB cfit_headers ${CFIT_DIR}/*.h)


if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	set(CMAKE_DEBUG_POSTFIX d)
endif()

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_library(cfitsio STATIC ${cfit_sources} ${cfit_headers})

if (NOT CMAKE_INSTALL_PREFIX)
	set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/INSTALLED)
endif()

install(FILES ${cfit_headers} DESTINATION include/cfitsio)
install(TARGETS cfitsio DESTINATION lib)


