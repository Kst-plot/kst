
set(CMAKE_SYSTEM_NAME Windows)

# MinGW on Ubuntu: i686-w64-mingw32/x86_64-w64-mingw32 or i586-mingw32msvc
set(TOOLNAME ${kst_cross})
set(TOOLCHAIN "/usr/bin/${TOOLNAME}-")
set(CMAKE_C_COMPILER   "${TOOLCHAIN}gcc"     CACHE PATH "Mingw C compiler" FORCE)
set(CMAKE_CXX_COMPILER "${TOOLCHAIN}g++"     CACHE PATH "Mingw C++ compiler" FORCE)
set(CMAKE_RC_COMPILER  "${TOOLCHAIN}windres" CACHE PATH "Mingw C++ compiler" FORCE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

if(kst_qt4)
	# Cross-compiled Qt, branch cross-mingw from https://gitorious.org/~syntheticpp/qt/qt4
	set(QT_MINGW_DIR ${kst_qt4} CACHE PATH "Qt for Mingw" FORCE)
	set(CMAKE_FIND_ROOT_PATH /usr/${TOOLNAME} ${QT_MINGW_DIR} CACHE PATH "Mingw find root path" FORCE)
endif()

set(QT_QMAKE_EXECUTABLE ${QT_MINGW_DIR}/bin/qmake CACHE PATH "Qt's qmake" FORCE)
set(QT_BINARY_DIR       ${QT_MINGW_DIR}/bin       CACHE PATH "Qt's bin dir" FORCE)


message(STATUS "Using qmake from ${QT_QMAKE_EXECUTABLE}")
