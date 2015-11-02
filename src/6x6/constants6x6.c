const int rows[6][6] = {
    {0, 1, 2, 3, 4, 5},
    {6, 7, 8, 9, 10, 11},
    {12, 13, 14, 15, 16, 17},
    {18, 19, 20, 21, 22, 23},
    {24, 25, 26, 27, 28, 29},
    {30, 31, 32, 33, 34, 35}
};

const int cols[6][6] = {
    {0, 6, 12, 18, 24, 30},
    {1, 7, 13, 19, 25, 31},
    {2, 8, 14, 20, 26, 32},
    {3, 9, 15, 21, 27, 33},
    {4, 10, 16, 22, 28, 34},
    {5, 11, 17, 23, 29, 35}
};

const int blocks[6][6] = {
    {0, 1, 2, 6, 7, 8},
    {3, 4, 5, 9, 10, 11},
    {12, 13, 14, 18, 19, 20},
    {15, 16, 17, 21, 22, 23},
    {24, 25, 26, 30, 31, 32},
    {27, 28, 29, 33, 34, 35}
};

const int (* const all_lines[3])[6] = {rows, cols, blocks};

const int locked_rows[6][2] = {
    {0, 1}, {0, 1},
    {2, 3}, {2, 3},
    {4, 5}, {4, 5}
};

const int locked_cols[6][3] = {
    {0, 1, 2}, {3, 4, 5},
    {0, 1, 2}, {3, 4, 5},
    {0, 1, 2}, {3, 4, 5}
};


