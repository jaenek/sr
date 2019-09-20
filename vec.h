#ifndef _VEC_H
#define _VEC_H

typedef struct vec3   // point in 3d space
{
	float x, y, z, w;
} vec3;

typedef struct vec2   // texture coordinate
{
	float u, v, w;
} vec2;

vec3 vecAdd(vec3 *v1, vec3 *v2);

vec3 vecSub(vec3 *v1, vec3 *v2);

vec3 vecMul(vec3 *v1, float k);

vec3 vecDiv(vec3 *v1, float k);

float vecDotProduct(vec3 *v1, vec3 *v2);

float vecLength(vec3 *v);

vec3 vecNormalise(vec3 *v);

vec3 vecCrossProduct(vec3 *v1, vec3 *v2);

vec3 vecIntersectPlane(vec3 *plane_p, vec3 *plane_n, vec3 *lineStart,
		vec3 *lineEnd);

#endif
