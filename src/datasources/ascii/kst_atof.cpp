// patched BSD V7 code
// http://www.bsdlover.cn/study/UnixTree/V7/usr/src/libc/gen/atof.c.html

#include <math.h>
#include <ctype.h>

#define LOGHUGE 39


double kst_atof(const char* p, const char sep)
{
	int c;
	double fl, flexp, exp5;
	double big = 72057594037927936.;  /*2^56*/
	int nd;
	int eexp, exp, neg, negexp, bexp;

	neg = 1;
	while((c = *p++) == ' ')
		;
	if (c == '-')
		neg = -1;
	else if (c=='+')
		;
	else
		--p;

	exp = 0;
	fl = 0;
	nd = 0;
	while ((c = *p++), isdigit(c)) {
		if (fl<big)
			fl = 10*fl + (c-'0');
		else
			exp++;
		nd++;
	}

	if (c == sep) {
		while ((c = *p++), isdigit(c)) {
			if (fl<big) {
				fl = 10*fl + (c-'0');
				exp--;
			}
		nd++;
		}
	}

	negexp = 1;
	eexp = 0;
	if ((c == 'E') || (c == 'e')) {
		if ((c= *p++) == '+')
			;
		else if (c=='-')
			negexp = -1;
		else
			--p;

		while ((c = *p++), isdigit(c)) {
			eexp = 10*eexp+(c-'0');
		}
		if (negexp<0)
			eexp = -eexp;
		exp = exp + eexp;
	}

	negexp = 1;
	if (exp<0) {
		negexp = -1;
		exp = -exp;
	}


	if((nd+exp*negexp) < -LOGHUGE){
		fl = 0;
		exp = 0;
	}
	flexp = 1;
	exp5 = 5;
	bexp = exp;
	for (;;) {
		if (exp&01)
			flexp *= exp5;
		exp >>= 1;
		if (exp==0)
			break;
		exp5 *= exp5;
	}
	if (negexp<0)
		fl /= flexp;
	else
		fl *= flexp;
	fl = ldexp(fl, negexp*bexp);
	if (neg<0)
		fl = -fl;
	return(fl);
}

// vim: ts=2 sw=2 et
