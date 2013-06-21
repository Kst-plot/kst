/***************************************************************************
                  healpix_tools.h  -  tools for healpix datasource
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

#ifndef HEALPIX_TOOLS_H
#define HEALPIX_TOOLS_H

#include <math.h>
//#include <libcfitsio0/fitsio.h>
#include <fitsio.h>
#include <datasource.h>

#  ifdef HEALPIX_PI
#    undef HEALPIX_PI
#  endif                        /* define pi... */
#  define HEALPIX_PI M_PI

#  ifdef HEALPIX_INVPI
#    undef HEALPIX_INVPI
#  endif                        /* define 1/pi... */
#  define HEALPIX_INVPI M_1_PI

#  ifdef HEALPIX_PISQ
#    undef HEALPIX_PISQ
#  endif                        /* define pi squared... */
#  define HEALPIX_PISQ 9.86960440108936

#  ifdef HEALPIX_HALFPI
#    undef HEALPIX_HALFPI
#  endif                        /* define half pi... */
#  define HEALPIX_HALFPI M_PI_2

#  ifdef HEALPIX_NSIDE_MAX
#    undef HEALPIX_NSIDE_MAX
#  endif                        /* max nside value */
#  define HEALPIX_NSIDE_MAX 8192

#  ifdef HEALPIX_STRNL
#    undef HEALPIX_STRNL
#  endif                        /* max string length */
#  define HEALPIX_STRNL 200

#  ifdef HEALPIX_MIN_EXP
#    undef HEALPIX_MIN_EXP
#  endif                        /* smallest IEEE double exponent */
#  define HEALPIX_MIN_EXP -1022

#  ifdef HEALPIX_MAX_EXP
#    undef HEALPIX_MAX_EXP
#  endif                        /* smallest IEEE double exponent */
#  define HEALPIX_MAX_EXP 1023

/* map parameters */

#  ifdef HEALPIX_RING
#    undef HEALPIX_RING
#  endif                        /* ordering = ring */
#  define HEALPIX_RING 0

#  ifdef HEALPIX_NEST
#    undef HEALPIX_NEST
#  endif                        /* ordering = nested */
#  define HEALPIX_NEST 1

#  ifdef HEALPIX_COORD_C
#    undef HEALPIX_COORD_C
#  endif                        /* coordsys = celestial/equatorial */
#  define HEALPIX_COORD_C 0

#  ifdef HEALPIX_COORD_G
#    undef HEALPIX_COORD_G
#  endif                        /* coordsys = galactic */
#  define HEALPIX_COORD_G 1

#  ifdef HEALPIX_COORD_E
#    undef HEALPIX_COORD_E
#  endif                        /* coordsys = ecliptic */
#  define HEALPIX_COORD_E 2

#  ifdef HEALPIX_COORD_O
#    undef HEALPIX_COORD_O
#  endif                        /* coordsys = other */
#  define HEALPIX_COORD_O 3

/* projection parameters */

#  ifdef HEALPIX_PROJ_CAR
#    undef HEALPIX_PROJ_CAR
#  endif                        /* projection = cartesian */
#  define HEALPIX_PROJ_CAR 0

#  ifdef HEALPIX_PROJ_SIN
#    undef HEALPIX_PROJ_SIN
#  endif                        /* projection = sinusoidal */
#  define HEALPIX_PROJ_SIN 1

/* FITS parameters */

#  ifdef HEALPIX_FITS_FULL
#    undef HEALPIX_FITS_FULL
#  endif                        /* file contains full-sphere map(s) */
#  define HEALPIX_FITS_FULL 0

#  ifdef HEALPIX_FITS_CUT
#    undef HEALPIX_FITS_CUT
#  endif                        /* file contains cut-sphere map(s) */
#  define HEALPIX_FITS_CUT 1

#  ifdef HEALPIX_FITS_MAXCOL
#    undef HEALPIX_FITS_MAXCOL
#  endif                        /* maximum number of table columns supported */
#  define HEALPIX_FITS_MAXCOL 50

#  ifdef HEALPIX_NULL
#    undef HEALPIX_NULL
#  endif                        /* null value for float maps */
#  define HEALPIX_NULL (-1.6375e30)

#  ifdef HEALPIX_EPSILON
#    undef HEALPIX_EPSILON
#  endif                        /* range around HEALPIX_NULL that is considered equal to NULL */
#  define HEALPIX_EPSILON (0.0001e30)

