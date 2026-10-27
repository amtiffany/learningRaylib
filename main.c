// inits raylib window that draws a dodecahdron wireframe and rotates the camera based on directional keys or mouse input

#include "raylib.h"
#include "raymath.h"

#define GOLDEN_RATIO 1.6180339887498949f
#define INV_GOLDEN_RATIO 0.6180339887498949f
#define DODEC_SIDE_LENGTH sqrt(1 + pow(1 - INV_GOLDEN_RATIO, 2) + pow(1 - GOLDEN_RATIO, 2))
#define DODEC_NUM_VERTS 20
#define DODEC_NUM_EDGES 30
#define DODEC_NUM_FACES 12


// indices of vertex array corresponding to the start and end of a line segment
typedef struct {
    int startVertIndex;
    int endVertIndex;
} WireframeEdge;

typedef struct {
    int v0;
    int v1;
    int v2;
    int v3;
    int v4;
} Face;

// given an array of verts and an array of index-pairs representing edges, draw each edge as a line segment
int drawWireframe(int numVerts, Vector3 *verts, int numEdges, WireframeEdge* edges, Color color) {
    for (int i = 0; i < numEdges; i++) {
        if (numVerts < edges[i].startVertIndex || numVerts < edges[i].endVertIndex) return 0;
        DrawLine3D(verts[edges[i].startVertIndex], verts[edges[i].endVertIndex], color); 
    }

    return 1;
}


Vector3 findCenterOfFace(int numVerts, Vector3 *verts, Face f) {
    if (numVerts < f.v0 || numVerts < f.v1 || numVerts < f.v2 || numVerts < f.v3 || numVerts < f.v4) return (Vector3){0,0,0};
    float sum_x = verts[f.v0].x + verts[f.v1].x + verts[f.v2].x + verts[f.v3].x + verts[f.v4].x;
    float sum_y = verts[f.v0].y + verts[f.v1].y + verts[f.v2].y + verts[f.v3].y + verts[f.v4].y;
    float sum_z = verts[f.v0].z + verts[f.v1].z + verts[f.v2].z + verts[f.v3].z + verts[f.v4].z;

    return (Vector3){sum_x/5, sum_y/5, sum_z/5};
}



