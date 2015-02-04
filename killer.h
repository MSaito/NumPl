#ifndef KILLER_H
#define KILLER_H
#include "numpl.h"
#include "solve.h"
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

    int kill_single(numpl_array * array);
    int kill_hidden_single(numpl_array * array);
    int kill_locked_candidate(numpl_array * array);
    int kill_tuple(numpl_array * array);
    int kill_fish(numpl_array * array);
    int kill_xywing(numpl_array * array);

    int64_t analyze_single(numpl_array * array, solve_info * info);
    int64_t analyze_hidden_single(numpl_array * array, solve_info * info);
    int64_t analyze_locked_candidate(numpl_array * array, solve_info * info);
    int64_t analyze_tuple(numpl_array * array, solve_info * info);
    int64_t analyze_fish(numpl_array * array, solve_info * info);
    int64_t analyze_xywing(numpl_array * array, solve_info * info);

#if defined(__cplusplus)
}
#endif
#endif
