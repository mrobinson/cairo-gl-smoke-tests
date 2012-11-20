#ifndef glx_utils_h
#define glx_utils_h

#include <cairo.h>
#include <X11/Xlib.h>

Display* getDisplay();
void createAndShowWindow(int width, int height, Window* window, cairo_device_t** cairoDevice, cairo_surface_t** windowSurface);

#endif
