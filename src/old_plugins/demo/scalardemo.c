#include <stdlib.h>
#include <math.h>


int scalardemo(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[])
{
	int j;

	if (outArrayLens[0] != inArrayLens[0]) {
		outArrays[0] = realloc(outArrays[0],
					inArrayLens[0]*sizeof(double));
		outArrayLens[0] = inArrayLens[0];
	}

	for (j = 0; j < inArrayLens[0]; j++) {
		outArrays[0][j] = inScalars[0]*inArrays[0][j];
	}

	outScalars[0] = inScalars[0] + 1;

	return 0;
}

