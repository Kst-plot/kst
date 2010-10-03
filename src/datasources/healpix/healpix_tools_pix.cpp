/***************************************************************************
                  healpix_tools.cpp  -  tools for healpix datasource
                             -------------------
    begin                : Wed June 01 2005
    copyright            : (C) 2005 Ted Kisner
    email                : tskisner.public@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
/***************************************************************************
 * Some of these pixel tools are based on code from Healpix_cxx 2.00.      *
 * See http://sourceforge.net/projects/healpix/ for the original code      *
 *                                                                         *
 * Healpix 2.00 was developed by by E. Hivon, M. Reinecke, W. O'Mullane,   *
 * H.K. Eriksen, K.M. Gorski, A.J. Banday                                  *
 ***************************************************************************/
 

#include "healpix_tools.h"
#include <qmutex.h>

#define HEALPIX_CHK if(!healpix_doneinit)healpix_init()

static size_t healpix_ctab[0x100];
static size_t healpix_utab[0x100];

static const size_t healpix_jrll[] = {2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4};
static const size_t healpix_jpll[] = {1, 3, 5, 7, 0, 2, 4, 6, 1, 3, 5, 7};

static int healpix_doneinit = 0;
void healpix_init();

/* NULL checking */

int healpix_is_dnull(double val)
{
  if ((val > HEALPIX_NULL - HEALPIX_EPSILON) && (val < HEALPIX_NULL + HEALPIX_EPSILON)) {
    return 1;
  } else {
    return 0;
  }
}

int healpix_is_fnull(float val)
{
  if ((val > HEALPIX_NULL - HEALPIX_EPSILON) && (val < HEALPIX_NULL + HEALPIX_EPSILON)) {
    return 1;
  } else {
    return 0;
  }
}

int healpix_is_inull(int val)
{
  if (val == HEALPIX_INT_NULL) {
    return 1;
  } else {
    return 0;
  }
}

/* simple tools */

char **healpix_strarr_alloc(size_t nstring)
{
  size_t i,j;
  char **array = NULL;
  if (nstring == 0) {
    return NULL;
  }
  array = (char **)calloc(nstring, sizeof(char *));
  if (!array) {
    return NULL;
  }
  for (i = 0; i < nstring; i++) {
    array[i] = NULL;
    array[i] = (char *)calloc(HEALPIX_STRNL, sizeof(char));
    if (!array[i]) {
      for (j = 0; j < i; j++) {
        free(array[j]);
      }
      free(array);
      return NULL;
    }
  }
  return array;
}

int healpix_strarr_free(char **array, size_t nstring)
{
  size_t i;
  if ((array == NULL)||(nstring == 0)) {
    return 0;
  }
  for (i = 0; i < nstring; i++) {
    free(array[i]);
  }
  free(array);
  return 0;
}

int healpix_nsidecheck(size_t nside)
{
  if (nside > HEALPIX_NSIDE_MAX) {
    return 1;
  }
  if (nside == 0) {
    return 1;
  }
  if (((~nside)&(nside-1)) != (nside-1)) {  
    return 1;
  }
  return 0;
}

size_t healpix_npix2nside(size_t npix)
{
  double fpix;
  int err;
  fpix = ((double)npix) / 12.0;
  fpix = sqrt(fpix);
  err = healpix_nsidecheck((size_t) fpix);
  if (err) {
    return 0;
  }
  return (size_t) fpix;
}

size_t healpix_nside2npix(size_t nside)
{
  int err;
  err = healpix_nsidecheck(nside);
  if (err) {
    return 0;
  }
  return 12 * nside * nside;
}

size_t healpix_nside2factor(size_t nside)
{
  int err;
  size_t factor = 0;
  err = healpix_nsidecheck(nside);
  if (err) {
    return 0;
  }
  while (nside != (1ul<<factor)) {
    factor++;
  }
  return factor;
}

size_t healpix_factor2nside(size_t factor)
{
  size_t nside = 1 << factor;
  if (nside > HEALPIX_NSIDE_MAX) {
    return 0;
  }
  return nside;
}

/* initialization */

