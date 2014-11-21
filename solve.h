#ifndef SOLVE_H
#define SOLVE_H

#if defined(__cplusplus)
extern "C" {
#endif

    typedef struct {
	int ks_count;
	int kh_count;
	int kt_count;
	int kl_count;
	int sf_count;
	int fx_count;
	int max_tuple;
	int max_hidden;
	int max_fish;
	int solved;
    } solve_info;

    int solve(numpl_array * array, solve_info * info);
    int is_solved(const numpl_array * array);

#if defined(__cplusplus)
}
#endif

#endif // SOLVE_H
