#include "SDL_geometry.h"
#include "SDL_assert.h"
#include "SDL_loadso.h"
#include "SDL_opengl.h"
#include "SDL_version.h"

typedef struct SDL_SW_YUVTexture SDL_SW_YUVTexture;

typedef struct SDL_Texture_2_0_12
{
    const void *magic;
    Uint32 format;              /**< The pixel format of the texture */
    int access;                 /**< SDL_TextureAccess */
    int w;                      /**< The width of the texture */
    int h;                      /**< The height of the texture */
    int modMode;                /**< The texture modulation mode */
    SDL_BlendMode blendMode;    /**< The texture blend mode */
    SDL_ScaleMode scaleMode;    /**< The texture scale mode */
    Uint8 r, g, b, a;           /**< Texture modulation values */

    SDL_Renderer *renderer;

    /* Support for formats not supported directly by the renderer */
    SDL_Texture *native;
    SDL_SW_YUVTexture *yuv;
    void *pixels;
    int pitch;
    SDL_Rect locked_rect;
    SDL_Surface *locked_surface;  /**< Locked region exposed as a SDL surface */

    Uint32 last_command_generation; /* last command queue generation this texture was in. */

    void *driverdata;           /**< Driver specific texture representation */

    SDL_Texture *prev;
    SDL_Texture *next;
} SDL_Texture_2_0_12;

typedef struct SDL_Texture_2_0_10
{
    const void *magic;
    Uint32 format;              /**< The pixel format of the texture */
    int access;                 /**< SDL_TextureAccess */
    int w;                      /**< The width of the texture */
    int h;                      /**< The height of the texture */
    int modMode;                /**< The texture modulation mode */
    SDL_BlendMode blendMode;    /**< The texture blend mode */
    SDL_ScaleMode scaleMode;    /**< The texture scale mode */
    Uint8 r, g, b, a;           /**< Texture modulation values */

    SDL_Renderer *renderer;

    /* Support for formats not supported directly by the renderer */
    SDL_Texture *native;
    SDL_SW_YUVTexture *yuv;
    void *pixels;
    int pitch;
    SDL_Rect locked_rect;

    Uint32 last_command_generation; /* last command queue generation this texture was in. */

    void *driverdata;           /**< Driver specific texture representation */

    SDL_Texture *prev;
    SDL_Texture *next;
} SDL_Texture_2_0_10;

typedef struct SDL_Texture_2_0_9
{
    const void *magic;
    Uint32 format;              /**< The pixel format of the texture */
    int access;                 /**< SDL_TextureAccess */
    int w;                      /**< The width of the texture */
    int h;                      /**< The height of the texture */
    int modMode;                /**< The texture modulation mode */
    SDL_BlendMode blendMode;    /**< The texture blend mode */
    SDL_ScaleMode scaleMode;    /**< The texture scale mode */
    Uint8 r, g, b, a;           /**< Texture modulation values */

    SDL_Renderer *renderer;

    /* Support for formats not supported directly by the renderer */
    SDL_Texture *native;
    SDL_SW_YUVTexture *yuv;
    void *pixels;
    int pitch;
    SDL_Rect locked_rect;

    void *driverdata;           /**< Driver specific texture representation */

    SDL_Texture *prev;
    SDL_Texture *next;
} SDL_Texture_2_0_9;


typedef struct SDL_Texture_2_0_8
{
    const void *magic;
    Uint32 format;              /**< The pixel format of the texture */
    int access;                 /**< SDL_TextureAccess */
    int w;                      /**< The width of the texture */
    int h;                      /**< The height of the texture */
    int modMode;                /**< The texture modulation mode */
    SDL_BlendMode blendMode;    /**< The texture blend mode */
    Uint8 r, g, b, a;           /**< Texture modulation values */

    SDL_Renderer *renderer;

    /* Support for formats not supported directly by the renderer */
    SDL_Texture *native;
    SDL_SW_YUVTexture *yuv;
    void *pixels;
    int pitch;
    SDL_Rect locked_rect;

    void *driverdata;           /**< Driver specific texture representation */

    SDL_Texture *prev;
    SDL_Texture *next;
} SDL_Texture_2_0_8;