void healpix_init() {
  size_t m;
  QMutex tablock;        
  tablock.lock();
  for (m = 0; m < 0x100; m++) {
  healpix_ctab[m] = (m&0x1) | ((m&0x2) << 7) | ((m&0x4) >> 1) | ((m&0x8) << 6) | ((m&0x10) >> 2) | ((m&0x20) << 5) | ((m&0x40) >> 3) | ((m&0x80) << 4);
  healpix_utab[m] = (m&0x1) | ((m&0x2) << 1) | ((m&0x4) << 2) | ((m&0x8) << 3) | ((m&0x10) << 4) | ((m&0x20) << 5) | ((m&0x40) << 6) | ((m&0x80) << 7);
  }
  healpix_doneinit = 1;
  tablock.unlock();
  return;
}

/* Definitions for 32bit pixel values.  Note that  */
/* HEALPIX_NSIDE_MAX must be <= 2^14 in order to have */
/* 12*nside^2 be < 2^32.                           */

int healpix_xy2pix(size_t x, size_t y, size_t *pix) {
  HEALPIX_CHK;
  (*pix) = healpix_utab[x&0xff] | (healpix_utab[x>>8]<<16) | (healpix_utab[y&0xff]<<1) | (healpix_utab[y>>8]<<17);
  return 0;
}

int healpix_x2pix(size_t x, size_t *pix) {
  HEALPIX_CHK;
  (*pix) = healpix_utab[x&0xff] | (healpix_utab[x>>8]<<16);
  return 0;
}

int healpix_y2pix(size_t y, size_t *pix) {
  HEALPIX_CHK;
  (*pix) = (healpix_utab[y&0xff]<<1) | (healpix_utab[y>>8]<<17);
  return 0;
}

int healpix_pix2xy(size_t pix, size_t *x, size_t *y) {
  size_t raw;
  HEALPIX_CHK;
  raw = (pix&0x5555) | ((pix&0x55550000)>>15);
  (*x) = healpix_ctab[raw&0xff] | (healpix_ctab[raw>>8]<<4);
  raw = ((pix&0xaaaa)>>1) | ((pix&0xaaaa0000)>>16);
  (*y) = healpix_ctab[raw&0xff] | (healpix_ctab[raw>>8]<<4);
  return 0;
}

/* low level pixel routines */

int healpix_nest2xyf (size_t nside, size_t pix, size_t *x, size_t *y, size_t *face_num) {
  int err;
  size_t factor = healpix_nside2factor(nside);
  (*face_num) = pix >> (2*factor);
  err = healpix_pix2xy(pix & (nside*nside-1), x, y);
  return err;
}

int healpix_xyf2nest (size_t nside, size_t x, size_t y, size_t face_num, size_t *pix) {
  int err;
  size_t factor = healpix_nside2factor(nside);
  err = healpix_xy2pix(x, y, pix);
  (*pix) += (face_num<<(2*factor));
  return err;
}

int healpix_ring2xyf (size_t nside, size_t pix, size_t *x, size_t *y, size_t *face_num) {
  long iring, iphi, kshift, nr;
  long nl2 = 2 * nside;
  size_t ncap = 2 * (nside*nside - nside);
  long tmp, ip, ire, irm, ifm, ifp, irt, ipt;
  long factor = (long int)healpix_nside2factor(nside);
  long face;

  if (pix < ncap) {
    iring = (long)(0.5*(1.0+sqrt((double)(1+2*pix))));
    iphi  = (long)(pix+1) - 2*iring*(iring-1);
    kshift = 0;
    nr = iring;
    face = 0;
    tmp = iphi-1;
    if (tmp >= (2*iring)) {
      face = 2;
      tmp -= 2*iring;
    }
    if (tmp >= iring) {
      face++;
    }
  } else if (pix < (12*nside*nside - ncap)) {
    ip = (long)(pix - ncap);
    iring = (ip>>(factor+2)) + (long)nside;
    iphi = (ip&(4*(long)nside-1)) + 1;
    kshift = (iring+(long)nside)&1;
    nr = (long)nside;
    ire = iring - (long)nside + 1;
    irm = nl2+2-ire;
    ifm = (iphi - ire/2 + (long)nside - 1) >> factor;
    ifp = (iphi - irm/2 + (long)nside - 1) >> factor;
    if (ifp == ifm) /* faces 4 to 7 */
      face = (ifp==4) ? 4 : ifp+4;
    else if (ifp < ifm) /* (half-)faces 0 to 3 */
      face = ifp;
    else /* (half-)faces 8 to 11 */
      face = ifm + 8;
  } else {
    ip = (long)(12*nside*nside - pix);
    iring = (long)(0.5*(1.0+sqrt((double)(2*ip-1))));
    iphi  = 4*iring + 1 - (ip - 2*iring*(iring-1));
    kshift = 0;
    nr = iring;
    iring = 2*nl2-iring;
    face = 8;
    tmp = iphi-1;
    if (tmp >= (2*nr)) {
      face = 10;
      tmp -= 2*nr;
    }
    if (tmp >= nr) face++;
  }

  irt = iring - (long)(healpix_jrll[face]*nside) + 1;
  ipt = 2*iphi - (long)healpix_jpll[face]*nr - kshift -1;
  if (ipt >= nl2) ipt -= 8*nside;

  (*face_num) = face;
  (*x) = (size_t)((ipt-irt) >> 1);
  (*y) = (size_t)((-(ipt+irt)) >> 1);
  
  return 0;
}

