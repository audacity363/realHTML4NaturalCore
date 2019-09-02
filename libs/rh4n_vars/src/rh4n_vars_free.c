#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"

void rh4nvarFreeList(RH4nVarList *varlist) {
    rh4nvarFreeFork(varlist->anker);
}

void rh4nvarFreeFork(RH4nVarEntry_t *forkanker) {
    RH4nVarEntry_t *hptr = NULL, *prev = NULL;
    if(forkanker == NULL) { return; }
    
    for(hptr = forkanker; hptr->next != NULL; hptr = hptr->next);
    
    while(hptr != forkanker) {
        prev = hptr->prev;
        free(hptr->name);
        if(hptr->var.type == RH4NVARTYPEARRAY) rh4nvarFreeArray(hptr);
        else if(hptr->var.type == RH4NVARTYPEGROUP) rh4nvarFreeFork(hptr->nextlvl);
        else if(hptr->var.value) free(hptr->var.value);

        free(hptr);

        hptr = prev;
    }
    free(forkanker->name);
    if(hptr->var.type == RH4NVARTYPEARRAY) rh4nvarFreeArray(hptr);
    else if(hptr->var.type == RH4NVARTYPEGROUP) rh4nvarFreeFork(hptr->nextlvl);
    else if(forkanker->var.value) free(forkanker->var.value);
    free(forkanker);
}

void rh4nvarFreeArray(RH4nVarEntry_t *var) {
    int dimension = 0, index[3] = { -1, -1, -1 }, length[3] = { -1, -1, -1 };
    RH4nVarObj *entry[3] = {NULL, NULL, NULL };

    rh4nvarGetArrayDimension(&var->var, &dimension);
    rh4nvarGetArrayLength(&var->var, length);
    
    for(index[0] = 0;index[0] < length[0]; index[0]++) {
        index[1] = index[2] = -1;
        rh4nvarGetArrayEntry(&var->var, index, &entry[0]);
        if(dimension > 1) {
            for(index[1] = 0; index[1] < length[1]; index[1]++) {
                index[2] = -1;
                rh4nvarGetArrayEntry(&var->var, index, &entry[1]);
                if(dimension > 2) {
                    for(index[2] = 0; index[2] < length[2]; index[2]++) {
                        rh4nvarGetArrayEntry(&var->var, index, &entry[2]);
                        if(entry[2]->value) free(entry[2]->value);
                    }
                } 
                if(entry[1]->value) free(entry[1]->value);
            }
        }
        if(entry[0]->value) free(entry[0]->value);
    }
    if(var->var.value) free(var->var.value);
}
