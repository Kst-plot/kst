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

ExternalProject_Add(numpy 
                    URL             ${pypi}/n/numpy/${numpy}.tar.gz
                    CONFIGURE_COMMAND ""
                    BUILD_IN_SOURCE ON
                    BUILD_COMMAND   python setup.py build --fcompiler=gnu95
                    INSTALL_COMMAND sudo python setup.py install)


ExternalProject_Add(scipy 
                    URL             ${pypi}/s/scipy/${scipy}.tar.gz
                    CONFIGURE_COMMAND ""
                    BUILD_IN_SOURCE ON
                    BUILD_COMMAND   python setup.py build
                    INSTALL_COMMAND sudo python setup.py install)
                   
                    
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pykstpp_h.py.cmake ${CMAKE_CURRENT_BINARY_DIR}/pykstpp_h.py)

#this works on Ubuntu
get_filename_component(pyscripts_path ${PYTHON_LIBRARY} PATH)
get_filename_component(pyscripts_dir ${PYTHON_EXECUTABLE} NAME)
set(pyscripts_path ${pyscripts_path}/${pyscripts_dir})


message(STATUS "Copying Kst's python files into ${pyscripts_path}")
configure_file(${kstpydir}/pyKst/pykst.py    ${pyscripts_path}/pykst.py COPYONLY)
configure_file(${kstpydir}/pyKst/pykstpp.py  ${pyscripts_path}/pykstpp.py COPYONLY)
configure_file(${CMAKE_CURRENT_BINARY_DIR}/pykstpp_h.py ${pyscripts_path}/pykstpp_h.py COPYONLY)


