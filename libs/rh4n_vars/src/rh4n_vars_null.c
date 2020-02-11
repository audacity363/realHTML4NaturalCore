#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"

int rh4nvarCreteNewNull(RH4nVarList *varlist, const char **pgroupnames, const char *pname) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    return(rh4nvarCreatenewVariable_m(varlist, pgroupnames, pname, RH4NVARTYPENULL, &_refvar));
}

int rh4nvarCreateNewNullArray_m(RH4nVarList *varlist, const char **pgroupnames, const char *pname, int dimensions, int length[3]) {
    return(rh4nvarCreateNewArray_m(varlist, pgroupnames, pname, dimensions, length, RH4NVARTYPENULL));
}
