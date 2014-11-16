#ifndef KILLER_H
#define KILLER_H
#include "numpl.h"

#if defined(__cplusplus)
extern "C" {
#endif

    int kill_single(numpl_array * array);
    int kill_hidden_single(numpl_array * array);
    int kill_locked_candidate(numpl_array * array);
    int kill_tuple(numpl_array * array);

#if defined(__cplusplus)
}
#endif
#endif
