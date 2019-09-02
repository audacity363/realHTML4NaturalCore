#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"

int rh4nvarCreateNewFloat(RH4nVarList *varlist, char *pgroupname, char *pname, double value) {
    int varlibret = 0;
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    if((varlibret = rh4nvarCreatenewVariable(varlist, pgroupname, pname, RH4NVARTYPEFLOAT, &_refvar)) != RH4N_RET_OK) { 
        return(varlibret); 
    }

    return(_rh4nvarCreateNewFloat(&_refvar, value));
}

int rh4nvarCreateNewFloat_m(RH4nVarList *varlist, char **pgroupnames, char *pname, double value) {
    int varlibret = 0;
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    if((varlibret = rh4nvarCreatenewVariable_m(varlist, pgroupnames, pname, RH4NVARTYPEFLOAT, &_refvar)) != RH4N_RET_OK) { 
        return(varlibret); 
    }

    return(_rh4nvarCreateNewFloat(&_refvar, value));
}

int _rh4nvarCreateNewFloat(RH4nVarRef *ref, double value) {
    return(rh4nvarSetFloat(&ref->var->var, value));
}


int rh4nvarEditFloat(RH4nVarList *varlist, char *pgroupname, char *pname, double value) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEFLOAT) { return(RH4N_RET_FORMAT_ERR); }
    if((varlibret = rh4nvarSetFloat(&_refvar.var->var, value)) != RH4N_RET_OK) { return(varlibret); }
    return(RH4N_RET_OK);
}

int rh4nvarGetFloat(RH4nVarList *varlist, char *pgroupname, char *pname, double *outbuff) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEFLOAT) { return(RH4N_RET_FORMAT_ERR); }
    *outbuff = *((double*)_refvar.var->var.value);
    return(RH4N_RET_OK);
}

int rh4nvarCreateNewFloatArray(RH4nVarList *varlist, char *pgroupname, char *pname, int dimensions, int length[3]) {
    return(rh4nvarCreateNewArray(varlist, pgroupname, pname, dimensions, length, RH4NVARTYPEFLOAT));
}

int rh4nvarCreateNewFloatArray_m(RH4nVarList *varlist, char **pgroupnames, char *pname, int dimensions, int length[3]) {
    return(rh4nvarCreateNewArray_m(varlist, pgroupnames, pname, dimensions, length, RH4NVARTYPEFLOAT));
}

int rh4nvarSetFloatArrayEntry(RH4nVarList *varlist, char *pgroupname, char *pname, int index[3], double value) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    RH4nVarObj *arrayentry = NULL;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEARRAY) { return(RH4N_RET_VAR_NOT_ARRAY); }
    if((varlibret = rh4nvarGetArrayEntry(&_refvar.var->var, index, &arrayentry)) != RH4N_RET_OK) { return(varlibret); }

    if(arrayentry->type != RH4NVARTYPEFLOAT) { return(RH4N_RET_FORMAT_ERR); }
    if((varlibret = rh4nvarSetFloat(arrayentry, value)) != RH4N_RET_OK) { return(varlibret); }
    return(RH4N_RET_OK);
}

//TODO: Canidate for a macro to save a Stack frame
int rh4nvarSetFloatArrayEntry_m(RH4nVarList *varlist, const char **pgroupname, const char *pname, int index[3], double value) {
    return(rh4nvarSetArrayEntry_m(varlist, pgroupname, pname, index, (void*)(&value)));
}

int rh4nvarGetFloatArrayEntry(RH4nVarList *varlist, char *pgroupname, char *pname, int index[3], double *outbuff) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    RH4nVarObj *arrayentry = NULL;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEARRAY) { return(RH4N_RET_VAR_NOT_ARRAY); }
    if((varlibret = rh4nvarGetArrayEntry(&_refvar.var->var, index, &arrayentry)) != RH4N_RET_OK) { return(varlibret); }

    if(arrayentry->type != RH4NVARTYPEFLOAT) { return(RH4N_RET_FORMAT_ERR); }
    *outbuff = *((double*)arrayentry->value);
    return(RH4N_RET_OK);
}

int rh4nvarSetFloat(RH4nVarObj *target, double value) {
    int varsize = 0;

    varsize = sizeof(double);
    if(target->value == NULL) { 
        if((target->value = malloc(varsize)) == NULL) { return(RH4N_RET_MEMORY_ERR); }
    }

    memcpy(target->value, &value, varsize);
    target->length = varsize;
    return(RH4N_RET_OK);
}

