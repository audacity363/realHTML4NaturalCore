#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "rh4n.h"

int rh4nvarLoadFromFile(char *filename, RH4nVarList *varlist) {
    int inputfile = -1, ret = 0;

    if((inputfile = open(filename, O_RDONLY)) < 0) {
        return(1);
    }

    ret = rh4nvarsLoadNode(inputfile, varlist);

    close(inputfile);
    
    return(ret);
}

int rh4nvarLoadFromFileOffset(int fileno, RH4nVarList *varlist) {
    return(rh4nvarsLoadNode(fileno, varlist)); 
}

int rh4nvarsLoadNode(int inputfile, RH4nVarList *varlist) {
    int namelength = -1, ret = 0;
    char next = -1, nextlvl = -1,
         prevnext = -1, prevnextlvl = -1;

    RH4nVarEntry_t *newNode = NULL, *lastNode = NULL;

    while(1) {
        //End of file
        ret = read(inputfile, &namelength, sizeof(namelength));
        printf("readret: [%d]\n", ret);
        if(ret == 0) {
            break;
        } else if(namelength == RH4NVAR_END_OF_LIST) {
            break;
        }
        printf("namelength: [%d]\n", namelength);
        if(namelength == 0) {
            break;
        }

        if((newNode = malloc(sizeof(RH4nVarEntry_t))) == NULL) return(RH4N_RET_MEMORY_ERR);
        memset(newNode, 0x00, sizeof(RH4nVarEntry_t));

        if((newNode->name = malloc(sizeof(char)*(namelength+1))) == NULL) return(RH4N_RET_MEMORY_ERR);

        memset(newNode->name, 0x00, sizeof(char)*(namelength+1));
        read(inputfile, newNode->name, namelength);

        if((ret = rh4nvarsLoadValue(inputfile, &newNode->var)) != RH4N_RET_OK) return(ret);

        prevnext = next; prevnextlvl = nextlvl;

        read(inputfile, &next, sizeof(next));
        read(inputfile, &nextlvl, sizeof(nextlvl));

        rh4nvarsPutInNewNode(prevnext, prevnextlvl, newNode, lastNode, varlist);
        lastNode = newNode;
    }

    return(RH4N_RET_OK);
}

int rh4nvarsLoadValue(int inputfile, RH4nVarObj *target) {
    read(inputfile, &target->type, sizeof(target->type));

    if(target->type == RH4NVARTYPEGROUP) {
        return(RH4N_RET_OK);
    }

    read(inputfile, &target->length, sizeof(target->length));

    if(target->type == RH4NVARTYPEARRAY) {
        return(rh4nvarsLoadArray(inputfile, target));
    }

    if(target->length > 0) {
        if((target->value = malloc(target->length)) == NULL) return(RH4N_RET_MEMORY_ERR);
        read(inputfile, target->value, target->length);
    } else {
        target->value = NULL;
    }

    return(RH4N_RET_OK);
}

int rh4nvarsLoadArray(int inputfile, RH4nVarObj *target) {
    int i = 0, ret = 0;

    target->value = malloc(sizeof(RH4nVarObj)*target->length);

    for(; i < target->length; i++) {
        if((rh4nvarsLoadValue(inputfile, &((RH4nVarObj*)target->value)[i])) != RH4N_RET_OK) return(ret);
    }
    return(0);
}

void rh4nvarsPutInNewNode(char next, char nextlvl, RH4nVarEntry_t *newNode, RH4nVarEntry_t *lastNode, RH4nVarList *varlist) {
    RH4nVarEntry_t *hptr = NULL;

    if(lastNode == NULL) {
        varlist->anker = newNode;
        return;
    }
    
    if(nextlvl == 1) {
        newNode->prev = lastNode;
        lastNode->nextlvl = newNode;
    } else if(next == 1) {
        newNode->prev = lastNode;
        lastNode->next = newNode;
    } else if(next == 0 && nextlvl == 0) {
        for(hptr = lastNode; hptr != NULL && (hptr->var.type != RH4NVARTYPEGROUP || hptr->next != NULL); hptr = hptr->prev);
        if(hptr == NULL) return;

        hptr->next = newNode;
        newNode->prev = hptr;
    }
}
