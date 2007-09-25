lotsofstuff.kst is a Kst session which instantiates many of the data objects available in Kst.

Some of the features used:
	DataSource w/ metadata - netCDF, HealPIX
	Vector
	Matrix
	Curve
	Image
	Label
	Equation
	Event
	Power Spectrum
	Cumulative Spectral Decay
	Histogram
	Filters
	Fits

It depends on two data sources (which have relative paths):
./GOTEX.C130_N130AR.LRT.RF06.PNI.nc	(this must be un-gzipped before loading the .kst file)
../healpix/healpix_example_sm.fits
