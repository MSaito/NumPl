#ifndef GENERATE_H
#define GENERATE_H
#include "numpl.h"

#if defined(__cplusplus)
extern "C" {
#endif

    typedef struct {
	unsigned hidden_single : 1;
	unsigned locked_candidate : 1;
	unsigned tuple : 1;
	unsigned fish : 1;
	unsigned xy : 1;
	unsigned symmetric : 1;
    } generate_type;
    int generate(numpl_array * array, generate_type * type);

#if defined(__cplusplus)
}
#endif

#endif // GENERATE_H