typedef struct SDL_Texture_2_0_7
{
    const void *magic;
    Uint32 format;              /**< The pixel format of the texture */
    int access;                 /**< SDL_TextureAccess */
    int w;                      /**< The width of the texture */
    int h;                      /**< The height of the texture */
    int modMode;                /**< The texture modulation mode */
    SDL_BlendMode blendMode;    /**< The texture blend mode */
    Uint8 r, g, b, a;           /**< Texture modulation values */

    SDL_Renderer *renderer;

    /* Support for formats not supported directly by the renderer */
    SDL_Texture *native;
    SDL_SW_YUVTexture *yuv;
    void *pixels;
    int pitch;
    SDL_Rect locked_rect;

    void *driverdata;           /**< Driver specific texture representation */

    SDL_Texture *prev;
    SDL_Texture *next;
} SDL_Texture_2_0_7;


// TODO: all the way to 2.0.4

static void* GEOM_GetTextureDriverData(SDL_Texture *texture) {
    SDL_version linked;
    SDL_GetVersion(&linked);
    
    // hope we're compiling with a compiler with the same ABI
    // is there a way to test these? e.g. create a texture, look for fields we know
    if (linked.major == 2 && linked.minor == 0) {
        if (linked.patch == 12) {
            return ((SDL_Texture_2_0_12*)texture)->driverdata;
        }
        if (linked.patch == 10) {
            return ((SDL_Texture_2_0_10*)texture)->driverdata;
        }
        if (linked.patch == 9) {
            return ((SDL_Texture_2_0_9*)texture)->driverdata;
        }
        if (linked.patch == 8) {
            return ((SDL_Texture_2_0_8*)texture)->driverdata;
        }
        if (linked.patch == 7) {
            return ((SDL_Texture_2_0_7*)texture)->driverdata;
        }
        // TODO: go at least to 2.0.4
        // if (linked.patch == 6) {
        //     return ((SDL_Texture_2_0_6*)texture)->driverdata;
        // }
        // if (linked.patch == 5) {
        //     return ((SDL_Texture_2_0_5*)texture)->driverdata;
        // }
        // if (linked.patch == 4) {
        //     return ((SDL_Texture_2_0_4*)texture)->driverdata;
        // }
        // if (linked.patch == 3) {
        //     return ((SDL_Texture_2_0_3*)texture)->driverdata;
        // }
        // if (linked.patch == 2) {
        //     return ((SDL_Texture_2_0_2*)texture)->driverdata;
        // }
        // if (linked.patch == 1) {
        //     return ((SDL_Texture_2_0_1*)texture)->driverdata;
        // }
        // if (linked.patch == 0) {
        //     return ((SDL_Texture_2_0_0*)texture)->driverdata;
        // }
    }
    
    // TODO: different way to do this? set up thunks on initialization?
    fprintf(stderr, "Error! don't know to use version %i.%i.%i\n", linked.major, linked.minor, linked.patch);
    return NULL;
}

// SDL: put into SDL_pixels.h and add ref in SDL_Color documentation
SDL_FORCE_INLINE SDL_bool GEOM_ColorEquals(SDL_Color left, SDL_Color right)
{
    return left.r == right.r && left.g == right.g && left.b == right.b && left.a == right.a;
}

