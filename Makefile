NAME := n-puzzle

SOURCES := $(wildcard sources/*.c)
OBJECTS := $(SOURCES:.c=.o)
DEPENDS := $(SOURCES:.c=.d)

LIBFT_SOURCES := $(wildcard libft/sources/*.c) $(wildcard libft/include/*.h) Makefile

all: $(NAME)

CPPFLAGS += -Wall -Wextra -Wshadow -Wvla -Werror -std=gnu11 -MMD -I include -I libft/include
CFLAGS += -O3
LDFLAGS += -Llibft
LDLIBS += -lft -lpthread -lm

$(NAME): $(OBJECTS) libft/libft.a
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LDLIBS)

libft/libft.a: $(LIBFT_SOURCES)
	+$(MAKE) -C libft

%.o: %.c Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

-include $(DEPENDS)

clean:
	rm -f $(OBJECTS) $(DEPENDS)
	$(MAKE) fclean -C libft

fclean:
	$(MAKE) clean
	rm -f $(NAME)

re:
	$(MAKE) fclean
	+$(MAKE) all

.PHONY: all clean fclean re
