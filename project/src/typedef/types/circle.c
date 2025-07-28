#include "circle.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct-only Functions
int s_circle_getRadius(T_Circle t) {
    return t.radius;
}
void s_circle_setRadius(T_Circle* t, int radius) {
    t->radius = radius;
}

// Custom Function Prototypes
void f_circle_shape_sayhi(F_SHAPE_SAYHI_PRM* prm, FunCall* call) {
    TypeInstance* i = *(TypeInstance**)prm;
    T_Circle* circle = fw_circle_getself(i);
    printf("I am a circle, im a bad boy with radius %d\n", circle->radius);
}

// Default Function Implementations
void f_circle_def_create(F_DEF_CREATE_PRM* prm, FunCall* call) {
    T_Circle* new_circle = (T_Circle*)malloc(sizeof(T_Circle));
    new_circle->radius = 0;
    new_circle->shape = fw_def_create(TID_SHAPE);
    
    *prm = new_circle;

    call->code = FUN_OK;
}

void f_circle_def_destroy(F_DEF_DESTROY_PRM* prm, FunCall* call) {

    T_Circle* circle = *(void**)prm;
    fw_def_destroy(&circle->shape);

    free(*prm);
    *prm = NULL;
    
    call->code = FUN_OK;
}

void f_circle_def_tostring(F_DEF_TOSTRING_PRM* prm, FunCall* call) {
    if (!prm || !prm->data || !prm->buffer || prm->buffer_length == 0) {
        call->code = FUN_ERR_INVALIDARGS;
        return;
    }
    T_Shape* shape = fw_shape_getself(prm->data);
    T_Circle* circle = fw_circle_getself(prm->data);

    snprintf(prm->buffer, prm->buffer_length, "Circle(radius:%d, shape:Shape(x:%d, y:%d, w:%d, h:%d))",circle->radius, shape->x, shape->y, shape->w, shape->h);

    call->code = FUN_OK;
}

void f_circle_def_serialize(F_DEF_SERIALIZE_PRM* prm, FunCall* call) {
    if (!prm || !prm->from || !prm->to) {
        call->code = FUN_ERR_INVALIDARGS;
        return;
    }

    T_Circle* circle = *(void**)prm;

    if(!memory_block_write(prm->to, &circle->radius, sizeof(circle->radius))){
        call->code = FUN_ERR_INTERNAL;
        return;
    }

    //Shape is an instance, call its serialize function!
    if(!fw_def_serialize(&circle->shape, prm->to)){
        call->code = FUN_ERR_INTERNAL;
        return;
    }
    
    call->code = FUN_OK;

}

void f_circle_def_deserialize(F_DEF_DESERIALIZE_PRM* prm, FunCall* call) {

    if (!prm || !prm->to || !prm->from) {
        call->code = FUN_ERR_INVALIDARGS;
        return;
    }

    T_Circle* circle = *(void**)prm;

    //read radius
    if(!memory_block_read(prm->from, &circle->radius, sizeof(circle->radius))){
        call->code = FUN_ERR_INTERNAL;
        return;
    }

    //Ok, now deserialize our shape
    if(!fw_def_deserialize(&circle->shape,prm->from)){
        call->code = FUN_ERR_INTERNAL;
        return;
    }

    call->code = FUN_OK;

}

void f_circle_def_hassub(F_DEF_HASSUB_PRM* prm, FunCall* call) {
    if (!prm || !prm->instance) {
        call->code = FUN_ERR_INVALIDARGS;
        return;
    }

    TypeID requested_type = t_gettypefromsub(prm->subID);
    LocalSubID requested_local_id = t_getlocalsubfromsub(prm->subID);

    prm->ret = false;
    if(requested_local_id == V_LOCAL_SELF){
        if(requested_type == TID_SHAPE || requested_type == TID_CIRCLE){
            prm->ret = true;
        }
    }

    call->code = FUN_OK;

}

