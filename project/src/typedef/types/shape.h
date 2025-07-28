//shape.h

#pragma once

#include "../sys/typesystem.h"

//Type definition

#define TID_SHAPE 0x0001

typedef struct T_Shape {
    int x;
    int y;
    int w;
    int h;
    
} T_Shape;

//Struct-only Functions

int s_shape_getX(T_Shape shape);
int s_shape_getY(T_Shape shape);
int s_shape_getW(T_Shape shape);
int s_shape_getH(T_Shape shape);
void s_shape_setX(T_Shape* shape, int x);
void s_shape_setY(T_Shape* shape, int y);
void s_shape_setW(T_Shape* shape, int w);
void s_shape_setH(T_Shape* shape, int h);


//Define custom functions


//local IDs
#define FID_LOCAL_SHAPE_SAYHI 0x0002

//Global IDs
#define FID_SHAPE_SAYHI t_formfun(TID_SHAPE,FID_LOCAL_SHAPE_SAYHI);

//Params
typedef TypeInstance* F_SHAPE_SAYHI_PRM;

//Custom Function Prototypes

void f_shape_sayhi(F_SHAPE_SAYHI_PRM* prm,FunCall* call);

//Default Function Prototypes

void f_shape_def_create(F_DEF_CREATE_PRM* prm,FunCall* call);
void f_shape_def_destroy(F_DEF_DESTROY_PRM* prm,FunCall* call);
void f_shape_def_tostring(F_DEF_TOSTRING_PRM* prm,FunCall* call);
void f_shape_def_serialize(F_DEF_SERIALIZE_PRM* prm,FunCall* call);
void f_shape_def_deserialize(F_DEF_DESERIALIZE_PRM* prm,FunCall* call);
void f_shape_def_hassub(F_DEF_HASSUB_PRM* prm,FunCall* call);
void f_shape_def_getsub(F_DEF_GETSUB_PRM* prm,FunCall* call);

//Handler functions for the shape

bool t_shape_has_func(FunID id);
void t_shape_call_func(FunCall* call);

//Register function

void t_shape_register();

//Function wrappers

void fw_shape_sayhi(TypeInstance* shape);
T_Shape* fw_shape_getself(TypeInstance* shape);