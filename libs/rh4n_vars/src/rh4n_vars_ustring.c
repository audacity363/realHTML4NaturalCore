#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "rh4n.h"

int rh4nvarCreateNewUString(RH4nVarList *varlist, const char *pgroupname, const char *pname, wchar_t *value) {
    int varlibret = 0;
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    if((varlibret = rh4nvarCreatenewVariable(varlist, pgroupname, pname, RH4NVARTYPEUSTRING, &_refvar)) != RH4N_RET_OK) { 
        return(varlibret); 
    }

    if((varlibret = rh4nvarSetUString(&_refvar.var->var, value)) != RH4N_RET_OK) { return(varlibret); }
    return(RH4N_RET_OK);
}


int rh4nvarEditUString(RH4nVarList *varlist, const char *pgroupname, const char *pname, wchar_t *value) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEUSTRING) { return(RH4N_RET_FORMAT_ERR); }
    if((varlibret = rh4nvarSetUString(&_refvar.var->var, value)) != RH4N_RET_OK) { return(varlibret); }
    return(RH4N_RET_OK);
}

int rh4nvarGetUString(RH4nVarList *varlist, const char *pgroupname, const char *pname, int bufflength, wchar_t *outbuff) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEUSTRING) { return(RH4N_RET_FORMAT_ERR); }
    wcsncpy(outbuff, (wchar_t*)_refvar.var->var.value, bufflength);
    return(RH4N_RET_OK);
}

int rh4nvarGetUStringLength(RH4nVarList *varlist, const char *pgroupname, const char *pname, int *length) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEUSTRING) { return(RH4N_RET_FORMAT_ERR); }
    *length = wcslen((wchar_t*)_refvar.var->var.value);
    return(RH4N_RET_OK);
}

int rh4nvarCreateNewUStringArray(RH4nVarList *varlist, const char *pgroupname, const char *pname, int dimensions, int length[3]) {
    return(rh4nvarCreateNewArray(varlist, pgroupname, pname, dimensions, length, RH4NVARTYPEUSTRING));
}

int rh4nvarSetUStringArrayEntry(RH4nVarList *varlist, const char *pgroupname, const char *pname, int index[3], wchar_t *value) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    RH4nVarObj *arrayentry = NULL;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEARRAY) { return(RH4N_RET_VAR_NOT_ARRAY); }
    if((varlibret = rh4nvarGetArrayEntry(&_refvar.var->var, index, &arrayentry)) != RH4N_RET_OK) { return(varlibret); }

    if(arrayentry->type != RH4NVARTYPEUSTRING) { return(RH4N_RET_FORMAT_ERR); }
    if((varlibret = rh4nvarSetUString(arrayentry, value)) != RH4N_RET_OK) { return(varlibret); }
    return(RH4N_RET_OK);
}

int rh4nvarGetUStringArrayEntry(RH4nVarList *varlist, const char *pgroupname, const char *pname, int index[3], int bufflength, wchar_t *outbuff) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    RH4nVarObj *arrayentry = NULL;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEARRAY) { return(RH4N_RET_VAR_NOT_ARRAY); }
    if((varlibret = rh4nvarGetArrayEntry(&_refvar.var->var, index, &arrayentry)) != RH4N_RET_OK) { return(varlibret); }
    if(arrayentry->type != RH4NVARTYPEUSTRING) { return(RH4N_RET_FORMAT_ERR); }

    wcsncpy(outbuff, (wchar_t*)arrayentry->value, bufflength);
    return(RH4N_RET_OK);
}

int rh4nvarGetUStringLengthArrayEntry(RH4nVarList *varlist, const char *pgroupname, const char *pname, int index[3], int *length) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    RH4nVarObj *arrayentry = NULL;
    int varlibret = 0;

    if((varlibret = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) { return(varlibret); }
    if(_refvar.var->var.type != RH4NVARTYPEARRAY) { return(RH4N_RET_VAR_NOT_ARRAY); }
    if((varlibret = rh4nvarGetArrayEntry(&_refvar.var->var, index, &arrayentry)) != RH4N_RET_OK) { return(varlibret); }
    if(arrayentry->type != RH4NVARTYPEUSTRING) { return(RH4N_RET_FORMAT_ERR); }

    *length = wcslen((wchar_t*)arrayentry->value);
    return(RH4N_RET_OK);
}

int rh4nvarSetUString(RH4nVarObj *target, wchar_t *value) {
    int varsize = 0;

    varsize = (wcslen(value)+1)*sizeof(wchar_t);
    if(target->value != NULL) { free(target->value); }

    if((target->value = malloc(varsize)) == NULL) { return(RH4N_RET_MEMORY_ERR); }
    wcscpy((wchar_t*)target->value, value);
    target->length = varsize;
    return(RH4N_RET_OK);
}