void f_circle_def_getsub(F_DEF_GETSUB_PRM* prm, FunCall* call) {
    if (!prm || !prm->instance) {
        call->code = FUN_ERR_INVALIDARGS;
        return;
    }

    TypeID requested_type = t_gettypefromsub(prm->subID);
    LocalSubID requested_local_id = t_getlocalsubfromsub(prm->subID);
    

    prm->ret = NULL;
    if(requested_local_id == V_LOCAL_SELF){
        if(requested_type == TID_CIRCLE){
            prm->ret = prm->instance;
        }
        else if(requested_type == TID_SHAPE){
            T_Circle* circle = fw_circle_getself(prm->instance);
            prm->ret = &circle->shape;
        }
    }
    if(prm->ret==NULL){
        call->code = FUN_ERR_SUBNOTFOUND;
        return;
    }

    call->code = FUN_OK;

}

// Handler functions
bool t_circle_has_func(FunID id) {
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
    } else if (type_id == TID_SHAPE) { // Overriding Shape function
        return (local_id == FID_LOCAL_SHAPE_SAYHI);
    }

    return false;
}

void t_circle_call_func(FunCall* call) {
    LocalFunID local_id = t_getlocalfunfromfun(call->target);
    TypeID type_id = t_gettypefromfun(call->target);

    if (type_id == T_NULL) { // Universal (default) functions
        switch (local_id) {
            case FID_LOCAL_DEF_CREATE:
                f_circle_def_create((F_DEF_CREATE_PRM*)call->argstruct, call);
                break;
            case FID_LOCAL_DEF_DESTROY:
                f_circle_def_destroy((F_DEF_DESTROY_PRM*)call->argstruct, call);
                break;
            case FID_LOCAL_DEF_TOSTRING:
                f_circle_def_tostring((F_DEF_TOSTRING_PRM*)call->argstruct, call);
                break;
            case FID_LOCAL_DEF_SERIALIZE:
                f_circle_def_serialize((F_DEF_SERIALIZE_PRM*)call->argstruct, call);
                break;
            case FID_LOCAL_DEF_DESERIALIZE:
                f_circle_def_deserialize((F_DEF_DESERIALIZE_PRM*)call->argstruct, call);
                break;
            case FID_LOCAL_DEF_HASSUB:
                f_circle_def_hassub((F_DEF_HASSUB_PRM*)call->argstruct, call);
                break;
            case FID_LOCAL_DEF_GETSUB:
                f_circle_def_getsub((F_DEF_GETSUB_PRM*)call->argstruct, call);
                break;
            default:
                call->code = FUN_ERR_FUNNOTFOUND;
                break;
        }
    } else if (type_id == TID_SHAPE) { // Overridden Shape functions (e.g., sayhi)
        switch (local_id) {
            case FID_LOCAL_SHAPE_SAYHI:
                f_circle_shape_sayhi((F_SHAPE_SAYHI_PRM*)call->argstruct, call);
                break;
            default:
                call->code = FUN_ERR_FUNNOTFOUND;
                break;
        }
    } else {
        call->code = FUN_ERR_FUNNOTFOUND;
    }
}

// Registration Function
void t_circle_register() {
    TypeDefinition def = {
        .id = TID_CIRCLE,
        .name = "Circle",
        .has_func = t_circle_has_func,
        .call_func = t_circle_call_func
    };

    t_typedef_register(def);
}

//Function Wrappers

void fw_circle_sayhi(TypeInstance* inst){
   fw_shape_sayhi(inst);
}

T_Circle* fw_circle_getself(TypeInstance* circle){
    TypeInstance* self = fw_def_getsub(circle, t_formsub(TID_CIRCLE, V_LOCAL_SELF),true);
    if(self == NULL){
        printf("Error: Shape instance is NULL.\n");
        return NULL;    
    }
    return (T_Circle*)self->data;
}

