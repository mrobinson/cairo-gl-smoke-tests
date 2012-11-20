#include "gl-utils.h"
#include <cairo.h>
#include <cairo-gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <GL/gl.h>

const int gTileHeight = 256;
const int gTileWidth = 256;

void render(cairo_t* cr, cairo_antialias_t antialias)
{
    cairo_save(cr);

    cairo_set_antialias(cr, antialias);

    cairo_rectangle(cr, 0, 0, gTileWidth, gTileHeight);
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

int main(int argc, char *argv[])
{
    /* Use the MSAA compositor if it's available. */
    setenv("CAIRO_GL_COMPOSITOR", "msaa", 1);

    Window window;
    cairo_device_t* device;
    cairo_surface_t* windowSurface;
    createAndShowWindow(gTileWidth * 2, gTileHeight * 2, &window, &device, &windowSurface);

    cairo_surface_t* textureSurface = cairo_gl_surface_create(device, CAIRO_CONTENT_COLOR_ALPHA, gTileWidth * 2, gTileHeight);

    while (1) {
        cairo_t* windowCr = cairo_create(windowSurface);

        cairo_translate(windowCr, 0, gTileHeight);
        render(windowCr, CAIRO_ANTIALIAS_NONE);
        cairo_translate(windowCr, gTileWidth, 0);
        render(windowCr, CAIRO_ANTIALIAS_GOOD);

        cairo_t* textureCr = cairo_create(textureSurface);
        render(textureCr, CAIRO_ANTIALIAS_NONE);
        cairo_translate(textureCr, gTileWidth, 0);
        render(textureCr, CAIRO_ANTIALIAS_GOOD);
        cairo_destroy(textureCr);

        cairo_translate(windowCr, -gTileWidth, -gTileHeight);
        cairo_set_source_surface(windowCr, textureSurface, 0, 0);
        cairo_rectangle(windowCr, 0, 0, gTileWidth * 2, gTileHeight);
        cairo_fill(windowCr);
        cairo_destroy(windowCr);

        cairo_gl_surface_swapbuffers(windowSurface);
        sleep(1);
    }

    cairo_surface_destroy(textureSurface);
    exit(EXIT_SUCCESS);
}
