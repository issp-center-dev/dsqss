#if defined(MEASURE_BOSON) && !defined(MEASURE_SPIN) && !defined(MEASURE_USER_SPECIFIC)
#include "SPECIFIC/Boson/measure_specific.h"
#elif !defined(MEASURE_BOSON) && defined(MEASURE_SPIN) && !defined(MEASURE_USER_SPECIFIC)
#include "SPECIFIC/Heisenberg/measure_specific.h"
#elif !defined(MEASURE_BOSON) && !defined(MEASURE_SPIN) && defined(MEASURE_USER_SPECIFIC)
#include "SPECIFIC/User/measure_specific.h"
#else
#error You should define ONE of the following tokens: MEASURE_BOSON, MEASURE_SPIN, or MEASURE_USER_SPECIFIC.
#endif
