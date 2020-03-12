#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_messaging.h"

int rh4n_messaging_sendVarlist(int sendSocket, RH4nVarList *varlist, RH4nProperties *props) {
    RH4N_CHECKERROR(rh4n_messaging_sendHeader(sendSocket, RH4NLIBMESSAGING_TYPEVARLIST, 
               varlist->anker == NULL ? 0 : 1, props));

    if(varlist->anker == NULL) {
        return(0);
    }

    RH4N_CHECKERROR(rh4n_messaging_processVarlistNode(sendSocket, varlist->anker, props, 1));

    rh4n_log_develop(props->logging, "Sending EOT");
    rh4n_messaging_sendAcknowledge(sendSocket, ASCII_EOT, props);
    return(0);
}

int rh4n_messaging_processVarlistNode(int sendSocket, RH4nVarEntry_t *target, RH4nProperties *props, uint32_t level) {
    RH4nVarEntry_t *hptr = target;

    for(; hptr != NULL; hptr = hptr->next) {
        RH4N_CHECKERROR(rh4n_messaging_sendVarlistNode(sendSocket, hptr, props));

        if((hptr->next || hptr->nextlvl) || level > 1) {
            rh4n_log_develop(props->logging, "Sending seperator");
            rh4n_messaging_sendAcknowledge(sendSocket, ASCII_RS, props);
        }

        if(hptr->nextlvl) {
            RH4N_CHECKERROR(rh4n_messaging_processVarlistNode(sendSocket, hptr->nextlvl, props, level+1));
        }
    }

    return(0);
}

int rh4n_messaging_sendVarlistNode(int sendSocket, RH4nVarEntry_t *target, RH4nProperties *props) {
    uint8_t nextValue = 0;
    uint32_t nameLength = 0;

    rh4n_log_develop(props->logging, "Start sending Node");

    nameLength = strlen(target->name);
    rh4n_log_develop(props->logging, "Sending name length");
    RH4N_CHECKERROR(rh4n_messaging_sendDataChunk(sendSocket, &nameLength, sizeof(nameLength), props));

    rh4n_log_develop(props->logging, "Sending name %s", target->name);
    RH4N_CHECKERROR(rh4n_messaging_sendDataChunk(sendSocket, target->name, nameLength, props));

    RH4N_CHECKERROR(rh4n_messaging_sendVarlistValue(sendSocket, &target->var, props));

    rh4n_log_develop(props->logging, "Sending next flags");
    nextValue = target->next ? (nextValue | RH4NLIBMESSAGINGFLAG_NEXT) : nextValue;
    nextValue = target->nextlvl ? (nextValue | RH4NLIBMESSAGINGFLAG_NEXTLVL) : nextValue;
    RH4N_CHECKERROR(rh4n_messaging_sendDataChunk(sendSocket, &nextValue, sizeof(nextValue), props));

    rh4n_log_develop(props->logging, "Done sending Node");
    return(0);
}

int rh4n_messaging_sendVarlistValue(int sendSocket, RH4nVarObj *target, RH4nProperties *props) {
    uint8_t vartype = target->type;
    
    rh4n_log_develop(props->logging, "Sending vartype");
    RH4N_CHECKERROR(rh4n_messaging_sendDataChunk(sendSocket, &vartype, sizeof(vartype), props));

    if(target->type == RH4NVARTYPEGROUP) { return(0); }

    rh4n_log_develop(props->logging, "Sending value length");
    RH4N_CHECKERROR(rh4n_messaging_sendDataChunk(sendSocket, &target->length, sizeof(target->length), props));

    if(target->type == RH4NVARTYPEARRAY) {
        return(rh4n_messaging_sendVarlistArray(sendSocket, target, props));
    }

    if(target->length > 0) {
        rh4n_log_develop(props->logging, "Sending value");
        return(rh4n_messaging_sendDataChunk(sendSocket, target->value, target->length, props));
    }

    return(0);
}

int rh4n_messaging_sendVarlistArray(int sendSocket, RH4nVarObj *target, RH4nProperties *props) {
    uint32_t i = 0;
    RH4nVarObj *entry = NULL;

    rh4n_log_develop(props->logging, "Start sending array");
    
    for(; i < target->length; i++) {
        entry = &((RH4nVarObj*)target->value)[i];
        RH4N_CHECKERROR(rh4n_messaging_sendVarlistValue(sendSocket, entry, props));
    }
    rh4n_log_develop(props->logging, "Done sending array");
    return(0);
}
