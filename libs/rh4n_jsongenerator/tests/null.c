#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "rh4n.h"
#include "rh4n_json.h"

int main(int argc, char *argv[]) {
    RH4nVarList varlist; memset(&varlist, 0x00, sizeof(varlist));
    RH4nProperties props;
    int length[3] = {-1, -1, -1};

    length[0] = 2;
    length[1] = 0;
    rh4nvarCreateNewNullArray_m(&varlist, NULL, "var2", 2, length);

    length[0] = 0;
    length[1] = -1;
    rh4nvarCreateNewNullArray_m(&varlist, NULL, "var1", 1, length);

    props.logging = rh4nLoggingCreateStreamingRule("VARLIB", "TEST", RH4N_DEVELOP, "");


    rh4njsonPrintJSON(&varlist, fileno(stdout), &props);
}