static void DrawTriangle(SDL_Renderer * renderer, SDL_Texture *texture,
                        GEOM_Vertex v1, GEOM_Vertex v2, GEOM_Vertex v3)
{
    const float minX = SDL_min(v1.position.x, SDL_min(v2.position.x, v3.position.x));
    const float minY = SDL_min(v1.position.y, SDL_min(v2.position.y, v3.position.y));
    const float maxX = SDL_max(v1.position.x, SDL_max(v2.position.x, v3.position.x));
    const float maxY = SDL_max(v1.position.y, SDL_max(v2.position.y, v3.position.y));

    // If we're outside the clip rect, then don't run the triangle rasterizer
    if (SDL_RenderIsClipEnabled(renderer)) {
        SDL_Rect clip_rect;
        SDL_RenderGetClipRect(renderer, &clip_rect);
        if ((minX > clip_rect.x + clip_rect.w || maxX < clip_rect.x) &&
            (minY > clip_rect.y + clip_rect.h || maxY < clip_rect.y))
        {
            return;
        }
    }

    // This function operates in s28.4 fixed point: it's more precise than
    // floating point and often faster. This also effectively lets us work in
    // a subpixel space where each pixel is divided into 256 subpixels.
    // TODO: check for overflow

    // Find integral bounding box, scale to fixed point. We use this to
    // iterate over all pixels possibly covered by the triangle.
    const Sint32 minXf = SDL_floor(minX) * 16;
    const Sint32 minYf = SDL_floor(minY) * 16;
    const Sint32 maxXf = SDL_ceil(maxX) * 16;
    const Sint32 maxYf = SDL_ceil(maxY) * 16;

    // Find center of bounding box, used for translating coordinates.
    // This accomplishes two things: 1) makes the fixed point calculations less
    // likely to overflow 2) makes it less likely that triangles will be rendered
    // differently on different parts of the screen (seems to happen sometimes
    // with fractional coordinates). Make sure they're on integer boundaries too,
    // to make it easy to calculate our starting position.
    const Sint32 meanXf = round((maxXf - minXf) / 2 / 16) * 16;
    const Sint32 meanYf = round((maxYf - minYf) / 2 / 16) * 16;

    // Translate vertex coordinates with respect to the center of the bounding
    // box, and scale to fixed point.
    Sint32 f1x = round(v1.position.x * 16) - meanXf;
    Sint32 f1y = round(v1.position.y * 16) - meanYf;
    Sint32 f2x = round(v2.position.x * 16) - meanXf;
    Sint32 f2y = round(v2.position.y * 16) - meanYf;
    Sint32 f3x = round(v3.position.x * 16) - meanXf;
    Sint32 f3y = round(v3.position.y * 16) - meanYf;

    // Calculate starting position for iteration. It's the top-left of our
    // bounding box with respect to the center of the bounding box. We add a
    // half-pixel on each axis to match hardware renderers, which evaluate at the
    // center of pixels.
    const Sint32 px = minXf - meanXf + 8;
    const Sint32 py = minYf - meanYf + 8;

    // Calculate barycentric coordinates at starting position. Barycentric
    // coordinates tell us the position of a point with respect to the
    // edges/vertices of a triangle: we can easily use these to calculate if
    // a point is inside a triangle (the three barycentric coordinates will all
    // be positive) and how to interpolate vertex attributes (multiply them by
    // the normalized barycentric coordinates at that point.)
    Sint32 w1 = (f3x - f2x) * (py - f2y) - (f3y - f2y) * (px - f2x);
    Sint32 w2 = (f1x - f3x) * (py - f3y) - (f1y - f3y) * (px - f3x);
    Sint32 w3 = (f2x - f1x) * (py - f1y) - (f2y - f1y) * (px - f1x);

    // Calculate the normalization factor for transforming our barycentric
    // coordinates into interpolation constants. If it's negative, then the
    // triangle is back-facing (wound the wrong way), and we flip two vertices
    // to make it front-facing. Keep this factor as a float since 1) we'll be
    // dividing by it later 2) we don't lose precision going through the raster
    // loop.
    float normalization = (w1 + w2 + w3);
    if (normalization < 0) {
        GEOM_Vertex vswap = v3;
        v3 = v2;
        v2 = vswap;

        Sint32 fxswap = f3x;
        f3x = f2x;
        f2x = fxswap;
        Sint32 fyswap = f3y;
        f3y = f2y;
        f2y = fyswap;

        Sint32 wswap = w3;
        w3 = -w2;
        w2 = -wswap;
        w1 = -w1;

        normalization = -normalization;
    }

    // We deal with shared edges between triangles by defining a fill rule: only
    // edges on the top or left of the triangle will be filled. We could change
    // the comparison in the loop below to differentiate between greater-than
    // and greater-or-equal-than, but since we're in fixed point space where
    // everything is an integer we instead add a bias to each barycentric
    // coordinate corresponding to a non-top, non-left edge.
    const int bias1 = ((f3y == f2y && f3x > f2x) || f3x < f2x) ? 0 : -1;
    const int bias2 = ((f3y == f1y && f1x > f3x) || f1x < f3x) ? 0 : -1;
    const int bias3 = ((f2y == f1y && f2x > f1x) || f2x < f1x) ? 0 : -1;
    w1 += bias1;
    w2 += bias2;
    w3 += bias3;

    // As we go through each pixel, we use the barycentric coordinates to check
    // if they're covered by the triangle. We could recalculate them every time,
    // but since they're linear we can calculate the linear factors with respect
    // to advancing through columns and rows and just add each time through the
    // loop. We multiply to get from subpixel space back to pixel space, since
    // we'll be iterating pixel by pixel.
    const Sint32 a1 = (f2y - f3y) * 16;
    const Sint32 a2 = (f3y - f1y) * 16;
    const Sint32 a3 = (f1y - f2y) * 16;
    const Sint32 b1 = (f3x - f2x) * 16;
    const Sint32 b2 = (f1x - f3x) * 16;
    const Sint32 b3 = (f2x - f1x) * 16;

    // Save the original texture color and alpha mod here, since we change it
    // according to vertex attributes and need to return it to its original state
    // afterwards.
    Uint8 original_mod_r, original_mod_g, original_mod_b, original_mod_a;
    if (texture) {
        SDL_GetTextureColorMod(texture, &original_mod_r, &original_mod_g, &original_mod_b);
        SDL_GetTextureAlphaMod(texture, &original_mod_a);
    }

    // Store texture width and height for use in mapping vertex attributes
    int texture_width = 0, texture_height = 0;
    if (texture) {
        SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);
    }

    // Precalculate normalized vertex attributes. We just need to multiply these
    // by the barycentric coordinates and sum them to get the interpolated vertex
    // attribute for any point. This can save a few frames per second.
    const float col1r = v1.color.r / normalization;
    const float col1g = v1.color.g / normalization;
    const float col1b = v1.color.b / normalization;
    const float col1a = v1.color.a / normalization;
    const float col2r = v2.color.r / normalization;
    const float col2g = v2.color.g / normalization;
    const float col2b = v2.color.b / normalization;
    const float col2a = v2.color.a / normalization;
    const float col3r = v3.color.r / normalization;
    const float col3g = v3.color.g / normalization;
    const float col3b = v3.color.b / normalization;
    const float col3a = v3.color.a / normalization;
    const float v1u = v1.tex_coord.x * texture_width / normalization;
    const float v1v = v1.tex_coord.y * texture_height / normalization;
    const float v2u = v2.tex_coord.x * texture_width / normalization;
    const float v2v = v2.tex_coord.y * texture_height / normalization;
    const float v3u = v3.tex_coord.x * texture_width / normalization;
    const float v3v = v3.tex_coord.y * texture_height / normalization;

    // If the triangle is uniformly-colored, we can get a big speed up by setting
    // the color once and drawing batches of rows, rather than drawing individually
    // colored pixels. Avoid malloc and a dynamic buffer size since it's slower
    // than just grabbing space from the stack.
    SDL_bool isUniformColor = SDL_FALSE;
    SDL_Rect rectsbuffer[1024];
    int rects_i = 0;
    if (!texture && GEOM_ColorEquals(v1.color, v2.color) && GEOM_ColorEquals(v1.color, v3.color)) {
        isUniformColor = SDL_TRUE;
        SDL_SetRenderDrawColor(renderer, v1.color.r, v1.color.g, v1.color.b, v1.color.a);
    }

    // Iterate over all pixels in the bounding box.
    for (int y = minYf / 16; y <= maxYf / 16; y++) {
        // Stash barycentric coordinates at start of row
        Sint32 w1_row = w1;
        Sint32 w2_row = w2;
        Sint32 w3_row = w3;

        // Keep track of where the triangle starts on this row, as an optimization
        // to draw uniformly-colored triangles.
        SDL_bool in_triangle = SDL_FALSE;
        int x_start;

        for (int x = minXf / 16; x <= maxXf / 16; x++) {
            // If all barycentric coordinates are positive, we're inside the triangle
            if (w1 >= 0 && w2 >= 0 && w3 >= 0) {
                if (!in_triangle) {
                    // We draw uniformly-colored triangles row by row, so we need to keep
                    // track of where the row starts and know when it ends.
                    x_start = x;
                    in_triangle = SDL_TRUE;
                }

                if (!isUniformColor) {
                    // Fix the adjustment due to fill rule. It's incorrect when calculating
                    // interpolation values.
                    const Sint32 alpha = w1 - bias1;
                    const Sint32 beta = w2 - bias2;
                    const Sint32 gamma = w3 - bias3;

                    // Interpolate color
                    const Uint8 r = col1r * alpha + col2r * beta + col3r * gamma;
                    const Uint8 g = col1g * alpha + col2g * beta + col3g * gamma;
                    const Uint8 b = col1b * alpha + col2b * beta + col3b * gamma;
                    const Uint8 a = col1a * alpha + col2a * beta + col3a * gamma;

                    if (!texture) {
                        // Draw a single colored pixel
                        SDL_SetRenderDrawColor(renderer, r, g, b, a);
                        SDL_RenderDrawPoint(renderer, x, y);
                    } else {
                        // Copy a pixel from the source texture to the target pixel. This
                        // effectively does nearest neighbor sampling. Could probably be
                        // extended to copy from a larger rect to do bilinear sampling if
                        // needed.
                        const int u = v1u * alpha + v2u * beta + v3u * gamma;
                        const int v = v1v * alpha + v2v * beta + v3v * gamma;
                        SDL_SetTextureColorMod(texture, r, g, b);
                        SDL_SetTextureAlphaMod(texture, a);
                        SDL_Rect srcrect;
                        srcrect.x = u;
                        srcrect.y = v;
                        srcrect.w = 1;
                        srcrect.h = 1;
                        SDL_Rect destrect;
                        destrect.x = x;
                        destrect.y = y;
                        destrect.w = 1;
                        destrect.h = 1;
                        SDL_RenderCopy(renderer, texture, &srcrect, &destrect);
                    }
                }
            } else if (in_triangle) {
                // No longer in triangle, so we're done with this row.
                if (isUniformColor) {
                    // For uniformly-colored triangles, store lines so we can send them
                    // to the renderer in batches. This provides a huge speedup in most
                    // cases (even with SDL 2.0.10's built-in batching!).
                    rectsbuffer[rects_i].x = x_start;
                    rectsbuffer[rects_i].y = y;
                    rectsbuffer[rects_i].w = x - x_start;
                    rectsbuffer[rects_i].h = 1;
                    rects_i++;
                    if (rects_i == 1024) {
                        SDL_RenderFillRects(renderer, rectsbuffer, rects_i);
                        rects_i = 0;
                    }
                }
                break;
            }
            // Increment barycentric coordinates one pixel rightwards
            w1 += a1;
            w2 += a2;
            w3 += a3;
        }
        // Increment barycentric coordinates one pixel downwards
        w1 = w1_row + b1;
        w2 = w2_row + b2;
        w3 = w3_row + b3;
    }

    if (isUniformColor) {
        SDL_RenderFillRects(renderer, rectsbuffer, rects_i);
    }

    // Restore original texture color and alpha mod.
    if (texture) {
        SDL_SetTextureColorMod(texture, original_mod_r, original_mod_g, original_mod_b);
        SDL_SetTextureAlphaMod(texture, original_mod_a);
    }
}

