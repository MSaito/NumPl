#pragma once
/**
 * @file convert.h
 * 対称形を維持したままブロックや行、列を入れ換えて問題の見た目を変更するヘッダー
 *
 */
#ifndef CONVERT_H
#define CONVERT_H

#include "numpl.h"

#if defined(__cplusplus)
extern "C" {
#endif

    int random_convert(numpl_array * array);
    void block_reverse(numpl_array * array, int mode);
    void line_change(numpl_array * array, int mode);
    void reverse_change(numpl_array * array, int mode);
    void random_symbol(numpl_array * array);

#if defined(__cplusplus)
}
#endif

#endif // CONVERT_H
