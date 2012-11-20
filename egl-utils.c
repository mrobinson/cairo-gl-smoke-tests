#include <cairo.h>
#include <cairo-gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <EGL/egl.h>

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

static EGLint multisampleAttributes[] = {
    EGL_RED_SIZE,        1,
    EGL_GREEN_SIZE,      1,
    EGL_BLUE_SIZE,       1,
    EGL_DEPTH_SIZE,      1,
    EGL_STENCIL_SIZE,    1,
    EGL_SAMPLES,         4,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    EGL_NONE
};

static EGLint singleSampleAttributes[] = {
    EGL_RED_SIZE,        1,
    EGL_GREEN_SIZE,      1,
    EGL_BLUE_SIZE,       1,
    EGL_DEPTH_SIZE,      1,
    EGL_STENCIL_SIZE,    1,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    EGL_NONE
};

static Bool waitForNotify(Display *dpy, XEvent *event, XPointer arg) {
    return (event->type == MapNotify) && (event->xmap.window == (Window) arg);
}

void createAndShowWindow(int width, int height, Window* window, cairo_device_t** cairoDevice, cairo_surface_t** windowSurface)
{
    Display* display = getDisplay();
    EGLDisplay eglDisplay = eglGetDisplay(display);

    if (!eglInitialize(eglDisplay, NULL, NULL)) {
        printf("Unable to initialize EGL display\n");
        exit(EXIT_FAILURE);
    }

    int numReturned;
    EGLConfig config;
    if (!eglChooseConfig(eglDisplay, multisampleAttributes, &config, 1, &numReturned) || numReturned <= 0) {
        eglChooseConfig(eglDisplay, singleSampleAttributes, &config, 1, &numReturned);
    }

    if (!numReturned) {
        printf("Unable to create a GL context with appropriate attributes.\n");
        exit(EXIT_FAILURE);
    }

    XVisualInfo visualTemplate;
    EGLint visualId;
    if (!eglGetConfigAttrib(eglDisplay, config, EGL_NATIVE_VISUAL_ID, &visualId)) {
        printf("Unable to create a GL context with appropriate attributes.\n");
        exit(EXIT_FAILURE);
    }

    visualTemplate.visualid = visualId;
    XVisualInfo* visualInfo = XGetVisualInfo(display, VisualIDMask, &visualTemplate, &numReturned);
    if (!numReturned || !visualInfo) {
        printf("Unable to create a GL context with appropriate attributes.\n");
        exit(EXIT_FAILURE);
    }

    XSetWindowAttributes windowAttributes;
    windowAttributes.border_pixel = 0;
    windowAttributes.background_pixel = 0;
    windowAttributes.event_mask = StructureNotifyMask;
    Window rootWindow = DefaultRootWindow(display);
    windowAttributes.colormap = XCreateColormap(display, rootWindow, visualInfo->visual, AllocNone);
    int defaultScreen = DefaultScreen(display);
    *window = XCreateWindow(display, rootWindow, 0, 0, width, height,
                            0, DefaultDepth(display, defaultScreen), InputOutput, visualInfo->visual,
                            CWBackPixel | CWBorderPixel | CWColormap | CWEventMask, &windowAttributes);

    XFree(visualInfo);
    if (!*window) {
        printf("Unable to create a window with appropriate attributes.\n");
        exit(EXIT_FAILURE);
    }

    eglBindAPI(EGL_OPENGL_ES_API);
    static const EGLint contextAttributes[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    EGLContext context = eglCreateContext(eglDisplay, config, NULL, contextAttributes);
    if (context == EGL_NO_CONTEXT) {
        printf("Unable to create a GL context with appropriate attributes.\n");
        exit(EXIT_FAILURE);
    }
    *cairoDevice = cairo_egl_device_create(eglDisplay, context);

    EGLSurface surface = eglCreateWindowSurface(eglDisplay, config, *window, 0);
    if (surface == EGL_NO_SURFACE) {
        printf("Unable to create a GL context with appropriate attributes.\n");
        exit(EXIT_FAILURE);
    }
    *windowSurface = cairo_gl_surface_create_for_egl(*cairoDevice, surface, width, height);

    /* Map the window to the screen, and wait for it to appear */
    XEvent event;
    XMapWindow(getDisplay(), *window);
    XIfEvent(getDisplay(), &event, waitForNotify, (XPointer) *window);
}
