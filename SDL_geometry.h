#ifndef _SDL_GEOMETRY_H
#define _SDL_GEOMETRY_H

#include <SDL_pixels.h> // SDL_Color
#include <SDL_rect.h> // SDL_FPoint
#include <SDL_render.h> // SDL_Renderer, SDL_Texture

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GEOM_Vertex
{
    SDL_FPoint position;
    union {
        SDL_Color color;
        SDL_Color colour;
    };
    SDL_FPoint tex_coord;
} GEOM_Vertex;

int GEOM_RenderGeometry(SDL_Renderer * renderer, SDL_Texture *texture,
                        GEOM_Vertex *vertices, int num_vertices, int* indices,
                        int num_indices);

#ifdef __cplusplus
} // extern "C"
#endif

#endif