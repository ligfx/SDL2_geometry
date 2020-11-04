#include "SDL_geometry.h"

// TODO: put into SDL_pixels.h and add ref in SDL_Color documentation
SDL_FORCE_INLINE SDL_bool GEOM_ColorEquals(SDL_Color left, SDL_Color right)
{
    return left.r == right.r && left.g == right.g && left.b == right.b && left.a == right.a;
}

static int DrawTriangle(SDL_Renderer * renderer, SDL_Texture *texture,
                        GEOM_Vertex v1, GEOM_Vertex v2, GEOM_Vertex v3)
{
    // This function operates in s28.4 fixed point: it's more precise than
    // floating point and often faster. This also effectively lets us work in
    // a subpixel space where each pixel is divided into 256 subpixels.
    // TODO: check for overflow

    // Find integral bounding box, scale to fixed point. We use this to
    // iterate over all pixels possibly covered by the triangle.
    const Sint32 minXf = SDL_floor(SDL_min(v1.position.x, SDL_min(v2.position.x, v3.position.x))) * 16;
    const Sint32 minYf = SDL_floor(SDL_min(v1.position.y, SDL_min(v2.position.y, v3.position.y))) * 16;
    const Sint32 maxXf = SDL_ceil(SDL_max(v1.position.x, SDL_max(v2.position.x, v3.position.x))) * 16;
    const Sint32 maxYf = SDL_ceil(SDL_max(v1.position.y, SDL_max(v2.position.y, v3.position.y))) * 16;

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
    return 0;
}

int GEOM_RenderGeometry(SDL_Renderer *renderer, SDL_Texture *texture,
                        GEOM_Vertex *vertices, int num_vertices, int *indices,
                        int num_indices)
{
    // TODO: check arguments are valid
    if (indices) {
        for (int i = 0; i + 3 <= num_indices; i += 3) {
            // TODO: check return value
            DrawTriangle(renderer, texture, vertices[indices[i]],
                         vertices[indices[i+1]], vertices[indices[i+2]]);
        }
    } else {
        for (int i = 0; i + 3 <= num_vertices; i += 3) {
            // TODO: check return value
            DrawTriangle(renderer, texture, vertices[i], vertices[i+1], vertices[i+2]);
        }
    }
    
    
    return 0;
}