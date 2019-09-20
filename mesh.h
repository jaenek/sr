#ifndef _MESH_H
#define _MESH_H

#include "tri.h"

typedef struct mesh // object made of triangles
{
	tri *tris;
	int size;
} mesh;

int meshFromFile(mesh *object, char *filename);

#endif
