#include "analyze.h"
#include "common.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

static int quiet = 0;
static char * mondai_p = NULL;
static int parse_opt(int argc, char * argv[]);
static int parse_opt(int argc, char * argv[])
{
    static struct option longopts[] = {
        {"silent", no_argument, NULL, 's'},
        {NULL, 0, NULL, 0}};
    quiet = 0;
    const char * pgm = argv[0];
    int c;
    int error = 0;
    for (;;) {
        if (error) {
            break;
        }
        c = getopt_long(argc, argv, "?s", longopts, NULL);
        if (c < 0) {
            break;
        }
        switch (c) {
        case 's':
            quiet = 1;
            break;
        case '?':
        default:
            error = 1;
            break;
        }
    }
    argc -= optind;
    argv += optind;
#if defined(DEBUG)
    printf("quiet = %d\n", quiet);
    printf("recursion = %d\n", recursion_flag);
    printf("argc = %d\n", argc);
#endif
    if (error) {
        printf("%s [-s] [puzzle_string]\n", pgm);
        printf("\t--silent -s 余分な情報を出力しない。\n"
               "\tpuzzle_string 変換する問題の文字列、コマンドラインで指定されなければ、"
               "\n\t標準入力から読み込む。\n");
        return -1;
    }
    if (argc > 0) {
        mondai_p = argv[0];
    }
    return 0;
}

int main(int argc, char * argv[])
{
    numpl_array work;
    int r;
    r = parse_opt(argc, argv);
    if (r < 0) {
        return r;
    }
    if (mondai_p == NULL) {
        char mondai[200];
        read_mondai(mondai, stdin);
        r = input(&work, mondai);
    } else {
        //printf("%s\n", mondai_p);
        r = input(&work, mondai_p);
    }
    if (r != 0) {
        printf("input string is wrong\n");
        return -1;
    }
    if (!quiet) {
        output(&work);
    }
    solve_info info;
    analyze(&work, &info);
    if (!quiet || !info.solved) {
        if (info.solved) {
            printf("\nsolved\n");
            output(&work);
        } else {
            output_detail(&work);
        }
        print_solve_info(&info, 1);
    } else {
        printf("\"");
        print_solve_info(&info, 0);
        printf("\":");
        fixed_only(&work, 0);
        print_array(&work);
        printf("\n");
    }
    return 0;
}
