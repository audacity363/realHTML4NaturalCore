#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"

int rh4nvarCreateNewGroup(RH4nVarList *varlist, char *pgroupname) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    return(rh4nvarCreatenewVariable(varlist, NULL, pgroupname, RH4NVARTYPEGROUP, &_refvar));
}

int rh4nvarCreateNewGroup_m(RH4nVarList *varlist, char **pparentgroups, char *pgroupname) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;
    
    return(rh4nvarCreatenewVariable_m(varlist, pparentgroups, pgroupname, RH4NVARTYPEGROUP, &_refvar));
}


int rh4nvarMoveVarToGroup(RH4nVarList *varlist, char *pvarname, char *pgroupname) {
    RH4nVarRef _groupref = RH4NVAR_REF_INIT, _varref = RH4NVAR_REF_INIT;
    int varlibret = 0;

    if((varlibret = rh4nvarSearchVarRef(varlist->anker, pgroupname, &_groupref)) != RH4N_RET_OK) { return(varlibret); }
    if(_groupref.var->var.type != RH4NVARTYPEGROUP) { return(RH4N_RET_FORMAT_ERR); }

    if((varlibret = rh4nvarSearchVarRef(varlist->anker, pvarname, &_varref)) != RH4N_RET_OK) { return(varlibret); }
    
    _rh4nvarmoveToGroup(varlist, _groupref.var, _varref.var);

    return(RH4N_RET_OK);
}

int rh4nvarMoveVarToGroup_m(RH4nVarList *varlist, char *pvarname, char **pgroupnames) {
    RH4nVarRef _refgrp = RH4NVAR_REF_INIT, _refvar = RH4NVAR_REF_INIT;
    RH4nVarEntry_t *forkanker = NULL;
    int i = 0, x = 0, varlibret = 0;

    forkanker = varlist->anker;

    for(; pgroupnames[i] != NULL; i++) {
        if((varlibret = rh4nvarSearchVarRef(forkanker, pgroupnames[i], &_refgrp)) != RH4N_RET_OK) {
            return(varlibret);
        }
        forkanker = _refgrp.var->nextlvl;
    }

    if((varlibret = rh4nvarSearchVarRef(varlist->anker, pvarname, &_refvar)) != RH4N_RET_OK) {
        return(varlibret);
    }

    _rh4nvarmoveToGroup(varlist, _refgrp.var, _refvar.var);

    return(RH4N_RET_OK);
}

void _rh4nvarmoveToGroup(RH4nVarList *varlist, RH4nVarEntry_t *targetgrp, RH4nVarEntry_t *targetvar) {
    RH4nVarEntry_t *hptr = NULL;

    if(targetvar->prev == NULL) { 
        varlist->anker = targetvar->next;
        varlist->anker->prev = NULL;
    } else {
        targetvar->prev->next = targetvar->next;
        if(targetvar->next != NULL) 
            targetvar->next->prev = targetvar->prev;
    }

    targetvar->prev = targetvar->next = NULL;

    if(targetgrp->nextlvl == NULL) {
        targetgrp->nextlvl = targetvar;
        targetvar->prev = targetgrp;
    } else {
        for(hptr = targetgrp->nextlvl; hptr->next != NULL; hptr = hptr->next);
        hptr->next = targetvar;
        targetvar->prev = hptr;
    }
}

int rh4nvarGroupExist(RH4nVarList *varlist, char *pgroupname) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    if(rh4nvarGetRef(varlist, NULL, pgroupname, &_refvar) != RH4N_RET_OK) return(0);
    if(_refvar.var->var.type != RH4NVARTYPEGROUP) return(0);
    return(1);
}

int rh4nvarGroupExist_m(RH4nVarList *varlist, char **parentgroups, char *pgroupname) {
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    if(rh4nvarGetRef_m(varlist, parentgroups, pgroupname, &_refvar) != RH4N_RET_OK) return(0);
    if(_refvar.var->var.type != RH4NVARTYPEGROUP) return(0);
    return(1);
}
