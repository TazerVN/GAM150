#pragma once
#ifndef MESH_FACTORY_HEADER
#define MESH_FACTORY_HEADER
#include "AEEngine.h"
#include "ecs.h"
#include "vector"


typedef enum { MESH_CENTER = 0, MESH_CORNER = 1 } MESH_POS;
typedef enum { TEXTURE = 1, COLOR = 0 } RENDER_MODE;
typedef enum { MESH_RECTANGLE_CENTER = 0, MESH_RECTANGLE_CORNER ,MESH_CIRCLE, MESH_LINE } MESH_TYPE;

class MeshFactory{
	private:
	std::vector<AEGfxVertexList*> mesh_arr;
	public:
	MeshFactory();
	void MeshFactoryInit();
	AEGfxVertexList* MeshGet(MESH_TYPE type);
	void MeshFree();
};

class MeshComponent : public ECSystem::Component
{
	private:
	AEGfxVertexList* mesh = nullptr;
	public:
	RENDER_MODE r_mode;
	MESH_TYPE type;
	MeshComponent(MeshFactory& factory, MESH_TYPE type, RENDER_MODE r);
	AEGfxVertexList* MeshGet();
};


AEGfxVertexList* createMesh_triangular_circle();
AEGfxVertexList* createMesh_triangular_rectangle(MESH_POS mode);
AEGfxVertexList* createMesh_line();

void mesh_factory_free(AEGfxVertexList* free_target);
#endif // !MESH_FACTORY_HEADER
