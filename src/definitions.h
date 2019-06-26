#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#ifdef FLOAT
  typedef float precision;
#else
  typedef double precision;
#endif

enum RNG_TYPES{
  MT19937 = 0,
  RANLXS2 = 1,
  RANLXD2 = 2,
  RANLUXS389 = 3,
  CMRG = 4,
  MRG = 5,
  TAUS =  6
};

// default rng type
#ifndef RNG_TYPE
#define RNG_TYPE TAUS
#endif

#endif // __DEFINITIONS_H__
