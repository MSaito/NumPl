SUBDIRS = 6x6
bin_PROGRAMS = numberplace_generate numberplace_digging_hole \
	numberplace_convert numberplace_analyze numberplace_normalize
numberplace_generate_SOURCES = generate.c normalize.c analyze.c constants.c \
	common.c convert.c solve.c xsadd.c \
	kill_single.c kill_hidden_single.c locked_candidate.c \
	tuple.c fish.c xywing.c xyzwing.c \
	analyze.h config.h convert.h inline_functions.h \
	normalize.h solve.h common.h constants.h generate.h \
	killer.h numpl.h xsadd.h
numberplace_digging_hole_SOURCES = digging_hole.c solve.c \
	constants.c common.c analyze.c xsadd.c \
	kill_single.c kill_hidden_single.c locked_candidate.c \
	tuple.c fish.c xywing.c xyzwing.c
numberplace_convert_SOURCES = convert_main.c convert.c constants.c common.c \
	xsadd.c \
	kill_single.c kill_hidden_single.c locked_candidate.c \
	tuple.c fish.c xywing.c xyzwing.c \
	solve.c analyze.c
numberplace_analyze_SOURCES = analyze_main.c analyze.c solve.c constants.c \
	common.c \
	kill_single.c kill_hidden_single.c locked_candidate.c \
	tuple.c fish.c xywing.c xyzwing.c
numberplace_normalize_SOURCES = normalize_main.c normalize.c analyze.c \
	constants.c \
	common.c convert.c solve.c \
	kill_single.c kill_hidden_single.c locked_candidate.c \
	tuple.c fish.c xywing.c xyzwing.c
AM_CFLAGS = -O3 -Wall -Wextra -Wno-unused-parameter -std=c99
