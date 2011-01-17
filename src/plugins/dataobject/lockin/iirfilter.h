/***************************************************************************
 *                                                                         *
 *   copyright : (C) 2010 The University of Toronto                        *
 *                   sbenton@phsyics.utoronto.ca                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <deque>
#include <vector>
#include <complex>

#ifndef _IIRFILTER_H
#define _IIRFILTER_H

using std::deque;
using std::vector;

typedef std::complex<double> complex;

/* 
 * allows for definition of general recursive/IIR filters
 *
 * Probably doesn't need to be a template, but I was experimenting
 * now you can filter a series of vectors! (or rather float/long double/complex)
 *
 */
template<class T> class IIRFilter {
  public:
    IIRFilter(unsigned int order);

    //do fitlering by calling your instance with most recent input
    T operator() (T x);

    //clear the filter history
    void clear();

    //evaluate transfer function at complex point z
    complex transfer(complex z);

    //function generates data stream containing impulse and frequency response
    //clears filter both before and after
    int FilterResponse(const char* out, double max_f=0.5, unsigned int n=100);

  protected:
    //to be used in derived class constructors
    void setCoefficients(vector<double> na, vector<double> nb);

  private:
    deque<T> x0;	//last filter input
    deque<T> y0;	//last filter output
    vector<double> a;	//coefficients to multiply y0
    vector<double> b;	//coefficients to multiply x, x0
    unsigned int order;	//order of the filter
};

/* 
 * A few sample filter types. Analytic forms for a few low-order Bessel filters
 *
 * TODO: it would be great to use numerically designed filters
 *
 */

//1st order bessel low-pass, with knee frequency f (in sample rate units)
template <class T> class BesselLP1 : public IIRFilter<T> {
  public:
    BesselLP1(double f);
};

//1st order bessel high-pass, with knee frequency f (in sample rate units)
template <class T> class BesselHP1 : public IIRFilter<T> {
  public:
    BesselHP1(double f);
};

//4th order bessel low-pass, with knee frequency f (in sample rate units)
template <class T> class BesselLP4 : public IIRFilter<T> {
  public:
    BesselLP4(double f);
};


/*******************************************************************************
 * IMPLEMENTATION
 * g++ doesn't support 'export' for template implementations in another file
 * so, all the implementations are here
 ******************************************************************************/

#include <iostream>
#include <fstream>
#include <numeric>
#include <cmath>
#include "iirfilter.h"

using std::cerr;
using std::endl;

template<class T> IIRFilter<T>::IIRFilter(unsigned int order)
{
  this->order = order;
  x0.resize(order, 0.0);
  y0.resize(order, 0.0);
  a.resize(order, 0.0);
  b.resize(order+1, 0.0);  //includes coefficient for new inupt x
}

template<class T> T IIRFilter<T>::operator() (T x)
{
  T y = b[0]*x;
  //cerr << "y = (" << b[0] << ")*x[n]" << endl;
  for (unsigned int i=0; i<order; i++) {
    y += b[i+1] * x0[i];
    y -= a[i]   * y0[i];
    //cerr << "    + (" << b[i+1] << ")*x[n-" << i+1 << "]";
    //cerr << " - (" << a[i] << ")*y[n-" << i+1 << "]" << endl;
  }

  x0.pop_back();
  x0.push_front(x);
  y0.pop_back();
  y0.push_front(y);

  return y;
}

template<class T> void IIRFilter<T>::clear() {
  x0.resize(order, T());
  y0.resize(order, T());
}

template<class T> complex IIRFilter<T>::transfer(complex z)
{
  complex numer = pow(z,4)*b[0];
  complex denom = pow(z,4);
  for (unsigned int i=0; i<order; i++) {
    numer += b[i+1]*pow(z,order-1-i);
    denom += a[i]*pow(z,order-1-i);
  }

  return numer/denom;
}

