/******************************************************************************
 *  filter.h : Digital Filter. Specified by its "s" transfer function.        *
 *             Synthesis of "z" transfert function is done by bilinear        *
 *             transformation :             2   z-1                           *
 *                                    p -> --- -----                          *
 *                                          T   z+1                           *
 *  Copyright 2005, Jean-Paul Petillon                                        *
 *  Released under the terms of the GPL.                                      *
 ******************************************************************************/
#ifndef FILTER_H
#define FILTER_H

#include "polynom.h"

template<class S> class filter {
public:
  S out;                                // output
  filter(                              // constructor
    // "s" transfert function
    polynom<S>& Ns,  // numerator
    polynom<S>& Ds,  // denominator
    double dT        // sampling period (s)
  );
  void Reset();                         // reset
  void ConnectTo(                       // connector
    S& in            // input address
  );
  void NextTimeStep();                  // update
 ~filter();                            // destructor
private:
  // pointer on input
  S* in;
  // order
  int n;
  // "z" transfert function
  polynom<S> Nz;   // numerator
  polynom<S> Dz;   // denominator
  // state vector
  S* x;
};

template<class S> filter<S>::filter(polynom<S>& Ns, polynom<S>& Ds, double dT)
:Nz(0), Dz(0)
{
  // the greatest degree between N & D
  n=Ns.GetDegree()>Ds.GetDegree() ? Ns.GetDegree() : Ds.GetDegree();
  // allocate memory for state vector
  x = new S[n];
  // reset it
  Reset();
  // as well as the output
  out=0.0;
  // computes "z" transfer function
  polynom<S> potzm1odt(0); potzm1odt[0]=1.0;                    // [2(z-1)/dT]^i
  polynom<S> dzm1odt(1); dzm1odt[1]=2.0/dT; dzm1odt[0]=-2.0/dT; // 2(z-1)/dT
  for (int i=0; i<=n; i++) {
    // (z+1)^(n-i) = 1
    polynom<S> pozp1(0); pozp1[0]=1.0;
    // z+1
    polynom<S> zp1(1); zp1[1]=1.0; zp1[0]=1.0;
    // (z+1)^(n-i)
    for (int j=i+1; j<=n; j++) pozp1 = pozp1 * zp1;
    polynom<S> dNz(0); dNz[0]=Ns[i];
    dNz = dNz * pozp1 * potzm1odt;
    Nz = Nz + dNz;
    polynom<S> dDz(0); dDz[0]=Ds[i];
    dDz = dDz * pozp1 * potzm1odt;
    Dz = Dz + dDz;
    potzm1odt = potzm1odt * dzm1odt;
  }
}
//------------------------------------------------------------------------------
template<class S> void filter<S>::Reset()
// reset state vector
{
  for (int i=0; i<n; i++) x[i]=0.0;
}
//------------------------------------------------------------------------------
template<class S> void filter<S>::ConnectTo(S& in)
{
  // memorizes input address
  this->in = &in;
}
//------------------------------------------------------------------------------
template<class S> void filter<S>::NextTimeStep()
{
  // Compute output
  out = (x[n-1] + *in * Nz[n])/ Dz[n];
  // update state vector x[0] .. x[n-1] (goes from output to input)
  for (int i=n-1; i>0; i--) {
    x[i] = x[i-1] + Nz[i]*(*in) - Dz[i]*out;
  }
  x[0] = Nz[0]*(*in) - Dz[0]*out;
}
//------------------------------------------------------------------------------
template<class S> filter<S>::~filter()
{
  // deallocate memory used by state vector
  delete[] x;
}
//------------------------------------------------------------------------------


#endif // #ifndef FILTER_H
