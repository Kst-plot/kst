/******************************************************************************
 *  polynom.h : polynom class. Implements the following operations:           *
 *              - the sum, the difference and the product between polynoms ;  *
 *              - the multiplication and the division by a scalar ;           *
 *              - the derivative and the integrate.                           *
 *                                                                            *
 *  Copyright 2005, Jean-Paul Petillon                                        *
 *  Released under the terms of the GPL V2, or (at your option) any later     * 
 *  version                                                                   *
 *                                                                            *
 ******************************************************************************/
#ifndef POLYNOM_H
#define POLYNOM_H

template<class S> class polynom {
public:
  polynom(int n);                       // constructor
  polynom(const polynom& P);            // copy constructor
 ~polynom();                            // destructor
  S& operator[] (int i);                // ith coefficient acces (read/write)
  S operator() (S x);                   // value at x
  polynom& operator=(const polynom& P); // set a polynom equal to another one
  polynom  operator+(const polynom& P); // sum of 2 polynoms
  polynom  operator-(const polynom& P); // difference between 2 polynoms
  polynom  operator*(const polynom& P); // product of 2 polynoms
  polynom  operator*(const S& x);       // product by a scalar
  polynom  operator/(const S& x);       // division by a scalar
  polynom  Derivative();                // derivative
  polynom  Integrate();                 // integrate
  int GetDegree();
  void SetDegree(int n);
private:
  S* C;                                 // coefficients
  S NulC;                               // null coefficient (for i>n)
  int n;                                // degree of polynom (table size is n+1)
};

// constructor
template<class S> polynom<S>::polynom(int n)
{
  C = new S[n+1];
  for (int i=0; i<=n; i++) C[i]=0.0;
  NulC = 0.0;
  this->n = n;
};
//------------------------------------------------------------------------------
// copy constructor
template<class S> polynom<S>::polynom(const polynom& P) {
  n = P.n;
  C = new S[n+1];
  for (int i=0; i<=n; i++) C[i]=P.C[i];
}
//------------------------------------------------------------------------------
// destructor
template<class S> polynom<S>::~polynom()
{
  delete[] C;
};
//------------------------------------------------------------------------------
// coefficient acces (read or write)
template<class S> S& polynom<S>::operator[] (int i)
{
  return i<=n ? C[i] : NulC;
};
//------------------------------------------------------------------------------
// value at x
template<class S> S polynom<S>::operator() (S x)
{
  S r=0.0;
  S xpi=1.0;  // x at the power i
  for (int i=0; i<=n; i++) {
    r+=C[i]*xpi;
    xpi*=x;
  }
  return r;
};
//------------------------------------------------------------------------------
// set a polynom equal to another one
template<class S> polynom<S>& polynom<S>::operator=(const polynom& P)
{
  if (n!=P.n) { // reallocate memory if different size
    delete[] C;
    n = P.n;
    C = new S[n+1];
    this->n = n;
  }
  for (int i=0; i<=n; i++) C[i]=P.C[i];
  return *this;
};
//------------------------------------------------------------------------------
// polynom sum
template<class S> polynom<S> polynom<S>::operator+(const polynom& P)
{
  polynom r(n>P.n?n:P.n);
  for (int i=0; i<=  n; i++) r.C[i]+=  C[i];
  for (int i=0; i<=P.n; i++) r.C[i]+=P.C[i];
  return r;
};
//------------------------------------------------------------------------------
// polynom difference
template<class S> polynom<S> polynom<S>::operator-(const polynom& P)
{
  polynom r(n>P.n?n:P.n);
  for (int i=0; i<=  n; i++) r.C[i]+=  C[i];
  for (int i=0; i<=P.n; i++) r.C[i]-=P.C[i];
  return r;
};
//------------------------------------------------------------------------------
// polynom product
template<class S> polynom<S> polynom<S>::operator*(const polynom<S>& P)
{
  polynom r(n+P.n);
  for (int i=0; i<=r.n; i++) {
    r.C[i]=0;
    for (int j=(0>i-n?0:i-n); j<=(i<P.n?i:P.n); j++) r.C[i]+=C[i-j]*P.C[j];
  }
  return r;
};
//------------------------------------------------------------------------------
// scalar multipication
template<class S> polynom<S> polynom<S>::operator*(const S& x)
{
  polynom r(n);
  for (int i=0; i<=n; i++) r.C[i]=C[i]*x;
  return r;
};
//------------------------------------------------------------------------------
// scalar division
template<class S> polynom<S> polynom<S>::operator/(const S& x)
{
  polynom<S> r(n);
  for (int i=0; i<=n; i++) r.C[i]=C[i]/x;
  return r;
};
//------------------------------------------------------------------------------
// derivative
template<class S> polynom<S> polynom<S>::Derivative()
{
  polynom<S> r(n-1);
  for (int i=0; i<=r.n; i++) r.C[i]=(i+1)*C[i+1];
  return r;
};
//------------------------------------------------------------------------------
// integrate
template<class S> polynom<S> polynom<S>::Integrate()
{
  polynom<S> r(n+1);
  r.C[0]=0;
  for (int i=1; i<=r.n; i++) r.C[i]=C[i-1]/i;
  return r;
};
//------------------------------------------------------------------------------
template<class S> int polynom<S>::GetDegree()
{
  return n;
}
//------------------------------------------------------------------------------
template<class S> void polynom<S>::SetDegree(int n)
{
  if (this->n != n) {
    delete[] C;
    C = new double[n+1];
    this->n = n;
    for (int i=0; i<=n; i++) C[i]=0.0;
  }
}

#endif // ifndef POLYNOM_H
