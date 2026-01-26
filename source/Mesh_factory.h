#pragma once
#ifndef MESH_FACTORY_HEADER
#define MESH_FACTORY_HEADER
#include "AEEngine.h"

typedef enum { MESH_CENTER = 0, MESH_CORNER = 1 } MESH_MODE;

typedef enum { TEXTURE = 1, COLOR = 0 } RENDER_MODE;

AEGfxVertexList* createMesh_triangular_rectangle(MESH_MODE mode);
AEGfxVertexList* createMesh_triangular_circle();
AEGfxVertexList* createMesh_line();

void mesh_factory_free(AEGfxVertexList* free_target);

#endif // !MESH_FACTORY_HEADER
