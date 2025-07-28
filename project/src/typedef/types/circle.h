

#pragma once

#include "../sys/typesystem.h"
#include "shape.h"

#define TID_CIRCLE 0x0002

// Struct Definition
typedef struct T_Circle {
    TypeInstance shape;
    int radius;
} T_Circle;

// Struct-only Functions
int s_circle_getRadius(T_Circle t);
void s_circle_setRadius(T_Circle* t, int radius);


// Custom Function Prototypes
void f_circle_shape_sayhi(F_SHAPE_SAYHI_PRM* prm, FunCall* call);

// Default Function Prototypes
void f_circle_def_create(F_DEF_CREATE_PRM* prm, FunCall* call);
void f_circle_def_destroy(F_DEF_DESTROY_PRM* prm, FunCall* call);
void f_circle_def_tostring(F_DEF_TOSTRING_PRM* prm, FunCall* call);
void f_circle_def_serialize(F_DEF_SERIALIZE_PRM* prm, FunCall* call);
void f_circle_def_deserialize(F_DEF_DESERIALIZE_PRM* prm, FunCall* call);
void f_circle_def_hassub(F_DEF_HASSUB_PRM* prm, FunCall* call);
void f_circle_def_getsub(F_DEF_GETSUB_PRM* prm, FunCall* call);

// Type handler and registration
bool t_circle_has_func(FunID id);
void t_circle_call_func(FunCall* call);
void t_circle_register();

// Function Wrappers
void fw_circle_sayhi(TypeInstance* inst);
T_Circle* fw_circle_getself(TypeInstance* inst);
