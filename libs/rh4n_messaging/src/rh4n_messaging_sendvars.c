#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_messaging.h"

int rh4n_messaging_sendVarlist(int sendSocket, RH4nVarList *varlist, RH4nProperties *props) {
    if(rh4n_messaging_sendHeader(sendSocket, RH4NLIBMESSAGING_TYPEVARLIST, props) < 0) {
        return(-1);
    }


    if(rh4n_messaging_processVarlistNode(sendSocket, varlist->anker, props, 1) < 0) { return(-1); }

    rh4n_log_develop(props->logging, "Sending EOT");
    rh4n_messaging_sendAcknowledge(sendSocket, ASCII_EOT, props);
    return(0);
}

int rh4n_messaging_processVarlistNode(int sendSocket, RH4nVarEntry_t *target, RH4nProperties *props, uint32_t level) {
    RH4nVarEntry_t *hptr = target;

    for(; hptr != NULL; hptr = hptr->next) {
        if(rh4n_messaging_sendVarlistNode(sendSocket, hptr, props) < 0) {
            return(-1);
        }

        if((hptr->next || hptr->nextlvl) || level > 1) {
            rh4n_log_develop(props->logging, "Sending seperator");
            rh4n_messaging_sendAcknowledge(sendSocket, ASCII_RS, props);
        }

        if(hptr->nextlvl) {
            if(rh4n_messaging_processVarlistNode(sendSocket, hptr->nextlvl, props, level+1) < 0) {
                return(-1);
            }
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
    if(rh4n_messaging_sendDataChunk(sendSocket, &nameLength, sizeof(nameLength), props) < 0) { return(-1); }
    rh4n_log_develop(props->logging, "Sending name %s", target->name);
    if(rh4n_messaging_sendDataChunk(sendSocket, target->name, nameLength, props) < 0) { return(-1); }

    if(rh4n_messaging_sendVarlistValue(sendSocket, &target->var, props) < 0) {
        return(-1);
    }

    rh4n_log_develop(props->logging, "Sending next flags");
    nextValue = target->next ? (nextValue | RH4NLIBMESSAGINGFLAG_NEXT) : nextValue;
    nextValue = target->nextlvl ? (nextValue | RH4NLIBMESSAGINGFLAG_NEXTLVL) : nextValue;
    if(rh4n_messaging_sendDataChunk(sendSocket, &nextValue, sizeof(nextValue), props) < 0) { return(-1); }

    rh4n_log_develop(props->logging, "Done sending Node");
    return(0);
}

int rh4n_messaging_sendVarlistValue(int sendSocket, RH4nVarObj *target, RH4nProperties *props) {
    uint8_t vartype = target->type;
    
    rh4n_log_develop(props->logging, "Sending vartype");
    if(rh4n_messaging_sendDataChunk(sendSocket, &vartype, sizeof(vartype), props) < 0) { return(-1); }

    if(target->type == RH4NVARTYPEGROUP) { return(0); }

    rh4n_log_develop(props->logging, "Sending value length");
    if(rh4n_messaging_sendDataChunk(sendSocket, &target->length, sizeof(target->length), props) < 0) { return(-1); }

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
        if(rh4n_messaging_sendVarlistValue(sendSocket, entry, props) < 0) { return(-1); }
    }
    rh4n_log_develop(props->logging, "Done sending array");
    return(0);
}
