#include <math.h>

#include "vec.h"

vec3 vecAdd(vec3 *v1, vec3 *v2)
{
	return (vec3) { v1->x + v2->x, v1->y + v2->y, v1->z + v2->z, 1.0f };
}

vec3 vecSub(vec3 *v1, vec3 *v2)
{
	return (vec3) { v1->x - v2->x, v1->y - v2->y, v1->z - v2->z, 1.0f };
}

vec3 vecMul(vec3 *v1, float k)
{
	return (vec3) { v1->x * k, v1->y * k, v1->z * k, 1.0f };
}

vec3 vecDiv(vec3 *v1, float k)
{
	return (vec3) { v1->x / k, v1->y / k, v1->z / k, 1.0f };
}

float vecDotProduct(vec3 *v1, vec3 *v2)
{
	return v1->x*v2->x + v1->y*v2->y + v1->z * v2->z;
}

float vecLength(vec3 *v)
{
	return sqrtf(vecDotProduct(v, v));
}

vec3 vecNormalise(vec3 *v)
{
	float l = vecLength(v);
	return (vec3) { v->x / l, v->y / l, v->z / l, 1.0f };
}

vec3 vecCrossProduct(vec3 *v1, vec3 *v2)
{
	vec3 v;
	v.x = v1->y * v2->z - v1->z * v2->y;
	v.y = v1->z * v2->x - v1->x * v2->z;
	v.z = v1->x * v2->y - v1->y * v2->x;
	v.w = 1.0f;
	return v;
}

vec3 vecIntersectPlane(vec3 *plane_p, vec3 *plane_n_ptr,
		vec3 *lineStart, vec3 *lineEnd)
{
	vec3 plane_n = vecNormalise(plane_n_ptr);
	float plane_d = -vecDotProduct(&plane_n, plane_p);
	float ad = vecDotProduct(lineStart, &plane_n);
	float bd = vecDotProduct(lineEnd, &plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	vec3 lineStartToEnd = vecSub(lineEnd, lineStart);
	vec3 lineToIntersect = vecMul(&lineStartToEnd, t);
	return vecAdd(lineStart, &lineToIntersect);
}
