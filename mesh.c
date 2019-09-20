#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mesh.h"
#include "vec.h"

int meshFromFile(mesh *object, char *filename)
{
	FILE *file;
	char buffer[256], *ptr;

	file = fopen(filename, "r");
	if (file == NULL) {
		printf("%s file does not exist!\n",filename);
		return 1;
	}

	vec3 *vert = NULL, v;
	tri t;
	int n, V = 0, T = 0;
	int f[3] = { 0 };
	v.w = 1.0f;
	while(fgets(buffer,sizeof(buffer),file)) {
		n = 1;
		ptr = buffer;
		switch (buffer[0]) {
            case 'v': // vertex
				sscanf(ptr += n, "%f%n", &v.x, &n);
				sscanf(ptr += n, "%f%n", &v.y, &n);
				sscanf(ptr += n, "%f%n", &v.z, &n);
				vert = realloc(vert, ++V * sizeof(*vert));
				vert[V-1] = v;
            break;
            case 'f': // faces
				sscanf(ptr += n, "%d%n", &f[0], &n);
				sscanf(ptr += n, "%d%n", &f[1], &n);
				sscanf(ptr += n, "%d%n", &f[2], &n);
				object->tris = realloc(object->tris, ++T * sizeof(*object->tris));
				t.p[0] = vert[f[0]-1];
				t.p[1] = vert[f[1]-1];
				t.p[2] = vert[f[2]-1];
				memmove(&object->tris[T-1], &t, sizeof(t));
				object->size = T;
			break;
		}
	}

	fclose(file);
	free(vert);
	printf("mesh loaded succesfuly from file: %s\n",filename);
	return 0;
}
