#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "rh4n.h"
#include "rh4n_messaging.h"
#include "rh4n_utils.h"

int main() {
    RH4nProperties props; memset(&props, 0x00, sizeof(props));

    props.logging = rh4nLoggingCreateStreamingRule("", "", RH4N_DEVELOP, "");
    //props.natparms ="parm=naton";
    strcpy(props.natlibrary, "PLAIN");
    strcpy(props.natprogram, "JSON");

    props.logpath = "/tmp/";
    props.outputfile = "/tmp/rh4nOutput";
    //props.natsrcpath = "/opt/softwareag/fuser/";
    strcpy(props.username, "MyUSer");

    //rh4n_log_develop(props.logging, "LoadfromFile: %d", rh4nvarLoadFromFile("./libs/rh4n_messaging/tests/testvars", &props.bodyvars));
    //rh4n_log_develop(props.logging, "strerror: %s", strerror(errno));
    //
    rh4nvarCreateNewGroup(&props.bodyvars, "MyGroup");
    rh4nvarCreateNewInt(&props.bodyvars, "MyGroup", "myInt", 2);


    rh4nvarPrintList(&props.bodyvars, NULL);


    rh4nUtilsPrintProperties(&props);

    int server = rh4n_messaging_createUDSServer("/tmp/rh4nTestUDSS", RH4NLIBMESSAGINGFLAG_OVERRIDE | RH4NLIBMESSAGINGFLAG_NONBLOCKING, &props);
    rh4n_log_develop(props.logging, "Waiting for client on server [%d]...", server);
    int client = 0;
    int i = 0;
    for(; i < 10; i++) {
        client = rh4n_messaging_waitForClient(server, &props);
        if(client == -2) {
            rh4n_log_develop(props.logging, "Timeout waiting for client");
            sleep(1);
            continue;
        } else if(client == -1) {
            rh4n_log_develop(props.logging, "Error waiting for client");
            exit(1);
        } else if(client > 0) {
            rh4n_log_develop(props.logging, "God new client %d", client);
            break;
        }

    }
    rh4n_log_develop(props.logging, "God client: [%d]", client);

    rh4n_messaging_sendSessionInformations(client, &props);

    rh4n_messaging_sendVarlist(client, &props.urlvars , &props);

    rh4n_messaging_sendVarlist(client, &props.bodyvars, &props);
}

