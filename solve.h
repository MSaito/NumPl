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
	int naked_tuple[LINE_SIZE / 2 + 1];
	int hidden_tuple[LINE_SIZE / 2 + 1];
	int fish[LINE_SIZE];
	int solved;
    } solve_info;

    typedef struct {
	numpl_array save;
	int saved;
    } recursion_solve_t;

    int solve(numpl_array * array, solve_info * info);
    int analyze(numpl_array * array, solve_info * info);
    int is_solved(const numpl_array * array);
    int simple_solve(numpl_array * array);
    int equal(const numpl_array * a, const numpl_array * b);
    int recursion_solve(numpl_array * array);
    void print_solve_info(solve_info * info, int verbose);
    void output_detail(numpl_array * array);
    void output(numpl_array * array);

#if defined(__cplusplus)
}
#endif

#endif // SOLVE_H
