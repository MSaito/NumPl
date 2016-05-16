#pragma once
/**
 * @file generate.h
 * ナンプレの問題を生成する関数ヘッダー
 *
 * 直観と経験と試行錯誤の結果としてのヒューリスティックアルゴリズムによる。
 */
#ifndef GENERATE_H
#define GENERATE_H
#include "numpl.h"

#if defined(__cplusplus)
extern "C" {
#endif

    /**
     * 生成情報構造体
     */
    typedef struct {
        unsigned hidden_single : 1; //!< hidden_single を含む問題を生成する
        unsigned locked_candidate : 1;//!< locled_candidate を含む問題を生成する
        unsigned tuple : 1; //!< naked および hidden tuple を含む問題を生成する
        unsigned fish : 1; //!< xwing, swordfishなどfish系を含む問題を生成する
        unsigned xy : 1;   //!< xywing を含む問題を生成する
        unsigned xyz : 1;   //!< xyzwing を含む問題を生成する
        unsigned symmetric : 1;//!< 対称形の問題を生成する
    } generate_type;
    int generate(numpl_array * array, generate_type * type);

#if defined(__cplusplus)
}
#endif

#endif // GENERATE_H