int healpix_xyf2ring (size_t nside, size_t x, size_t y, size_t face_num, size_t *pix) {
  long nl4 = 4*(long)nside;
  long jr = (long)healpix_jrll[face_num];
  long nr, kshift, n_before;
  long ncap = 2 * (long)(nside*nside - nside);
  long jp;
  
  jr = (jr*(long)nside) - (long)x - (long)y - 1;
  if (jr < (long)nside) {
    nr = jr;
    n_before = 2 * nr * (nr-1);
    kshift = 0;
  } else if (jr > (long)(3*nside)) {
    nr = nl4 - jr;
    n_before = (long)(12*nside*nside) - 2*(nr+1)*nr;
    kshift = 0;
  } else {
    nr = (long)nside;
    n_before = ncap + (jr - (long)nside)*nl4;
    kshift = (jr - (long)nside)&1;
  }

  jp = (long)((long)((long)healpix_jpll[face_num]*nr + (long)x - (long)y + 1 + kshift) / 2);
  if (jp > nl4) {
    jp -= nl4;
  } else {
    if (jp < 1) jp += nl4;
  }
  (*pix) = (size_t)(n_before + jp - 1);
  
  return 0;
}

/* slightly higher-level functions */

int healpix_nest2ring (size_t nside, size_t pnest, size_t *pring) {
  int err;
  size_t x, y, face_num;

  err = healpix_nsidecheck(nside);
  if (err) {
    return err;
  }
  if (pnest > (12 * nside * nside - 1)) {
    return 1;
  }
  err = healpix_nest2xyf(nside, pnest, &x, &y, &face_num);
  
  if (err) {
    return err;
  }
  err = healpix_xyf2ring(nside, x, y, face_num, pring);
  fflush(stdout);
  return err;
}

int healpix_ring2nest(size_t nside, size_t pring, size_t *pnest) {
  int err;
  size_t x, y, face_num;
  err = healpix_nsidecheck(nside);
  if (err) {
    return err;
  }
  if (pring > (12 * nside * nside - 1)) {
    return 1;
  }
  err = healpix_ring2xyf(nside, pring, &x, &y, &face_num);
  fflush(stdout);
  if (err) {
    return err;
  }
  err = healpix_xyf2nest(nside, x, y, face_num, pnest);
  fflush(stdout);
  return err;
}

// In normal healpix operations, we want to convert the angle
// to a pixel value *at the maximum resolution*, and then degrade
// this pixel number to the desired NSIDE.  This ensures that
// round-off error is consistent between different resolutions.
// This is the method used in the HPIC library (for example).
// For the purposes of display, we don't really care that much
// about round-off error- we want speed.  These functions do a
// straight conversion to the desired NSIDE in order to eliminate
// the need to do many degrade operations.

