// typedef.c
#include "typedef.h"
#include <string.h>
#include <stdlib.h>

bool definition_bank_lock = true;
TypeDefinition definition_bank[T_MAX];

void t_typedef_init() {
    memset(definition_bank, 0, sizeof(definition_bank));
    definition_bank_lock = false;
    printf(TYPE_SYSTEM_LONGNAME " Initialized.\n");
}

void t_typedef_register(TypeDefinition def) {
    if (definition_bank_lock) {
        fprintf(stderr, "Error: Type system is locked. Cannot register type '%s' (%u).\n", def.name, def.id);
        return;
    }

    if (def.id == T_NULL || def.call_func == NULL || def.has_func == NULL) {
        fprintf(stderr, "Error: Invalid definition. Cannot register.\n");
        return;
    }

    if (definition_bank[def.id].id != T_NULL) {
        fprintf(stderr, "Error: Type ID %u is already registered.\n", def.id);
        return;
    }

    definition_bank[def.id] = def;
    printf("Type '%s' (%u) registered.\n", def.name, def.id);
}

void t_typedef_lock() {
    definition_bank_lock = true;
    printf("Type system locked.\n");
}

bool t_typeinstance_hasfunc(TypeID id, FunID fun) {
    if (t_typedef_exists(id)) {
        return definition_bank[id].has_func(fun);
    }
    fprintf(stderr, "Error: Type ID %u is not registered.\n", id);
    return false;
}

void t_typeinstance_callfunc(TypeID id,FunCall* call) {
    if (t_typedef_exists(id)) {
        if(definition_bank[id].has_func(call->target)){
            definition_bank[id].call_func(call);
        }
        else{
            call->code = FUN_ERR_FUNNOTFOUND;
        }
    } else {
        fprintf(stderr, "Error: Type ID %u is not registered.\n", id);
        call->code = FUN_ERR_INVALIDINSTANCE;
    }
}
