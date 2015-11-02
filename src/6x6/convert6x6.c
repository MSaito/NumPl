#include "numpl6x6.h"
#include "convert.h"
#include "convert6x6.h"
#include "normalize6x6.h"
#include "constants.h"
#include "inline_functions.h"
#include "common.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

static int vl_change(numpl_array * array, const int vl1[], const int vl2[]);

// 上下反転
void block_reverse6x6(numpl_array * array)
{
    int blo[2][2] = {{0, 4}, {1, 5}};
    for (int i = 0; i < 2; i++) {
        vl_change(array, blocks[blo[i][0]], blocks[blo[i][1]]);
    }
}

// 対称性を保たない
// mode = 0 column 0, 1
// mode = 1 column 1, 2
// mode = 2 column 3, 4
// mode = 3 column 4, 5
// mode = 4 row 1
// mode = 5 row 2
// mode = 6 row 3
// 2^7 = 128
void line_change6x6(numpl_array * array, int mode)
{
    static const int * change_line[7][2] = {
        {cols[0], cols[1]},
        {cols[1], cols[2]},
        {cols[3], cols[4]},
        {cols[4], cols[5]},
        {rows[0], rows[1]},
        {rows[2], rows[3]},
        {rows[4], rows[5]}
    };
    for (int i = 0; i < 7; i++) {
        int mask = 1 << i;
        if ((mode & mask) != 0) {
#if defined(DEBUG)
            printf("%x ", mask);
#endif
            vl_change(array, change_line[i][0], change_line[i][1]);
        }
    }
#if defined(DEBUG)
    printf("\n");
#endif
}

static int vl_change(numpl_array * array, const int vl1[], const int vl2[])
{
#if defined(DEBUG)
    printf("vl_change\n");
    for (int i = 0; i < LINE_SIZE; i++) {
        printf("%d,", vl1[i]);
    }
    printf("\n");
    fflush(stdout);
    for (int i = 0; i < LINE_SIZE; i++) {
        printf("%d,", vl2[i]);
    }
    printf("\n");
    fflush(stdout);
#endif
    for (int i = 0; i < LINE_SIZE; i++) {
        int idx1 = vl1[i];
        int idx2 = vl2[i];
        cell_t tmp = array->ar[idx1];
        array->ar[idx1] = array->ar[idx2];
        array->ar[idx2] = tmp;
    }
    return 0;
}