typedef struct GL_RenderData {
    void (APIENTRY * glEnableClientState)(GLenum cap);
    void (APIENTRY * glVertexPointer)(GLint size, GLenum type, GLsizei stride, const void *pointer);
    void (APIENTRY * glColorPointer)(GLint size, GLenum type, GLsizei stride, const void *pointer);
    void (APIENTRY * glTexCoordPointer)(GLint size, GLenum type, GLsizei stride, const void *pointer);
    void (APIENTRY * glDrawElements)(GLenum mode, GLsizei count, GLenum type, const void *indices);
    void (APIENTRY * glDrawArrays)(GLenum mode, GLint first, GLsizei count);
    void (APIENTRY * glScalef)(GLfloat x, GLfloat y, GLfloat z);
    void (APIENTRY * glPushMatrix)(void);
    void (APIENTRY * glPopMatrix)(void);
} GL_RenderData;

static SDL_bool LoadOpenGL(GL_RenderData *data) {
    // We need to load functions per-context on Windows. We don't have a simple
    // way to do that / store the functions, so just load them every time.
    // TODO: some drivers can return non-null even if function isn't available!
#define LOAD_GL_FUNC(name) \
    data->name = (typeof(data->name)) SDL_GL_GetProcAddress("" #name); \
    if (!data->name) { \
        return SDL_FALSE; \
    }

    LOAD_GL_FUNC(glEnableClientState);
    LOAD_GL_FUNC(glVertexPointer);
    LOAD_GL_FUNC(glColorPointer);
    LOAD_GL_FUNC(glTexCoordPointer);
    LOAD_GL_FUNC(glDrawElements);
    LOAD_GL_FUNC(glDrawArrays);
    LOAD_GL_FUNC(glScalef);
    LOAD_GL_FUNC(glPushMatrix);
    LOAD_GL_FUNC(glPopMatrix);

#undef LOAD_GL_FUNC
    return SDL_TRUE;
}

static int FakeRenderFlush(SDL_Renderer *renderer) {
    return 0;
}

static int ForceRenderFlush(SDL_Renderer *renderer) {
    SDL_Texture *tex = SDL_CreateTexture(renderer, 0, SDL_TEXTUREACCESS_TARGET, 1, 1);
    if (!tex) {
        return -1;
    }
    SDL_Texture *old_target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, tex);
    SDL_SetRenderTarget(renderer, old_target);
    SDL_DestroyTexture(tex);
    return 0;
}

