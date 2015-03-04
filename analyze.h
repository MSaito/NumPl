#pragma once
/**
 * @file analyze.h
 * やや詳細に解法の種類を求める。
 */
#ifndef ANALYZE_H
#define ANALYZE_H
#include "numpl.h"
#include "solve.h"
double analyze(numpl_array * array, solve_info * info);
double get_analyze_value(solve_info * info);
#endif // ANALYZE_H
