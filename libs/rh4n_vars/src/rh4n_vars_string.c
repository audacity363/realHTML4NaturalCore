#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"

int rh4nvarCreateNewString(RH4nVarList *varlist, char *pgroupname, char *pname, char *value) {
    int varlibret = 0;
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    if((varlibret = rh4nvarCreatenewVariable(varlist, pgroupname, pname, RH4NVARTYPESTRING, &_refvar)) != RH4N_RET_OK) { 
        return(varlibret); 
    }
    return(_rh4nvarCreateNewString(&_refvar, value));
}

int rh4nvarCreateNewString_m(RH4nVarList *varlist, char **pgroupnames, char *pname, char *value) {
    int varlibret = 0;
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    if((varlibret = rh4nvarCreatenewVariable_m(varlist, pgroupnames, pname, RH4NVARTYPESTRING, &_refvar)) != RH4N_RET_OK) { 
        return(varlibret); 
    }
    return(_rh4nvarCreateNewString(&_refvar, value));
}


int _rh4nvarCreateNewString(RH4nVarRef *ref, char *value) {
    return(rh4nvarSetString(&ref->var->var, value));
}


int rh4nvarEditString(RH4nVarList *varlist, char *pgroupname, char *pname, char *value) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPESTRING) { return(RH4N_RET_FORMAT_ERR); }
    if((varlibret = rh4nvarSetString(&_refvar.var->var, value)) != RH4N_RET_OK) { return(varlibret); }
    return(RH4N_RET_OK);
}

int rh4nvarGetString(RH4nVarList *varlist, char *pgroupname, char *pname, int bufflength, char *outbuff) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPESTRING) { return(RH4N_RET_FORMAT_ERR); }
    strncpy(outbuff, (char*)_refvar.var->var.value, bufflength);
    return(RH4N_RET_OK);
}

int rh4nvarGetStringLength(RH4nVarList *varlist, char *pgroupname, char *pname, int *length) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPESTRING) { return(RH4N_RET_FORMAT_ERR); }
    *length = strlen((char*)_refvar.var->var.value);
    return(RH4N_RET_OK);
}

int rh4nvarCreateNewStringArray(RH4nVarList *varlist, char *pgroupname, char *pname, int dimensions, int length[3]) {
    return(rh4nvarCreateNewArray(varlist, pgroupname, pname, dimensions, length, RH4NVARTYPESTRING));
}

int rh4nvarCreateNewStringArray_m(RH4nVarList *varlist, char **pgroupname, char *pname, int dimensions, int length[3]) {
    return(rh4nvarCreateNewArray_m(varlist, pgroupname, pname, dimensions, length, RH4NVARTYPESTRING));
}

int rh4nvarSetStringArrayEntry(RH4nVarList *varlist, char *pgroupname, char *pname, int index[3], char *value) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    RH4nVarObj *arrayentry = NULL;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEARRAY) { return(RH4N_RET_VAR_NOT_ARRAY); }
    if((varlibret = rh4nvarGetArrayEntry(&_refvar.var->var, index, &arrayentry)) != RH4N_RET_OK) { return(varlibret); }

    if(arrayentry->type != RH4NVARTYPESTRING) { return(RH4N_RET_FORMAT_ERR); }
    if((varlibret = rh4nvarSetString(arrayentry, value)) != RH4N_RET_OK) { return(varlibret); }
    return(RH4N_RET_OK);
}

//TODO: Canidate for a macro to save a Stack frame
int rh4nvarSetStringArrayEntry_m(RH4nVarList *varlist, const char **pgroupname, const char *pname, int index[3], char *value) {
    return(rh4nvarSetArrayEntry_m(varlist, pgroupname, pname, index, (void*)value));
}

int rh4nvarGetStringArrayEntry(RH4nVarList *varlist, char *pgroupname, char *pname, int index[3], int bufflength, char *outbuff) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    RH4nVarObj *arrayentry = NULL;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEARRAY) { return(RH4N_RET_VAR_NOT_ARRAY); }
    if((varlibret = rh4nvarGetArrayEntry(&_refvar.var->var, index, &arrayentry)) != RH4N_RET_OK) { return(varlibret); }
    if(arrayentry->type != RH4NVARTYPESTRING) { return(RH4N_RET_FORMAT_ERR); }

    strncpy(outbuff, (char*)arrayentry->value, bufflength);
    return(RH4N_RET_OK);
}

int rh4nvarGetStringLengthArrayEntry(RH4nVarList *varlist, char *pgroupname, char *pname, int index[3], int *length) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    RH4nVarObj *arrayentry = NULL;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEARRAY) { return(RH4N_RET_VAR_NOT_ARRAY); }
    if((varlibret = rh4nvarGetArrayEntry(&_refvar.var->var, index, &arrayentry)) != RH4N_RET_OK) { return(varlibret); }
    if(arrayentry->type != RH4NVARTYPESTRING) { return(RH4N_RET_FORMAT_ERR); }

    *length = strlen((char*)arrayentry->value);
    return(RH4N_RET_OK);
}

int rh4nvarSetString(RH4nVarObj *target, char *value) {
    int varsize = 0;

    varsize = (strlen(value)+1)*sizeof(char);
    if(target->value != NULL) { free(target->value); }

    if((target->value = malloc(varsize)) == NULL) { return(RH4N_RET_MEMORY_ERR); }
    strcpy((char*)target->value, value);
    target->length = varsize;
    return(RH4N_RET_OK);
}

