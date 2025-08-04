//entity.c

#include <stdlib.h>


#include "../sys/typesystem.h"
#include "entity.h"
#include "component.h"

TypeInstance* r_entity_get_component(InstanceReference ref,TypeID tid, bool createifnull){
    if(r_isvalid(ref)){
        F_ENTITY_GET_COMPONENT_PRM prm = { .createifnull = createifnull, .tid = tid, .i = ref.ptr };
        FunCall call = { .target = FID_ENTITY_GET_COMPONENT, .code = FUN_OK, .argstruct = &prm};
        t_typeinstance_callfunc(ref.ptr->id,&call);
        if(call.code == FUN_OK){
            return prm.ret_instance;
        }
        else{
            return NULL;
        }
    }
    else{
        return NULL;
    }
}
size_t r_entity_get_component_count(InstanceReference ref){
    if(r_isvalid(ref)){
        F_ENTITY_GET_COMPONENT_COUNT_PRM prm = { .i = ref.ptr };
        FunCall call = { .target = FID_ENTITY_GET_COMPONENT_COUNT, .code = FUN_OK, .argstruct = &prm};
        t_typeinstance_callfunc(ref.ptr->id,&call);
        if(call.code == FUN_OK){
            return prm.ret_amount;
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }
}
bool r_entity_remove_component(InstanceReference ref,TypeID tid){
    if(r_isvalid(ref)){
        F_ENTITY_REMOVE_COMPONENT_PRM prm = { .i = ref.ptr, .tid = tid };
        FunCall call = { .target = FID_ENTITY_REMOVE_COMPONENT, .code = FUN_OK, .argstruct = &prm};
        t_typeinstance_callfunc(ref.ptr->id,&call);
        if(call.code == FUN_OK){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}
bool r_entity_remove_all_components(InstanceReference ref){
    if(r_isvalid(ref)){
        F_ENTITY_REMOVE_ALL_COMPONENTS_PRM prm = { .i = ref.ptr };
        FunCall call = { .target = FID_ENTITY_REMOVE_ALL_COMPONENTS, .code = FUN_OK, .argstruct = &prm};
        t_typeinstance_callfunc(ref.ptr->id,&call);
        if(call.code == FUN_OK){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}
void r_entity_component_shout(InstanceReference ref,FunCall* call){
    if(r_isvalid(ref)){
        F_ENTITY_COMPONENT_SHOUT_PRM prm = { .i = ref.ptr, .call = call };
        FunCall call = { .target = FID_ENTITY_COMPONENT_SHOUT, .code = FUN_OK, .argstruct = &prm};
        t_typeinstance_callfunc(ref.ptr->id,&call);
    }
}

bool s_entity_realloc(Entity* e, size_t tofit){
    size_t newcap = next_power_of_2(tofit);

    if(newcap <= e->component_capacity){
        return false;
    }

    if(e->components == NULL){
        e->components = malloc(newcap*sizeof(InstanceReference));
        if(e->components == NULL){
            return false;
        }
    }
    else{
        InstanceReference* newcomponents = realloc(e->components,newcap*sizeof(InstanceReference));
        if(newcomponents == NULL){
            return false;
        }
        e->components = newcomponents;
        e->component_capacity = newcap;
    }
    return true;
}

bool s_entity_remove_component_byind(Entity* e, size_t ind){
    if(ind >= e->component_amount){
        return false;
    }

    InstanceReference ref = e->components[ind];
    if(r_isnull(ref)){
        e->components[ind] = e->components[e->component_amount-1];
        e->components[e->component_amount-1] = R_NULL;
        //dont reduce, since number of components is not changed
        return false;
    }

    

    r_destroy(ref);
    r_forget(&ref);

    e->components[ind] = e->components[e->component_amount-1];
    e->components[e->component_amount-1] = R_NULL;
    e->component_amount--;

    return true;
}

size_t s_entity_find_component(TypeInstance* i, TypeID type, bool createifnull){
    Entity* e = ENTITY_get_self_struct(i);
    if(e == NULL){
        return SIZE_MAX;
    }

    for(size_t i = 0; i < e->component_amount; i++){
        InstanceReference ref = e->components[i];
        if(!r_isnull(ref)){
            TypeID t = ref.ptr->id;
            if(t == type){
                return i;
            }
        }
    }
    if(createifnull){
        s_entity_realloc(e,e->component_amount+1);
        e->components[e->component_amount] = r_create(type);
        Component* c = COMPONENT_get_self_struct(e->components[e->component_amount].ptr);
        if(c == NULL){
            r_destroy(e->components[e->component_amount]);
            r_forget(&e->components[e->component_amount]);
            e->components[e->component_amount] = R_NULL;
        }
        else{
            c->entity = r_createfor(i);
            e->component_amount++;
        }
        return e->component_amount-1;
    }
    return SIZE_MAX;
}

bool s_entity_remove_all_components(Entity* e){
    for(size_t i = 0; i < e->component_amount; i++){
        InstanceReference ref = e->components[i];
        if(!r_isnull(ref)){
            r_destroy(ref);
            r_forget(&ref);
        }
    }
    e->component_amount = 0;
    return true;

}



IMP_FUN(ENTITY_GET_COMPONENT         , ENTITY ){
    Entity* e = ENTITY_get_self_struct(prm->i);
    size_t ind = s_entity_find_component(prm->i,prm->tid,prm->createifnull);

    if(ind == SIZE_MAX){
        *code = FUN_OK;
        prm->ret_instance = NULL;
        return;
    }
    else{
        prm->ret_instance = e->components[ind].ptr;
        *code = FUN_OK;
        return;
    }
    
}
IMP_FUN(ENTITY_GET_COMPONENT_COUNT   , ENTITY ){
    Entity* e = ENTITY_get_self_struct(prm->i);
    prm->ret_amount = e->component_amount;
    return;
}
IMP_FUN(ENTITY_REMOVE_COMPONENT      , ENTITY ){
    Entity* e = ENTITY_get_self_struct(prm->i);
    size_t ind = s_entity_find_component(prm->i,prm->tid,false);
    if(ind == SIZE_MAX){
        *code = FUN_OK;
        return;
    }
    if(s_entity_remove_component_byind(e,ind)){
        *code = FUN_OK;
    }
    else{
        *code = FUN_ERR_INTERNAL;
    }
    return;
}
IMP_FUN(ENTITY_REMOVE_ALL_COMPONENTS , ENTITY ){
    Entity* e = ENTITY_get_self_struct(prm->i);
    if(s_entity_remove_all_components(e)){
        *code = FUN_OK;
    }
    else{
        *code = FUN_ERR_INTERNAL;
    }
    return;
}
IMP_FUN(ENTITY_COMPONENT_SHOUT       , ENTITY ){

    Entity* e = ENTITY_get_self_struct(prm->i);

    F_ENTITY_EVENT_CALL_PRM mainargs;
    mainargs.call = prm->call;
    mainargs.self = prm->i;

    FunCall maincall = { .argstruct = &mainargs, .code = FUN_OK, .target = FID_ENTITY_EVENT_CALL };

    t_typeinstance_callfunc(prm->i->id,&maincall);

    for(size_t j = 0; j < e->component_amount; j++){
        InstanceReference ref = e->components[j];
        if(!r_isnull(ref)){
            mainargs.self = ref.ptr;
            t_typeinstance_callfunc(ref.ptr->id,&maincall);
        }
    }
    *code = FUN_OK;
    return;
}


IMP_FUN(ENTITY_EVENT_CALL            , ENTITY ){
    //Entity* e = ENTITY_get_self_struct(prm->self);
}

//Default functions
IMP_FUN(DEF_CREATE                   , ENTITY ){
    TypeInstance* i = prm->ret;

    if(i == NULL){
        *code = FUN_ERR_INTERNAL;
        return;
    }

    i->data = malloc(sizeof(Entity));
    if(i->data == NULL){
        *code = FUN_ERR_INTERNAL;
        return;
    }

    Entity* e = (Entity*)i->data;
    e->component_amount = 0;
    e->component_capacity = 0;
    e->components = NULL;
}
IMP_FUN(DEF_DESTROY                  , ENTITY ){
    TypeInstance* i = prm->ptr;

    Entity* e = (Entity*)i->data;

    if(e->components != NULL){

        s_entity_remove_all_components(e);

        free(e->components);
    }

    if(i != NULL && i->data != NULL){
        free(i->data);
    }
    else{
        *code = FUN_ERR_INTERNAL;
    }
    
    *code = FUN_OK;
}
IMP_FUN(DEF_TOSTRING, ENTITY) {
    Entity* e = ENTITY_get_self_struct(prm->data);
    if (e == NULL) {
        *code = FUN_ERR_INTERNAL;
        return;
    }

    size_t written = 0;
    written += sprintf_s(prm->buffer + written, prm->buffer_length - written,
                         "%p: component_amount = %d ", e, e->component_amount);

    if (e->components != NULL) {
        for (size_t j = 0; j < e->component_amount; j++) {
            InstanceReference ref = e->components[j];
            if (!r_isnull(ref)) {
                char buffer[256];
                r_def_tostring(ref, buffer, sizeof(buffer));
                written += sprintf_s(prm->buffer + written, prm->buffer_length - written,
                                     "( %s )", buffer);
            } else {
                written += sprintf_s(prm->buffer + written, prm->buffer_length - written,
                                     "( NULL )");
            }

            // Stop if we run out of space
            if (written >= prm->buffer_length) {
                break;
            }
        }
    }

    *code = FUN_OK;
}
IMP_FUN(DEF_SERIALIZE                , ENTITY ){
    Entity* e = (Entity*)(prm->from->data);

    size_t succesfully_saved = 0;

    size_t cursor_at_size = prm->to->cursor;

    memory_block_write(prm->to,&succesfully_saved,sizeof(succesfully_saved));

    if(e->components != NULL){
        for(size_t j = 0; j < e->component_amount; j++){
            InstanceReference ref = e->components[j];
            if(!r_isnull(ref)){
                succesfully_saved++;
                r_def_serialize(ref,prm->to);
            }
        }
    }

    memory_block_seek_to(prm->to,cursor_at_size);
    memory_block_write(prm->to,&succesfully_saved,sizeof(succesfully_saved));
}
IMP_FUN(DEF_DESERIALIZE              , ENTITY ){
    Entity* e = (Entity*)(prm->to->data);
    size_t succesfully_saved = 0;
    memory_block_read(prm->from,&succesfully_saved,sizeof(succesfully_saved));

    //Clear old components
    if(e->components != NULL){
        s_entity_remove_all_components(e);
        free(e->components);
    }

    if(succesfully_saved == 0){
        e->components = NULL;
        e->component_amount = 0;
        e->component_capacity = 0;
    }
    else{

        if(!s_entity_realloc(e,succesfully_saved)){
            *code = FUN_ERR_INTERNAL;
            return;
        }

        for(size_t j = 0; j < succesfully_saved; j++){
            InstanceReference ref = r_def_createdeserialized(prm->from);
            if(r_isvalid(ref)){
                e->components[j] = ref;
                Component* comp = COMPONENT_get_self_struct(ref.ptr);
                comp->entity = r_createfor(prm->to);
            }
            else{
                *code = FUN_ERR_INTERNAL;
                return;
            }
        }

        e->component_amount = succesfully_saved;

    }

    *code = FUN_OK;
    
}
IMP_FUN(DEF_HASSUB                   , ENTITY ){
    LocalSubID localid = t_getlocalsubfromsub(prm->subID);
    TypeID typeid = t_gettypefromsub(prm->subID);

    prm->ret = false;

    if(typeid == T_ENTITY){
        if(localid == V_LOCAL_SELF){ //They are asking for us! yay!
            prm->ret = true; 
        }
    }

    *code = FUN_OK;
}
IMP_FUN(DEF_GETSUB                   , ENTITY ){
    LocalSubID localid = t_getlocalsubfromsub(prm->subID);
    TypeID typeid = t_gettypefromsub(prm->subID);

    prm->ret = NULL;

    if(typeid == T_ENTITY){
        if(localid == V_LOCAL_SELF){ //They are asking for us! yay!
            prm->ret = prm->instance;
        }
    }

}