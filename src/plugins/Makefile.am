if include_noise_plugins
NOISE_SUBDIR=noise_addition
endif
if include_fits_plugins
FITS_SUBDIR=fits
endif
if include_fitsnonlinear_plugins
FITSNONLINEAR_SUBDIR=fits_nonlinear
endif
if include_interpolations_plugins
INTERPOLATIONS_SUBDIR=interpolations
endif
if include_pass_filters_plugins
PASS_FILTERS_SUBDIR=pass_filters
endif
if include_convolution_plugins
CONVOLUTION_SUBDIR=convolution
endif
if include_correlation_plugins
CORRELATION_SUBDIR=correlation
endif

SUBDIRS=bin linefit periodogram phase statistics chop crossspectrum syncbin effective_bandwidth despike $(NOISE_SUBDIR) $(FITS_SUBDIR) $(FITSNONLINEAR_SUBDIR) $(INTERPOLATIONS_SUBDIR) $(PASS_FILTERS_SUBDIR) $(CORRELATION_SUBDIR) $(CONVOLUTION_SUBDIR) cumulative_sum differentiation shift discretizing_filters

