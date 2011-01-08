

kst_init_plugin(src/plugins)



kst_add_plugin(dataobject bin)
kst_add_plugin(dataobject chop)
kst_add_plugin(dataobject linefit)
kst_add_plugin(dataobject lockin)
kst_add_plugin(dataobject phase)
kst_add_plugin(dataobject shift)
kst_add_plugin(dataobject statistics)
kst_add_plugin(dataobject syncbin)
kst_add_plugin(dataobject crossspectrum)
kst_add_plugin(dataobject differentiation)
kst_add_plugin(dataobject effectivebandwidth)
kst_add_plugin(dataobject genericfilter)

kst_add_plugin(filters cumulativesum)
kst_add_plugin(filters despike)
kst_add_plugin(filters differentiation)

if(gsl)
	include_directories(${GSL_INCLUDE_DIR})
	macro(kst_add_gsl_plugin dir name)
		kst_add_plugin(${dir} ${name})
		kst_link(${GSL_LIBRARIES})
	endmacro()
	
	kst_add_gsl_plugin(dataobject/convolution    convolve)
	kst_add_gsl_plugin(dataobject/convolution    deconvolve)
	kst_add_gsl_plugin(dataobject/correlation    autocorrelation)
	kst_add_gsl_plugin(dataobject/correlation    crosscorrelation)
	kst_add_gsl_plugin(dataobject/interpolations akima)
	kst_add_gsl_plugin(dataobject/interpolations akimaperiodic)
	kst_add_gsl_plugin(dataobject/interpolations cspline)
	kst_add_gsl_plugin(dataobject/interpolations csplineperiodic)
	kst_add_gsl_plugin(dataobject/interpolations linear)
	kst_add_gsl_plugin(dataobject/interpolations polynomial)
	kst_add_gsl_plugin(dataobject                noiseaddition)
	kst_add_gsl_plugin(dataobject                periodogram)

	kst_add_gsl_plugin(filters butterworth_bandpass)
	kst_add_gsl_plugin(filters butterworth_bandstop)
	kst_add_gsl_plugin(filters butterworth_highpass)
	kst_add_gsl_plugin(filters butterworth_lowpass)
	
	kst_add_gsl_plugin(fits exponential_unweighted)
	kst_add_gsl_plugin(fits exponential_weighted)
	kst_add_gsl_plugin(fits gaussian_unweighted)
	kst_add_gsl_plugin(fits gaussian_weighted)
	kst_add_gsl_plugin(fits gradient_unweighted)
	kst_add_gsl_plugin(fits gradient_weighted)
	kst_add_gsl_plugin(fits kneefrequency)
	kst_add_gsl_plugin(fits linear_unweighted)
	kst_add_gsl_plugin(fits linear_weighted)
	kst_add_gsl_plugin(fits lorentzian_unweighted)
	kst_add_gsl_plugin(fits lorentzian_weighted)
	kst_add_gsl_plugin(fits polynomial_unweighted)
	kst_add_gsl_plugin(fits polynomial_weighted)
	kst_add_gsl_plugin(fits sinusoid_unweighted)
	kst_add_gsl_plugin(fits sinusoid_weighted)
endif()


message(STATUS)



