#include <stdio.h>
#include <stdlib.h>

#include "rh4n.h"


int main() {
    RH4nLogrule *logging = rh4nLoggingCreateStreamingRule("TESTLIB", "TESTPROG", RH4N_ERROR, "/tmp/");

    rh4n_log_error(logging, "Hello World");
}

