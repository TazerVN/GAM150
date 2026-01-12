#include "Mesh_factory.h"

AEGfxVertexList* createMesh_triangular_rectangle(MESH_MODE mode) {
    AEGfxVertexList* targetMesh;
    AEGfxMeshStart();
    if (mode == MESH_CENTER) {
        AEGfxTriAdd(
            -0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
            0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
            -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

        AEGfxTriAdd(
            0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
            0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
            -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
    }
    else if (mode == MESH_CORNER) {
        AEGfxTriAdd(
            0.f, 0.f, 0xFFFFFFFF, 0.0f, 0.0f,
            0.f, -1.f, 0xFFFFFFFF, 0.0f, 1.0f,
            1.f, -1.f, 0xFFFFFFFF, 1.0f, 1.0f);

        AEGfxTriAdd(
            0.f, 0.f, 0xFFFFFFFF, 0.0f, 0.0f,
            1.f, 0.f, 0xFFFFFFFF, 1.0f, 0.0f,
            1.f, -1.f, 0xFFFFFFFF, 1.0f, 1.0f);
    }

    targetMesh = AEGfxMeshEnd();

    return targetMesh;
}

AEGfxVertexList* createMesh_triangular_circle() {
    AEGfxVertexList* targetMesh;
    AEGfxMeshStart();
    int total = 32;
    float angle = 2 * PI / total;
    for (int i = 1; i <= total; ++i) {

        float radius = 0.5f;

        float angle_v1 = angle * (i - 1);
        float angle_v2 = angle * i;

        float v1x, v1y;
        float v2x, v2y;

        v1x = 0.0f + radius * cosf(angle_v1);
        v1y = 0.0f + radius * sinf(angle_v1);

        v2x = 0.0f + radius * cosf(angle_v2);
        v2y = 0.0f + radius * sinf(angle_v2);

        AEGfxTriAdd(
            0.f, 0.f, 0xFFFFFFFF, 0.0f, 1.0f,
            v1x, v1y, 0xFFFFFFFF, 0.0f, 0.0f,
            v2x, v2y, 0xFFFFFFFF, 1.0f, 1.0f);
    }
    targetMesh = AEGfxMeshEnd();

    return targetMesh;
}

AEGfxVertexList* createMesh_line() {
    AEGfxVertexList* targetMesh;
    AEGfxMeshStart();

    AEGfxVertexAdd(0.f,0.f,0xFFFFFF,0.f,0.f);
    AEGfxVertexAdd(1.f, 0.f, 0xFFFFFF, 1.f, 0.f);

    targetMesh = AEGfxMeshEnd();
    return targetMesh;
}

void mesh_factory_free(AEGfxVertexList* free_target) {
    if (free_target != nullptr) AEGfxMeshFree(free_target);
}