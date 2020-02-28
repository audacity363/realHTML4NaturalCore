#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "natni.h"
#include "natuser.h"

long rh4nnatSleep(WORD nparms, void *parmhandle, void *traditional) {
    sleep(1);
    return(0);
}
