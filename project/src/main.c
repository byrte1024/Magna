#include <stdio.h>
#include <stdbool.h>

#include "resource.h"
#include "quicksine.h"
#include "./typedef/sys/typesystem.h"
#include "./typedef/types/shape.h"
#include "./typedef/types/circle.h"

int main() {

    ensureAllPathsExist();
    m_quicksine_init();

    {
        t_typedef_init();

        t_shape_register();
        t_circle_register();

        t_typedef_lock();
    }


    TypeInstance shape = fw_def_create(TID_SHAPE);

    T_Shape* shape_data = fw_shape_getself(&shape);

    shape_data->x = 23;
    shape_data->y = 42;
    shape_data->w = 10;
    shape_data->h = 50;

    fw_shape_sayhi(&shape);

    TypeInstance circle = fw_def_create(TID_CIRCLE);

    T_Circle* circle_data = fw_circle_getself(&circle);

    circle_data->radius = 5;
    T_Shape* circle_shape = fw_shape_getself(&circle);

    circle_shape->x = 10;
    circle_shape->y = 20;
    circle_shape->w = 30;
    circle_shape->h = 40;

    fw_shape_sayhi(&circle);

    fw_def_print(&shape,stdout,256);
    fw_def_print(&circle,stdout,256);

    MemoryBlock mb = {0};

    fw_def_serialize(&circle, &mb);

    memory_block_dump(&mb,512,stdout);

    memory_block_seek_to_start(&mb);

    TypeInstance circle2 = fw_def_createdeserialized( &mb);

    fw_def_print(&circle2,stdout,256);


    fw_def_destroy(&shape);
    fw_def_destroy(&circle);

    getchar();
    return 0;
}