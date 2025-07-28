#include <stdio.h>
#include <stdbool.h>

#include "resource.h"
#include "quicksine.h"
#include "./typedef/sys/typesystem.h"
#include "./typedef/types/shape.h"

int main() {

    ensureAllPathsExist();
    m_quicksine_init();

    {
        t_typedef_init();

        t_shape_register();

        t_typedef_lock();
    }

    FunID id = FID_DEF_CREATE;
    LocalFunID local = t_getlocalfunfromfun(id);
    TypeID type = t_gettypefromfun(id);

    //print hex values of id local and type
    printf("id: %x\n", id);
    printf("local: %x\n", local);
    printf("type: %x\n", type);

    TypeInstance shape = fw_def_create(TID_SHAPE);

    T_Shape* shape_data = fw_shape_getself(&shape);

    shape_data->x = 23;
    shape_data->y = 42;
    shape_data->w = 10;
    shape_data->h = 50;

    getchar();
    return 0;
}