#ifndef _TRI_H
#define _TRI_H

#include <SDL.h>

#include "defaults.h"
#include "vec.h"

/* Swap two bytes */
#define swap(type, i, j) {type t = i; i = j; j = t;}

/* triangle primitive */
typedef struct tri
{
	vec3 p[3];
	vec2 t[3];
	SDL_Color col;
} tri;

SDL_Surface *surface;
float fDepthBuffer[W*H];

void drawLine(int x1, int y1, int x2, int y2, SDL_Color col);

void triDraw(int x1, int y1, int x2, int y2, int x3, int y3, SDL_Color col);

void triDither(int x1, int y1, float w1, int x2, int y2, float w2,
		int x3, int y3, float w3, SDL_Color col);

void triFill(int x1, int y1, float w1, int x2, int y2, float w2,
		int x3, int y3, float w3, SDL_Color col);

int triClipAgainstPlane(vec3 plane_p, vec3 plane_n, tri *in_tri,
		tri *out_tri1, tri *out_tri2);

#endif
