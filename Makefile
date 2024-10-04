NAME := 9cc
RM := rm -rf

INCLUDE := include/
SRC_DIR := src/
OBJ_DIR := obj/

CFLAGS :=-std=c17 -g -static -I $(INCLUDE)

SRC := $(wildcard $(SRC_DIR)*.c)
OBJ := $(addprefix $(OBJ_DIR), $(notdir $(SRC:.c=.o)))

#command
all:$(NAME) tags

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $^

$(OBJ_DIR)%.o:$(SRC_DIR)%.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(NAME)
	./test.sh

tags:$(wildcard *.c *.h)
	@if [ ! -f tags ] || [ "$?" ]; then \
		echo "Updating ctags..."; \
		ctags -R --exclude=Makefile; \
	fi

clean:
	$(RM) $(OBJ) *~ tmp* a.out

fclean:clean
	$(RM) $(NAME)

re:clean
	make all

debug:CFLAGS += -g3 -O0
debug:$(NAME)

.PHONY: all test clean re debug tags
