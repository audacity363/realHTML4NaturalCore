#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "rh4n.h"
#include "rh4n_utils.h"
#include "rh4n_utils_namestack.h"

void rh4nUtilsInitNameStack(rh4nNameStack *stack) {
    stack->length = 0;

    if((stack->names = malloc(sizeof(char*))) == NULL) { return; }
    stack->names[0] = NULL;
}

char *rh4nUtilsPopNamefromStack(rh4nNameStack *stack) {
    if(stack == NULL || stack->length == 0) { return(NULL); }

    char *retptr = stack->names[--stack->length];
    stack->names[stack->length] = NULL;
    
    stack->names = realloc(stack->names, sizeof(char*)*(stack->length+1));
    if(stack->names == NULL && stack->length > 0) {
        return(NULL);
    }

    return(retptr);
}

char *rh4nUtilsPushNametoStack(rh4nNameStack *stack, const char *name) {
    if(name == NULL || stack == NULL) { return(NULL); }

    char *namebuff = NULL;

    if((namebuff = malloc((strlen(name)+1)*sizeof(char))) == NULL) {
        return(NULL);
    }

    if((stack->names = realloc(stack->names, sizeof(char*)*((++stack->length)+1))) == NULL) {
        free(namebuff);
        stack->length--;
        return(NULL);
    }

    strcpy(namebuff, name);

    stack->names[stack->length-1] = namebuff;
    stack->names[stack->length] = NULL;
    return(namebuff);
}

void rh4nUtilsDeinitNameStack(rh4nNameStack *stack) {
    if(stack == NULL) { return; }

    for(; stack->length > 0; stack->length--) {
        free(stack->names[stack->length-1]);
    }

    free(stack->names);
    stack->names = NULL;
}


void rh4nUtilsPrintNameStack(rh4nNameStack *stack) {
    if(stack == NULL) { return; }

    uint32_t i = 0;

    for(; i <= stack->length; i++) {
        if(stack->names[i] == NULL) { 
            printf("NULL");
            continue;
        }

        printf("%s", stack->names[i]);
        if(i+1 <= stack->length) { printf("."); }
    }
    printf("\n");
}
