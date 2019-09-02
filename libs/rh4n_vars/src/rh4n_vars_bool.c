#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"

int rh4nvarCreateNewBool(RH4nVarList *varlist, char *pgroupname, char *pname, bool value) {
    int varlibret = 0;
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    if((varlibret = rh4nvarCreatenewVariable(varlist, pgroupname, pname, RH4NVARTYPEBOOLEAN, &_refvar)) != RH4N_RET_OK) { 
        return(varlibret); 
    }
    return(_rh4nvarCreateNewBool(&_refvar, value));

}

int rh4nvarCreateNewBool_m(RH4nVarList *varlist, char **pgroupnames, char *pname, bool value) {
    int varlibret = 0;
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    if((varlibret = rh4nvarCreatenewVariable_m(varlist, pgroupnames, pname, RH4NVARTYPEBOOLEAN, &_refvar)) != RH4N_RET_OK) { 
        return(varlibret); 
    }
    return(_rh4nvarCreateNewBool(&_refvar, value));

}

int _rh4nvarCreateNewBool(RH4nVarRef *ref, bool value) {
    return(rh4nvarSetBool(&ref->var->var, value));
}


int rh4nvarEditBool(RH4nVarList *varlist, char *pgroupname, char *pname, bool value) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEBOOLEAN) { return(RH4N_RET_FORMAT_ERR); }
    if((varlibret = rh4nvarSetBool(&_refvar.var->var, value)) != RH4N_RET_OK) { return(varlibret); }
    return(RH4N_RET_OK);
}

int rh4nvarGetBool(RH4nVarList *varlist, char *pgroupname, char *pname, bool *outbuff) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEBOOLEAN) { return(RH4N_RET_FORMAT_ERR); }
    *outbuff = *((bool*)_refvar.var->var.value);
    return(RH4N_RET_OK);
}

int rh4nvarCreateNewBoolArray(RH4nVarList *varlist, char *pgroupname, char *pname, int dimensions, int length[3]) {
    return(rh4nvarCreateNewArray(varlist, pgroupname, pname, dimensions, length, RH4NVARTYPEBOOLEAN));
}

int rh4nvarCreateNewBoolArray_m(RH4nVarList *varlist, char **pgroupnames, char *pname, int dimensions, int length[3]) {
    return(rh4nvarCreateNewArray_m(varlist, pgroupnames, pname, dimensions, length, RH4NVARTYPEBOOLEAN));
}

int rh4nvarSetBoolArrayEntry(RH4nVarList *varlist, char *pgroupname, char *pname, int index[3], bool value) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    RH4nVarObj *arrayentry = NULL;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEARRAY) { return(RH4N_RET_VAR_NOT_ARRAY); }
    if((varlibret = rh4nvarGetArrayEntry(&_refvar.var->var, index, &arrayentry)) != RH4N_RET_OK) { return(varlibret); }

    if(arrayentry->type != RH4NVARTYPEBOOLEAN) { return(RH4N_RET_FORMAT_ERR); }
    if((varlibret = rh4nvarSetBool(arrayentry, value)) != RH4N_RET_OK) { return(varlibret); }
    return(RH4N_RET_OK);
}

//TODO: Canidate for a macro to save a Stack frame
int rh4nvarSetBoolArrayEntry_m(RH4nVarList *varlist, const char **pgroupname, const char *pname, int index[3], bool value) {
    return(rh4nvarSetArrayEntry_m(varlist, pgroupname, pname, index, (void*)(&value)));
}

int rh4nvarGetBoolArrayEntry(RH4nVarList *varlist, char *pgroupname, char *pname, int index[3], bool *outbuff) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    RH4nVarObj *arrayentry = NULL;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEARRAY) { return(RH4N_RET_VAR_NOT_ARRAY); }
    if((varlibret = rh4nvarGetArrayEntry(&_refvar.var->var, index, &arrayentry)) != RH4N_RET_OK) { return(varlibret); }

    if(arrayentry->type != RH4NVARTYPEBOOLEAN) { return(RH4N_RET_FORMAT_ERR); }
    *outbuff = *((bool*)arrayentry->value);
    return(RH4N_RET_OK);
}

int rh4nvarSetBool(RH4nVarObj *target, bool value) {
    int varsize = 0;

    varsize = sizeof(bool);
    if(target->value == NULL) { 
        if((target->value = malloc(varsize)) == NULL) { return(RH4N_RET_MEMORY_ERR); }
    }

    *((bool*)target->value) = value;
    target->length = varsize;
    return(RH4N_RET_OK);
}

