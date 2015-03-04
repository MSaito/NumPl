#pragma once
/**
 * @file constants.h
 * このファイルではナンプレ用の定数を宣言する。
 *
 * 配列のサイズは先に定義したconstantでいいんだけ？　いいとしても
 * constant.cとの間で循環参照になるような気がするんだが。
 * ともかく defineも参照しないとならないという不完全さ。
 *
 * やりたいことは、constant.c だけを変更すれば、あるいはリンクすれば
 * 9x9 以外の盤面にも対応できるようにすること。だが今の私の実力では無理のようだ。
 */
#ifndef CONSTANTS_H
#define CONSTANTS_H
#include "numpl.h"
#if defined(__cplusplus)
extern "C" {
#endif

    /** 行の配列(arrayのindexが入っている) */
    extern const int rows[LINE_SIZE][LINE_SIZE];
    /** 列の配列(arrayのindexが入っている) */
    extern const int cols[LINE_SIZE][LINE_SIZE];
    /** ブロックの配列(arrayのindexが入っている) */
    extern const int blocks[LINE_SIZE][LINE_SIZE];
    /** 行の配列、列の配列、ブロックの配列へのポインタの配列 */
    extern const int (* const all_lines[LINE_KINDS])[LINE_SIZE];
    /** Locked Candidate で使う行とブロックの共通部分の配列 */
    extern const int locked_rows[LINE_SIZE][BLOCK_ROWS];
    /** Locked Candidate で使う列とブロックの共通部分の配列 */
    extern const int locked_cols[LINE_SIZE][BLOCK_COLS];

#if defined(__cplusplus)
}
#endif
#endif // CONSTANTS_H
