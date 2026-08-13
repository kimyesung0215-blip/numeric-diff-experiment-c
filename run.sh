#!/usr/bin/env sh
set -eu

cc_bin="${CC:-gcc}"

"$cc_bin" -O2 -std=c11 -Wall -Wextra -o numeric_diff_experiment numeric_diff_experiment.c -lm
./numeric_diff_experiment
