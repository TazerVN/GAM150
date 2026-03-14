#pragma once

#include "../types.h"
#include <vector>
#include "AEEngine.h"


class MeshFactory{
	private:
	std::vector<AEGfxVertexList*> mesh_arr;
	public:
	MeshFactory();
	void MeshFactoryInit();
	AEGfxVertexList* MeshGet(MESH_TYPE type);
	void MeshFree();
};



AEGfxVertexList* createMesh_triangular_circle();
AEGfxVertexList* createMesh_triangular_rectangle(MESH_POS mode);
AEGfxVertexList* createMesh_line();

void mesh_factory_free(AEGfxVertexList* free_target);
