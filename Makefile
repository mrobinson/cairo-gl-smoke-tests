gl: CFLAGS=-Wall -O0 -g3 `pkg-config --cflags cairo cairo-glx x11` glx-utils.c
gl: LDFLAGS=`pkg-config --libs cairo cairo-glx x11` -lm
gl: masking-filling-stroking operators

egl: CFLAGS=-Wall -O0 -g3 `pkg-config --cflags cairo cairo-egl x11` egl-utils.c
egl: LDFLAGS=`pkg-config --libs cairo cairo-egl x11` -lm
egl: masking-filling-stroking operators

clean:
	rm -f masking-filling-stroking operators *~