static int GEOM_RenderFlush(SDL_Renderer *renderer)
{
#if (SDL_MINOR_VERSION == 1 && SDL_PATCHLEVEL >= 10) || SDL_MINOR_VERSION > 0
    return SDL_RenderFlush(renderer);
#else
    static int (SDLCALL * s_render_flush)(SDL_Renderer *renderer) = NULL;
    if (!s_render_flush) {
        SDL_version linked;
        SDL_GetVersion(&linked);
        if ((linked.minor == 0 && linked.patch >= 10) || linked.minor > 0) {
            void *object = SDL_LoadObject(SDL_getenv("SDL_DYNAMIC_API"));
            s_render_flush = SDL_LoadFunction(object, "SDL_RenderFlush");
            if (!s_render_flush) {
                SDL_UnloadObject(object);
                object = SDL_LoadObject(NULL);
                s_render_flush = SDL_LoadFunction(object, "SDL_RenderFlush");
            }
            if (!s_render_flush) {
                SDL_UnloadObject(object);
                fprintf(stderr,
                        "SDL_Geometry: Couldn't load SDL_RenderFlush, will try "
                        "to force render flushes by switching targets. This might"
                        " be slow.\n");
                s_render_flush = &ForceRenderFlush;
            }
        } else {
            s_render_flush = &FakeRenderFlush;
        }
    }
    return s_render_flush(renderer);
#endif
}

