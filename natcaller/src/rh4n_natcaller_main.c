#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "rh4n.h"
#include "rh4n_messaging.h"
#include "rh4n_natcaller.h"

int main(int argc, char *argv[]) {
    struct stat fileStatus;
    int udsClient = -1, initRet = 0;
    RH4nProperties props; memset(&props, 0x00, sizeof(props));

    if(argc != 2) {
        fprintf(stderr, "Usage: %s <socketfile>\n", argv[0]);
        return(-1);
    }

    /*if(access(argv[1], (R_OK | W_OK)) < 0) {
        fprintf(stderr, "Could not access file [%s] - %s\n", argv[1], strerror(errno));
        return(-1);
    }
    
    if(stat(argv[1], &fileStatus) < 0) {
        fprintf(stderr, "Could not get informations about file [%s] - %s\n", argv[1], strerror(errno));
        return(-1);
    }

    if(!(S_ISSOCK(fileStatus.st_mode))) {
        fprintf(stderr, "File [%s] is not a socket file\n", argv[1]);
        return(-1);
    }*/
   
    //TODO; parse the Loglevel from arguments
    if((props.logging = rh4nLoggingCreateStreamingRule("", "", RH4N_DEVELOP, "")) == NULL) {
        fprintf(stderr, "Could not initialize logging to stdout\n");
        return(-1);
    }

    int udsServer = 0;
    if((udsServer = rh4n_messaging_createUDSServer(argv[1], RH4NLIBMESSAGINGFLAG_NONBLOCKING, &props)) < 0) {
        exit(1);
    }

    rh4n_log_debug(props.logging, "Waiting for a new client.");
    int i = 0;
    for(; i < 500000; i++) {
        udsClient = rh4n_messaging_waitForClient(udsServer, &props);
        if(udsClient > 0) {
            rh4n_log_debug(props.logging, "Got new client %d", udsClient);
            break;
        } else if(udsClient == -2) {
            //rh4n_log_debug(props.logging, "No client on socket. Sleeping");
            usleep(10);
            continue;
        } else if(udsClient == -1) {
            rh4n_log_fatal(props.logging, "Could not wait for new client");
            exit(1);
        }
    }
    if(udsClient < 0) {
        rh4n_log_fatal(props.logging, "Timeout while waiting for a new client");
        exit(1);
    }


    /*if((udsClient = rh4n_messaging_connectToUDSServer(argv[1], &props)) < 0) {
        return(-1);
    }*/

    RH4N_CHECKERROR(rh4n_main_loadSessionInformations(&props, udsClient));
    
    switch(props.mode) {
        case RH4N_MODE_PLAIN:
            initRet = rh4n_natcaller_init_plain(&props, udsClient);
            break;
        case RH4N_MODE_WS:
            rh4n_log_fatal(props.logging, "Websocket mode is not yet implemented!");
            break;
        default:
            rh4n_log_fatal(props.logging, "God unkown startup mode: [0x%.2x]", props.mode);
            return(-1);
            break;
    }

    if(initRet < 0) {
        rh4n_log_fatal(props.logging, "Something went wrong while initializing");
        return(-1);
    }

    rh4n_natcaller_callNatural(&props);
    
    return(0);
}

int rh4n_main_loadSessionInformations(RH4nProperties *props, int recvSocket) {
    RH4N_CHECKERROR(rh4n_messaging_recvSessionInformations(recvSocket, props));

    rh4n_del_log_rule(props->logging);

    if((props->logging = rh4nLoggingCreateStreamingRule(props->natlibrary, props->natprogram, 
                    props->i_loglevel, props->logpath)) == NULL) {
        fprintf(stderr, "Could not initialize logging on directory [%s]\n", props->logpath);
        return(-1);
    }
    return(0);
}

int rh4n_natcaller_init_plain(RH4nProperties *props, int recvSocket) {
    RH4N_CHECKERROR(rh4n_messaging_recvVarlist(recvSocket, &props->urlvars, props));
    RH4N_CHECKERROR(rh4n_messaging_recvVarlist(recvSocket, &props->bodyvars, props));

    return(0);
}
