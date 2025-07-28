// typedef.h
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "../../util.h"
#include "../../memoryBlock.h"

/*
FOR ANYONE READING THIS SCRIPT

this system can be a bit unintiuitive, so here's how it works.

Types are kind of like classes, and we can define a "Type Definition"
These simply state the ID name and a few handlers (I'll get to those in a bit)

A type instance is a simple struct which points to a void* with the data of the type, along with containg the ID.
a type instance can be "null" in multiple ways, since each container holds both the type and the data.
it can have a null data but not a null ID, I will call this "typed null"
the data is missing, but the data it wants to have is known.
if both data and ID are null, this is a "pure null" object (just null)
if the ID is null and data is not, this is a "corrupted" object, since we cant effectively use its data.
corrupted objects should be left alone, even if it means accumelating memory.

Each definition can include definitions for as many "functions" as it can be simply changing the handler to respond to certain function ids
Function ID's are seperated into TYPEID+LOCALID.
For functions your class defines, just put your TYPEID and asign whatever localid (NOT 0!)
If you want to inherit a class, just respond to the function ID's of that type. 

If you inherit, make sure to contain inside your object a `TypeInstance parent` field, and define a getsub function on local 0
If you have generic fields that have TypeInstance, define them as a local sub (not 0)

for example, lets say i have the type T_SHAPE

shape defines the function FID_SHAPE_SAYNAME (prints "i dont know")
shape has variables of x,y and w,h. (all are ints).

when the shape is pinged with getsub with the typeid of shape and the local id of 0, it will return itself!

now lets say we create a T_CIRCLE
to "inherit", inside the data we create we store:
Radius (int)
Shape (TypeInstance (Not a pointer! just raw!))

when a circle is created, we also create and initialize shape.

When the circle is pinged with getsub with the typeID of shape, and localID of 0, it will return &Shape.data

now, if we have a TypeInstance and it could be either shape or circle, we can still get the coordinates with the same process

we simply call getsub(shapeID + 0)
now, no matter if the og was a circle or a raw shape, we "interpreted" it as a shape. now we can run our stuff.

Note! make sure if you contain nested TypeInstance's, you call their serialize function instead of just writing the typeinstance object to memory, if you wont it WILL break XD

Also, this is not thread-safe, do NOT try to multithread with this, its not feasible right now.

*/

#define TYPE_SYSTEM_LONGNAME "C Type System"
#define TYPE_SYSTEM_SHORTNAME "CTS"

typedef uint16_t TypeID;
typedef uint16_t LocalSubID;
typedef uint32_t SubID;
typedef uint16_t LocalFunID;
typedef uint32_t FunID;

#define T_NULL 0x0000
#define T_MAX  0xffff
#define V_LOCAL_SELF 0x0000

#define I_NULL (TypeInstance){0}

static inline SubID t_formsub(TypeID typeID, LocalSubID localSubID) { return (typeID << 16) | localSubID; }
static inline LocalSubID t_getlocalsubfromsub(SubID subID) { return subID & 0xFFFF; }
static inline TypeID t_gettypefromsub(SubID subID) { return subID >> 16; }

static inline FunID t_formfun(TypeID typeID, LocalFunID localFunID) { return (typeID << 16) | localFunID; }
static inline LocalFunID t_getlocalfunfromfun(FunID funID) { return funID & 0xFFFF; }
static inline TypeID t_gettypefromfun(FunID funID) { return funID >> 16; }

typedef struct TypeDefinition TypeDefinition;
typedef struct TypeInstance TypeInstance;

typedef struct FunCall {
    FunID target;
    char code;
    void* argstruct;
} FunCall;

#define FUN_OK                    0x0
#define FUN_ERR_FUNNOTFOUND       0x1
#define FUN_ERR_INVALIDINSTANCE   0x2
#define FUN_ERR_INVALIDARGS       0x3
#define FUN_ERR_INTERNAL          0x4
#define FUN_ERR_SUBNOTFOUND       0x5

// Function IDs
#define FID_LOCAL_DEF_CREATE       0x0001
#define FID_LOCAL_DEF_DESTROY      0x0002
#define FID_LOCAL_DEF_TOSTRING     0x0003
#define FID_LOCAL_DEF_SERIALIZE    0x0004
#define FID_LOCAL_DEF_DESERIALIZE  0x0005
#define FID_LOCAL_DEF_GETSUB       0x0006
#define FID_LOCAL_DEF_HASSUB       0x0007

//Important note, HASSUB and GETSUB are the only functions which talk to the definition with the TypeInstance* pointer itself, and not the raw data pointer.
//This is because of some semantics when getsub needs to return itself.
#define FID_DEF_CREATE       t_formfun(T_NULL,FID_LOCAL_DEF_CREATE)
#define FID_DEF_DESTROY      t_formfun(T_NULL,FID_LOCAL_DEF_DESTROY)
#define FID_DEF_TOSTRING     t_formfun(T_NULL,FID_LOCAL_DEF_TOSTRING)
#define FID_DEF_SERIALIZE    t_formfun(T_NULL,FID_LOCAL_DEF_SERIALIZE)
#define FID_DEF_DESERIALIZE  t_formfun(T_NULL,FID_LOCAL_DEF_DESERIALIZE)
#define FID_DEF_GETSUB       t_formfun(T_NULL,FID_LOCAL_DEF_GETSUB)
#define FID_DEF_HASSUB       t_formfun(T_NULL,FID_LOCAL_DEF_HASSUB)

typedef void* F_DEF_CREATE_PRM;
typedef void* F_DEF_DESTROY_PRM;

typedef struct F_DEF_TOSTRING_PRM {
    TypeInstance* data;
    char* buffer;
    size_t buffer_length;
} F_DEF_TOSTRING_PRM;

typedef struct F_DEF_SERIALIZE_PRM {
    void* from;
    MemoryBlock* to;
} F_DEF_SERIALIZE_PRM;

typedef struct F_DEF_DESERIALIZE_PRM {
    void* to;
    MemoryBlock* from;
} F_DEF_DESERIALIZE_PRM;

typedef struct F_DEF_GETSUB_PRM {
    TypeInstance* instance;
    SubID subID;
    TypeInstance* ret;
} F_DEF_GETSUB_PRM;

typedef struct F_DEF_HASSUB_PRM {
    TypeInstance* instance;
    SubID subID;
    bool ret;
} F_DEF_HASSUB_PRM;


struct TypeDefinition {
    TypeID id;
    char* name;
    bool (*has_func)(FunID id);
    void (*call_func)(FunCall* call);
} ;

struct TypeInstance {
    TypeID id;
    void* data;
};

extern bool definition_bank_lock;
extern TypeDefinition definition_bank[T_MAX];

void t_typedef_init();
void t_typedef_register(TypeDefinition def);
void t_typedef_lock();
bool t_typeinstance_hasfunc(TypeID id, FunID fun);
void t_typeinstance_callfunc(TypeID id,FunCall* call);

static inline bool t_typedef_exists(TypeID id) {
    return (id != T_NULL && definition_bank[id].id == id);
}
static inline bool t_typedef_id_valid(TypeID id) {
    return id != T_NULL;
}
static inline bool t_typeinstance_isvalid(TypeInstance* instance) {
    return (instance != NULL && instance->id != T_NULL && instance->data != NULL);
}
