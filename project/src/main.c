#include <stdio.h>
#include <stdbool.h>
#include "stdlib.h"

#include "main.h"

#include "resource.h"
#include "quicksine.h"
#include "./typedef/sys/typesystem.h"
#include "./typedef/types/entity.h"
#include "./typedef/types/component.h"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <math.h>
/*
void recalcscreen(Camera* cam,
    Vector3* tl, Vector3* tr, Vector3* br, Vector3* bl,
    Vector3* center,
    Vector2 screenSize,
    float planeZ,
    float minAspectRatio,
    float maxAspectRatio, GameObject* go)
{
    Vector2 adjustedSize = screenSize;

    float w = screenSize.x;
    float h = screenSize.y;
    float aspect = w / h;

    // Clamp aspect ratio between min and max
    if (aspect > maxAspectRatio)
    {
        // Too wide – clamp width
        w = h * maxAspectRatio;
    }
    else if (aspect < minAspectRatio)
    {
        // Too tall – clamp height
        h = w / minAspectRatio;
    }

    adjustedSize.x = w;
    adjustedSize.y = h;

    // Center the adjusted region
    Vector2 offset = {
        (screenSize.x - adjustedSize.x) * 0.5f,
        (screenSize.y - adjustedSize.y) * 0.5f
    };

    Vector2 screenCorners[4] = {
        { offset.x,                      offset.y },
        { offset.x + adjustedSize.x,     offset.y },
        { offset.x + adjustedSize.x,     offset.y + adjustedSize.y },
        { offset.x,                      offset.y + adjustedSize.y }
    };

    Vector3 points[4];
    for (int i = 0; i < 4; i++)
    {
        Ray ray = GetMouseRay(screenCorners[i], *cam);
        if (fabsf(ray.direction.z) > 1e-6f)
        {
            float t = (planeZ - ray.position.z) / ray.direction.z;
            points[i] = Vector3Add(ray.position,
                Vector3Scale(ray.direction, t));
        }
        else
        {
            points[i] = cam->position;
        }
    }

    *tl = points[0];
    *tr = points[1];
    *br = points[2];
    *bl = points[3];

    center->x = (tl->x + tr->x + br->x + bl->x) * 0.25f;
    center->y = (tl->y + tr->y + br->y + bl->y) * 0.25f;
    center->z = (tl->z + tr->z + br->z + bl->z) * 0.25f;

    go->global_position = *center;

    go->global_size = (Vector2){
        (tr->x - tl->x),
        (tr->y - br->y)
    };
    go->global_origin = (Vector3){
        go->global_size.x * 0.5f,
        go->global_size.y * 0.5f,
        0
    };

    s_gameobject_recalculatecorners(go);
}
*/




int main() {

    
    ensureAllPathsExist();
    m_quicksine_init();

    {
        t_typedef_init();

        register_ENTITY_type();
        register_COMPONENT_type();

        t_typedef_lock();
    }

    InstanceReference r = r_create(T_ENTITY);

    r_def_print(r,stdout,255);

    InstanceReference c = r_createfor(r_entity_get_component(r,T_COMPONENT,true));

    r_def_print(c,stdout,255);

    Component* comp = COMPONENT_get_self_struct(c.ptr);
    comp->kaboom=4.20f;
    comp->ss = '?';

    r_def_print(c,stdout,255);

    r_def_print(r,stdout,255); 

    MemoryBlock m = { 0 };

    r_def_serialize(r,&m);
    memory_block_dump(&m,1024,stdout);

    memory_block_seek_to_start(&m);

    InstanceReference r2 = r_def_createdeserialized(&m);

    r_def_print(r2,stdout,255);

    SaveFileData("test.magna",m.data,m.capacity);

    m.data = (char*) LoadFileData("test.magna",(int*)&m.capacity);

    memory_block_seek_to_start(&m);

    InstanceReference r3 = r_def_createdeserialized(&m);

    r_def_print(r3,stdout,255);

    getchar();

    /*

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Magna");
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60); 
    
    rlDisableBackfaceCulling();
    rlSetClipPlanes(0.01f,100000.0f);


    
    

    while (!WindowShouldClose()) {



        BeginDrawing();

            ClearBackground(WHITE);



        EndDrawing();
    }

    CloseWindow();
*/



    return 0;
}