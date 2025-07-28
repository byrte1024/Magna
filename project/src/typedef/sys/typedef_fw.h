// typedef_fw.h
#pragma once

#include "typedef.h"

TypeInstance fw_def_create(TypeID id);
void fw_def_destroy(TypeInstance* instance);
void fw_def_tostring(TypeInstance* instance, char* buffer, size_t buffer_length);
void fw_def_print(TypeInstance* instance, FILE* out, int buffersize);
bool fw_def_serialize(TypeInstance* from, MemoryBlock* to);
bool fw_def_deserialize(TypeInstance* to, MemoryBlock* from);
TypeInstance fw_def_createdeserialized(MemoryBlock* from);
bool fw_def_hassub(TypeInstance* instance, SubID subID);
TypeInstance* fw_def_getsub(TypeInstance* instance, SubID subID, bool check);
