#include <cairo.h>
#include <cairo-gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <GL/glx.h>

Display* getDisplay()
{
    static Display* display = NULL;
    if (display)
        return display;

    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Could not open display.");
        exit(EXIT_FAILURE);
    }
    return display;
}

static int multisampleAttributes[] = {
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    GLX_DOUBLEBUFFER,  True,  /* Request a double-buffered color buffer with */
    GLX_RED_SIZE,      1,     /* the maximum number of bits per component    */
    GLX_GREEN_SIZE,    1,
    GLX_BLUE_SIZE,     1,
    GLX_STENCIL_SIZE,  1,
    GLX_SAMPLES,       4,
    GLX_SAMPLE_BUFFERS,1,
    None
};

static int singleSampleAttributes[] = {
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    GLX_DOUBLEBUFFER,  True,  /* Request a double-buffered color buffer with */
    GLX_RED_SIZE,      1,     /* the maximum number of bits per component    */
    GLX_GREEN_SIZE,    1,
    GLX_BLUE_SIZE,     1,
    GLX_STENCIL_SIZE,  1,
    None
};

static Bool waitForNotify(Display *dpy, XEvent *event, XPointer arg) {
    return (event->type == MapNotify) && (event->xmap.window == (Window) arg);
}

void createAndShowWindow(int width, int height, Window* window, cairo_device_t** cairoDevice, cairo_surface_t** windowSurface)
{
    Display* display = getDisplay();

    int numReturned = 0;
    GLXFBConfig* fbConfigs = glXChooseFBConfig(display, DefaultScreen(display), multisampleAttributes, &numReturned);
    if (fbConfigs == NULL) {
        fbConfigs = glXChooseFBConfig(display, DefaultScreen(display), singleSampleAttributes, &numReturned);
    }

    if (fbConfigs == NULL) {
        printf("Unable to create a GL context with appropriate attributes.\n");
        exit(EXIT_FAILURE);
    }

    XVisualInfo* visualInfo = glXGetVisualFromFBConfig(display, fbConfigs[0]);
    Window rootWindow = RootWindow(display, visualInfo->screen);

    XSetWindowAttributes windowAttributes;
    windowAttributes.border_pixel = 0;
    windowAttributes.event_mask = StructureNotifyMask;
    windowAttributes.colormap = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);

    /* Create the XWindow. */
    *window = XCreateWindow(display, rootWindow, 0, 0, width, height,
                            0, visualInfo->depth, InputOutput, visualInfo->visual,
                            CWBorderPixel | CWColormap | CWEventMask, &windowAttributes);

    /* Create a GLX context for OpenGL rendering */
    GLXContext glxContext = glXCreateNewContext(display, fbConfigs[0], GLX_RGBA_TYPE, NULL, True);

    /* Map the window to the screen, and wait for it to appear */
    XEvent event;
    XMapWindow(getDisplay(), *window);
    XIfEvent(getDisplay(), &event, waitForNotify, (XPointer) *window);

    *cairoDevice = cairo_glx_device_create(getDisplay(), glxContext);
    *windowSurface = cairo_gl_surface_create_for_window(*cairoDevice, *window, width, height);
}
