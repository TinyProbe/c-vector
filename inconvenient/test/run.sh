#!/bin/bash

# if [ ! -f ../lib/libvector.a ]; then
  make re -C ..;
# fi

cc --std=c99 -g2 -Wall -Wextra -Werror -o a.out -I../inc/ test.c -L../lib/ -lvector
if [ -f a.out ]; then
  # valgrind --leak-check=full --show-leak-kinds=all ./a.out;
  ./a.out;
  rm a.out;
fi
