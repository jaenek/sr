#include "tri.h"

void drawLine(int x1, int y1, int x2, int y2, SDL_Color col)
{
	int *p = (int *) surface->pixels;

	int dx = abs(x2-x1), sx = x1<x2 ? 1 : -1;
	int dy = abs(y2-y1), sy = y1<y2 ? 1 : -1;
	int err = (dx>dy ? dx : -dy)/2, e2;

	for(;;){
		p[y1*W+x1] = SDL_MapRGB(surface->format, col.r, col.g, col.b);
		if (x1==x2 && y1==y2) {
			break;
		}

		e2 = err;
		if (e2 >-dx) {
			err -= dy;
			x1 += sx;
		}
		if (e2 < dy) {
			err += dx;
			y1 += sy;
		}
	}
}

void triDraw(int x1, int y1, int x2, int y2, int x3, int y3, SDL_Color col)
{
	drawLine(x1, y1, x2, y2, col);
	drawLine(x2, y2, x3, y3, col);
	drawLine(x3, y3, x1, y1, col);
}

void triDither(int x1, int y1, float w1, int x2, int y2, float w2,
		int x3, int y3, float w3, SDL_Color col)
{
	int *p = (int *) surface->pixels;
	Uint8 d[2][2] = {
		{ 64, 128 },
		{ 192, 0 }
	};

	if (y2 < y1) {
		swap(int, y1, y2);
		swap(int, x1, x2);
		swap(float, w1, w2);
	}

	if (y3 < y1) {
		swap(int, y1, y3);
		swap(int, x1, x3);
		swap(float, w1, w3);
	}

	if (y3 < y2) {
		swap(int, y2, y3);
		swap(int, x2, x3);
		swap(float, w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dw2 = w3 - w1;

	float w;

	float dax_step = 0.0f, dbx_step = 0.0f,
		  dw1_step = 0.0f, dw2_step = 0.0f;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1) {
		for (int i = y1; i <= y2; i++) {
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float sw = w1 + (float)(i - y1) * dw1_step;

			float ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx) {
				swap(float, ax, bx);
				swap(float, sw, ew);
			}

			w = sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++) {
				w = (1.0f - t) * sw + t * ew;

				Uint8 c = d[i%2][j%2];
				if (c < col.r) {
					c = 255;
				} else {
					c = 0;
				}

				if (w > fDepthBuffer[abs(i*W+j)]) {
					p[abs(i*W+j)] = SDL_MapRGB(surface->format,
							c, c, c);
					fDepthBuffer[abs(i*W+j)] = w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1) {
		for (int i = y2; i <= y3; i++) {
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float sw = w2 + (float)(i - y2) * dw1_step;
			float ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx) {
				swap(float, ax, bx);
				swap(float, sw, ew);
			}

			w = sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++) {
				w = (1.0f - t) * sw + t * ew;

				Uint8 c = d[i%2][j%2];
				if (c < col.r) {
					c = 255;
				} else {
					c = 0;
				}

				if (w > fDepthBuffer[abs(i*W+j)]) {
					p[abs(i*W+j)] = SDL_MapRGB(surface->format,
							c, c, c);
					fDepthBuffer[abs(i*W+j)] = w;
				}
				t += tstep;
			}
		}
	}
}

