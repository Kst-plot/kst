
#include <math.h>

#define SMALL_NON_ZERO 0.00000000001

template<class T>
inline
bool doubleEqual(const T& a, const T& b) {
  return a == b || fabs(a - b) < SMALL_NON_ZERO;
}

template<class T>
inline
bool doubleLessThan(const T& a, const T& b) {
  return b - a > SMALL_NON_ZERO;
}

template<class T>
inline
bool doubleLessThanEqual(const T& a, const T& b) {
  return doubleEqual(a, b) || doubleLessThan(a, b);
}

template<class T>
inline
bool doubleGreaterThan(const T& a, const T& b) {
  return a - b > SMALL_NON_ZERO;
}

template<class T>
inline
bool doubleGreaterThanEqual(const T& a, const T& b) {
  return doubleEqual(a, b) || doubleGreaterThan(a, b);
}
