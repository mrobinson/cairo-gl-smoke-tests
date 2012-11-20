all: masking-filling-stroking-glx operators-glx
egl: masking-filling-stroking-egl operators-egl

FLAGS = -Wall -O0 -g3 -lm `pkg-config --libs --cflags cairo x11`
SOURCES = glx-utils.c gl-utils.h egl-utils.c masking-filling-stroking.c operators.c

%-glx: $(SOURCES)
	gcc -o $@ $(FLAGS) `pkg-config --libs --cflags cairo-glx` glx-utils.c $*.c
%-egl: $(SOURCES)
	gcc -o $@ $(FLAGS) `pkg-config --libs --cflags egl cairo-egl` egl-utils.c $*.c

clean:
	rm -f \
	masking-filling-stroking-glx \
	operators-glx \
	masking-filling-stroking-egl \
	operators-egl \
	*~
