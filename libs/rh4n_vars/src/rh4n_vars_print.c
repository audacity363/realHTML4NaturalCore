#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "rh4n.h"

#define RH4N_VAR_PRINT_TABS "\t\t\t\t\t\t"

void rh4nvarPrintList(RH4nVarList *varlist, RH4nProperties *props) {
    if(!varlist) { 
        printf("varlist is empty\n");
        return; 
    }
    
    rh4nvarPrintFork(varlist->anker, -1, 0, props);
}

void rh4nvarPrintFork(RH4nVarEntry_t *forkanker, int mode, int level, RH4nProperties *props) {
    if(!forkanker) { return; }

    RH4nVarEntry_t *hptr = forkanker;
    int dimensions = 0, length[3];

    for(; hptr != NULL; hptr = hptr->next) {
        RH4NVARPRINTFUNC(props, "%.*sName: %s", level, RH4N_VAR_PRINT_TABS, hptr->name);
        RH4NVARPRINTFUNC(props, "%.*sType: %s", level, RH4N_VAR_PRINT_TABS, rh4nvarPrintGetTypeString(hptr->var.type));
        if(hptr->var.type == RH4NVARTYPEARRAY) {
            memset(length, 0x00, sizeof(length));
            dimensions = 0;
            rh4nvarGetArrayDimension(&hptr->var, &dimensions);
            rh4nvarGetArrayLength(&hptr->var, length);
            switch(dimensions) {
                case 1:
                    RH4NVARPRINTFUNC(props, "%.*sDims: 1 x: %d", level, RH4N_VAR_PRINT_TABS, length[0]);
                    break;
                case 2:
                    RH4NVARPRINTFUNC(props, "%.*sDims: 2 x: %d y: %d", level, RH4N_VAR_PRINT_TABS, length[0], length[1]);
                    break;
                case 3:
                    RH4NVARPRINTFUNC(props, "%.*sDims: 3 x: %d y: %d z: %d", level, RH4N_VAR_PRINT_TABS, length[0], length[1], length[2]);
                    break;
            }
        }
        RH4NVARPRINTFUNC(props, " ");
        if(hptr->var.type == RH4NVARTYPEGROUP) rh4nvarPrintFork(hptr->nextlvl, mode, level+1, props);
    }
}

char *rh4nvarPrintGetTypeString(int vartype) {
    static char *typestrs[] = {
        "String",
        "Unicode String",
        "Integer",
        "Float", 
        "Boolean",
        "Group",
        "Array"
    };

    if(vartype > sizeof(typestrs)/sizeof(char*)) return(NULL);
    return(typestrs[vartype-1]);
}


void rh4nvarPrintVar(RH4nVarObj *variable, RH4nProperties *props, FILE *outputfile) {
    RH4nVarPrint printTable[] = {
        {RH4NVARTYPESTRING, rh4nvarPrintString},
        {RH4NVARTYPEUSTRING, rh4nvarPrintUString},
        {RH4NVARTYPEBOOLEAN, rh4nvarPrintBool},
        {RH4NVARTYPEINTEGER, rh4nvarPrintInt},
        {RH4NVARTYPEFLOAT, rh4nvarPrintFloat}
    };

    int i = 0;
    for(; i < sizeof(printTable)/sizeof(RH4nVarPrint); i++) {
        if(variable->type == printTable[i].vartype) { printTable[i].printfunction(variable, props, outputfile); }
    }
}

bool checkArrayGroup(RH4nVarEntry_t *variable) {
    RH4nVarEntry_t *hptr = NULL;
    int length[3] = { -1, -1, -1}, curlength[3] = { -1, -1, -1}, dimension = -1, curdimension = -1,
        varlibret = 0;

    for(hptr = variable; hptr != NULL; hptr = hptr->next) {
        if(hptr->var.type != RH4NVARTYPEARRAY) { return(false); }

        if((varlibret = rh4nvarGetArrayDimension(&hptr->var, &curdimension)) != RH4N_RET_OK) { return(false); }
        if(dimension == -1) { dimension = curdimension; }
        else if(dimension != curdimension) { return(false); }

        
        if((varlibret = rh4nvarGetArrayDimension(&hptr->var, curlength)) != RH4N_RET_OK) { return(false); }
        if(length[0] == -1 && length[1] == -1 && length[2] == -1) { memcpy(length, curlength, sizeof(length)); }
        else if(memcmp(length, curlength, sizeof(length)) != 0) { return(false); }
    }

    return(true);
}

void rh4nvarPrintString(RH4nVarObj *variable, RH4nProperties *props, FILE *outputfile) {
    if(variable->value == NULL) {
        fwprintf(outputfile, L"\"\"");
        return;
    }
    fwprintf(outputfile, L"\"%s\"", (char*)variable->value);
}

void rh4nvarPrintUString(RH4nVarObj *variable, RH4nProperties *props, FILE *outputfile) {
    if(variable->value == NULL) {
        fwprintf(outputfile, L"\"\"");
        return;
    }
    fwprintf(outputfile, L"\"%S\"", (wchar_t*)variable->value);
}

void rh4nvarPrintBool(RH4nVarObj *variable, RH4nProperties *props, FILE *outputfile) {
    if(variable->value == NULL) {
        fwprintf(outputfile, L"false");
        return;
    }
    fwprintf(outputfile, L"%s", *((bool*)variable->value) == 1 ? "true" : "false");
}

void rh4nvarPrintInt(RH4nVarObj *variable, RH4nProperties *props, FILE *outputfile) {
    if(variable->value == NULL) {
        fwprintf(outputfile, L"0");
        return;
    }
    fwprintf(outputfile, L"%d", *((int*)variable->value));
}

void rh4nvarPrintFloat(RH4nVarObj *variable, RH4nProperties *props, FILE *outputfile) {
    if(variable->value == NULL) {
        fwprintf(outputfile, L"0");
        return;
    }
    fwprintf(outputfile, L"%f", *((double*)variable->value));
}

void rh4nvarPrintTabs(int level, RH4nProperties *props) {
    int i = 0;
    for(; i < level; i++) { printf("\t"); }
}
