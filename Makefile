all: demo

demo: main.c path-bar.c path-bar.h path-element.c path-element.h simple-widgets.c simple-widgets.h Makefile
	gcc -o $@ $(filter %.c,$^) -g -O2 -Wall -Wextra -I. $(shell pkg-config --cflags --libs gtk+-2.0)
