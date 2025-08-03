// typedef_fw.h
#pragma once

#include "typedef.h"


static inline bool r_isnull(InstanceReference r){
    return ( r.ptr == NULL );
}

static inline bool r_isdangling(InstanceReference r){
    return ( r.ptr != NULL && (r.ptr->data == NULL) );
}

static inline bool r_isvalid(InstanceReference r){
    return ( !r_isnull(r) && ! r_isdangling(r) );
}

InstanceReference r_create(TypeID id);
void r_forget(InstanceReference* ref);
void r_destroy(InstanceReference ref);
InstanceReference r_createfrom(InstanceReference ref);
InstanceReference r_createfor(TypeInstance* i);
void r_setfrom(InstanceReference target, InstanceReference value);

void r_def_tostring(InstanceReference instance, char* buffer, size_t buffer_length);
void r_def_print(InstanceReference instance, FILE* out, int buffersize);
bool r_def_serialize(InstanceReference from, MemoryBlock* to);
bool r_def_deserialize(InstanceReference to, MemoryBlock* from, bool checktid);
InstanceReference r_def_createdeserialized(MemoryBlock* from);
bool r_def_hassub(InstanceReference instance, SubID subID);
TypeInstance* r_def_getsub(InstanceReference instance, SubID subID, bool check);
TypeInstance* i_def_getsub(TypeInstance* i, SubID subID, bool check);
TypeInstance* r_getself(InstanceReference ref, TypeID type);

TypeInstance* i_getself(TypeInstance* i, TypeID type);