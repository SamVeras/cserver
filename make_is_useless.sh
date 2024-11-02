#!/usr/bin/env bash

gcc *.c -I. -o server -O3 -fsanitize=address,undefined -Wall -Werror -Wextra 