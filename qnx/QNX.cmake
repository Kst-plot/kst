#INCLUDE(CMakeForceCompiler)

SET(CMAKE_SYSTEM_NAME QNX)

SET(CMAKE_SYSTEM_PROCESSOR ntoarm)

# specify the cross compiler

#CMAKE_FORCE_C_COMPILER(qcc GNU)
#CMAKE_FORCE_CXX_COMPILER(QCC GNU)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH /hddhome/joshua/bbndk-2.1.0-beta1/target/qnx6/armle-/ /usr/local/Trolltech)

#SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(QNX TRUE)

SET(CMAKE_MAKE_PROGRAM "make${HOST_EXECUTABLE_SUFFIX}"    CACHE PATH "QNX Make Program")
SET(CMAKE_SH           "sh${HOST_EXECUTABLE_SUFFIX}"      CACHE PATH "QNX shell Program")
SET(CMAKE_AR           "ntoarm-ar${HOST_EXECUTABLE_SUFFIX}"      CACHE PATH "QNX ar Program")
SET(CMAKE_RANLIB       "ntoarm-ranlib${HOST_EXECUTABLE_SUFFIX}"      CACHE PATH "QNX ranlib Program")
SET(CMAKE_NM           "ntoarm-nm${HOST_EXECUTABLE_SUFFIX}"      CACHE PATH "QNX nm Program")
SET(CMAKE_OBJCOPY      "ntoarm-objcopy${HOST_EXECUTABLE_SUFFIX}" CACHE PATH "QNX objcopy Program")
SET(CMAKE_OBJDUMP      "ntoarm-objdump${HOST_EXECUTABLE_SUFFIX}" CACHE PATH "QNX objdump Program")
SET(CMAKE_LINKER       "ntoarm-ld"     CACHE PATH "QNX Linker Program")
SET(CMAKE_STRIP        "ntoarm-strip${HOST_EXECUTABLE_SUFFIX}"   CACHE PATH "QNX Strip Program")

SET(CMAKE_C_COMPILER ntoarmv7-gcc)
SET(CMAKE_C_FLAGS_DEBUG "-fPIC -fstack-protector -fstack-protector-all -Wno-psabi -Wl,--export-dynamic -Wl,-E")
SET(CMAKE_C_FLAGS_MINSIZEREL "-fPIC -fstack-protector -fstack-protector-all -Wno-psabi -Wl,--export-dynamic -Wl,-E")
SET(CMAKE_C_FLAGS_RELEASE "-fPIC -fstack-protector -fstack-protector-all -Wno-psabi -Wl,--export-dynamic -Wl,-E")
SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-fPIC -fstack-protector -fstack-protector-all -Wno-psabi -Wl,--export-dynamic -Wl,-E")

SET(CMAKE_CXX_COMPILER ntoarmv7-g++)
SET(CMAKE_C_FLAGS "-fPIC -fstack-protector -fstack-protector-all -Wno-psabi -Wl,--export-dynamic -Wl,-E")
SET(CMAKE_CXX_FLAGS "-lbps -fPIC -fstack-protector -fstack-protector-all -Wno-psabi -Wl,--export-dynamic -Wl,-E")

SET(CMAKE_CXX_FLAGS_DEBUG "-lbps -fPIC -fstack-protector -fstack-protector-all -Wno-psabi -Wl,--export-dynamic -Wl,-E")
SET(CMAKE_CXX_FLAGS_MINSIZEREL "-lbps -fPIC -fstack-protector -fstack-protector-all -Wno-psabi -lang-c++ -Wl,--export-dynamic -Wl,-E")
SET(CMAKE_CXX_FLAGS_RELEASE "-lbps -fPIC -fstack-protector -fstack-protector-all -Wno-psabi -Wl,--export-dynamic -Wl,-E")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-lbps -fPIC -fstack-protector -fstack-protector-all -Wno-psabi -lang-c++ -Wl,--export-dynamic -Wl,-E")
