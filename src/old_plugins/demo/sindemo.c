#include <stdlib.h>
#include <math.h>


int sindemo(const double *const inArrays[], const int inArrayLens[],
		const double inScalars[],
		double *outArrays[], int outArrayLens[],
		double outScalars[])
{
	int i;
	int j;

	for (i = 0; i < 2; i++) {
		if (outArrayLens[i] != inArrayLens[i]) {
			outArrays[i] = realloc(outArrays[i],
						inArrayLens[i]*sizeof(double));
			outArrayLens[i] = inArrayLens[i];
		}

		for (j = 0; j < inArrayLens[i]; j++) {
			outArrays[i][j] = sin(inArrays[i][j]);
		}
	}

	return 0;
}

