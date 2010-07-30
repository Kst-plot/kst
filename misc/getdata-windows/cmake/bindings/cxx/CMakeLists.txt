
GD_FILES(s bindings/cxx)
GD_FILES(h bindings/cxx/getdata)

include_directories(${GETDATA_DIR}/bindings/cxx)

add_library(getdata++ STATIC ${h_headers} ${s_sources})


install(FILES ${h_headers} DESTINATION ${GETDATA_INSTALL_DIR}/include/getdata)
install(TARGETS getdata++ DESTINATION ${GETDATA_INSTALL_DIR}/lib)