template<class T> int IIRFilter<T>::FilterResponse(const char* out,  double max_f, unsigned int n)
{
  std::ofstream oo(out);
  if (!oo) {
    cerr << "Couldn't open FilterResponse output file" << endl;
    return 1;
  }
  this->clear();

  oo << "INDEX\timpulse\tfreq\tgain\tphase" << endl;
  for (unsigned int i=0; i<n; i++) {
    oo << i+1 << "\t";				//INDEX
    oo << (*this)((i==0)?1.0:0.0) << "\t";	//impulse response
    //frequency goes up to Nyquist (0.5 in sample frequency units)
    double freq = (double)i*max_f/(double)n;
    oo << freq << "\t";				//frequency (in sample units)
    complex resp = transfer(complex(cos(2*M_PI*freq),sin(2*M_PI*freq)));
    oo << abs(resp) << "\t";			//gain
    oo << arg(resp) << "\n";			//phase
  }

  this->clear();
  return 0;
}

template<class T> 
void IIRFilter<T>::setCoefficients(vector<double> na, vector<double> nb)
{
  if (na.size() != order || nb.size() != order+1) {
    cerr << "Invalid size of coefficient vector\n" << endl;
    return;
  }
  a = na;
  b = nb;
}



//The filter forms below were solved analytically using a bilinear transform

//bessel scale factors
//convert normalization from unit delay at f=0 to 3dB attenutation at 1rad/s
const double norm_bessel_O1  = 1.0;
const double norm_bessel_O2  = 1.36165412871613;
const double norm_bessel_O3  = 1.75567236868121;
const double norm_bessel_O4  = 2.11391767490422;
const double norm_bessel_O5  = 2.42741070215263;
const double norm_bessel_O6  = 2.70339506120292;
const double norm_bessel_O7  = 2.95172214703872;
const double norm_bessel_O8  = 3.17961723751065;
const double norm_bessel_O9  = 3.39169313891166;
const double norm_bessel_O10 = 3.59098059456916;

template<class T> BesselLP1<T>::BesselLP1(double f) : IIRFilter<T>(1)
{
  vector<double> a(1), b(2);
  double alpha = M_PI*f/norm_bessel_O1;
  alpha = tan(alpha);   //pre-warp
  a[0] = (alpha-1.0)/(1.0+alpha);
  b[0] = alpha/(1.0+alpha);
  b[1] = alpha/(1.0+alpha);
  this->setCoefficients(a, b);
}

template<class T> BesselHP1<T>::BesselHP1(double f) : IIRFilter<T>(1)
{
  vector<double> a(1), b(2);
  double alpha = M_PI*f/norm_bessel_O1;
  alpha = tan(alpha);   //pre-warp
  a[0] = (alpha-1.0)/(1.0+alpha);
  b[0] = 1.0/(1.0+alpha);
  b[1] = -1.0/(1.0+alpha);
  this->setCoefficients(a, b);
}

template<class T> BesselLP4<T>::BesselLP4(double f) : IIRFilter<T>(4)
{
  vector<double> a(4), b(5);
  double alpha = M_PI*f/norm_bessel_O4;
  alpha = tan(alpha);   //pre-warp
  double denom = (1.0 + 1.0/alpha + 45.0/105.0/pow(alpha,2) 
      + 10.0/105.0/pow(alpha,3) + 1.0/105.0/pow(alpha,4));

  a[0] = (4.0 + 2.0/alpha 
      - 20.0/105.0/pow(alpha,3) - 4.0/105.0/pow(alpha,4))/denom;
  a[1] = (6.0 - 90.0/105.0/pow(alpha,2) + 6.0/105.0/pow(alpha,4))/denom;
  a[2] = (4.0 - 2.0/alpha 
      + 20.0/105.0/pow(alpha,3) - 4.0/105.0/pow(alpha,4))/denom;
  a[3] = (1.0 - 1.0/alpha + 45.0/105.0/pow(alpha,2) 
      - 10.0/105.0/pow(alpha,3) + 1.0/105.0/pow(alpha,4))/denom;

  b[0] = 1.0/denom;
  b[1] = 4.0/denom;
  b[2] = 6.0/denom;
  b[3] = 4.0/denom;
  b[4] = 1.0/denom;

  /*
  cerr << "y = (" << b[0] << ")*x[n]" << endl;
  for (unsigned int i=0; i<4; i++) {
    cerr << "    + (" << b[i+1] << ")*x[n-" << i+1 << "]";
    cerr << " - (" << a[i] << ")*y[n-" << i+1 << "]" << endl;
  }
  */

  this->setCoefficients(a, b);
}

#endif
