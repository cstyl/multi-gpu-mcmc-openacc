#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#ifdef _FLOAT_
  typedef float precision;
#else
  typedef double precision;
#endif

#ifdef _FLOAT_
  #define PRECISION_TOLERANCE  1.0e-07
#else
  #define PRECISION_TOLERANCE 1.0e-14
#endif

#endif // __DEFINITIONS_H__