int main(void)
{
    // 16x9 aspect ratio
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "raylib dodecahedron");
    SetTargetFPS(60);

    // main camera
    // initialized as looking at (0,0,0) from (0,1,0)
    Camera3D camera = { 0 };
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f }; // where the camera is pointed at
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f }; // where the camera is located
    camera.fovy = 45.0f; // width of the camera view
    camera.projection = CAMERA_PERSPECTIVE;

    const float cameraDistance = 7.0f;
    float cameraYaw = 45.0f;
    float cameraPitch = 30.0f;


    // init verts of dodec
    static const Vector3 dodecahedronVertices[] = {
        { -1.0f,           -1.0f,           -1.0f            },
        {  1.0f,           -1.0f,           -1.0f            },
        {  1.0f,            1.0f,           -1.0f            },
        { -1.0f,            1.0f,           -1.0f            },
        { -1.0f,           -1.0f,            1.0f            },
        {  1.0f,           -1.0f,            1.0f            },
        {  1.0f,            1.0f,            1.0f            },
        { -1.0f,            1.0f,            1.0f            },
        {  0.0f,           -INV_GOLDEN_RATIO,  -GOLDEN_RATIO      },
        {  0.0f,            INV_GOLDEN_RATIO,  -GOLDEN_RATIO      },
        {  0.0f,           -INV_GOLDEN_RATIO,   GOLDEN_RATIO      },
        {  0.0f,            INV_GOLDEN_RATIO,   GOLDEN_RATIO      },
        { -INV_GOLDEN_RATIO,  -GOLDEN_RATIO,      0.0f            },
        { -INV_GOLDEN_RATIO,   GOLDEN_RATIO,      0.0f            },
        {  INV_GOLDEN_RATIO,  -GOLDEN_RATIO,      0.0f            },
        {  INV_GOLDEN_RATIO,   GOLDEN_RATIO,      0.0f            },
        { -GOLDEN_RATIO,       0.0f,           -INV_GOLDEN_RATIO  },
        {  GOLDEN_RATIO,       0.0f,           -INV_GOLDEN_RATIO  },
        { -GOLDEN_RATIO,       0.0f,            INV_GOLDEN_RATIO  },
        {  GOLDEN_RATIO,       0.0f,            INV_GOLDEN_RATIO  },
    };

    // init edges of dodec, encoded as a index pairs corresponding to the start and end verts
    static const WireframeEdge dodecahedronEdges[] = {
        { 0,   8 }, { 0,  12 }, { 0,  16 },
        { 1,   8 }, { 1,  14 }, { 1,  17 },
        { 2,   9 }, { 2,  15 }, { 2,  17 },
        { 3,   9 }, { 3,  13 }, { 3,  16 },
        { 4,  10 }, { 4,  12 }, { 4,  18 },
        { 5,  10 }, { 5,  14 }, { 5,  19 },
        { 6,  11 }, { 6,  15 }, { 6,  19 },
        { 7,  11 }, { 7,  13 }, { 7,  18 },
        { 8,   9 }, { 10, 11 }, { 12, 14 },
        { 13, 15 }, { 16, 18 }, { 17, 19 },
    };

    static const Face faces[] = {
        {0, 8, 4,14,12},
        {0,12, 1,17,16},
        {0,16, 2,10, 8},
        {4, 8,10, 6,18},
        {6,10, 2,13,15},
        {1,12,14, 5, 9},
        {1, 9,11, 3,17},
        {2,13, 3,11,10},
        {2,16,17, 3,13},
        {4,18,19, 5,14},
        {5,19, 7,11, 9},
        {6,15, 7,19,18}
    };


    Vector3 centers[] = {
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[0]),
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[1]),
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[2]),
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[3]),
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[4]),
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[5]),
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[6]),
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[7]),
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[8]),
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[9]),
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[10]),
        findCenterOfFace(DODEC_NUM_VERTS, dodecahedronVertices, faces[11])
    };




    while (!WindowShouldClose())
    {
        // check for mouse inputs and increment yaw/pitch by distance moved in x/y direction
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Vector2 delta = GetMouseDelta();
            cameraYaw += delta.x * 0.5f;
            cameraPitch += delta.y * 0.5f;
        }

        // check for key inputs and increment yaw/pitch for each frame pressed
        // L/R cooresponds to yaw; U/D to pitch
        if (IsKeyDown(KEY_LEFT)) cameraYaw -= 1.0f;
        if (IsKeyDown(KEY_RIGHT)) cameraYaw += 1.0f;
        if (IsKeyDown(KEY_UP)) cameraPitch -= 1.0f;
        if (IsKeyDown(KEY_DOWN)) cameraPitch += 1.0f;

        if (cameraPitch > 89.0f) cameraPitch = 89.0f;
        if (cameraPitch < -89.0f) cameraPitch = -89.0f;

        // convert from degrees to radians
        const float pitchRad = cameraPitch * DEG2RAD; 
        const float yawRad = cameraYaw * DEG2RAD;

        // move the camera s.t. it is looking at its target from a distance = cameraDistance
        // at an angle determined by yaw and pitch (determined by user input)
        camera.position.x = camera.target.x + cameraDistance * cosf(pitchRad) * sinf(yawRad);
        camera.position.y = camera.target.y + cameraDistance * sinf(pitchRad);
        camera.position.z = camera.target.z + cameraDistance * cosf(pitchRad) * cosf(yawRad);

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode3D(camera);

        // draw 2x2x2 red cube w/ grey outline centered at 0 
        // DrawCube(Vector3Zero(), 2.0f, 2.0f, 2.0f, MAROON);
        // DrawCubeWires(Vector3Zero(), 2.0f, 2.0f, 2.0f, DARKGRAY);
        // DrawGrid(10, 1.0f);


        // draw purple wireframe of dodecahdron
        // note: there does not exist a draw call for polyhedra more complex than a cube,
        // so this is drawn by pre-baking the coordinates of the edges and simply drawing indivudual lines
        drawWireframe(DODEC_NUM_VERTS, dodecahedronVertices, DODEC_NUM_EDGES, dodecahedronEdges, PURPLE);
        // for (int i = 0; i < DODEC_NUM_FACES; i++) {
        //     DrawPoint3D(centers[i], RED);
        // }
        

        EndMode3D();

        DrawText("Drag with mouse or use arrow keys to orbit the camera", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
