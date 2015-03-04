#pragma once
/**
 * @file solve.h
 * ナンプレ問題を解くための関数ヘッダファイル
 */
#ifndef SOLVE_H
#define SOLVE_H

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * ナンプレを解くためにどの解法を使ったかを表す構造体
 */
    typedef struct {
	int ks_count;		//!< kill_single を使った回数
	int kh_count;		//!< kill_hidden_single を使った回数
	int kt_count;		//!< kill_tuple を使った回数
	int kl_count;		//!< locked_candidate を使った回数
	int sf_count;		//!< xwing, swordfish などfish系を使った回数
	int xy_count;		//!< xywing を使った回数
	int fx_count;		//!< 初期数字の入っている場所の数
	int naked_tuple[LINE_SIZE / 2 + 1];//!< naked_tuple の詳細
	int hidden_tuple[LINE_SIZE / 2 + 1];//!< hidden_tuple の詳細
	int fish[LINE_SIZE];		//!< fish系の詳細
	int solved;		//!< 解けたら1
    } solve_info;

/**
 * recursion_solve 用の構造体
 */
    typedef struct {
	numpl_array save;
	int saved;
    } recursion_solve_t;

    int solve(numpl_array * array, solve_info * info);
//    int analyze(numpl_array * array, solve_info * info);
    int is_solved(const numpl_array * array);
    int simple_solve(numpl_array * array);
    int equal(const numpl_array * a, const numpl_array * b);
    int recursion_solve(numpl_array * array);
    int random_solve(numpl_array *array);
    void print_solve_info(solve_info * info, int verbose);
    void output_detail(numpl_array * array);
    void output(numpl_array * array);

#if defined(__cplusplus)
}
#endif

#endif // SOLVE_H
