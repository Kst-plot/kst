

# kst_init_plugin(src/datasources datasource)

kst_init_plugin(src/datasources datasource)

if(MSVC OR MINGW)
	add_definitions(-DKST_USE_KST_ATOF)
endif()

if(CMAKE_COMPILER_IS_GNUCC AND NOT GCC_VERSION VERSION_GREATER 4.2)
    add_definitions(-DKST_NO_THREAD_LOCAL)
endif()


if(UNIX)
  # speedup on Linux x2.7
  add_definitions(-DKST_USE_KST_ATOF)
endif()

kst_add_plugin(. ascii)
kst_add_plugin(. qimagesource)
kst_add_plugin(. sourcelist)
kst_add_plugin(. its)

if(TARGET Getdata::getdata)
	include_directories(${GETDATA_INCLUDE_DIR})
  kst_add_plugin(. dirfilesource LINK_LIBRARIES ${GETDATA_LIBRARIES})

endif()

if(TARGET CFITSIO::CFITSIO)
	include_directories(${CFITSIO_INCLUDE_DIR})
	kst_add_plugin(. fitsimage LINK_LIBRARIES ${CFITSIO_LIBRARIES})
  #kst_add_plugin(. fitstable)
	#kst_add_plugin(. healpix)
endif()

if(TARGET Netcdf::Netcdf)
	include_directories(${NETCDF_INCLUDE_DIR})
	kst_add_plugin(. netcdf LINK_LIBRARIES  ${NETCDF_LIBRARIES})
endif()
	
if(TARGET Matio::Matio)
	include_directories(${MATIO_INCLUDE_DIR})
	kst_add_plugin(. matlab LINK_LIBRARIES ${MATIO_LIBRARIES}) # Directory named after the file format, not the lib
endif()

if(TARGET TIFF::TIFF)
	kst_add_plugin(. tiff16source LINK_LIBRARIES TIFF::TIFF) # Directory named after the file format, not the lib
endif()

if(TARGET HDF5::HDF5)
  include_directories(${HDF5_INCLUDE_DIR})
	kst_add_plugin(. hdf5 LINK_LIBRARIES ${HDF5_LIBRARIES})
endif()

find_program(CARGO_EXECUTABLE cargo)
set(SSDB_WORKSPACE_ROOT "${CMAKE_SOURCE_DIR}/../ss/cmd-tlm" CACHE PATH
		"Location of the telemetry_c Cargo workspace (leave empty to disable SSDB)")
set(SSDB_RUST_DIR ${SSDB_WORKSPACE_ROOT}/telemetry_c)
set(SSDB_RUST_WORKSPACE_READY FALSE)
if(NOT SSDB_WORKSPACE_ROOT STREQUAL "")
	set(SSDB_RUST_WORKSPACE_READY TRUE)
endif()

set(SSDB_RUST_MANIFESTS
	${SSDB_WORKSPACE_ROOT}/Cargo.toml
	${SSDB_WORKSPACE_ROOT}/telemetry/Cargo.toml
	${SSDB_WORKSPACE_ROOT}/telemetry_c/Cargo.toml
	${SSDB_WORKSPACE_ROOT}/telemetry-derived/Cargo.toml
	${SSDB_WORKSPACE_ROOT}/tokenize/Cargo.toml
	${SSDB_WORKSPACE_ROOT}/util/Cargo.toml)
foreach(_ssdb_manifest ${SSDB_RUST_MANIFESTS})
	if(NOT EXISTS ${_ssdb_manifest})
		set(SSDB_RUST_WORKSPACE_READY FALSE)
	endif()
endforeach()

if(CARGO_EXECUTABLE AND SSDB_RUST_WORKSPACE_READY)
	set(SSDB_RUST_LIB ${SSDB_WORKSPACE_ROOT}/target/release/libtelemetry_c.a)
	file(GLOB_RECURSE SSDB_RUST_SOURCES CONFIGURE_DEPENDS
		${SSDB_WORKSPACE_ROOT}/telemetry_c/src/*.rs
		${SSDB_WORKSPACE_ROOT}/telemetry/src/*.rs
		${SSDB_WORKSPACE_ROOT}/telemetry-derived/src/*.rs
		${SSDB_WORKSPACE_ROOT}/tokenize/src/*.rs
		${SSDB_WORKSPACE_ROOT}/util/src/*.rs)
	if(EXISTS ${SSDB_WORKSPACE_ROOT}/Cargo.lock)
		list(APPEND SSDB_RUST_MANIFESTS ${SSDB_WORKSPACE_ROOT}/Cargo.lock)
	endif()

	add_custom_command(OUTPUT ${SSDB_RUST_LIB}
		COMMAND ${CARGO_EXECUTABLE} build --release -p telemetry_c
		DEPENDS ${SSDB_RUST_SOURCES} ${SSDB_RUST_MANIFESTS} ${SSDB_RUST_DIR}/build.rs
		WORKING_DIRECTORY ${SSDB_RUST_DIR}
		COMMENT "Building ssdb Rust telemetry_c library"
		VERBATIM
	)
	add_custom_target(ssdb_rust_lib DEPENDS ${SSDB_RUST_LIB})

	add_library(telemetry_c STATIC IMPORTED GLOBAL)
	set_target_properties(telemetry_c PROPERTIES IMPORTED_LOCATION ${SSDB_RUST_LIB})
	add_dependencies(telemetry_c ssdb_rust_lib)

	include_directories(${SSDB_RUST_DIR}/c)
	kst_add_plugin(. ssdb LINK_LIBRARIES telemetry_c dl pthread m)
else()
	message(STATUS "Skipping ssdb plugin: cargo not found, workspace disabled or incomplete at ${SSDB_WORKSPACE_ROOT}")
endif()
