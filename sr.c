#include <SDL.h>
#include <time.h>

#include "defaults.h"
#include "vec.h"
#include "tri.h"
#include "mesh.h"
#include "mat.h"

mesh object;
mat4x4 matProj;
vec3 vCamera = { 0.0f, 0.0f, 0.0f, 1.0f };
vec3 vLookDir = { 0.0f, 0.0f, 0.0f, 1.0f };
float fYaw = 0.0f;

void loadData()
{
	meshFromFile(&object, "objects/monkey.obj");

	/* set up projection matrix */
	matProj = matMakeProjection(90.0f, (float)H / (float)W, 0.1f, 1000.0f);
}

void update(float fElapsedTime)
{
	/* clear surface */
	SDL_memset(surface->pixels, 0, surface->h * surface->pitch);

	/* clear depth buffer */
	memset(&fDepthBuffer, 0, sizeof(fDepthBuffer));

	/* set up rotation matrices */
	mat4x4 matRotZ = matMakeRotationZ(fElapsedTime * 1.0f);
	mat4x4 matRotX = matMakeRotationX(fElapsedTime * 0.5f);

	/* set up translation matrix */
	mat4x4 matTrans = matMakeTranslation(0.0f, 0.0f, 6.0f);

	/* set up world transform */
	mat4x4 matWorld = matMakeIdentity();
	matWorld = matMultiplyMatrix(&matRotZ, &matRotX);
	matWorld = matMultiplyMatrix(&matWorld, &matTrans);

	/* create "point at" matrix for camera */
	vec3 vUp = { 0.0f, 1.0f, 0.0f, 1.0f };
	vec3 vTarget = { 0.0f, 0.0f, 1.0f, 1.0f };
	mat4x4 matCameraRot = matMakeRotationY(fYaw);
	vLookDir = matMultiplyVector(&matCameraRot, &vTarget);
	vTarget = vecAdd(&vCamera, &vLookDir);
	mat4x4 matCamera = matPointAt(&vCamera, &vTarget, &vUp);

	/* make view matrix from camera */
	mat4x4 matView = matQuickInverse(&matCamera);

	/* draw triangles */
	for (tri *t = object.tris; t < object.tris + object.size; ++t) {
		tri triTrans, triView;

		/* world Matrix Transform */
		triTrans.p[0] = matMultiplyVector(&matWorld, &(t->p[0]));
		triTrans.p[1] = matMultiplyVector(&matWorld, &(t->p[1]));
		triTrans.p[2] = matMultiplyVector(&matWorld, &(t->p[2]));

		/* calculate triangle normal */
		vec3 normal = { 0.0f, 0.0f, 0.0f, 1.0f },
			 line1 =  { 0.0f, 0.0f, 0.0f, 1.0f },
			 line2 =  { 0.0f, 0.0f, 0.0f, 1.0f };

		/* get lines either side of triangle */
		line1 = vecSub(&(triTrans.p[1]), &triTrans.p[0]);
		line2 = vecSub(&(triTrans.p[2]), &triTrans.p[0]);

		/* take cross product of lines to get normal to triangle surface */
		normal = vecCrossProduct(&line1, &line2);

		/* normalise a normal */
		normal = vecNormalise(&normal);

		/* cast a ray from triangle to camera */
		vec3 vCameraRay = { 0.0f, 0.0f, 0.0f, 1.0f };
		vCameraRay = vecSub(&triTrans.p[0], &vCamera);

		/* if ray is aligned with normal, then triangle is visible */
		if (vecDotProduct(&normal, &vCameraRay) < 0.0f) {
			tri triProj;

			/* convert world space --> view space */
			triView.p[0] = matMultiplyVector(&matView, &triTrans.p[0]);
			triView.p[1] = matMultiplyVector(&matView, &triTrans.p[1]);
			triView.p[2] = matMultiplyVector(&matView, &triTrans.p[2]);

			/* illumination */
			vec3 light_direction = { 0.0f, 1.0f, -1.0f, 1.0f };
			light_direction = vecNormalise(&light_direction);

			/* acquire similarity of light direction and surface normal */
			float d = fmax(0.1f, vecDotProduct(&light_direction, &normal));

			/* calculate RGB color */
			uint8_t c = (uint8_t) (0xFF * d);
			triView.col = (SDL_Color) { c, c/2, c/3 };

			// Clip Viewed Triangle against near plane, this could form two additional
			// additional triangles.
			int nClippedTriangles = 0;
			tri clipped[2];
			nClippedTriangles = triClipAgainstPlane(
					(vec3){ 0.0f, 0.0f, 0.1f, 1.0f },
					(vec3){ 0.0f, 0.0f, 1.0f, 1.0f },
					&triView, &clipped[0], &clipped[1]);

			// We may end up with multiple triangles form the clip, so project as
			// required
			for (int n = 0; n < nClippedTriangles; n++) {
				// Project triangles from 3D --> 2D
				triProj.p[0] = matMultiplyVector(&matProj, &clipped[n].p[0]);
				triProj.p[1] = matMultiplyVector(&matProj, &clipped[n].p[1]);
				triProj.p[2] = matMultiplyVector(&matProj, &clipped[n].p[2]);
				triProj.col = clipped[n].col;

				triProj.t[0].w = 1.0f / triProj.p[0].w;
				triProj.t[1].w = 1.0f / triProj.p[1].w;
				triProj.t[2].w = 1.0f / triProj.p[2].w;

				// Scale into view, we moved the normalising into cartesian space
				// out of the matrix.vector function from the previous videos, so
				// do this manually
				triProj.p[0] = vecDiv(&triProj.p[0], triProj.p[0].w);
				triProj.p[1] = vecDiv(&triProj.p[1], triProj.p[1].w);
				triProj.p[2] = vecDiv(&triProj.p[2], triProj.p[2].w);

				// X/Y are inverted so put them back
				triProj.p[0].x *= -1.0f;
				triProj.p[1].x *= -1.0f;
				triProj.p[2].x *= -1.0f;
				triProj.p[0].y *= -1.0f;
				triProj.p[1].y *= -1.0f;
				triProj.p[2].y *= -1.0f;

				// Offset verts into visible normalised space
				vec3 vOffsetView = { 1.0f, 1.0f, 0.0f };
				triProj.p[0] = vecAdd(&triProj.p[0], &vOffsetView);
				triProj.p[1] = vecAdd(&triProj.p[1], &vOffsetView);
				triProj.p[2] = vecAdd(&triProj.p[2], &vOffsetView);
				triProj.p[0].x *= 0.5f * (float)W;
				triProj.p[0].y *= 0.5f * (float)H;
				triProj.p[1].x *= 0.5f * (float)W;
				triProj.p[1].y *= 0.5f * (float)H;
				triProj.p[2].x *= 0.5f * (float)W;
				triProj.p[2].y *= 0.5f * (float)H;

				//printf("w: %f, %f, %f\n", triProj.p[0].w, triProj.p[1].w, triProj.p[2].w);
				//printf("w: %f, %f, %f\n", w0, w1, w2);
				/* draw dithered triangle */
				triDither(triProj.p[0].x, triProj.p[0].y, triProj.t[0].w,
				          triProj.p[1].x, triProj.p[1].y, triProj.t[1].w,
				          triProj.p[2].x, triProj.p[2].y, triProj.t[2].w,
				          triProj.col);
			}
			/*triFill(triView.p[0].x, triView.p[0].y, triView.p[0].w,
				  triView.p[1].x, triView.p[1].y, triView.p[1].w,
				  triView.p[2].x, triView.p[2].y, triView.p[2].w,
				  triView.col);*/

		}
	}
}

