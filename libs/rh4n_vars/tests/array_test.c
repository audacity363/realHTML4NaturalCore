#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_json.h"

int main() {
    RH4nVarList varlist;
    int length[3] = {2, -1, -1};

    rh4nvarInitList(&varlist);
    rh4nvarCreateNewIntArray(&varlist, NULL, "test1", 1, length);
    length[0] = 0;
    rh4nvarSetIntArrayEntry_m(&varlist, NULL, "test1", length, 5);
     
    RH4nProperties props; memset(&props, 0x00, sizeof(props));
    rh4njsonPrintJSON(&varlist, fileno(stdout), &props);
}
