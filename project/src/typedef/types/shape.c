// shape.c
#include "shape.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// Struct-only Functions Implementations
int s_shape_getX(T_Shape shape) { return shape.x; }
int s_shape_getY(T_Shape shape) { return shape.y; }
int s_shape_getW(T_Shape shape) { return shape.w; }
int s_shape_getH(T_Shape shape) { return shape.h; }
void s_shape_setX(T_Shape* shape, int x) { shape->x = x; }
void s_shape_setY(T_Shape* shape, int y) { shape->y = y; }
void s_shape_setW(T_Shape* shape, int w) { shape->w = w; }
void s_shape_setH(T_Shape* shape, int h) { shape->h = h; }

// Custom Function Implementations
void f_shape_sayhi(F_SHAPE_SAYHI_PRM* prm, FunCall* call) {
    TypeInstance* i = *(TypeInstance**)prm;
    T_Shape* shape = (T_Shape*)i->data;

    printf("Hi, I'm a good little shape! (x:%d, y:%d, w:%d, h:%d)\n", shape->x, shape->y, shape->w, shape->h);

    call->code = FUN_OK;
}

// Default Function Implementations
void f_shape_def_create(F_DEF_CREATE_PRM* prm, FunCall* call) {
    T_Shape* new_shape = (T_Shape*)malloc(sizeof(T_Shape));
    if (!new_shape) {
        call->code = FUN_ERR_INTERNAL;
        return;
    }
    new_shape->x = 0;
    new_shape->y = 0;
    new_shape->w = 0;
    new_shape->h = 0;

    *prm = new_shape;

    call->code = FUN_OK;
}

void f_shape_def_destroy(F_DEF_DESTROY_PRM* prm, FunCall* call) {
    if (!prm) {
        call->code = FUN_ERR_INVALIDARGS;
        return;
    }

    free(*prm);
    *prm = NULL;

    call->code = FUN_OK;
}

void f_shape_def_tostring(F_DEF_TOSTRING_PRM* prm, FunCall* call) {
    if (!prm || !prm->data || !prm->buffer || prm->buffer_length == 0) {
        call->code = FUN_ERR_INVALIDARGS;
        return;
    }
    T_Shape* shape = (T_Shape*)prm->data;
    snprintf(prm->buffer, prm->buffer_length, "Shape(x:%d, y:%d, w:%d, h:%d)",shape->x, shape->y, shape->w, shape->h);

    call->code = FUN_OK;
}

void f_shape_def_serialize(F_DEF_SERIALIZE_PRM* prm, FunCall* call) {
    if (!prm || !prm->from || !prm->to) {
        call->code = FUN_ERR_INVALIDARGS;
        return;
    }
    T_Shape* shape = (T_Shape*)prm->from;
    if (!memory_block_ensurespaceforwrite(prm->to, sizeof(T_Shape))) {
        call->code = FUN_ERR_INTERNAL;
        return;
    }
    if (!memory_block_write(prm->to, shape, sizeof(T_Shape))) {
        call->code = FUN_ERR_INTERNAL;
        return;
    }

    call->code = FUN_OK;
}

void f_shape_def_deserialize(F_DEF_DESERIALIZE_PRM* prm, FunCall* call) {
    if (!prm || !prm->to || !prm->from) {
        call->code = FUN_ERR_INVALIDARGS;
        return;
    }
    T_Shape* shape = (T_Shape*)prm->to;
    if (!memory_block_read(prm->from, shape, sizeof(T_Shape))) {
        call->code = FUN_ERR_INTERNAL;
        return;
    }

    call->code = FUN_OK;
}

void f_shape_def_hassub(F_DEF_HASSUB_PRM* prm, FunCall* call) {
    if (!prm || !prm->instance) {
        call->code = FUN_ERR_INVALIDARGS;
        return;
    }
    // A raw shape only has itself as a "sub" (V_LOCAL_SELF)
    prm->ret = (t_gettypefromsub(prm->subID) == TID_SHAPE && t_getlocalsubfromsub(prm->subID) == V_LOCAL_SELF);

    call->code = FUN_OK;
}

