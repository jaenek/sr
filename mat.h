#ifndef _MAT_H
#define _MAT_H

#include "vec.h"

typedef struct mat4x4 {
	float m[4][4];
} mat4x4;

vec3 matMultiplyVector(mat4x4 *m, vec3 *i);

mat4x4 matNew();

mat4x4 matMakeIdentity();

mat4x4 matMakeRotationX(float fAngleRad);

mat4x4 matMakeRotationY(float fAngleRad);

mat4x4 matMakeRotationZ(float fAngleRad);

mat4x4 matMakeTranslation(float x, float y, float z);

mat4x4 matMakeProjection(float fFovDegrees, float fAspectRatio,
		float fNear, float fFar);

mat4x4 matMultiplyMatrix(mat4x4 *m1, mat4x4 *m2);

mat4x4 matPointAt(vec3 *pos, vec3 *target, vec3 *up);

mat4x4 matQuickInverse(mat4x4 *m);

#endif
