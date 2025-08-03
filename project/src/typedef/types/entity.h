//entity.h

#pragma once

#include "../sys/typesystem.h"
#include "../../util.h"
#include "../../quicksine.h"

#include <raylib.h>
#include <raymath.h>

#define T_ENTITY 0xE000

typedef struct Entity {
    InstanceReference* components;
    size_t component_capacity;
    size_t component_amount;

} Entity;

// === Define functions ===

// - Components -

DEF_FUN(GET_COMPONENT          , ENTITY , 0xC001 , TypeInstance* i; TypeID tid; bool createifnull; TypeInstance* ret_instance;); // Get a component from the entity. acts as a addcomponent function as well
DEF_FUN(GET_COMPONENT_COUNT    , ENTITY , 0xC002 , TypeInstance* i; size_t* ret_amount;); // Get the amount of components in the entity
DEF_FUN(REMOVE_COMPONENT       , ENTITY , 0xC003 , TypeInstance* i; TypeID tid;); // Remove a component from the entity
DEF_FUN(REMOVE_ALL_COMPONENTS  , ENTITY , 0xC004 , TypeInstance* i;); // Remove all components from the entity
DEF_FUN(COMPONENT_SHOUT        , ENTITY , 0xC005 , TypeInstance* i; FunCall* call;); // Call a function in all components


// === Implement functions ===

//Our things
IMP_FUN(ENTITY_GET_COMPONENT         , ENTITY );
IMP_FUN(ENTITY_GET_COMPONENT_COUNT   , ENTITY );
IMP_FUN(ENTITY_REMOVE_COMPONENT      , ENTITY );
IMP_FUN(ENTITY_REMOVE_ALL_COMPONENTS , ENTITY );
IMP_FUN(ENTITY_COMPONENT_SHOUT       , ENTITY );

//Default functions
IMP_FUN(DEF_CREATE                   , ENTITY );
IMP_FUN(DEF_DESTROY                  , ENTITY );
IMP_FUN(DEF_TOSTRING                 , ENTITY );
IMP_FUN(DEF_SERIALIZE                , ENTITY );
IMP_FUN(DEF_DESERIALIZE              , ENTITY );
IMP_FUN(DEF_HASSUB                   , ENTITY );
IMP_FUN(DEF_GETSUB                   , ENTITY );


BEGIN_TYPE_FUNCTIONS(ENTITY)

    TYPE_FUNCTION(DEF_CREATE                       , ENTITY )
    TYPE_FUNCTION(DEF_DESTROY                      , ENTITY )
    TYPE_FUNCTION(DEF_TOSTRING                     , ENTITY )
    TYPE_FUNCTION(DEF_SERIALIZE                    , ENTITY )
    TYPE_FUNCTION(DEF_DESERIALIZE                  , ENTITY )
    TYPE_FUNCTION(DEF_GETSUB                       , ENTITY )
    TYPE_FUNCTION(DEF_HASSUB                       , ENTITY )

    TYPE_FUNCTION(ENTITY_GET_COMPONENT             , ENTITY )
    TYPE_FUNCTION(ENTITY_GET_COMPONENT_COUNT       , ENTITY )
    TYPE_FUNCTION(ENTITY_REMOVE_COMPONENT          , ENTITY )
    TYPE_FUNCTION(ENTITY_REMOVE_ALL_COMPONENTS     , ENTITY )
    TYPE_FUNCTION(ENTITY_COMPONENT_SHOUT           , ENTITY )

END_TYPE_FUNCTIONS(ENTITY)


REGISTER_TYPE(ENTITY,"Entity")

