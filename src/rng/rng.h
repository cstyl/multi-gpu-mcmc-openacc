#ifndef __RNG_H__
#define __RNG_H__
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"

int setup_rng(rng *rng);
int destroy_rng(rng *rng);

#endif // __RNG_H__
