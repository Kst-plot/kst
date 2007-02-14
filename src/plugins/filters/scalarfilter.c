#include <stdlib.h>
#include <math.h>

int scalarfilter(const double *const inArray, int inArrayLen,
		const double inScalars[],
		double *outArray[], int *outArrayLen)
{
	int j;

	if (*outArrayLen != inArrayLen) {
		*outArray = realloc(*outArray, inArrayLen*sizeof(double));
		*outArrayLen = inArrayLen;
	}

	for (j = 0; j < inArrayLen; j++) {
		(*outArray)[j] = inScalars[0]*inArray[j];
	}

	return 0;
}
