cmake_minimum_required(VERSION 2.8)

if(kst_clang)
    set(CMAKE_C_COMPILER   "clang"   CACHE PATH "Clang C compiler")
    set(CMAKE_CXX_COMPILER "clang++" CACHE PATH "Clang C++ compiler")
endif()

project(Kst)

add_subdirectory(cmake)