void f_shape_def_getsub(F_DEF_GETSUB_PRM* prm, FunCall* call) {
    if (!prm || !prm->instance) {
        call->code = FUN_ERR_INVALIDARGS;
        return;
    }

    TypeID requested_type = t_gettypefromsub(prm->subID);
    LocalSubID requested_local_id = t_getlocalsubfromsub(prm->subID);

    if (requested_type == TID_SHAPE && requested_local_id == V_LOCAL_SELF) {
        // If the request is for the shape itself, return a pointer to the original instance
        prm->ret = prm->instance;
    } else {
        prm->ret = NULL; // No other sub-objects
        call->code = FUN_ERR_SUBNOTFOUND; // Or a more specific error for sub not found
    }

    call->code = FUN_OK;
}

// Handler functions

bool t_shape_has_func(FunID id) {
    LocalFunID local_id = t_getlocalfunfromfun(id);
    TypeID type_id = t_gettypefromfun(id);

    if (type_id == T_NULL) { // Universal functions
        return (local_id == FID_LOCAL_DEF_CREATE ||
                local_id == FID_LOCAL_DEF_DESTROY ||
                local_id == FID_LOCAL_DEF_TOSTRING ||
                local_id == FID_LOCAL_DEF_SERIALIZE ||
                local_id == FID_LOCAL_DEF_DESERIALIZE ||
                local_id == FID_LOCAL_DEF_HASSUB ||
                local_id == FID_LOCAL_DEF_GETSUB);
    } else if (type_id == TID_SHAPE) { // Shape-specific functions
        return (local_id == FID_LOCAL_SHAPE_SAYHI);
    }

    return false;
}

void t_shape_call_func(FunCall* call) {
    LocalFunID local_id = t_getlocalfunfromfun(call->target);
    TypeID type_id = t_gettypefromfun(call->target);

    if (type_id == T_NULL) { // Universal functions
        switch (local_id) {
            case FID_LOCAL_DEF_CREATE:
                f_shape_def_create((F_DEF_CREATE_PRM*)call->argstruct, call);
                break;
            case FID_LOCAL_DEF_DESTROY:
                f_shape_def_destroy((F_DEF_DESTROY_PRM*)call->argstruct, call);
                break;
            case FID_LOCAL_DEF_TOSTRING:
                f_shape_def_tostring((F_DEF_TOSTRING_PRM*)call->argstruct, call);
                break;
            case FID_LOCAL_DEF_SERIALIZE:
                f_shape_def_serialize((F_DEF_SERIALIZE_PRM*)call->argstruct, call);
                break;
            case FID_LOCAL_DEF_DESERIALIZE:
                f_shape_def_deserialize((F_DEF_DESERIALIZE_PRM*)call->argstruct, call);
                break;
            case FID_LOCAL_DEF_HASSUB: {
                f_shape_def_hassub((F_DEF_HASSUB_PRM*)call->argstruct, call);
                break;
            }
            case FID_LOCAL_DEF_GETSUB: {
                f_shape_def_getsub((F_DEF_GETSUB_PRM*)call->argstruct, call);
                break;
            }
            default:
                call->code = FUN_ERR_FUNNOTFOUND;
                break;
        }
    } else if (type_id == TID_SHAPE) { // Shape-specific functions
        switch (local_id) {
            case FID_LOCAL_SHAPE_SAYHI:
                f_shape_sayhi((F_SHAPE_SAYHI_PRM*)call->argstruct, call);
                break;
            default:
                call->code = FUN_ERR_FUNNOTFOUND;
                break;
        }
    } else {
        call->code = FUN_ERR_FUNNOTFOUND;
    }
}

void t_shape_register() {

    TypeDefinition def = {
        .id = TID_SHAPE,
        .name = "Shape",
        .has_func = t_shape_has_func,
        .call_func = t_shape_call_func
    };

    t_typedef_register(def);

}

// function wrappers

void fw_shape_sayhi(TypeInstance* shape) {

    F_SHAPE_SAYHI_PRM prm = fw_def_getsub(shape, t_formsub(TID_SHAPE, V_LOCAL_SELF),true);
    if(prm == NULL){
        printf("Error: Shape instance is NULL.\n");
        return;
    }
    FunCall call = {0};
    call.code = FUN_OK;
    call.target = FID_SHAPE_SAYHI;
    call.argstruct = &prm;
    t_typeinstance_callfunc(TID_SHAPE, &call);
}

T_Shape* fw_shape_getself(TypeInstance* shape){
    TypeInstance* self = fw_def_getsub(shape, t_formsub(TID_SHAPE, V_LOCAL_SELF),true);
    if(self == NULL){
        printf("Error: Shape instance is NULL.\n");
        return NULL;    
    }
    return (T_Shape*)self->data;
}