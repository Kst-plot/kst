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

if(kst_python_prefix)
    set(pydist_dir ${kst_python_prefix})
    message(STATUS "Installing python support into ${pydist_dir}")
else()
    # find install path for pykst files
    execute_process(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/finddistdir.py
                OUTPUT_VARIABLE pydist_dir
                OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "Installing python support into ${pydist_dir}. Overwrite using -Dkst_python_prefix=")
endif()

if(NOT kst_python_build)
    macro(checkversion _name _required)
        string(TOUPPER ${_name} _n)
        if(${${_n}_VERSION} VERSION_LESS ${_required})
            message(FATAL_ERROR "Python support: '${_name}' is too old (${${_n}_VERSION} < ${_required})")
        endif()
    endmacro()
    include(CMakePackageConfigHelpers)
    find_package(NumPy REQUIRED)
    checkversion(NumPy 1.6.0)
    find_package(SciPy REQUIRED)
    checkversion(SciPy 0.10.0)
endif()


if(kst_python_build)
    if(WIN32)
        # Python 2.6(7?)    : http://www.python.org/getit/windows
        # NumPy/Scipy (bin) : http://www.lfd.uci.edu/~gohlke/pythonlibs
        # Sip/PyQt    (src) : http://www.riverbankcomputing.co.uk/software/pyqt/intro
        # Status -> could not load QtGui (has nothing to do with kst)
        set(BUILD_NUM_PY FALSE)
        set(BUILD_SCI_PY FALSE)
    else()
        set(BUILD_NUM_PY TRUE)
        set(BUILD_SCI_PY TRUE)
    endif()

    include(ExternalProject)
    set(pypi http://pypi.python.org/packages/source)

    if(NOT EXISTS ${pydist_dir}/numpy  AND BUILD_NUM_PY)
        set(numpy numpy-1.6.1)
        ExternalProject_Add(numpy
                        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/numpy
                        URL    ${pypi}/n/numpy/${numpy}.tar.gz
                        CONFIGURE_COMMAND ""
                        BUILD_IN_SOURCE   ON
                        BUILD_COMMAND     python setup.py build --fcompiler=gnu95
                        INSTALL_COMMAND   sudo python setup.py install)
    endif()

    if(NOT EXISTS ${pydist_dir}/scipy  AND BUILD_SCI_PY)
        set(scipy scipy-0.10.0)
        ExternalProject_Add(scipy
                        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/scipy
                        URL    ${pypi}/s/scipy/${scipy}.tar.gz
                        CONFIGURE_COMMAND ""
                        BUILD_IN_SOURCE   ON
                        BUILD_COMMAND     python setup.py build
                        INSTALL_COMMAND   sudo python setup.py install)
    endif()

    message(STATUS "Python support: installing Kst's python files into '${pydist_dir}'")
    message(STATUS)
endif()


set(pykstpp_header ${CMAKE_CURRENT_BINARY_DIR}/pykstpp_h.py)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pykstpp_h.py.cmake ${pykstpp_header})

install(FILES ${kstpydir}/pyKst/pykst.py ${kstpydir}/pyKst/pykstpp.py ${pykstpp_header} 
        DESTINATION ${pydist_dir})


