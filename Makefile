LINK=gcc -o $@ $^ -g -O2 -Wall -Wextra -I. $(shell pkg-config --libs gtk+-2.0)

CFLAGS:=$(shell pkg-config --cflags gtk+-2.0) -I.

all: demo test-path-bar

check: all
	./test-path-bar

demo: main.c libpathbar.a | Makefile
	$(LINK) $(CFLAGS)

libpathbar_a_SOURCES=\
	path-bar.c \
	path-bar.h \
	path-element.c \
	path-element.h \
	simple-widgets.c \
	simple-widgets.h \
	$(NULL)
libpathbar.a: $(patsubst %.c,%.o,$(filter %.c,$(libpathbar_a_SOURCES))) | Makefile
	ar -rs $@ $^

test-path-bar: test-path-bar.c libpathbar.a | Makefile
	$(LINK) $(CFLAGS)
