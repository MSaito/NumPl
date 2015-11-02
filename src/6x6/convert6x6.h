#ifndef CONVERT6x6_H
#define CONVERT6x6_H

#if defined(__cplusplus)
extern "C" {
#endif

    void block_reverse6x6(numpl_array * array);
    void line_change6x6(numpl_array * array, int mode);

#if defined(__cplusplus)
}
#endif

#endif // CONVERT6x6_H
