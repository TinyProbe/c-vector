#!/bin/bash

# if [ ! -f ../lib/libvector.a ]; then
  make re -C ..;
# fi

cc --std=c99 -g2 -Wall -Wextra -Werror -fsanitize=leak -o a.out \
  -I../inc/ test.c -L../lib/ -lvector
./a.out
rm a.out
