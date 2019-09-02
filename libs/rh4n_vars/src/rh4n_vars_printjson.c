#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <errno.h>

#include "rh4n.h"
#include "rh4n_json.h"

int rh4nvarPrintJSONToFile(RH4nVarList *varlist, char *filename, RH4nProperties *props) {
    FILE *outputfile = NULL;
    
    if((outputfile = fopen(filename, "w")) == NULL) {
        rh4n_log_fatal(props->logging, "Error while opening file [%s]: %s", filename, strerror(errno));
        return(RH4N_RET_INTERNAL_ERR);
    }

    rh4nvarPrintJSONFork(varlist->anker, -1, 1, props, outputfile);

    fclose(outputfile);
    return(RH4N_RET_OK);
}

void rh4nvarPrintJSONFork(RH4nVarEntry_t *forkanker, int mode, int level, RH4nProperties *props, FILE *outputfile) {
    if(!forkanker) { return; }

    RH4nVarEntry_t *hptr = forkanker;

    if(checkArrayGroup(forkanker)) {
        rh4nvarPrintGroupJSONArray(forkanker, mode, level, props, outputfile); 
        return;
    }

    if(level == 1) fprintf(outputfile, "{");
    for(; hptr != NULL; hptr = hptr->next) {
        fprintf(outputfile, "\"%s\":", hptr->name);
        if(hptr->var.type == RH4NVARTYPEARRAY) { rh4nvarPrintJSONArray(&hptr->var, mode, level, props, outputfile); }
        else { rh4nvarPrintVar(&hptr->var, props, outputfile); }
        if(hptr->var.type == RH4NVARTYPEGROUP) { 
            if(checkArrayGroup(hptr->nextlvl)) {
                rh4nvarPrintGroupJSONArray(hptr->nextlvl, mode, level, props, outputfile); 
            }
            else {
                fprintf(outputfile, "{");
                rh4nvarPrintJSONFork(hptr->nextlvl, mode, level+1, props, outputfile); 
                fprintf(outputfile, "}");
            }
        }

        if(hptr->next) fprintf(outputfile, ",");
    }
    if(level == 1) fprintf(outputfile, "}");
}

void rh4nvarPrintJSONArray(RH4nVarObj *variable, int mode, int level,RH4nProperties *props, FILE *outputfile) {
    rh4nvarPrintJSONArrayDim(variable, mode, level, props, outputfile);
}

void rh4nvarPrintJSONArrayDim(RH4nVarObj *variable, int mode, int level, RH4nProperties *props, FILE *outputfile) {
    int i = 0;
    RH4nVarObj *target = NULL;

    fprintf(outputfile, "[");

    for(; i < variable->length; i++) {
        target = &((RH4nVarObj*)variable->value)[i];
        if(target->type == RH4NVARTYPEARRAY) {
            rh4nvarPrintJSONArrayDim(target, mode, level+1, props, outputfile);
        } else {
            rh4nvarPrintVar(target, props, outputfile);
        }
        if(i+1 < variable->length) {
            fprintf(outputfile, ",");
        }
    }

    fprintf(outputfile, "]");

}

void rh4nvarPrintGroupJSONArray(RH4nVarEntry_t *variable, int mode, int level, RH4nProperties *props, FILE *outputfile) {
    int dimension = -1, length[3] = { -1, -1, -1 }, varlibret = 0;

    if((varlibret = rh4nvarGetArrayDimension(&variable->var, &dimension)) != RH4N_RET_OK) { return; }
    if((varlibret = rh4nvarGetArrayLength(&variable->var, length)) != RH4N_RET_OK) { return; }

    switch(dimension) {
        case(1):
            rh4nvarPrintGroup1DJSONArray(variable, mode, level, props, length, outputfile);
            break;
        case(2):
            rh4nvarPrintGroup2DJSONArray(variable, mode, level, props, length, outputfile);
            break;
        case(3):
            rh4nvarPrintGroup3DJSONArray(variable, mode, level, props, length, outputfile);
            break;
    }
}

