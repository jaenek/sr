#include <math.h>
#include <string.h>

#include "mat.h"

vec3 matMultiplyVector(mat4x4 *m, vec3 *i)
{
	vec3 v;
	v.x = i->x * m->m[0][0] + i->y * m->m[1][0]
		+ i->z * m->m[2][0] + i->w * m->m[3][0];
	v.y = i->x * m->m[0][1] + i->y * m->m[1][1]
		+ i->z * m->m[2][1] + i->w * m->m[3][1];
	v.z = i->x * m->m[0][2] + i->y * m->m[1][2]
		+ i->z * m->m[2][2] + i->w * m->m[3][2];
	v.w = i->x * m->m[0][3] + i->y * m->m[1][3]
		+ i->z * m->m[2][3] + i->w * m->m[3][3];
	return v;
}

mat4x4 matMakeIdentity()
{
	mat4x4 matrix;
	memset(&matrix, 0.0f, sizeof(matrix));
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 matMakeRotationX(float fAngleRad)
{
	mat4x4 matrix;
	memset(&matrix, 0.0f, sizeof(matrix));
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[1][2] = sinf(fAngleRad);
	matrix.m[2][1] = -sinf(fAngleRad);
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 matMakeRotationY(float fAngleRad)
{
	mat4x4 matrix;
	memset(&matrix, 0.0f, sizeof(matrix));
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][2] = sinf(fAngleRad);
	matrix.m[2][0] = -sinf(fAngleRad);
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = cosf(fAngleRad);
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 matMakeRotationZ(float fAngleRad)
{
	mat4x4 matrix;
	memset(&matrix, 0.0f, sizeof(matrix));
	matrix.m[0][0] = cosf(fAngleRad);
	matrix.m[0][1] = sinf(fAngleRad);
	matrix.m[1][0] = -sinf(fAngleRad);
	matrix.m[1][1] = cosf(fAngleRad);
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 matMakeTranslation(float x, float y, float z)
{
	mat4x4 matrix;
	memset(&matrix, 0.0f, sizeof(matrix));
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	matrix.m[3][0] = x;
	matrix.m[3][1] = y;
	matrix.m[3][2] = z;
	return matrix;
}

mat4x4 matMakeProjection(float fFovDegrees, float fAspectRatio,
		float fNear, float fFar)
{
	float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
	mat4x4 matrix;
	memset(&matrix, 0.0f, sizeof(matrix));
	matrix.m[0][0] = fAspectRatio * fFovRad;
	matrix.m[1][1] = fFovRad;
	matrix.m[2][2] = fFar / (fFar - fNear);
	matrix.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matrix.m[2][3] = 1.0f;
	matrix.m[3][3] = 0.0f;
	return matrix;
}

mat4x4 matMultiplyMatrix(mat4x4 *m1, mat4x4 *m2)
{
	mat4x4 matrix;
	memset(&matrix, 0.0f, sizeof(matrix));
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.m[r][c] = m1->m[r][0] * m2->m[0][c]
							+ m1->m[r][1] * m2->m[1][c]
							+ m1->m[r][2] * m2->m[2][c]
							+ m1->m[r][3] * m2->m[3][c];
	return matrix;
}

mat4x4 matPointAt(vec3 *pos, vec3 *target, vec3 *up)
{
	vec3 newForward = vecSub(target, pos);
	newForward = vecNormalise(&newForward);

	vec3 a = vecMul(&newForward, vecDotProduct(up, &newForward));
	vec3 newUp = vecSub(up, &a);
	newUp = vecNormalise(&newUp);

	vec3 newRight = vecCrossProduct(&newUp, &newForward);

	mat4x4 matrix;
	memset(&matrix, 0.0f, sizeof(matrix));
	matrix.m[0][0] = newRight.x;
	matrix.m[0][1] = newRight.y;
	matrix.m[0][2] = newRight.z;
	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = newUp.x;
	matrix.m[1][1] = newUp.y;
	matrix.m[1][2] = newUp.z;
	matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = newForward.x;
	matrix.m[2][1] = newForward.y;
	matrix.m[2][2] = newForward.z;
	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos->x;
	matrix.m[3][1] = pos->y;
	matrix.m[3][2] = pos->z;
	matrix.m[3][3] = 1.0f;
	return matrix;

}

mat4x4 matQuickInverse(mat4x4 *m)
{
	mat4x4 matrix;
	memset(&matrix, 0.0f, sizeof(matrix));
	matrix.m[0][0] = m->m[0][0]; matrix.m[0][1] = m->m[1][0];
	matrix.m[0][2] = m->m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m->m[0][1]; matrix.m[1][1] = m->m[1][1];
	matrix.m[1][2] = m->m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m->m[0][2]; matrix.m[2][1] = m->m[1][2];
	matrix.m[2][2] = m->m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m->m[3][0] * matrix.m[0][0]
			+ m->m[3][1] * matrix.m[1][0] + m->m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m->m[3][0] * matrix.m[0][1]
			+ m->m[3][1] * matrix.m[1][1] + m->m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m->m[3][0] * matrix.m[0][2]
			+ m->m[3][1] * matrix.m[1][2] + m->m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}
