#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"

int rh4nvarInitList(RH4nVarList *varlist) {
    RH4NVAR_CHECKVARLIST(varlist);
    varlist->anker = NULL;
    return(RH4N_RET_OK);
}

int rh4nvarCreatenewVariable(RH4nVarList *varlist, const char *pgroupname, const char *pname, int type, RH4nVarRef *pnewvar) {
    RH4NVAR_CHECKVARLIST(varlist);
    if(!pnewvar) { return(RH4N_RET_PARM_ERR); }
    
    if(rh4nvarExist(varlist, pgroupname, pname)) { return(RH4N_RET_VAR_EXISTS); }

    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    RH4nVarEntry_t *forkanker = NULL, *newvarEntry = NULL, *hptr = NULL;
    int searchret = 0;

    if(pgroupname) {
        if((searchret = rh4nvarGetRef(varlist, NULL, pgroupname, &_refvar)) != RH4N_RET_OK) {
            return(searchret);
        }
        if(!_refvar.var) { return(RH4N_RET_VAR_PTR_ERR); }
        if(_refvar.var->var.type != RH4NVARTYPEGROUP) { return(RH4N_RET_UNKOWN_VAR); }
        if(!_refvar.var->nextlvl) { 
            RH4NVAR_NEW(newvarEntry);
            RH4NVAR_ENTRY_INIT(newvarEntry);
            _refvar.var->nextlvl = newvarEntry;
            newvarEntry->prev = _refvar.var;
        } else {
            forkanker = _refvar.var->nextlvl;
        }
    } else {
        if(!varlist->anker) {
            RH4NVAR_NEW(newvarEntry);
            RH4NVAR_ENTRY_INIT(newvarEntry);
            varlist->anker = newvarEntry;
        } else {
            forkanker = varlist->anker;
        }
    }

    if(!newvarEntry) {
        for(hptr = forkanker; hptr->next != NULL; hptr = hptr->next);
        RH4NVAR_NEW(newvarEntry);
        RH4NVAR_ENTRY_INIT(newvarEntry);
        hptr->next = newvarEntry;
        newvarEntry->prev = hptr;
    }

    if((newvarEntry->name = malloc((strlen(pname)+1)*sizeof(char))) == NULL) { return(RH4N_RET_MEMORY_ERR); }
    strcpy(newvarEntry->name, pname);

    newvarEntry->var.type = type;
    pnewvar->name = newvarEntry->name;
    pnewvar->var = newvarEntry;
    return(RH4N_RET_OK);
}

int rh4nvarCreatenewVariable_m(RH4nVarList *varlist, const char **pgroupname, const char *pname, int type, RH4nVarRef *pnewvar) {
    RH4nVarList tmplist, *targetlist = NULL;
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int i = 0, varlibret = 0;
    const char *grpname = NULL;

    if(varlist->anker != NULL) {
        rh4nvarInitList(&tmplist);
        tmplist.anker = varlist->anker;

        if(pgroupname) {
            if(pgroupname[0] != NULL) {
                for(;pgroupname[i+1] != NULL; i++) {
                    if((varlibret = rh4nvarGetRef(&tmplist, NULL, pgroupname[i], &_refvar)) != RH4N_RET_OK) {
                        return(varlibret);
                    }
                    tmplist.anker = _refvar.var->nextlvl;
                }
            }
        }

        if(pgroupname != NULL) {
            grpname = pgroupname[i];
        } 

        targetlist = &tmplist;
    } else {
        targetlist = varlist;
    }

    return(rh4nvarCreatenewVariable(targetlist, grpname, pname, type, pnewvar));
}

int rh4nvarGetRef(RH4nVarList *varlist, const char *pgroupname, const char *pname, RH4nVarRef *prefvar) {
    RH4NVAR_CHECKVARLIST(varlist);
    if(!prefvar) { return(RH4N_RET_PARM_ERR); }

    RH4nVarEntry_t *forkanker = NULL;
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int searchret = 0;

    if(pgroupname) {
        if((searchret = rh4nvarSearchVarRef(varlist->anker, pgroupname, &_refvar)) != RH4N_RET_OK) {
            return(searchret);
        }
        if(!_refvar.var) { return(RH4N_RET_VAR_PTR_ERR); }
        if(_refvar.var->var.type != RH4NVARTYPEGROUP) { return(RH4N_RET_UNKOWN_VAR); }
        if(!_refvar.var->nextlvl) { return(RH4N_RET_VAR_PTR_ERR); }
        forkanker = _refvar.var->nextlvl;
    } else {
        forkanker = varlist->anker;
    }

    RH4NVAR_REF_RESET(_refvar);
    if((searchret = rh4nvarSearchVarRef(forkanker, pname, &_refvar)) != RH4N_RET_OK) {
        return(searchret);
    }
    if(!_refvar.var) { return(RH4N_RET_VAR_PTR_ERR); }

    prefvar->name = _refvar.name;
    prefvar->var = _refvar.var;
    return(RH4N_RET_OK);
}

int rh4nvarGetRef_m(RH4nVarList *varlist, const char **pgroupname, const char *pname, RH4nVarRef *prefvar) {
    RH4nVarList tmplist;
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int i = 0, varlibret = 0;

    rh4nvarInitList(&tmplist);
    tmplist.anker = varlist->anker;

    if(pgroupname) {
        for(;pgroupname[i] != NULL; i++) {
            if((varlibret = rh4nvarGetRef(&tmplist, NULL, pgroupname[i], &_refvar)) != RH4N_RET_OK) {
                return(varlibret);
            }
            tmplist.anker = _refvar.var->nextlvl;
        }
    }
    return(rh4nvarGetRef(&tmplist, NULL, pname, prefvar));
}


int rh4nvarSearchVarRef(RH4nVarEntry_t *forkanker, const char *pname, RH4nVarRef *prefvar) {
    RH4NVAR_CHECKVARLIST(forkanker);
    if(!prefvar) { return(RH4N_RET_PARM_ERR); }

    RH4nVarEntry_t *hptr = forkanker;

    for(; hptr != NULL; hptr = hptr->next) {
        if(strcmp(hptr->name, pname) == 0) {
            prefvar->name = hptr->name;
            prefvar->var = hptr;
            return(RH4N_RET_OK);
        }
    }
    return(RH4N_RET_UNKOWN_VAR);
}

int rh4nvarExist(RH4nVarList *varlist, const char *pgroupname, const char *pname) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    if(rh4nvarGetRef(varlist, pgroupname, pname, &_refvar) != RH4N_RET_OK) {
        return(0);
    }
    return(1);
}

int rh4nvarGetVarType(RH4nVarList *varlist, const char *pgroupname, const char *pname, int *vartype) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    int rc = 0;

    if((rc = rh4nvarGetRef(varlist, pgroupname, pname, &_refvar)) != RH4N_RET_OK) return(rc);
    *vartype = _refvar.var->var.type; 
    return(RH4N_RET_OK);
}
