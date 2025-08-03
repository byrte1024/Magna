//component.c

#include <stdlib.h>

#include "component.h"


IMP_FUN(DEF_CREATE                   , COMPONENT ){
    prm = (F_DEF_CREATE_PRM*)prm;
    TypeInstance* i = prm->ret;

    if(i == NULL){
        *code = FUN_ERR_INTERNAL;
        return;
    }

    i->data = malloc(sizeof(Component));

    if(i->data == NULL){
        *code = FUN_ERR_INTERNAL;
        return;
    }

    Component* c = (Component*)i->data;
    c->entity = R_NULL;
    c->kaboom = 0;
    c->ss = 'e';

    *code = FUN_OK;
}
IMP_FUN(DEF_DESTROY                  , COMPONENT ){
    TypeInstance* i = prm->ptr;

    if(i != NULL && i->data != NULL){
        free(i->data);
    }
    else{
        *code = FUN_ERR_INTERNAL;
    }
    *code = FUN_OK;
}
IMP_FUN(DEF_TOSTRING                 , COMPONENT ){
    TypeInstance* i = prm->data;
    TypeInstance* ii = i_getself(i,T_COMPONENT);
    Component* c = (Component*)ii->data;


    //Print values
    sprintf_s(prm->buffer,prm->buffer_length,"%p: kaboom = %.2f, ss = %c",c,c->kaboom,c->ss);

    *code = FUN_OK;
}
IMP_FUN(DEF_SERIALIZE                , COMPONENT ){
    Component* c = (Component*)(prm->from);

    memory_block_write(prm->to,&c->kaboom,sizeof(c->kaboom));
    memory_block_write(prm->to,&c->ss,sizeof(c->ss));

    *code = FUN_OK;
}
IMP_FUN(DEF_DESERIALIZE              , COMPONENT ){
    Component* c = (Component*)(prm->to);

    memory_block_read(prm->from,&c->kaboom,sizeof(c->kaboom));
    memory_block_read(prm->from,&c->ss,sizeof(c->ss));
    
    *code = FUN_OK;
}
IMP_FUN(DEF_HASSUB                   , COMPONENT ){
    LocalSubID localid = t_getlocalsubfromsub(prm->subID);
    TypeID typeid = t_gettypefromsub(prm->subID);

    prm->ret = false;

    if(typeid == T_COMPONENT){
        if(localid == V_LOCAL_SELF){ //They are asking for us! yay!
            prm->ret = true; 
        }
    }

    *code = FUN_OK;
}
IMP_FUN(DEF_GETSUB                   , COMPONENT ){
    LocalSubID localid = t_getlocalsubfromsub(prm->subID);
    TypeID typeid = t_gettypefromsub(prm->subID);

    prm->ret = NULL;

    if(typeid == T_COMPONENT){
        if(localid == V_LOCAL_SELF){ //They are asking for us! yay!
            prm->ret = prm->instance;
        }
    }
}




