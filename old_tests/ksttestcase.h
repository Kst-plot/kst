/*
 *  Copyright 2004, The University of Toronto
 *  Licensed under GPL.
 */

#ifndef KSTTESTCASE_H
#define KSTTESTCASE_H
#include <assert.h>
#include <kapplication.h>
#include <kstdatacollection.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef NAN
double NOPOINT = NAN;
#else
double NOPOINT = 0.0/0.0; // NaN
#endif

#ifndef INF
double INF = 1.0/0.0;
#endif


#define KstTestSuccess 0
#define KstTestFailure -1
#define KstTestFailed() rc--

#endif

// vim: ts=2 sw=2 et
