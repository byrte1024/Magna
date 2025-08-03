// typedef_fw.c
#include "typedef_fw.h"
#include <stdlib.h>

InstanceReference r_create(TypeID id){
    if(t_typedef_exists(id)){

        TypeInstance* instance = malloc(sizeof(TypeInstance));
        instance->id = id;
        instance->references = 0;
        instance->data = NULL;

        F_DEF_CREATE_PRM prm = { .ret = instance };

        FunCall c = {
            .target = FID_DEF_CREATE,
            .code = FUN_OK,
            .argstruct = &prm
        };
        
        t_typeinstance_callfunc(id, &c);

        if(c.code != FUN_OK){
            free(instance);
            return R_NULL;
        }

        instance->id = id;
        
        return r_createfor(instance);

    }
    else{
        return (InstanceReference){NULL};
    }
}

void r_forget(InstanceReference* ref){
    if(ref!=NULL){
        if(!r_isnull(*ref)){
            ref->ptr->references--;
            if(ref->ptr->references == 0){
                TypeInstance* i = ref->ptr;

                if(i->data!=NULL){
                    F_DEF_DESTROY_PRM prm = { .ptr = i };
                    FunCall c = {
                        .target = FID_DEF_DESTROY,
                        .code = FUN_OK,
                        .argstruct = &prm
                    };
                    t_typeinstance_callfunc(i->id, &c);
                    if(c.code != FUN_OK){
                        fprintf(stderr, "ERROR: r_forget() : FUN_ERR_%u", c.code);
                    }
                    
                }

                free(i);
            }
            
        }
        *ref = (InstanceReference){NULL};
    }
}

void r_destroy(InstanceReference ref){
    if(r_isvalid(ref)){
        TypeInstance* i = ref.ptr;
        if(i->data!=NULL){
            F_DEF_DESTROY_PRM prm = { .ptr = i };
            FunCall c = {
                .target = FID_DEF_DESTROY,
                .code = FUN_OK,
                .argstruct = &prm
            };
            t_typeinstance_callfunc(i->id, &c);
            if(c.code != FUN_OK){
                fprintf(stderr, "ERROR: r_destroy() : FUN_ERR_%u", c.code);
            }
            i->data = NULL;
        }
    }
}

InstanceReference r_createfrom(InstanceReference ref){
    if(r_isvalid(ref)){
        ref.ptr->references++;
        return ref;
    }
    return (InstanceReference){NULL};
}
void r_setfrom(InstanceReference target, InstanceReference value){
    if(r_isvalid(value)){
        r_forget(&target);
        target.ptr = value.ptr;
        target.ptr->references++;
    }
}

InstanceReference r_createfor(TypeInstance* i){
    if(i!=NULL){
        InstanceReference r = {i};
        i->references++;
        return r;
    }
    return (InstanceReference){NULL};
}

