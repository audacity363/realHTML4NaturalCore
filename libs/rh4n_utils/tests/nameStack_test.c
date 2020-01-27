#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_utils.h"
#include "rh4n_utils_namestack.h"

int main(int argc, char *argv[]) {
    rh4nNameStack stack;
    char *popedName = NULL, namebuff[50];
    int i = 0;

    rh4nUtilsInitNameStack(&stack);
    for(i = 1; i < 5; i++) {
        sprintf(namebuff, "Name%d", i);
        if(rh4nUtilsPushNametoStack(&stack, namebuff) == NULL) { 
            fprintf(stderr, "Could not push \"%s\"\n", namebuff); 
            exit(1); 
        }
    }

    if(stack.length != 4) { fprintf(stderr, "stack->length != 4\n"); exit(1); };

    for(i = 4; i > 0; i--) {
        sprintf(namebuff, "Name%d", i);

        popedName = rh4nUtilsPopNamefromStack(&stack);
        if(popedName == NULL) {
            fprintf(stderr, "rh4nUtilsPopNamefromStack returned NULL in [%s]\n", namebuff);
            exit(1);
        }

        if(strcmp(popedName, namebuff) != 0) {
            fprintf(stderr, "rh4nUtilsPopNamefromStack returned something else: is: [%s] should be [%s]\n", popedName, namebuff);
            exit(1);
        }
        free(popedName);
    }

    rh4nUtilsDeinitNameStack(&stack);
    printf("%s - OK!\n", argv[0]);
    return(0);
}