#  ifdef HEALPIX_INT_NULL
#    undef HEALPIX_INT_NULL
#  endif                        /* null value for int maps */
#  define HEALPIX_INT_NULL (-2147483646)

  typedef struct {              /* healpix FITS keys */
    size_t nskeys;
    size_t nikeys;
    size_t nfkeys;
    char **skeynames;
    char **skeyvals;
    char **skeycoms;
    char **ikeynames;
    int *ikeyvals;
    char **ikeycoms;
    char **fkeynames;
    float *fkeyvals;
    char **fkeycoms;
  } healpix_keys;
    
  int healpix_is_dnull(double val);
  int healpix_is_fnull(float val);
  int healpix_is_inull(int val);
  char **healpix_strarr_alloc(size_t nstring);
  int healpix_strarr_free(char **array, size_t nstring);
  int healpix_nsidecheck(size_t nside);
  size_t healpix_nside2npix(size_t nside);
  size_t healpix_npix2nside(size_t npix);
  size_t healpix_nside2factor(size_t nside);
  size_t healpix_factor2nside(size_t factor);
  
  int healpix_xy2pix(size_t x, size_t y, size_t *pix);
  int healpix_x2pix(size_t x, size_t *pix);
  int healpix_y2pix(size_t y, size_t *pix);
  int healpix_pix2xy(size_t pix, size_t *x, size_t *y);
  
  int healpix_nest2xyf (size_t nside, size_t pix, size_t *x, 
                     size_t *y, size_t *face_num);
  int healpix_xyf2nest (size_t nside, size_t x, size_t y, 
                     size_t face_num, size_t *pix);
  int healpix_ring2xyf (size_t nside, size_t pix, size_t *x, 
                     size_t *y, size_t *face_num);
  int healpix_xyf2ring (size_t nside, size_t x, size_t y, 
                     size_t face_num, size_t *pix);
  
  int healpix_nest2ring (size_t nside, size_t pnest, size_t *pring);
  int healpix_ring2nest(size_t nside, size_t pring, size_t *pnest);
  int healpix_ang2pix_ring(size_t nside, double theta, double phi, 
                        size_t *pix);
  int healpix_ang2pix_nest(size_t nside, double theta, double phi, 
                        size_t *pix);
  int healpix_pix2ang_ring(size_t nside, size_t pix, double *theta, 
                        double *phi);
  int healpix_pix2ang_nest(size_t nside, size_t pix, double *theta, 
                        double *phi);
  int healpix_degrade_nest(size_t oldnside, size_t oldpix, size_t newnside,
                        size_t * newpix);
  int healpix_degrade_ring(size_t oldnside, size_t oldpix, size_t newnside,
                        size_t * newpix);
  int healpix_neighbors(size_t nside, int ordering, size_t pixel, long *parray);
    
  int healpix_proj_car(double mintheta, double maxtheta, double minphi,
               double maxphi, double xmax, double ymax, double theta,
               double phi, double *x, double *y);
  int healpix_proj_sin(double mintheta, double maxtheta, double minphi,
               double maxphi, double xmax, double ymax, double theta,
               double phi, double *x, double *y);
  int healpix_proj_rev_car(double mintheta, double maxtheta, double minphi,
                   double maxphi, double xmax, double ymax, double x,
                   double y, double *theta, double *phi);
  int healpix_proj_rev_sin(double mintheta, double maxtheta, double minphi,
                   double maxphi, double xmax, double ymax, double x,
                   double y, double *theta, double *phi);
  
  double healpix_loc_dist(size_t nside, int order, size_t pix1, 
                          size_t pix2);
  
  /* FITS keys operations */

  healpix_keys *healpix_keys_alloc();
  int healpix_keys_free(healpix_keys * keys);
  int healpix_keys_clear(healpix_keys * keys);
  int healpix_keys_sadd(healpix_keys * keys, char *keyname, char *keyval,
                     char *keycom);
  int healpix_keys_iadd(healpix_keys * keys, char *keyname, int keyval,
                     char *keycom);
  int healpix_keys_fadd(healpix_keys * keys, char *keyname, float keyval,
                     char *keycom);
  int healpix_keys_read(healpix_keys * keys, fitsfile * fp, int *ret);
  
  /* file info */
  
  int healpix_fits_map_test(char *filename, size_t * nside, int *order, 
                         int *coord, int *type, size_t * nmaps);
  
  int healpix_fits_map_info(char *filename, size_t * nside, int *order,
                         int *coord, int *type, size_t * nmaps, char *creator, char *extname, char **names, char **units, healpix_keys *keys);
   

#endif
// vim: ts=2 sw=2 et