int healpix_ang2pix_ring(size_t nside, double theta, double phi, size_t *pix) {
  double z = cos(theta);
  double za = fabs(z);
  double tt;
  double intpart;
  long longpart;
  double temp1, temp2;
  long jp, jm;
  long ip, ir, kshift;
  size_t ncap = 2 * (nside*nside - nside);
  double tp, tmp;
  while (phi < 0.0) {
    phi += 2.0 * HEALPIX_PI;
  }
  intpart = floor(phi / (2.0*HEALPIX_PI));
  phi -= intpart;
  tt = 2.0 * phi / HEALPIX_PI;
  if (za <= (2.0/3.0)) {
    temp1 = (double)nside * (0.5+tt);
    temp2 = (double)nside * z * 0.75;
    jp = (long)(temp1 - temp2); /* index of  ascending edge line */
    jm = (long)(temp1 + temp2); /* index of descending edge line */
    /* ring number counted from z=2/3 */
    ir = nside + 1 + jp - jm; /* in {1,2n+1} */
    kshift = 1 - (ir&1); /* kshift=1 if ir even, 0 otherwise */

    ip = (long)((long)(jp + jm - nside + kshift + 1) / 2); /* in {0,4n-1} */
    ip = ip % (long)(4*nside);

    (*pix) = ncap + (size_t)((ir-1) * 4 * nside + ip);
  } else {
    tp = tt - floor(tt);
    tmp = (double)(nside) * sqrt(3.0 * (1.0 - za));
    jp = (long)(tp * tmp); /* increasing edge line index */
    jm = (long)((1.0-tp)*tmp); /* decreasing edge line index */
    ir = jp + jm + 1; /* ring number counted from the closest pole */
    ip = (long)(tt * (double)ir); /* in {0,4*ir-1} */
    longpart = (long)(ip / (4*ir));
    ip -= longpart;
 
    if (z > 0.0) {
      (*pix) = (size_t)(2 * ir * (ir-1) + ip);
    } else {
      (*pix) = (size_t)((long)(12*nside*nside) - 2*ir*(ir+1) + ip);
    }
  }
  return 0;
}

int healpix_ang2pix_nest(size_t nside, double theta, double phi, size_t *pix) {
  double z = cos(theta);
  double za = fabs(z);
  double tt;
  double intpart;
  double temp1, temp2;
  long jp, jm;
  long face, x, y;
  long ifp, ifm, ntt;
  size_t sipf;
  size_t factor;
  int err;
  double tp, tmp;
  
  while (phi < 0.0) {
    phi += 2.0 * HEALPIX_PI;
  }
  intpart = floor(phi / (2.0*HEALPIX_PI));
  phi -= intpart;
  tt = 2.0 * phi / HEALPIX_PI;
  
  factor = healpix_nside2factor(nside);
  if (za <= (2.0/3.0)) {
    temp1 = (double)nside * (0.5+tt);
    temp2 = (double)nside * z * 0.75;
    jp = (long)(temp1-temp2); /* index of  ascending edge line */
    jm = (long)(temp1+temp2); /* index of descending edge line */
    ifp = jp >> factor;  /* in {0,4} */
    ifm = jm >> factor;
    if (ifp == ifm) {          /* faces 4 to 7 */
      face = (ifp==4) ? (long)4 : ifp+4;
    } else if (ifp < ifm) {    /* (half-)faces 0 to 3 */
      face = ifp;
    } else {                   /* (half-)faces 8 to 11 */
      face = ifm + 8;
    }
    x = jm & (long)(nside-1);
    y = (long)nside - (jp & (long)(nside-1)) - 1;
  } else {
    ntt = (long)tt;
    tp = tt - (double)ntt;
    tmp = (double)nside * sqrt(3.0*(1.0-za));
    jp = (long)(tp*tmp); /* increasing edge line index */
    jm = (long)((1.0-tp)*tmp); /* decreasing edge line index */
    if (jp >= (long)nside) {
      jp = (long)nside-1; /* for points too close to the boundary */
    }
    if (jm >= (long)nside) {
      jm = (long)nside-1;
    }
    if (z >= 0) {
      face = ntt;  /* in {0,3} */
      x = nside - jm - 1;
      y = nside - jp - 1;
    } else {
      face = ntt + 8; /* in {8,11} */
      x = jp;
      y = jm;
    }
  }

  err = healpix_xy2pix((size_t)x, (size_t)y, &sipf);
  if (err) {
    return err;
  }

  (*pix) = sipf + (size_t)(face << (2*factor));
  
  return err;
}

