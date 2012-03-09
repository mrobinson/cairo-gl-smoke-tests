#include <GL/gl.h>
#include <GL/glx.h>

#ifndef glx_utils_h
#define glx_utils_h

Display* getDisplay();
void showWindow(Window window);
void createWindowAndGLXContext(int width, int height, Window* window, GLXContext* glxContext);

#endif
