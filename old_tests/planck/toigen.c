/* No Copyright
 * Written By Staikos Computing Services Inc
 * Released to the public domain without warranty.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <HL2_PIOLIB/PIOLib.h>

#include <math.h>

int main(int argc, char **argv) {
int VECSZ = 10240;
PIOFLOAT vec[VECSZ];
register int i = 0;
char *toi, *roi, *obj;
PIOLONG rc1;
PIOErr rc2;

	if (argc < 5) {
		fprintf(stderr, "usage: %s <filename> <toi group> <roi group> <object>\n", argv[0]);
		return -1;
	}

	for (; i < VECSZ; ++i) {
		vec[i] = sin(((float)i) / 2000.0);
	}

	toi = (char *)malloc(strlen(argv[1]) + strlen(argv[2]) + 50);
	sprintf(toi, "group:%s:%s", argv[1], argv[2]);

	roi = (char *)malloc(strlen(argv[1]) + strlen(argv[3]) + 50);
	sprintf(roi, "group:%s:%s", argv[1], argv[3]);

	printf("Writing TOI group:\n[%s]\n[%s]\n", toi, roi);
	if (0 > (rc1 = PIOCreateTOIGrp(toi, roi, 0))) {
		fprintf(stderr, "Error writing TOI group. rc=%ld\n", rc1);
		free(roi);
		free(toi);
		return -2;
	}

	obj = (char *)malloc(strlen(argv[1]) + strlen(argv[2]) + strlen(argv[4]) + 50);
	sprintf(obj, "object:%s:%s:%s", argv[1], argv[2], argv[4]);

	if (VECSZ != (rc2 = PIOWriteTOIObject(vec, obj, "PIOFLOAT", "begin=42;end=10281", NULL))) {
		fprintf(stderr, "Error writing object. rc=%ld [%s]\n", rc2, PIOErrMess(rc2));
		free(obj);
		free(roi);
		free(toi);
		return -3;
	}

	free(obj);
	free(roi);
	free(toi);

return 0;
}