int healpix_pix2ang_ring(size_t nside, size_t pix, double *theta, double *phi){
  size_t ncap = 2 * (nside*nside - nside);
  long iring, iphi;
  long ip, nl2;
  double fodd;
  
  if (pix < ncap) {
    iring = (long)(0.5*(1.0+sqrt(1.0+2.0*(double)pix)));
    iphi  = (long)(pix+1) - 2 * iring * (iring-1);
    (*theta) = acos(1.0 - (double)(iring*iring) / (double)(3*nside*nside));
    (*phi) = ((double)iphi - 0.5) * HEALPIX_PI / (2.0 * (double)iring);
  } else if (pix < (12*nside*nside-ncap)) {
    ip  = (long)pix - (long)ncap;
    iring = (long)(ip / (4*(long)nside)) + (long)nside; /* counted from North pole */
    iphi  = ip % (4*(long)nside) + 1; /* 1 if iring+nside is odd, 1/2 otherwise */
    fodd = ((iring+(long)nside)&1) ? 1.0 : 0.5;
    nl2 = 2 * (long)nside;
    
    (*theta) = acos((double)(nl2-iring) * 2.0 / (double)(3*nside));
    (*phi) = ((double)iphi - fodd) * HEALPIX_PI / (double)nl2;
  } else {
    ip = (long)(12*nside*nside - pix);
    iring = (long)(0.5*(1.0+sqrt((double)(2*ip-1))));
    iphi = 4*iring + 1 - (ip - 2*iring*(iring-1));

    (*theta) = acos(-1.0 + (double)(iring*iring) / (double)(3*nside*nside));
    (*phi) = ((double)iphi - 0.5) * HEALPIX_PI / (double)(2*iring);
  }
  return 0;
}

int healpix_pix2ang_nest(size_t nside, size_t pix, double *theta, double *phi){
  size_t factor = healpix_nside2factor(nside);
  long nl4 = (long)(nside*4);
  long face = (long)pix >> (2*factor);
  long ipf = (long)pix & (long)(nside*nside-1);
  long x, y;
  size_t sx, sy;
  long jr, nr, kshift, jp;
  double z;
  int err;
  
  err = healpix_pix2xy((size_t)ipf, &sx, &sy);
  if (err) {
    return err;
  }
  x = (long)sx;
  y = (long)sy;

  jr = (long)(healpix_jrll[face]<<factor) - x - y - 1;
  if (jr < (long)nside) {
    nr = jr;
    z = 1.0 - (double)(nr * nr) / (double)(3*nside*nside);
    kshift = 0;
  } else if (jr > (long)(3*nside)) {
    nr = nl4 - jr;
    z = (double)(nr*nr) / (double)(3*nside*nside) - 1.0;
    kshift = 0;
  } else {
    nr = (long)nside;
    z = (double)((long)(2*nside)-jr) * 2.0 / (double)(3*nside);
    kshift = (jr-(long)nside)&1;
  }

  jp = (long)((long)((long)healpix_jpll[face]*nr + x - y + 1 + kshift) / 2);
  if (jp > nl4) jp -= nl4;
  if (jp < 1) jp += nl4;

  (*theta) = acos(z);
  (*phi) = ((double)jp - (double)(kshift+1)*0.5) * HEALPIX_HALFPI / (double)nr;
  
  return 0;
}

/* degrade */

int healpix_degrade_nest(size_t oldnside, size_t oldpix, size_t newnside, size_t * newpix)
{
  size_t oldfactor;
  size_t newfactor;
  size_t face;
  int err;

  err = healpix_nsidecheck(oldnside);
  if (err) {
    return err;
  }
  err = healpix_nsidecheck(newnside);
  if (err) {
    return err;
  }
  if (oldnside < newnside) {
    return 1;
  }
  oldfactor = healpix_nside2factor(oldnside);
  newfactor = healpix_nside2factor(newnside);
  face = oldpix >> 2*oldfactor;
  
  (*newpix) = ((oldpix & (oldnside*oldnside-1)) >> (2*(oldfactor-newfactor))) + (face << 2*newfactor);
  
  return 0;
}