static int GL_RenderGeometry(GL_RenderData *data, SDL_Renderer *renderer,
                             SDL_Texture *texture, GEOM_Vertex *vertices,
                             int num_vertices, Uint16 *indices, int num_indices)
{
    // SDL: GL_RenderData *data = (GL_RenderData *) renderer->driverdata;

    // cheating way to activate renderer, bind texture, enable correct shader, etc
    // TODO: better place to put this on screen? maybe do a blend mode that makes it not exist? or transparency?
    // TODO: watch out for clip rect discarding this?
    if (texture) {
        SDL_Rect srcrect = {.x = 0, .y = 0, .w = 1, .h = 1};
        SDL_Rect destrect = {.x = 0, .y = 0, .w = 1, .h = 1};
        SDL_RenderCopy(renderer, texture, &srcrect, &destrect);
    } else {
        SDL_Rect destrect = {.x = 0, .y = 0, .w = 1, .h = 1};
        SDL_RenderFillRect(renderer, &destrect);
    }
    GEOM_RenderFlush(renderer);

    // SDL applies scale in software before it even gets to the render backends,
    // so we have to do it here
    float scale_x, scale_y;
    SDL_RenderGetScale(renderer, &scale_x, &scale_y);
    data->glPushMatrix();
    data->glScalef(scale_x, scale_y, 1.0);

    // TODO: disable these afterwards
    if (texture) {
        data->glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        data->glTexCoordPointer(2, GL_FLOAT, sizeof(GEOM_Vertex), &vertices[0].tex_coord);
    }
    data->glEnableClientState(GL_VERTEX_ARRAY);
    data->glEnableClientState(GL_COLOR_ARRAY);
    data->glVertexPointer(2, GL_FLOAT, sizeof(GEOM_Vertex), &vertices[0].position);
    data->glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(GEOM_Vertex), &vertices[0].color);

    if (indices) {
        data->glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, indices);
    } else {
        data->glDrawArrays(GL_TRIANGLES, 0, num_vertices);
    }

    data->glPopMatrix();

    return 0;
}

