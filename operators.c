#include "glx-utils.h"
#include <cairo.h>
#include <cairo-gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <GL/gl.h>
#include <GL/glx.h>

const int gSize = 500;

void renderCheckerboard(cairo_t* cr)
{
    int x = 0, y = 0;
    for (y = 0; y < gSize + 20; y += 20) {
        for (x = 0; x < gSize + 20; x += 20) {
            if ((x / 20 % 2) == (y / 20 % 2))
                cairo_set_source_rgb(cr, 0.75, 0.75, 0.75);
            else
                cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
            cairo_rectangle(cr, x, y, 20, 20);
            cairo_fill(cr);
        }
    }
}

void renderOperators(cairo_surface_t* surface)
{
    cairo_t* cr = cairo_create(surface);

    cairo_operator_t operators[] = {
        CAIRO_OPERATOR_CLEAR,
        CAIRO_OPERATOR_SOURCE,
        CAIRO_OPERATOR_OVER,
        CAIRO_OPERATOR_IN,
        CAIRO_OPERATOR_OUT,
        CAIRO_OPERATOR_ATOP,
        CAIRO_OPERATOR_DEST,
        CAIRO_OPERATOR_DEST_OVER,
        CAIRO_OPERATOR_DEST_IN,
        CAIRO_OPERATOR_DEST_OUT,
        CAIRO_OPERATOR_DEST_ATOP,
        CAIRO_OPERATOR_XOR,
        CAIRO_OPERATOR_ADD,
        //CAIRO_OPERATOR_SATURATE,
        //CAIRO_OPERATOR_MULTIPLY,
        //CAIRO_OPERATOR_SCREEN,
        //CAIRO_OPERATOR_OVERLAY,
        //CAIRO_OPERATOR_DARKEN,
        //CAIRO_OPERATOR_LIGHTEN,
        //CAIRO_OPERATOR_COLOR_DODGE,
        //CAIRO_OPERATOR_COLOR_BURN,
        //CAIRO_OPERATOR_HARD_LIGHT,
        //CAIRO_OPERATOR_SOFT_LIGHT,
        //CAIRO_OPERATOR_DIFFERENCE,
        //CAIRO_OPERATOR_EXCLUSION,
        //CAIRO_OPERATOR_HSL_HUE,
        //CAIRO_OPERATOR_HSL_SATURATION,
        //CAIRO_OPERATOR_HSL_COLOR,
        //CAIRO_OPERATOR_HSL_LUMINOSITY
    };

    const char* operatorNames[] =  {
        "CLEAR",
        "SOURCE",
        "OVER",
        "IN",
        "OUT",
        "ATOP",
        "DEST",
        "DEST_OVER",
        "DEST_IN",
        "DEST_OUT",
        "DEST_ATOP",
        "XOR",
        "ADD",
        "SATURATE",
        "MULTIPLY",
        "SCREEN",
        "OVERLAY",
        "DARKEN",
        "LIGHTEN",
        "COLOR_DODGE",
        "COLOR_BURN",
        "HARD_LIGHT",
        "SOFT_LIGHT",
        "DIFFERENCE",
        "EXCLUSION",
        "HSL_HUE",
        "HSL_SATURATION",
        "HSL_COLOR",
        "HSL_LUMINOSITY",
    };

    int numberOfOperators = sizeof(operators) / sizeof(operators[0]);
    int gridSquares = (int) ceil(sqrtf(numberOfOperators));
    int gridSize = gSize / gridSquares;

    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

    int i = 0;
    for (i = 0; i < numberOfOperators; i++) {
        int startX = i % gridSquares * gridSize;
        int startY = i / gridSquares * gridSize;

        cairo_save(cr);
        cairo_translate(cr, startX, startY);

        cairo_rectangle(cr, 0, 0, gridSize, gridSize);
        cairo_clip(cr);

        cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
        cairo_set_source_rgba(cr, 1, 0, 0, 1);
        cairo_rectangle(cr, 5,  5, (gridSize/2) - 5, (gridSize/2) - 5);
        cairo_fill(cr);

        cairo_set_operator(cr, operators[i]);
        cairo_rectangle(cr, 5 + (gridSize / 5), 5 + (gridSize / 5),
                        (gridSize / 2) - 5, (gridSize / 2) - 5);
        cairo_set_source_rgba(cr, 0, 0, 1, 0.5);
        cairo_fill(cr);

        cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
        cairo_move_to(cr, 0, 20);
        cairo_set_source_rgba(cr, 0, 0, 0, 1);
        cairo_show_text(cr, operatorNames[i]);

        cairo_restore(cr);
    }

    cairo_set_font_size(cr, 20);

    cairo_destroy(cr);
}

int main(int argc, char *argv[])
{
    Window window;
    GLXContext glxContext;
    createWindowAndGLXContext(gSize, gSize, &window, &glxContext);
    showWindow(window);

    /* Use the MSAA compositor if it's available. */
    setenv("CAIRO_GL_COMPOSITOR", "msaa", 1);

    cairo_device_t* device = cairo_glx_device_create(getDisplay(), glxContext);
    cairo_surface_t* windowSurface = cairo_gl_surface_create_for_window(device, window, gSize, gSize);
    cairo_surface_t* textureSurface = cairo_gl_surface_create(device, CAIRO_CONTENT_COLOR_ALPHA, gSize, gSize);

    while (1) {
        renderOperators(textureSurface);

        cairo_t* windowCr = cairo_create(windowSurface);
        renderCheckerboard(windowCr);

        cairo_set_source_surface(windowCr, textureSurface, 0, 0);
        cairo_paint(windowCr);
        cairo_destroy(windowCr);

        cairo_gl_surface_swapbuffers(windowSurface);
        sleep(1);
    }

    cairo_surface_destroy(textureSurface);
    exit(EXIT_SUCCESS);
}
