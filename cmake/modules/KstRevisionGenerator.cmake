# ***************************************************************************
# *                                                                         *
# *   Copyright : (C) 2014 Peter Kümmel                                     *
# *   email     : syntheticpp@gmx.net                                       *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# ***************************************************************************

find_program(KST_GIT git)
if(KST_GIT)
  execute_process(COMMAND ${KST_GIT} log -1 --format="%as"
                  WORKING_DIRECTORY "${kst_dir}"
                  OUTPUT_VARIABLE _date
                  ERROR_VARIABLE _error
                  RESULT_VARIABLE _result
                  OUTPUT_STRIP_TRAILING_WHITESPACE)

  # execute_process(COMMAND ${KST_GIT} rev-parse -q --short HEAD
  #                   WORKING_DIRECTORY "${kst_dir}"
  #                   OUTPUT_VARIABLE _revision
  #                   ERROR_VARIABLE _error
  #                   RESULT_VARIABLE _result
  #                   OUTPUT_STRIP_TRAILING_WHITESPACE)
    # write a file with the KST_REVISION define
    file(WRITE ${header_file}.tmp "#define KST_REVISION ${_date}\n")
else()
    file(WRITE ${header_file}.tmp "#define KST_REVISION \"unknown\"\n")
endif()

message(STATUS "Revision: ${_date}")

# copy the file to the final header only if the version changes reduces needless rebuilds
execute_process(COMMAND ${CMAKE_COMMAND}
                -E copy_if_different
                ${header_file}.tmp ${header_file})