void rh4nvarPrintGroup1DJSONArray(RH4nVarEntry_t *variable, int mode, int level, RH4nProperties *props, int length[3], FILE *outputfile) {
    int index[3] = { 0, 0, 0}, varlibret = 0;
    RH4nVarEntry_t *hptr = NULL;
    RH4nVarObj *target = NULL;

    index[1] = index[2] = -1;
    fprintf(outputfile, "[");
    for(; index[0] < length[0]; index[0]++) {
        fprintf(outputfile, "{");
        for(hptr = variable; hptr != NULL; hptr = hptr->next) {
            if((varlibret = rh4nvarGetArrayEntry(&hptr->var, index, &target)) != RH4N_RET_OK) { return; }
            fprintf(outputfile, "\"%s\":", hptr->name);
            rh4nvarPrintVar(target, props, outputfile);
            if(hptr->next) fprintf(outputfile, ",");
        }
        fprintf(outputfile, "}");
        if(index[0]+1 < length[0]) fprintf(outputfile, ",");
    }
    fprintf(outputfile, "]");
}

void rh4nvarPrintGroup2DJSONArray(RH4nVarEntry_t *variable, int mode, int level, RH4nProperties *props, int length[3], FILE *outputfile) {
    int index[3] = { 0, 0, 0}, varlibret = 0;
    RH4nVarEntry_t *hptr = NULL;
    RH4nVarObj *target = NULL;

    index[2] = -1;
    fprintf(outputfile, "[");
    for(; index[0] < length[0]; index[0]++) {
        fprintf(outputfile, "[");
        for(index[1] = 0; index[1] < length[1]; index[1]++) {
            fprintf(outputfile, "{");
            for(hptr = variable; hptr != NULL; hptr = hptr->next) {
                if((varlibret = rh4nvarGetArrayEntry(&hptr->var, index, &target)) != RH4N_RET_OK) { return; }
                fprintf(outputfile, "\"%s\":", hptr->name);
                rh4nvarPrintVar(target, props, outputfile);
                if(hptr->next) fprintf(outputfile, ",");
            }
            fprintf(outputfile, "}");
            if(index[1]+1 < length[1]) fprintf(outputfile, ",");
        }
        fprintf(outputfile, "]");
        if(index[0]+1 < length[0]) fprintf(outputfile, ",");
    }
    fprintf(outputfile, "]");
}

void rh4nvarPrintGroup3DJSONArray(RH4nVarEntry_t *variable, int mode, int level, RH4nProperties *props, int length[3], FILE *outputfile) {
    int index[3] = { 0, 0, 0}, varlibret = 0;
    RH4nVarEntry_t *hptr = NULL;
    RH4nVarObj *target = NULL;

    fprintf(outputfile, "[");
    for(; index[0] < length[0]; index[0]++) {
        fprintf(outputfile, "[");
        for(index[1] = 0; index[1] < length[1]; index[1]++) {
            fprintf(outputfile, "[");
            for(index[2] = 0; index[2] < length[2]; index[2]++) {
                fprintf(outputfile, "{");
                for(hptr = variable; hptr != NULL; hptr = hptr->next) {
                    if((varlibret = rh4nvarGetArrayEntry(&hptr->var, index, &target)) != RH4N_RET_OK) { return; }
                    fprintf(outputfile, "\"%s\":", hptr->name);
                    rh4nvarPrintVar(target, props, outputfile);
                    if(hptr->next) fprintf(outputfile, ",");
                }
                fprintf(outputfile, "}");
                if(index[2]+1 < length[2]) fprintf(outputfile, ",");
            }
            fprintf(outputfile, "]");
            if(index[1]+1 < length[1]) fprintf(outputfile, ",");
        }
        fprintf(outputfile, "]");
        if(index[0]+1 < length[0]) fprintf(outputfile, ",");
    }
    fprintf(outputfile, "]");
}
