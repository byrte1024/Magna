// typedef_fw.c
#include "typedef_fw.h"
#include <stdlib.h>

TypeInstance fw_def_create(TypeID id) {
    F_DEF_CREATE_PRM prm = NULL;
    FunCall call = { .code = FUN_OK, .target = FID_DEF_CREATE, .argstruct = &prm };
    t_typeinstance_callfunc(id,&call);

    
    TypeInstance instance = { id, NULL };

    if (call.code != FUN_OK) {
        instance.id = T_NULL;
        instance.data = NULL;
        fprintf(stderr, "Error: Creating object of type %u failed. Code: %u\n", id, call.code);
    }
    else{
        instance.data = prm;
    }

    return instance;
}

void fw_def_destroy(TypeInstance* instance) {
    if (!t_typeinstance_isvalid(instance)) {
        fprintf(stderr, "Error: Destroying invalid instance.\n");
        return;
    }

    F_DEF_DESTROY_PRM prm = instance->data;
    FunCall call = { .code = FUN_OK, .target = FID_DEF_DESTROY, .argstruct = &prm };
    t_typeinstance_callfunc(instance->id,&call);

    if (call.code == FUN_OK) {
        instance->id = T_NULL;
        instance->data = NULL;
    } else {
        fprintf(stderr, "Error: Destroy failed. Code: %u\n", call.code);
    }
}

void fw_def_tostring(TypeInstance* instance, char* buffer, size_t buffer_length) {
    if (!t_typeinstance_isvalid(instance)) {
        fprintf(stderr, "Error: Invalid instance.\n");
        return;
    }

    F_DEF_TOSTRING_PRM prm = { .data= instance, .buffer= buffer, .buffer_length = buffer_length };
    FunCall call = { .code = FUN_OK, .target = FID_DEF_TOSTRING, .argstruct = &prm };
    t_typeinstance_callfunc(instance->id,&call);
}

void fw_def_print(TypeInstance* instance, FILE* out, int buffersize) {
    char* buffer = malloc(buffersize);
    fw_def_tostring(instance, buffer, buffersize);
    fprintf(out, "%s\n", buffer);
    free(buffer);
}

bool fw_def_serialize(TypeInstance* from, MemoryBlock* to) {
    if (!t_typeinstance_isvalid(from)) {
        fprintf(stderr, "Error: Invalid instance.\n");
        return false;
    }

    memory_block_ensurespaceforwrite(to, sizeof(TypeID)*2);
    memory_block_write(to, &from->id, sizeof(TypeID));
    F_DEF_SERIALIZE_PRM prm = { from->data, to };
    FunCall call = { .code = FUN_OK, .target = FID_DEF_SERIALIZE, .argstruct = &prm };
    t_typeinstance_callfunc(from->id,&call);
    return call.code == FUN_OK;
}

bool fw_def_deserialize(TypeInstance* to, MemoryBlock* from) {
    if (!t_typeinstance_isvalid(to)) {
        fprintf(stderr, "Error: Invalid instance.\n");
        return false;
    }

    TypeID type;
    memory_block_read(from, &type, sizeof(type));
    if (type != to->id) {
        fprintf(stderr, "Error: Type mismatch.\n");
        return false;
    }

    F_DEF_DESERIALIZE_PRM prm = { to->data, from };
    FunCall call = { .code = FUN_OK, .target = FID_DEF_DESERIALIZE, .argstruct = &prm };
    t_typeinstance_callfunc(to->id,&call);
    to->data = prm.to;
    return call.code == FUN_OK;
}

TypeInstance fw_def_createdeserialized(MemoryBlock* from) {
    TypeID id;
    memory_block_peek(from, &id, sizeof(id));
    TypeInstance instance = fw_def_create(id);
    if (!t_typeinstance_isvalid(&instance)) return instance;

    if (!fw_def_deserialize(&instance, from)) {
        fprintf(stderr, "Error: Deserialization failed.\n");
    }
    return instance;
}

bool fw_def_hassub(TypeInstance* instance, SubID subID) {
    if (!t_typeinstance_isvalid(instance)) return false;

    F_DEF_HASSUB_PRM prm = {.instance = instance, .ret = false, .subID = subID };
    FunCall call = { .code = FUN_OK, .target = FID_DEF_HASSUB, .argstruct = &prm };
    t_typeinstance_callfunc(instance->id,&call);
    return prm.ret;
}

TypeInstance* fw_def_getsub(TypeInstance* instance, SubID subID, bool check) {
    if (!t_typeinstance_isvalid(instance)) return NULL;
    if (check && !fw_def_hassub(instance, subID)) return NULL;

    F_DEF_GETSUB_PRM prm = { .instance = instance, .ret = NULL, .subID = subID };
    FunCall call = { .code = FUN_OK, .target = FID_DEF_GETSUB, .argstruct = &prm };
    t_typeinstance_callfunc(instance->id,&call);
    return prm.ret;
}
