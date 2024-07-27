#!/bin/bash

# if [ ! -f ../lib/libvector.a ]; then
  make re -C ..;
# fi

cc --std=c99 -g2 -Wall -Wextra -Werror -o a.out -I../inc/ test.c \
  -L../lib/ -lvector
# valgrind --leak-check=full --show-leak-kinds=all ./a.out
if [ -f a.out ]; then
  ./a.out;
  rm a.out;
fi