int healpix_degrade_ring(size_t oldnside, size_t oldpix, size_t newnside, size_t * newpix)
{
  size_t oldnest;
  size_t newnest;
  int err;

  err = healpix_ring2nest(oldnside, oldpix, &oldnest);
  if (err) {
    return err;
  }
  err = healpix_degrade_nest(oldnside, oldnest, newnside, &newnest);
  if (err) {
    return err;
  }
  err = healpix_nest2ring(newnside, newnest, newpix);
  if (err) {
    return err;
  }

  return 0;
}

/* unit sphere vectors */

int healpix_vec2ang(double xcomp, double ycomp, double zcomp, double *theta, double *phi)
{
  double sint;
  (*theta) = acos(zcomp);
  sint = sin((*theta));
  if (ycomp >= 0.0) {
    (*phi) = acos(xcomp / sint);
  } else {
    (*phi) = acos(xcomp / sint) + HEALPIX_PI;
  }
  return 0;
}

int healpix_ang2vec(double theta, double phi, double *xcomp, double *ycomp, double *zcomp)
{
  (*xcomp) = sin(theta) * cos(phi);
  (*ycomp) = sin(theta) * sin(phi);
  (*zcomp) = cos(theta);
  return 0;
}

int healpix_pix2vec_ring(size_t nside, size_t pix, double *xcomp, double *ycomp, double *zcomp)
{
  double theta;
  double phi;
  int err;
  err = healpix_pix2ang_ring(nside, pix, &theta, &phi);
  if (err) {
    return err;
  }
  err = healpix_ang2vec(theta, phi, xcomp, ycomp, zcomp);
  if (err) {
    return err;
  }
  return 0;
}

int healpix_pix2vec_nest(size_t nside, size_t pix, double *xcomp, double *ycomp, double *zcomp)
{
  double theta;
  double phi;
  int err;
  err = healpix_pix2ang_nest(nside, pix, &theta, &phi);
  if (err) {
    return err;
  }
  err = healpix_ang2vec(theta, phi, xcomp, ycomp, zcomp);
  if (err) {
    return err;
  }
  return 0;
}

int healpix_vec2pix_ring(size_t nside, double xcomp, double ycomp, double zcomp, size_t * pix)
{
  double theta;
  double phi;
  int err;
  err = healpix_vec2ang(xcomp, ycomp, zcomp, &theta, &phi);
  if (err) {
    return err;
  }
  err = healpix_ang2pix_ring(nside, theta, phi, pix);
  if (err) {
    return err;
  }
  return 0;
}

int healpix_vec2pix_nest(size_t nside, double xcomp, double ycomp, double zcomp, size_t * pix)
{
  double theta;
  double phi;
  int err;
  err = healpix_vec2ang(xcomp, ycomp, zcomp, &theta, &phi);
  if (err) {
    return err;
  }
  err = healpix_ang2pix_nest(nside, theta, phi, pix);
  if (err) {
    return err;
  }
  return 0;
}

/* nearest neighbors */

