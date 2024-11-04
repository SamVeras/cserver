#!/usr/bin/env bash

gcc ./source/*.c -I./include -o server -O3 -fsanitize=address,undefined -Wall -Werror -Wextra
