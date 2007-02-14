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

#include "healpix_tools.h"

/* single pixel forward projections */

int healpix_proj_car(double mintheta, double maxtheta, double minphi, double maxphi, double xmax, double ymax, double theta, double phi, double *x, double *y)
{

  if ((mintheta < 0.0) || (mintheta > HEALPIX_PI)) {
    return 0;
  }
  if ((maxtheta < 0.0) || (maxtheta > HEALPIX_PI)) {
    return 0;
  }
  if ((minphi < 0.0) || (minphi > 2 * HEALPIX_PI)) {
    return 0;
  }
  if ((maxphi < 0.0) || (maxphi > 2 * HEALPIX_PI)) {
    return 0;
  }
  if (maxtheta <= mintheta) {
    return 0;
  }
  if (maxphi == minphi) {
    return 0;
  }
  if ((theta < 0.0) || (theta > HEALPIX_PI)) {
    return 0;
  }
  if ((phi < 0.0) || (phi > 2 * HEALPIX_PI)) {
    return 0;
  }

  if ((theta > maxtheta) || (theta < mintheta)) {
    (*x) = HEALPIX_NULL;
    (*y) = HEALPIX_NULL;
    return 0;
  }
  if (maxphi > minphi) {
    if ((phi > maxphi) || (phi < minphi)) {
      (*x) = HEALPIX_NULL;
      (*y) = HEALPIX_NULL;
      return 0;
    }
  } else {
    if ((phi > maxphi) && (phi < minphi)) {
      (*x) = HEALPIX_NULL;
      (*y) = HEALPIX_NULL;
      return 0;
    }
  }

  (*y) = ((maxtheta - theta) / (maxtheta - mintheta)) * ymax;

  if (maxphi > minphi) {
    (*x) = ((phi - minphi) / (maxphi - minphi)) * xmax;
  } else {
    if (phi <= maxphi) {
      (*x) =
          ((phi + (2.0 * HEALPIX_PI - minphi)) / (maxphi +
          (2.0 * HEALPIX_PI -
          minphi))) * xmax;
    } else {
      (*x) = ((phi - minphi) / (maxphi + (2.0 * HEALPIX_PI - minphi))) * xmax;
    }
  }
  return 0;
}

int healpix_proj_sin(double mintheta, double maxtheta, double minphi, double maxphi, double xmax, double ymax, double theta, double phi, double *x, double *y)
{

  double centphi;
  double centtheta;
  double halfx;
  double tempx;

  if ((mintheta < 0.0) || (mintheta > HEALPIX_PI)) {
    return 0;
  }
  if ((maxtheta < 0.0) || (maxtheta > HEALPIX_PI)) {
    return 0;
  }
  if ((minphi < 0.0) || (minphi > 2 * HEALPIX_PI)) {
    return 0;
  }
  if ((maxphi < 0.0) || (maxphi > 2 * HEALPIX_PI)) {
    return 0;
  }
  if (maxtheta <= mintheta) {
    return 0;
  }
  if (maxphi == minphi) {
    return 0;
  }
  if ((theta < 0.0) || (theta > HEALPIX_PI)) {
    return 0;
  }
  if ((phi < 0.0) || (phi > 2 * HEALPIX_PI)) {
    return 0;
  }

  (*y) = ((maxtheta - theta) / (maxtheta - mintheta)) * ymax;
  if (((*y) > ymax) || ((*y) < 0.0)) {
    (*x) = HEALPIX_NULL;
    (*y) = HEALPIX_NULL;
    return 0;
  }

  /* find half width of viewing area */

  centtheta = (maxtheta + mintheta) / 2.0;

  if (maxphi > minphi) {
    centphi = (maxphi + minphi) / 2.0;
    halfx = ((maxphi - minphi) / 2.0) * cos(-(centtheta - HEALPIX_PI / 2.0));
  } else {
    centphi = minphi + (minphi + (2.0 * HEALPIX_PI - maxphi)) / 2.0;
    if (minphi >= 2.0 * HEALPIX_PI) {
      centphi -= 2.0 * HEALPIX_PI;
    }
    halfx =
        ((minphi + (2.0 * HEALPIX_PI - maxphi)) / 2.0) * cos(-(centtheta -
        HEALPIX_PI / 2.0));
  }

  /* find x offset from center */

  if (maxphi > minphi) {
    tempx = phi - centphi;
  } else {
    if (centphi > maxphi) {
      if (phi <= centphi) {
        tempx = phi - centphi;
      } else {
        if (phi < maxphi) {
          tempx = phi + (2.0 * HEALPIX_PI - centphi);
        } else {
          tempx = phi - centphi;
        }
      }
    } else {
      if (phi < centphi) {
        if (phi > minphi) {
          tempx = -(centphi + 2.0 * HEALPIX_PI - phi);
        } else {
          tempx = phi - centphi;
        }
      } else {
        tempx = phi - centphi;
      }
    }
  }
  tempx *= cos(-(theta - HEALPIX_PI / 2.0));

  if ((tempx > halfx) || (tempx < -halfx)) {
    (*x) = HEALPIX_NULL;
    (*y) = HEALPIX_NULL;
    return 0;
  }

  (*x) = (xmax / 2.0) * ((tempx / halfx) + 1.0);

  return 0;
}

