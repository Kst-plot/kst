# Define if you have cfitsio - not used on unix
HAVE_CFITSIO = 0
# Set to 1 to disable usage of cfitsio - only for unix
DISABLE_CFITSIO = 0

#Define if you have dirfile getdata library version 0.4.2 or greater required. - not used on unix
HAVE_DIRFILE = 0
# Set to 1 to disable usage of dirfile - only for unix
DISABLE_DIRFILE = 0

#Define if you have the gsl library installed - not used on unix
HAVE_GSL = 0
# Set to 1 to disable usage of gsl - only for unix
DISABLE_GSL = 0

win32:CONFIG += debug_and_release

defineTest(LibExists) {
  lib = $$ARGS

unix { 
  !mac {
  contains(lib, gsl) {
    contains(DISABLE_GSL, 1) {
      # Overridden, do not check.
      return(false);
    }
  }
  contains(lib, cfitsio) {
    contains(DISABLE_CFITSIO, 1) {
      # Overridden, do not check.
      return(false);
    }
  }
  contains(lib, getdata) {
    contains(DISABLE_DIRFILE, 1) {
      # Overridden, do not check.
      return(false);
    }
  }
  PKGCONFIGRESULT = $$system(pkg-config --libs $$lib)
  !contains(PKGCONFIGRESULT, -l$$lib) {
    message(Could not find $$lib disabling associated features)
    return(false)
  }
  message(Found $$lib at $$PKGCONFIGRESULT)
  return(true)
} }

contains(lib, gsl) {
  contains(HAVE_GSL, 1) {
    return(true)
  }
  return(false)
}
contains(lib, cfitsio) {
  contains(HAVE_CFITSIO, 1) {
    return(true)
  }
  return(false)
}
contains(lib, getdata) {
  contains(HAVE_DIRFILE, 1) {
    return(true)
  }
  return(false)
}
}