// SDL: in SDL_render.c
#define CHECK_RENDERER_MAGIC(renderer, retval) \
    SDL_assert(renderer); \
    if (!renderer) { \
        SDL_SetError("Invalid renderer"); \
        return retval; \
    }

// SDL: in SDL_render.c
#define CHECK_TEXTURE_MAGIC(texture, retval) \
    SDL_assert(texture); \
    if (!texture) { \
        SDL_SetError("Invalid texture"); \
        return retval; \
    }

int GEOM_RenderGeometry(SDL_Renderer *renderer, SDL_Texture *texture,
                        GEOM_Vertex *vertices, int num_vertices, Uint16 *indices,
                        int num_indices)
{
    CHECK_RENDERER_MAGIC(renderer, -1);

    if (texture) {
        CHECK_TEXTURE_MAGIC(texture, -1);
        // SDL:
        // if (renderer != texture->renderer) {
        //     SDL_SetError("Texture was not created with this renderer");
        //     return -1;
        // }
    }

    SDL_assert(vertices);
    if (!vertices) {
        SDL_SetError("RenderGeometry: vertices parameter can not be null");
        return -1;
    }

    if (indices) {
        SDL_assert(num_indices % 3 == 0);
        if (num_indices % 3 != 0) {
            SDL_SetError("RenderGeometry: num_indices parameter must be a multiple of 3");
            return -1;
        }
    } else {
        SDL_assert(num_vertices % 3 == 0);
        if (num_vertices % 3 != 0) {
            SDL_SetError("RenderGeometry: num_vertices parameter must be a multiple of 3 when not using indices");
            return -1;
        }
    }

    SDL_RendererInfo renderer_info;
    SDL_GetRendererInfo(renderer, &renderer_info);
    if (SDL_strcmp(renderer_info.name, "opengl") == 0) {
        GL_RenderData data;
        if (LoadOpenGL(&data)) {
            return GL_RenderGeometry(&data, renderer, texture, vertices,
                                     num_vertices, indices, num_indices);
        }
    }

    for (int i = 0; i + 3 <= (indices ? num_indices : num_vertices); i += 3) {
        DrawTriangle(renderer, texture,
                     vertices[indices ? indices[i] : i],
                     vertices[indices ? indices[i+1] : i+1],
                     vertices[indices ? indices[i+2] : i+2]);
    }

    return 0;
}