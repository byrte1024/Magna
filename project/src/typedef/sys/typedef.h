// typedef.h
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#include "../../util.h"
#include "../../memoryBlock.h"

#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"

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

#define t_formsub(typeID, localSubID)     (((typeID) << 16) | (localSubID))
#define t_getlocalsubfromsub(subID)       ((subID) & 0xFFFF)
#define t_gettypefromsub(subID)           ((subID) >> 16)

#define t_formfun(typeID, localFunID)     (((typeID) << 16) | (localFunID))
#define t_getlocalfunfromfun(funID)       ((funID) & 0xFFFF)
#define t_gettypefromfun(funID)           ((funID) >> 16)

typedef struct TypeDefinition TypeDefinition;
typedef struct TypeInstance TypeInstance;
typedef struct InstanceReference InstanceReference;

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

typedef struct F_DEF_CREATE_PRM {
    TypeInstance* ret;
} F_DEF_CREATE_PRM;

typedef struct F_DEF_DESTROY_PRM {
    TypeInstance* ptr;
} F_DEF_DESTROY_PRM;

typedef struct F_DEF_TOSTRING_PRM {
    TypeInstance* data;
    char* buffer;
    size_t buffer_length;
} F_DEF_TOSTRING_PRM;

typedef struct F_DEF_SERIALIZE_PRM {
    TypeInstance* from;
    MemoryBlock* to;
} F_DEF_SERIALIZE_PRM;

typedef struct F_DEF_DESERIALIZE_PRM {
    TypeInstance* to;
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
    size_t references;
};

struct InstanceReference {
    TypeInstance* ptr;
    //uint16_t refid;
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

#define R_NULL (InstanceReference){.ptr = NULL}

// Usage:
// DEF_FUN(Name, TypeName, LocalID, struct { ... })
//
// Expands to:
//   #define FID_LOCAL_TypeName_Name LocalID
//   #define FID_TypeName_Name t_formfun(T_TypeName, FID_LOCAL_TypeName_Name)
//   typedef struct F_TypeName_Name_PRM { ... } F_TypeName_Name_PRM;

#define DEF_FUN(Name, TypeName, LocalID, StructBody)   \
    enum { FID_LOCAL_##TypeName##_##Name = LocalID }; \
    enum { FID_##TypeName##_##Name = t_formfun(T_##TypeName, FID_LOCAL_##TypeName##_##Name) }; \
    typedef struct F_##TypeName##_##Name##_PRM { StructBody } F_##TypeName##_##Name##_PRM 


// Usage:
// IMP_FUN(Name, TypeName)
//
// Expands to:
// void f_typename_imp_name(F_Name_PRM* prm, char* code);

#define IMP_FUN(Name, TypeName)                                  \
    void f_##TypeName##_imp_##Name(F_##Name##_PRM* prm, char* code)
    

#define BEGIN_TYPE_FUNCTIONS(TypeName) \
    static TypeFunctionEntry TypeName##_function_table[] = {

#define TYPE_FUNCTION(Name,TypeName) \
    { FID_##Name, (TypeFunctionCallback)f_##TypeName##_imp_##Name },

#define END_TYPE_FUNCTIONS(TypeName) \
    }; \
    static bool TypeName##_has_func(FunID id) { \
        for (size_t i = 0; i < sizeof(TypeName##_function_table)/sizeof(TypeFunctionEntry); ++i) { \
            if (TypeName##_function_table[i].id == id) return true; \
        } \
        return false; \
    } \
    static void TypeName##_call_func(FunCall* call) { \
        for (size_t i = 0; i < sizeof(TypeName##_function_table)/sizeof(TypeFunctionEntry); ++i) { \
            if (TypeName##_function_table[i].id == call->target) { \
                TypeName##_function_table[i].callback(call->argstruct,&call->code); \
                return; \
            } \
        } \
        call->code = FUN_ERR_FUNNOTFOUND; \
    } \
    static void* TypeName##_get_self_struct(TypeInstance* i){ \
       TypeInstance* ii = i_getself(i,T_##TypeName); \
       if(ii==NULL){return NULL;} \
       return ii->data; \
    } \

    #define REGISTER_TYPE(TypeName, DisplayName) \
    static void register_##TypeName##_type() { \
        TypeDefinition def = { \
            .id = T_##TypeName, \
            .name = DisplayName, \
            .has_func = TypeName##_has_func, \
            .call_func = TypeName##_call_func \
        }; \
        t_typedef_register(def); \
    }


    typedef void (*TypeFunctionCallback)(void* argstruct,char* code);

    typedef struct {
        FunID id;
        TypeFunctionCallback callback;
    } TypeFunctionEntry;