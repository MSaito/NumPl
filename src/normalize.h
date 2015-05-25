#pragma once
/**
 * @file normalize.h
 * ナンプレの標準形を求める関数のヘッダファイル
 *
 * なお、かなりの手抜きなので本来の意味の標準形ではない。
 */
#ifndef NORMALIZE_H
#define NORMALIZE_H

#include "analyze.h"

#if defined(__cplusplus)
extern "C" {
#endif

    int lazy_normalize(numpl_array * array, solve_info * info);

#if defined(__cplusplus)
}
#endif

#endif // NORMALIZE_H
