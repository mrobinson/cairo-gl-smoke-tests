all: masking-filling-stroking-glx operators-glx
egl: masking-filling-stroking-egl operators-egl

FLAGS = -Wall -O0 -g3

GLX_DEPS = cairo x11 cairo-glx
GLX_CFLAGS = `pkg-config --cflags $(GLX_DEPS)`
GLX_LIBS = `pkg-config --libs $(GLX_DEPS)` -lm

EGL_DEPS = cairo x11 cairo-egl
EGL_CFLAGS = `pkg-config --cflags $(EGL_DEPS)`
EGL_LIBS = `pkg-config --libs $(EGL_DEPS)` -lm

SOURCES = glx-utils.c gl-utils.h egl-utils.c masking-filling-stroking.c operators.c

%-glx: $(SOURCES)
	gcc -o $@ $(FLAGS) $(GLX_CFLAGS) glx-utils.c $*.c $(GLX_LIBS)
%-egl: $(SOURCES)
	gcc -o $@ $(FLAGS) $(EGL_CFLAGS) egl-utils.c $*.c $(EGL_LIBS)

clean:
	rm -f \
	masking-filling-stroking-glx \
	operators-glx \
	masking-filling-stroking-egl \
	operators-egl \
	*~
