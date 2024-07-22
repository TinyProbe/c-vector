TARGET=lib/libvector.a
CC=cc
CFLAGS=--std=c99 -g2 -Wall -Wextra -Werror -c -I./inc/

INC=\
		inc/vector.h\

SRC=\
		src/vector.c\

OBJ=\
		src/vector.o\

$(TARGET): $(INC) $(OBJ) Makefile
	@if [ ! -f lib ]; then \
		mkdir -p lib; \
	fi
	@ar -cvq $(TARGET) $(OBJ)

%.o: %.c
	@$(CC) $(CFLAGS) -o $@ $<

all: $(TARGET)

re: fclean all

clean:
	@if [ -f $(OBJ) ]; then \
		rm $(OBJ); \
	fi

fclean: clean
	@if [ -f $(TARGET) ]; then \
		rm $(TARGET); \
	fi

.PHONY: all re clean fclean
