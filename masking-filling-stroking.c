#include <cairo.h>
#include <cairo-gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

#define HEIGHT 256
#define WIDTH 256

int multisampleAttributes[] = {
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

int singleSampleAttributes[] = {
    GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,   GLX_RGBA_BIT,
    GLX_DOUBLEBUFFER,  True,  /* Request a double-buffered color buffer with */
    GLX_RED_SIZE,      1,     /* the maximum number of bits per component    */
    GLX_GREEN_SIZE,    1,
    GLX_BLUE_SIZE,     1,
    GLX_STENCIL_SIZE,  1,
    None
};

void render(cairo_t* cr, cairo_antialias_t antialias)
{
    cairo_save(cr);

    cairo_set_antialias(cr, antialias);

    cairo_rectangle(cr, 0, 0, WIDTH, HEIGHT);
    cairo_clip(cr);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    cairo_surface_t* image = cairo_image_surface_create_from_png("cat.png");
    cairo_surface_t* mask = cairo_image_surface_create_from_png("mask.png");

    cairo_arc(cr, 25, 25, 75, 0, 2 * M_PI);
    cairo_set_source_surface(cr, image, 0, 0);
    cairo_mask_surface(cr, mask, 0, 0);

    cairo_set_source_surface(cr, image, 25, 25);
    cairo_arc(cr, 75, 75, 75, 0, 2 * M_PI);
    cairo_fill(cr);

    cairo_set_source_rgba(cr, 0, 0, 1, 0.3);
    cairo_arc(cr, 125, 125, 75, 0, 2 * M_PI);
    cairo_fill(cr);

    cairo_set_line_width(cr, 10);
    cairo_set_source_rgba(cr, 0, 0, 1, 0.3);
    cairo_arc(cr, 200, 200, 75, 0, 2 * M_PI);
    cairo_stroke(cr);

    cairo_surface_destroy(image);
    cairo_surface_destroy(mask);

    cairo_restore(cr);
}

static Bool WaitForNotify(Display *dpy, XEvent *event, XPointer arg) {
    return (event->type == MapNotify) && (event->xmap.window == (Window) arg);
}

int main(int argc, char *argv[])
{
    Display              *dpy;
    Window                xWin;
    XEvent                event;
    XVisualInfo          *vInfo;
    XSetWindowAttributes  swa;
    GLXFBConfig          *fbConfigs;
    GLXContext            context;
    int                   swaMask;
    int                   numReturned;

    /* Use the MSAA compositor if it's available. */
    setenv("CAIRO_GL_COMPOSITOR", "msaa", 1);

    /* Open a connection to the X server */
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        printf("Unable to open a connection to the X server\n");
        exit(EXIT_FAILURE);
    }

    fbConfigs = glXChooseFBConfig(dpy, DefaultScreen(dpy), multisampleAttributes, &numReturned);

    if (fbConfigs == NULL) {
        fbConfigs = glXChooseFBConfig(dpy, DefaultScreen(dpy), singleSampleAttributes, &numReturned);
    }

    if (fbConfigs == NULL) {
        printf("Unable to create a GL context with appropriate attributes.\n");
        exit(EXIT_FAILURE);
    }

    /* Create an X colormap and window with a visual matching the first
    ** returned framebuffer config */
    vInfo = glXGetVisualFromFBConfig(dpy, fbConfigs[0]);

    swa.border_pixel = 0;
    swa.event_mask = StructureNotifyMask;
    swa.colormap = XCreateColormap(dpy, RootWindow(dpy, vInfo->screen), vInfo->visual, AllocNone);

    swaMask = CWBorderPixel | CWColormap | CWEventMask;

    xWin = XCreateWindow(dpy, RootWindow(dpy, vInfo->screen), 0, 0, WIDTH * 2, HEIGHT * 2,
                         0, vInfo->depth, InputOutput, vInfo->visual,
                         swaMask, &swa);

    /* Create a GLX context for OpenGL rendering */
    context = glXCreateNewContext(dpy, fbConfigs[0], GLX_RGBA_TYPE, NULL, True);

    /* Map the window to the screen, and wait for it to appear */
    XMapWindow(dpy, xWin);
    XIfEvent(dpy, &event, WaitForNotify, (XPointer) xWin);

    /* Bind the GLX context to the Window */
    glXMakeCurrent(dpy, xWin, context);

    cairo_device_t* device = cairo_glx_device_create(dpy, context);
    cairo_surface_t* windowSurface = cairo_gl_surface_create_for_window(device, xWin, WIDTH * 2, HEIGHT * 2);
    cairo_surface_t* textureSurface = cairo_gl_surface_create(device, CAIRO_CONTENT_COLOR_ALPHA, WIDTH * 2, HEIGHT);

    while (1) {
        cairo_t* windowCr = cairo_create(windowSurface);

        cairo_translate(windowCr, 0, HEIGHT);
        render(windowCr, CAIRO_ANTIALIAS_NONE);
        cairo_translate(windowCr, WIDTH, 0);
        render(windowCr, CAIRO_ANTIALIAS_GOOD);

        cairo_t* textureCr = cairo_create(textureSurface);
        render(textureCr, CAIRO_ANTIALIAS_NONE);
        cairo_translate(textureCr, WIDTH, 0);
        render(textureCr, CAIRO_ANTIALIAS_GOOD);
        cairo_destroy(textureCr);

        cairo_translate(windowCr, -WIDTH, -HEIGHT);
        cairo_set_source_surface(windowCr, textureSurface, 0, 0);
        cairo_rectangle(windowCr, 0, 0, WIDTH * 2, HEIGHT);
        cairo_fill(windowCr);
        cairo_destroy(windowCr);

        cairo_gl_surface_swapbuffers(windowSurface);
        sleep(1);
    }

    cairo_surface_destroy(textureSurface);
    exit(EXIT_SUCCESS);
}