void r_def_tostring(InstanceReference instance, char* buffer, size_t buffer_length){
    if(r_isnull(instance)){
        snprintf(buffer, buffer_length, "[ERR!NULL]");
    }
    else {
        if(r_isdangling(instance)){
            snprintf(buffer, buffer_length, "[ERR!DANGLING]");
        }
        else{
            F_DEF_TOSTRING_PRM prm = { .data = instance.ptr, .buffer = buffer, .buffer_length = buffer_length };
            FunCall t = {
                .target = FID_DEF_TOSTRING,
                .code = FUN_OK,
                .argstruct = &prm
            };
            t_typeinstance_callfunc(instance.ptr->id, &t);
            if(t.code != FUN_OK){
                snprintf(buffer, buffer_length, "[ERR!%u]", t.code);   
            }
        }
    }
}
void r_def_print(InstanceReference instance, FILE* out, int buffersize){
    if(r_isnull(instance)){
        fprintf(out, "[ERR!NULL]");
    }
    else {
        char buffer[buffersize];
        r_def_tostring(instance, buffer, buffersize);
        fprintf(out, "(%s (%lld) : ( %s ) )\n", definition_bank[instance.ptr->id].name, instance.ptr->references, buffer);
    }
}
bool r_def_serialize(InstanceReference from, MemoryBlock* to){
    if(r_isvalid(from)){

        TypeID id = from.ptr->id;
        memory_block_write(to,&id,sizeof(TypeID));

        F_DEF_SERIALIZE_PRM prm = { .from = from.ptr->data, .to = to };
        FunCall t = {
            .target = FID_DEF_SERIALIZE,
            .code = FUN_OK,
            .argstruct = &prm
        };

        t_typeinstance_callfunc(id, &t);
        if(t.code != FUN_OK){
            return false;
        }
        return true;
    }
    return false;
}
bool r_def_deserialize(InstanceReference to, MemoryBlock* from, bool checktid){
    if(r_isvalid(to)){

        TypeID id = T_NULL;
        if(checktid){
            memory_block_read(from,&id,sizeof(TypeID));
            if(id == T_NULL){
                return false;
            }
            if(to.ptr->id != id){
                return false;
            }
        }
        else{
            id=to.ptr->id;
        }

        F_DEF_DESERIALIZE_PRM prm = { .to = to.ptr->data, .from = from };
        FunCall t = {
            .target = FID_DEF_DESERIALIZE,
            .code = FUN_OK,
            .argstruct = &prm
        };
        t_typeinstance_callfunc(id, &t);
        if(t.code != FUN_OK){
            return false;
        }
        return true;
    }
    return false;
}
InstanceReference r_def_createdeserialized(MemoryBlock* from){
    InstanceReference ret = {NULL};

    TypeID id = T_NULL;
    memory_block_read(from,&id,sizeof(TypeID));
    if(id == T_NULL){
        return ret;
    }
    ret = r_create(id);
    if(r_isvalid(ret)){
        r_def_deserialize(ret,from,false);
    }
    return ret;
}
bool r_def_hassub(InstanceReference instance, SubID subID){
    if(r_isvalid(instance)){

        F_DEF_HASSUB_PRM prm = { .instance = instance.ptr->data, .subID = subID };
        FunCall t = {
            .target = FID_DEF_HASSUB,
            .code = FUN_OK,
            .argstruct = &prm
        };
        t_typeinstance_callfunc(instance.ptr->id, &t);
        if(t.code != FUN_OK){
            return false;
        }
        return prm.ret;

    }
    return false;
}

TypeInstance* i_def_getsub(TypeInstance* i, SubID subID, bool check){
    if(i!=NULL){
        F_DEF_GETSUB_PRM prm = { .instance = i, .subID = subID };
        FunCall t = {
            .target = FID_DEF_GETSUB,
            .code = FUN_OK,
            .argstruct = &prm
        };
        t_typeinstance_callfunc(i->id, &t);
        if(t.code != FUN_OK){
            return NULL;
        }
        return prm.ret;
    }
    return NULL;
}

TypeInstance* r_def_getsub(InstanceReference instance, SubID subID, bool check){
    if(r_isvalid(instance)){

        if(check){
            if(!r_def_hassub(instance,subID)){
                return NULL;
            }
        }

        F_DEF_GETSUB_PRM prm = { .instance = instance.ptr, .subID = subID };
        FunCall t = {
            .target = FID_DEF_GETSUB,
            .code = FUN_OK,
            .argstruct = &prm
        };
        t_typeinstance_callfunc(instance.ptr->id, &t);
        if(t.code != FUN_OK){
            return NULL;
        }
        return prm.ret;

    }
    return NULL;
}
TypeInstance* r_getself(InstanceReference ref, TypeID type){
    if(r_isvalid(ref)){
        TypeInstance* sub = r_def_getsub(ref,t_formsub(type,V_LOCAL_SELF),true);
        if(sub==NULL){
            return NULL;
        }
        return sub;
    }
    return NULL;
}

TypeInstance* i_getself(TypeInstance* i, TypeID type){
    if(i!=NULL){
        TypeInstance* sub = i_def_getsub(i,t_formsub(type,V_LOCAL_SELF),true);
        if(sub==NULL){
            return NULL;
        }
        return sub;
    }
    return NULL;
}