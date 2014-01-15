# ***************************************************************************
# *                                                                         *
# *   Copyright : (C) 2010 The University of Toronto                        *
# *   email     : netterfield@astro.utoronto.ca                             *
# *                                                                         *
# *   Copyright : (C) 2010 Peter Kümmel                                     *
# *   email     : syntheticpp@gmx.net                                       *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# ***************************************************************************

set(CMAKE_SYSTEM_NAME Windows)

# MinGW on Ubuntu: i686-w64-mingw32/x86_64-w64-mingw32 or i586-mingw32msvc
set(TOOLNAME ${kst_cross})
set(TOOLCHAIN "${TOOLNAME}-")
set(CMAKE_C_COMPILER   "${TOOLCHAIN}gcc"     CACHE PATH "Mingw C compiler" FORCE)
set(CMAKE_CXX_COMPILER "${TOOLCHAIN}g++"     CACHE PATH "Mingw C++ compiler" FORCE)
set(CMAKE_RC_COMPILER  "${TOOLCHAIN}windres" CACHE PATH "Mingw rc compiler" FORCE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

