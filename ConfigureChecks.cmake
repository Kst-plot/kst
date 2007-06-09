#FIXME Need to do kst specific configure checks here... port configure.in.in

check_function_exists(unsetenv HAVE_UNSETENV)

# Check for the CFITSIO library

find_package(CFITSIO)
if (CFITSIO_FOUND)
  set(HAVE_CFITSIO TRUE)
endif (CFITSIO_FOUND)

