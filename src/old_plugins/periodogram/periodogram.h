/***************************************************************************
                   periodogram.h
                             -------------------
    begin                : 12/07/06
    copyright            : (C) 2006 The University of Toronto
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef PERIODOGRAM_H
#define PERIODOGRAM_H

#include <kstbasicplugin.h>

class Periodogram : public KstBasicPlugin {
  Q_OBJECT
  public:
    Periodogram(QObject *parent, const char *name, const QStringList &args);
    virtual ~Periodogram();

    virtual bool algorithm();

    virtual QStringList inputVectorList() const;
    virtual QStringList inputScalarList() const;
    virtual QStringList inputStringList() const;
    virtual QStringList outputVectorList() const;
    virtual QStringList outputScalarList() const;
    virtual QStringList outputStringList() const;

  private:
    int max(int a, int b);
    int min(int a, int b);
    void spread(double y, double yy[], unsigned long n, double x, int m);
    void four1(double data[], unsigned long nn, int isign);
    void realft(double data[], unsigned long n, int isign);
    void avevar(double const  data[], unsigned long n, double* ave, double* var);
    void FastLombPeriodogram(
      double const x[],
      double const y[],
      unsigned long   n,
      double          ofac,
      double          hifac,
      double          wk1[],
      double          wk2[],
      unsigned long   ndim,
      unsigned long*  nout,
      unsigned long*  jmax,
      double*         prob,
      double*         pvar,
      int            iIsWindowFunction);
    void SlowLombPeriodogram(
      double const    x[],
      double const    y[],
      unsigned long   n,
      double          ofac,
      double          hifac,
      double          px[],
      double          py[],
      unsigned long   ndim,
      unsigned long*  nout,
      unsigned long*  jmax,
      double*         prob,
      double*         pvar,
      int            iIsWindowFunction);
};

#endif
