cmake_minimum_required(VERSION 2.8)

project(pyKst)

if(kst_binary_name) #stand-alone?
    set(kstpydir ${CMAKE_SOURCE_DIR}/..)
else()
    find_package(Qt4 REQUIRED)
    set(kstpydir ${CMAKE_SOURCE_DIR}/../..)
endif()

find_package(PythonInterp REQUIRED)
find_package(PythonLibs REQUIRED)

include(ExternalProject)

set(pypi http://pypi.python.org/packages/source)
set(numpy numpy-1.6.1)
set(scipy scipy-0.10.0)


#Path mystic: this works on Ubuntu
get_filename_component(pylib_path ${PYTHON_LIBRARY} PATH)
get_filename_component(pybin_name ${PYTHON_EXECUTABLE} NAME)
set(pylib_dir ${pylib_path}/${pybin_name})
set(pydist_dir /usr/local/lib/${pybin_name}/dist-packages)

if(NOT EXISTS ${pydist_dir}/numpy)
    ExternalProject_Add(numpy
                        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/numpy
                        URL             ${pypi}/n/numpy/${numpy}.tar.gz
                        CONFIGURE_COMMAND ""
                        BUILD_IN_SOURCE ON
                        BUILD_COMMAND   python setup.py build --fcompiler=gnu95
                        INSTALL_COMMAND sudo python setup.py install)
endif()

if(NOT EXISTS ${pydist_dir}/scipy)
    ExternalProject_Add(scipy
                        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/scipy
                        URL             ${pypi}/s/scipy/${scipy}.tar.gz
                        CONFIGURE_COMMAND ""
                        BUILD_IN_SOURCE ON
                        BUILD_COMMAND   python setup.py build
                        INSTALL_COMMAND sudo python setup.py install)
endif()
                   
set(pykstpp_header ${CMAKE_CURRENT_BINARY_DIR}/pykstpp_h.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pykstpp_h.py.cmake ${pykstpp_header})

install(FILES ${kstpydir}/pyKst/pykst.py ${kstpydir}/pyKst/pykstpp.py ${pykstpp_header} 
        DESTINATION ${pylib_dir})


