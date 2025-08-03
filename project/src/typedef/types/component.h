//component.h

#pragma once

#include "../sys/typesystem.h"
#include "../../util.h"
#include "../../quicksine.h"

#include <raylib.h>
#include <raymath.h>

#define T_COMPONENT 0xC000

typedef struct Component {
    InstanceReference entity;

    float kaboom;
    char ss;

} Component;

// === Define functions ===

// === Implement functions ===

//Our things

//Default functions
IMP_FUN(DEF_CREATE                   , COMPONENT );
IMP_FUN(DEF_DESTROY                  , COMPONENT );
IMP_FUN(DEF_TOSTRING                 , COMPONENT );
IMP_FUN(DEF_SERIALIZE                , COMPONENT );
IMP_FUN(DEF_DESERIALIZE              , COMPONENT );
IMP_FUN(DEF_HASSUB                   , COMPONENT );
IMP_FUN(DEF_GETSUB                   , COMPONENT );


BEGIN_TYPE_FUNCTIONS(COMPONENT)

    TYPE_FUNCTION(DEF_CREATE                       , COMPONENT )
    TYPE_FUNCTION(DEF_DESTROY                      , COMPONENT )
    TYPE_FUNCTION(DEF_TOSTRING                     , COMPONENT )
    TYPE_FUNCTION(DEF_SERIALIZE                    , COMPONENT )
    TYPE_FUNCTION(DEF_DESERIALIZE                  , COMPONENT )
    TYPE_FUNCTION(DEF_GETSUB                       , COMPONENT )
    TYPE_FUNCTION(DEF_HASSUB                       , COMPONENT )

END_TYPE_FUNCTIONS(COMPONENT)

REGISTER_TYPE(COMPONENT,"Component")