/* single pixel reverse projections */

int healpix_proj_rev_car(double mintheta, double maxtheta, double minphi, double maxphi, double xmax, double ymax, double x, double y, double *theta, double *phi)
{

  if ((mintheta < 0.0) || (mintheta > HEALPIX_PI)) {
    return 0;
  }
  if ((maxtheta < 0.0) || (maxtheta > HEALPIX_PI)) {
    return 0;
  }
  if ((minphi < 0.0) || (minphi > 2 * HEALPIX_PI)) {
    return 0;
  }
  if ((maxphi < 0.0) || (maxphi > 2 * HEALPIX_PI)) {
    return 0;
  }
  if (maxtheta <= mintheta) {
    return 0;
  }
  if (maxphi == minphi) {
    return 0;
  }
  
  if ((y > ymax) || (y < 0.0) || (x > xmax) || (x < 0.0)) {
    (*theta) = HEALPIX_NULL;
    (*phi) = HEALPIX_NULL;
    return 0;
  }

  (*theta) = maxtheta - (y / ymax) * (maxtheta - mintheta);

  if (maxphi > minphi) {
    (*phi) = minphi + (x / xmax) * (maxphi - minphi);
  } else {
    (*phi) = minphi + (x / xmax) * (maxphi + (2.0 * HEALPIX_PI - minphi));
    if ((*phi) >= 2.0 * HEALPIX_PI) {
      (*phi) -= 2.0 * HEALPIX_PI;
    }
  }
  return 0;
}

int healpix_proj_rev_sin(double mintheta, double maxtheta, double minphi, double maxphi, double xmax, double ymax, double x, double y, double *theta, double *phi)
{

  double centphi;
  double centtheta;
  double halfx;
  double ph, th;

  if ((mintheta < 0.0) || (mintheta > HEALPIX_PI)) {
    return 0;
  }
  if ((maxtheta < 0.0) || (maxtheta > HEALPIX_PI)) {
    return 0;
  }
  if ((minphi < 0.0) || (minphi > 2 * HEALPIX_PI)) {
    return 0;
  }
  if ((maxphi < 0.0) || (maxphi > 2 * HEALPIX_PI)) {
    return 0;
  }
  if (maxtheta <= mintheta) {
    return 0;
  }
  if (maxphi == minphi) {
    return 0;
  }
  
  if ((y > ymax) || (y < 0.0) || (x > xmax) || (x < 0.0)) {
    (*theta) = HEALPIX_NULL;
    (*phi) = HEALPIX_NULL;
    return 0;
  }

  th = maxtheta - (y / ymax) * (maxtheta - mintheta);

  if ((th == 0.0) || (th == HEALPIX_PI)) { /*at a pole */
    if (x != (xmax / 2.0)) {
      (*theta) = HEALPIX_NULL;
      (*phi) = HEALPIX_NULL;
      return 0;
    } else {
      (*theta) = th;
      if (maxphi > minphi) {
        (*phi) = (maxphi + minphi) / 2.0;
      } else {
        ph = minphi + (maxphi + (2.0 * HEALPIX_PI - minphi)) / 2.0;
        if (ph >= 2.0 * HEALPIX_PI) {
          ph -= 2.0 * HEALPIX_PI;
        }
        (*phi) = ph;
      }
      return 0;
    }
  }

  /* find half width of viewing area */

  centtheta = (maxtheta + mintheta) / 2.0;

  if (maxphi > minphi) {
    centphi = (maxphi + minphi) / 2.0;
    halfx = ((maxphi - minphi) / 2.0) * cos(-(centtheta - HEALPIX_PI / 2.0));
  } else {
    centphi = minphi + (maxphi + (2.0 * HEALPIX_PI - minphi)) / 2.0;
    if (centphi >= 2.0 * HEALPIX_PI) {
      centphi -= 2.0 * HEALPIX_PI;
    }
    halfx =
        ((maxphi + (2.0 * HEALPIX_PI - minphi)) / 2.0) * cos(-(centtheta -
        HEALPIX_PI / 2.0));
  }

  /* find phi offset from center */

  ph = ((2.0 * x / xmax - 1.0) * halfx) / (cos(-(th - HEALPIX_PI / 2.0)));

  if ((ph > HEALPIX_PI) || (ph < -HEALPIX_PI)) {
    (*theta) = HEALPIX_NULL;
    (*phi) = HEALPIX_NULL;
    return 0;
  }

  ph = ph + centphi;
  while (ph >= 2.0 * HEALPIX_PI) {
    ph -= 2.0 * HEALPIX_PI;
  }
  while (ph < 0.0) {
    ph += 2.0 * HEALPIX_PI;
  }

  (*theta) = th;
  (*phi) = ph;

  return 0;
}

