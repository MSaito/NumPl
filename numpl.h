#pragma once
/**
 * @file numpl.h
 * ナンプレ問題作成共通ヘッダファイル
 */
#ifndef NUMPL_H
#define NUMPL_H

#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

#define LINE_SIZE 9
#define ARRAY_SIZE (LINE_SIZE * LINE_SIZE)
#define LINE_KINDS 3
#define BLOCK_ROWS 3
#define BLOCK_COLS 3
#define MAX_SYMBOL 0x100
#define FULL_SYMBOL 0x1ff

/**
 * ナンプレのひとマスを表す構造体
 */
    typedef struct {
	unsigned symbol : 9;	//!< 数字（候補）
	unsigned fixed : 1; 	//!< 固定フラグ 問題で最初に数字が入っているマス
#if defined(SINGLE_FLAG)
	unsigned single : 1;
#endif
    } cell_t;

/**
 * ナンプレの盤面を表す配列を入れた構造体
 */
    typedef struct {
	cell_t ar[ARRAY_SIZE];	//!< 実際の配列
    } numpl_array;



#if defined(__cplusplus)
}
#endif

#endif // NUMPL_H
