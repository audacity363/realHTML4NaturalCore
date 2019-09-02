#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "rh4n.h"

int rh4nvarDumpToFile(RH4nVarList *varlist, char *filename) {
    int outputfileFD = -1, endofList = RH4NVAR_END_OF_LIST;

    if((outputfileFD = open(filename, O_WRONLY)) < 0) {
        return(RH4N_RET_FILE_ERR);
    }

    rh4nvarsDumpNode(varlist->anker, outputfileFD);
    write(outputfileFD, &endofList, sizeof(endofList));

    close(outputfileFD);

    return(RH4N_RET_OK);
}

void rh4nvarDumpAppendToFile(int fileno, RH4nVarList *varlist) {
    int endofList = RH4NVAR_END_OF_LIST;

    rh4nvarsDumpNode(varlist->anker, fileno);
    write(fileno, &endofList, sizeof(endofList));
}

void rh4nvarsDumpNode(RH4nVarEntry_t *target, int outputfileFD) {
    RH4nVarEntry_t *hptr = target;

    for(; hptr != NULL; hptr = hptr->next) {
        rh4nvarsWriteNodeToFile(hptr, outputfileFD);
        if(hptr->nextlvl) {
            rh4nvarsDumpNode(hptr->nextlvl, outputfileFD);
        }
    }
}

void rh4nvarsWriteNodeToFile(RH4nVarEntry_t *target, int outputfileFD) {
    int i_tmp= 0;
    char c_tmp = 0;

    i_tmp = strlen(target->name);
    write(outputfileFD, &i_tmp, sizeof(i_tmp));
    write(outputfileFD, target->name, i_tmp);

    rh4nvarDumpValue(&target->var, outputfileFD);

    c_tmp = target->next == NULL ? 0 : 1;
    write(outputfileFD, &c_tmp, sizeof(c_tmp));

    c_tmp = target->nextlvl == NULL ? 0 : 1;
    write(outputfileFD, &c_tmp, sizeof(c_tmp));
}

void rh4nvarDumpValue(RH4nVarObj *target, int outputfileFD) {
    write(outputfileFD, &target->type, sizeof(target->type));
    if(target->type == RH4NVARTYPEGROUP) {
        return;
    }

    write(outputfileFD, &target->length, sizeof(target->length));

    if(target->type == RH4NVARTYPEARRAY) {
        rh4nvarDumpArray(target, outputfileFD);
        return;
    }
    if(target->length > 0) {
        write(outputfileFD, target->value, target->length);
    }
}

void rh4nvarDumpArray(RH4nVarObj *target, int outputfileFD) {
    int i = 0;
    RH4nVarObj *entry = NULL;

    for(; i < target->length; i++) {
        entry = &((RH4nVarObj*)target->value)[i];
        rh4nvarDumpValue(entry, outputfileFD);
    }
}

