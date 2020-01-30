#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#include "rh4n.h"
#include "rh4n_messaging.h"
#include "rh4n_utils.h"

int main() {
    RH4nProperties props; memset(&props, 0x00, sizeof(props));

    props.logging = rh4nLoggingCreateStreamingRule("", "", RH4N_DEVELOP, "");

    int client = rh4n_messaging_connectToUDSServer("/tmp/rh4nTestUDSS", &props);
    rh4n_log_develop(props.logging, "Connected to server: [%d]", client);
    rh4n_messaging_recvSessionInformations(client , &props);
        
    rh4nUtilsPrintProperties(&props);
}
