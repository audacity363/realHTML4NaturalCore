#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#include "rh4n.h"
#include "rh4n_messaging.h"

struct RH4nMessagingLookup {
    const char *name;
    void *target;
    uint32_t length;
};

int rh4n_messaging_sendSessionInformations(int sendSocket, RH4nProperties *props) {
    struct RH4nMessagingLookup lookup[] = {
        {"library", props->natlibrary, strlen(props->natlibrary)},
        {"program", props->natprogram, strlen(props->natprogram)},
        {"parms", &props->natparms, -1},
        {"srcPath", &props->natsrcpath, -1},
        {"logpath", &props->logpath, -1},
        {"loglevel", &props->i_loglevel, sizeof(props->i_loglevel)},
        {"username", props->username, strlen(props->username)},
        {"outputfile", &props->outputfile, -1}
    };
    uint8_t i = 0;
    uint32_t length = 0;
    void *target = NULL;

    RH4N_CHECKERROR(rh4n_messaging_sendHeader(sendSocket, RH4NLIBMESSAGING_TYPESESSIONINFORMATIONS, 1, props));

    for(; i < sizeof(lookup)/sizeof(struct RH4nMessagingLookup); i++) {
        target = lookup[i].length == -1 ? *((void**)lookup[i].target) : lookup[i].target;

        if(lookup[i].length == -1 && target == NULL) {
            length = 0;
        } else if(lookup[i].length == -1) {
            length = strlen(target);
        } else {
            length = lookup[i].length;
        }

        if(rh4n_messaging_writeToSocket(sendSocket, &length, sizeof(uint32_t), props) < 0) {
            rh4n_log_fatal(props->logging, "Could not write length to socket");
            return(-1);
        }
        
        RH4N_CHECKERROR(rh4n_messaging_recvAcknowledge(sendSocket, NULL, props));
        if(length == 0) { continue; }

        if(rh4n_messaging_writeToSocket(sendSocket, target, length, props) < 0) {
            rh4n_log_fatal(props->logging, "Could not write length to socket");
            return(-1);
        }
    }

    return(0);
}

int rh4n_messaging_recvSessionInformations(int recvSocket, RH4nProperties *props) {
    struct RH4nMessagingLookup lookup[] = {
        {"library", props->natlibrary, sizeof(props->natlibrary)},
        {"program", props->natprogram, sizeof(props->natprogram)},
        {"parms", &props->natparms, -1},
        {"srcPath", &props->natsrcpath, -1},
        {"logpath", &props->logpath, -1},
        {"loglevel", &props->i_loglevel, sizeof(props->i_loglevel)},
        {"username", &props->username, sizeof(props->username)},
        {"outputfile", &props->outputfile, -1}
    };
    RH4nMessageingHeader_t header; memset(&header, 0x00, sizeof(header));
    uint8_t i = 0;
    uint32_t length = 0;
    void *target = NULL;

    RH4N_CHECKERROR(rh4n_messaging_recvHeader(recvSocket, &header, props));

    if(header.messageType != RH4NLIBMESSAGING_TYPESESSIONINFORMATIONS) {
        rh4n_log_fatal(props->logging, "Wrong messageType. Expected: [0x%.2x] Is: [0x%.2x]", 
                RH4NLIBMESSAGING_TYPESESSIONINFORMATIONS, header.messageType);
        return(-1);
    }

    for(; i < sizeof(lookup)/sizeof(struct RH4nMessagingLookup); i++) {
        if(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props) < 0) {
            rh4n_log_fatal(props->logging, "Timeout while waiting for length data");
            return(-1);
        }

        RH4N_CHECKERROR(rh4n_messaging_readFromSocket(recvSocket, &length, sizeof(length), props));

        if(lookup[i].length == -1) {
            if((*((void**)lookup[i].target) = malloc(length+1)) == NULL) {
                rh4n_log_fatal(props->logging, "Could not allocate memory for incoming entry");
                rh4n_messaging_sendAcknowledge(recvSocket, ASCII_NACK, props);
                return(-1);
            }

            target = *((void**)lookup[i].target);
            memset(target, 0x00, length+1);
        } else {
            target = lookup[i].target;
        }
        rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

        if(length == 0) { continue; }

        if(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props) < 0) {
            rh4n_log_fatal(props->logging, "Timeout while waiting for data");
            return(-1);
        }

        if(rh4n_messaging_readFromSocket(recvSocket, target, length, props) < 0) {
            rh4n_log_fatal(props->logging, "Could not read data from socket");
            return(-1);
        }
    }

    return(0);
}
