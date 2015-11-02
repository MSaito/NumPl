noinst_PROGRAMS = numberplace_convert6x6 numberplace_normalize6x6
bin_PROGRAMS = numberplace_generate6x6
common = constants6x6.c common6x6.c convert6x6.c normalize6x6.c analyze6x6.c \
	convert_common.c solve6x6.c xsadd6x6.c \
	kill_single6x6.c kill_hidden_single6x6.c locked_candidate6x6.c \
	tuple6x6.c fish6x6.c xywing6x6.c \
	convert6x6.h normalize6x6.h numpl6x6.h
numberplace_generate6x6_SOURCES = generate6x6.c ${common}
numberplace_convert6x6_SOURCES = convert6x6_main.c ${common}
numberplace_normalize6x6_SOURCES = normalize6x6_main.c ${common}
AM_CFLAGS = -I.. -O3 -Wall -Wextra -Wmissing-prototypes \
	-Wno-unused-parameter -std=c99
