#include "numpl.h"
#include "constants.h"
#include "killer.h"
#include "inline_functions.h"

static int kill_hidden_line(const int line[], cell_t * ar);

int kill_hidden_single(numpl_array * array)
{
    int result = 0;
    for (int i = 0; i < LINE_KINDS; i++) {
	for (int j = 0; j < LINE_SIZE; j++) {
	    result += kill_hidden_line(all_lines[i][j], array->ar);
	}
    }
    return result;
}

static int kill_hidden_line(const int line[], cell_t * ar)
{
    int pos = 0;
    for (uint16_t sym = 1; sym <= MAX_SYMBOL; sym <<= 1) {
	int count = 0;
	for (int i = 0; i < LINE_SIZE; i++) {
	    int idx = line[i];
	    if (ar[idx].symbol == sym) {
		count = 0;
		break;
	    }
	    if (ar[idx].symbol & sym) {
		pos = idx;
		count++;
	    }
	}
	if (count == 1) {
	    ar[pos].symbol = sym;
	    return 1;
	}
    }
    return 0;
}
