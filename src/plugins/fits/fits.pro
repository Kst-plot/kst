include($$PWD/../../../config.pri)

TEMPLATE = subdirs
CONFIG += ordered

LibExists(gsl) {
  message(GSL configured.  Fits plugins will be built.)
  SUBDIRS += \
      exponential_unweighted \
      exponential_weighted \
      gaussian_unweighted \
      gaussian_weighted \
      gradient_weighted \
      gradient_unweighted \
      kneefrequency \
      linear_weighted \
      linear_unweighted \
      lorentzian_unweighted \
      lorentzian_weighted \
      polynomial_unweighted \
      polynomial_weighted \
      sinusoid_unweighted \
      sinusoid_weighted
    CONFIG += link_pkgconfig
    PKGCONFIG += gsl
}
