CFLAGS=-Wall -O0 -g3 `pkg-config --cflags cairo cairo-glx x11` glx-utils.c
LDFLAGS=`pkg-config --libs cairo cairo-glx x11` -lm

all: masking-filling-stroking operators

clean:
	rm -f masking-filling-stroking operators *~