void triFill(int x1, int y1, float w1, int x2, int y2, float w2,
		int x3, int y3, float w3, SDL_Color col)
{
	int *p = (int *) surface->pixels;

	if (y2 < y1) {
		swap(float, y1, y2);
		swap(float, x1, x2);
		swap(float, w1, w2);
	}

	if (y3 < y1) {
		swap(float, y1, y3);
		swap(float, x1, x3);
		swap(float, w1, w3);
	}

	if (y3 < y2) {
		swap(float, y2, y3);
		swap(float, x2, x3);
		swap(float, w2, w3);
	}

	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dw1 = w2 - w1;

	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dw2 = w3 - w1;

	float w;

	float dax_step = 0.0f, dbx_step = 0.0f,
		  dw1_step = 0.0f, dw2_step = 0.0f;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);

	if (dy1) {
		for (int i = y1; i <= y2; i++) {
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float sw = w1 + (float)(i - y1) * dw1_step;

			float ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx) {
				swap(float, ax, bx);
				swap(float, sw, ew);
			}

			w = sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++) {
				w = (1.0f - t) * sw + t * ew;
				if (w > fDepthBuffer[abs(i*W+j)]) {
					p[abs(i*W+j)] = SDL_MapRGB(surface->format,
							col.r, col.g, col.b);
					fDepthBuffer[abs(i*W+j)] = w;
				}
				t += tstep;
			}

		}
	}

	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dw1 = w3 - w2;

	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);

	if (dy1) {
		for (int i = y2; i <= y3; i++) {
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;

			float sw = w2 + (float)(i - y2) * dw1_step;
			float ew = w1 + (float)(i - y1) * dw2_step;

			if (ax > bx) {
				swap(float, ax, bx);
				swap(float, sw, ew);
			}

			w = sw;

			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;

			for (int j = ax; j < bx; j++) {
				w = (1.0f - t) * sw + t * ew;

				if (w > fDepthBuffer[abs(i*W+j)]) {
					p[abs(i*W+j)] = SDL_MapRGB(surface->format,
							col.r, col.g, col.b);
					fDepthBuffer[abs(i*W+j)] = w;
				}
				t += tstep;
			}
		}
	}
}

/* Vector distance */
float dist(vec3 pp, vec3 pn, vec3 p) {
	return pn.x * p.x + pn.y * p.y + pn.z * p.z - vecDotProduct(&pn, &pp);
}

/* this needs rewrite */
int triClipAgainstPlane(vec3 plane_p, vec3 plane_n,
		tri *in_tri, tri *out_tri1, tri *out_tri2) {
	plane_n = vecNormalise(&plane_n);

	vec3 inside_points[3] = { 0.0f };  int nInsidePointCount = 0;
	vec3 outside_points[3] = { 0.0f }; int nOutsidePointCount = 0;

	float d0 = dist(plane_n, plane_p, in_tri->p[0]);
	float d1 = dist(plane_n, plane_p, in_tri->p[1]);
	float d2 = dist(plane_n, plane_p, in_tri->p[2]);

	if (d0 >= 0) {
		inside_points[nInsidePointCount++] = in_tri->p[0];
	}
	else {
		outside_points[nOutsidePointCount++] = in_tri->p[0];
	}
	if (d1 >= 0) {
		inside_points[nInsidePointCount++] = in_tri->p[1];
	}
	else {
		outside_points[nOutsidePointCount++] = in_tri->p[1];
	}
	if (d2 >= 0) {
		inside_points[nInsidePointCount++] = in_tri->p[2];
	}
	else {
		outside_points[nOutsidePointCount++] = in_tri->p[2];
	}

	/* triangle off the screen */
	if (nInsidePointCount == 0) {
		return 0;
	}

	/* triangle fully on the screen */
	if (nInsidePointCount == 3) {
		out_tri1->col =  in_tri->col;

		out_tri1->p[0] = in_tri->p[0];
        out_tri1->p[1] = in_tri->p[1];
		out_tri1->p[2] = in_tri->p[2];
		return 1;
	}

	/* triangle becomes smaller */
	if (nInsidePointCount == 1 && nOutsidePointCount == 2) {
		out_tri1->col =  in_tri->col;

		out_tri1->p[0] = inside_points[0];

		out_tri1->p[1] = vecIntersectPlane(&plane_p, &plane_n,
				&inside_points[0], &outside_points[0]);
		out_tri1->p[2] = vecIntersectPlane(&plane_p, &plane_n,
				&inside_points[0], &outside_points[1]);

		return 1;
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		out_tri1->col =  in_tri->col;

		out_tri2->col =  in_tri->col;

		out_tri1->p[0] = inside_points[0];
		out_tri1->p[1] = inside_points[1];
		out_tri1->p[2] = vecIntersectPlane(&plane_p, &plane_n,
				&inside_points[0], &outside_points[0]);

		out_tri2->p[0] = inside_points[1];
		out_tri2->p[1] = out_tri1->p[2];
		out_tri2->p[2] = vecIntersectPlane(&plane_p, &plane_n,
				&inside_points[1], &outside_points[0]);

		return 2;
	}

	return 0;
}