int healpix_neighbors(size_t nside, int ordering, size_t pixel, long *parray) {
  
  int err;
  size_t i;
  size_t ptemp;
  static const int xoffset[] = { -1, 1, 0, 0,-1,-1, 1, 1 };
  static const int yoffset[] = {  0, 0,-1, 1,-1, 1, 1,-1 };
  static const int facearray[][12] =
  { {  8, 9,10,11,-1,-1,-1,-1,10,11, 8, 9 },   /* S */
  {  5, 6, 7, 4, 8, 9,10,11, 9,10,11, 8 },   /* SE */
  { -1,-1,-1,-1, 5, 6, 7, 4,-1,-1,-1,-1 },   /* E */
  {  4, 5, 6, 7,11, 8, 9,10,11, 8, 9,10 },   /* SW */
  {  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11 },   /* center */
  {  1, 2, 3, 0, 0, 1, 2, 3, 5, 6, 7, 4 },   /* NE */
  { -1,-1,-1,-1, 7, 4, 5, 6,-1,-1,-1,-1 },   /* W */
  {  3, 0, 1, 2, 3, 0, 1, 2, 4, 5, 6, 7 },   /* NW */
  {  2, 3, 0, 1,-1,-1,-1,-1, 0, 1, 2, 3 } }; /* N */
  static const int swaparray[][12] =
  { {  0,0,0,0,0,0,0,0,3,3,3,3 },   /* S */
  {  0,0,0,0,0,0,0,0,6,6,6,6 },   /* SE */
  {  0,0,0,0,0,0,0,0,0,0,0,0 },   /* E */
  {  0,0,0,0,0,0,0,0,5,5,5,5 },   /* SW */
  {  0,0,0,0,0,0,0,0,0,0,0,0 },   /* center */
  {  5,5,5,5,0,0,0,0,0,0,0,0 },   /* NE */
  {  0,0,0,0,0,0,0,0,0,0,0,0 },   /* W */
  {  6,6,6,6,0,0,0,0,0,0,0,0 },   /* NW */
  {  3,3,3,3,0,0,0,0,0,0,0,0 } }; /* N */
  int x, y, ix, iy, f;
  size_t stx, sty;
  size_t face;
  const size_t nsm1 = nside - 1;
  size_t nbnum;
  int tmp;
  
  if (ordering == HEALPIX_RING) {
    err = healpix_ring2xyf(nside, pixel, &stx, &sty, &face);
  } else {    
    err = healpix_nest2xyf(nside, pixel, &stx, &sty, &face);
  }
  if (err) {
    return err;
  }
  ix = (int)stx;
  iy = (int)sty;
  
  for (i = 0; i < 8; i++) {
    parray[i] = -1l;
  }
    
  if ((ix > 0)&&(ix < (int)nsm1)&&(iy > 0)&&(iy < (int)nsm1)) {
    if (ordering == HEALPIX_RING) {
      for (i = 0; i < 8; i++) {
        err = healpix_xyf2ring(nside, (size_t)(ix + xoffset[i]), (size_t)(iy+yoffset[i]), face, &ptemp);
        parray[i] = ptemp;
      }
    } else {
      for (i = 0; i < 8; i++) {
        err = healpix_xyf2nest(nside, (size_t)(ix + xoffset[i]), (size_t)(iy+yoffset[i]), face, &ptemp);
        parray[i] = ptemp;
      }
    }
  } else {
    for (i = 0; i < 8; i++) {
      x = ix + xoffset[i];
      y = iy + yoffset[i];
      nbnum = 4;
      if (x < 0) { 
        x += (int)nside;
        nbnum -= 1; 
      } else if (x >= (int)nside) { 
        x -= (int)nside;
        nbnum += 1; 
      }
      if (y < 0) {
        y += (int)nside;
        nbnum -= 3; 
      } else if (y >= (int)nside) {
        y -= (int)nside;
        nbnum += 3; 
      }

      f = facearray[nbnum][face];
      if (f >= 0) {
        if (swaparray[nbnum][face]&1) {
          x = (int)nside - x - 1;
        }
        if (swaparray[nbnum][face]&2) {
          y = (int)nside - y - 1;
        }
        if (swaparray[nbnum][face]&4) {
          tmp = x;
          x = y;
          y = tmp;
        }
        if (ordering == HEALPIX_RING) {
          err = healpix_xyf2ring(nside, (size_t)x, (size_t)y, (size_t)f, &ptemp);
        } else {
          err = healpix_xyf2nest(nside, (size_t)x, (size_t)y, (size_t)f, &ptemp);
        }
        parray[i] = ptemp;
      }
    }
  }
  return 0;
}

/* angular distance between two pixels */

double healpix_loc_dist(size_t nside, int order, size_t pix1, size_t pix2)
{
  double x1, y1, z1;
  double x2, y2, z2;
  double alpha;

  if (healpix_nsidecheck(nside)) {
    return 0.0;
  }
  if (pix1 > 12 * nside * nside) {
    return 0.0;
  }
  if (pix2 > 12 * nside * nside) {
    return 0.0;
  }
  if (order == HEALPIX_NEST) {
    healpix_pix2vec_nest(nside, pix1, &x1, &y1, &z1);
    healpix_pix2vec_nest(nside, pix2, &x2, &y2, &z2);
  } else {
    healpix_pix2vec_ring(nside, pix1, &x1, &y1, &z1);
    healpix_pix2vec_ring(nside, pix2, &x2, &y2, &z2);
  }
  alpha = acos((x1 * x2) + (y1 * y2) + (z1 * z2));

  return alpha;
}