int main(int argc, char *argv[])
{
	SDL_Renderer *renderer;
	SDL_Window *window;
	SDL_Texture *texture;
	SDL_Event e;
	clock_t start, stop;
	loadData();

	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

	if (SDL_CreateWindowAndRenderer(W, H, SDL_WINDOW_MAXIMIZED, &window,
				&renderer)) {
		printf("Couldn't create window and renderer: %s\n", SDL_GetError());
		return 1;
	}

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
			SDL_TEXTUREACCESS_STREAMING, W, H);
	if (!texture) {
		printf("Couldn't create texture: %s\n", SDL_GetError());
		return 1;
	}

	surface = SDL_CreateRGBSurface(0, W, H, 32, 0, 0, 0, 0);
	if (surface == NULL) {
		printf("Couldn't create surface: %s\n", SDL_GetError());
		return 1;
	}
	SDL_SetSurfaceRLE(surface, 1);

	/* start the clock */
	float fElapsedTime;
	start = clock();

	int quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT || e.key.keysym.sym == SDLK_ESCAPE) {
				quit = 1;
			}
		}

		/* update the clock */
		stop = clock();
		fElapsedTime = (float)(stop - start) / CLOCKS_PER_SEC;

		/* clear screen */
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);

		/* copy frame from buffer */
		SDL_LockTexture(texture, NULL, &surface->pixels, &surface->pitch);

		/* update surface */
		update(fElapsedTime);

		/* update video memory */
		SDL_UnlockTexture(texture);

		/* render frame */
		SDL_RenderCopy(renderer, texture, NULL, NULL);

		/* update screen */
		SDL_RenderPresent(renderer);
	}

	/* free the allocated space */
	free(object.tris);
	SDL_ShowCursor(SDL_DISABLE);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	texture  = NULL;
	renderer = NULL;
	window   = NULL;
	SDL_Quit();
	printf("program exiting successfuly, ran for: %f\n",fElapsedTime);
	return 0;
}
