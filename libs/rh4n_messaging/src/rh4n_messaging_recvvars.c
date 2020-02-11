#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_messaging.h"

int rh4n_messaging_recvVarlist(int recvSocket, RH4nVarList *varlist, RH4nProperties *props) {
    RH4nMessageingHeader_t header; memset(&header, 0x00, sizeof(header));
    RH4nVarEntry_t *newEntry = NULL, *lastNode = NULL;
    uint8_t nextFlags = 0, pnextFlags = 0, seperator = 0;

    RH4N_CHECKERROR(rh4n_messaging_recvHeader(recvSocket, &header, props));

    if(header.messageType != RH4NLIBMESSAGING_TYPEVARLIST) {
        rh4n_log_fatal(props->logging, "Wrong messageType. Expected: [0x%.2x] Is: [0x%.2x]", 
                RH4NLIBMESSAGING_TYPEVARLIST, header.messageType);
        return(-1);
    }

    if(header.dataLength == 0) {
        return(0);
    }

    while(1) {
        if((newEntry = malloc(sizeof(RH4nVarEntry_t))) == NULL) {
            rh4n_log_fatal(props->logging, "Could not allocate memory for new node");
            return(-1);
        }
        memset(newEntry, 0x00, sizeof(RH4nVarEntry_t));

        rh4n_log_develop(props->logging, "Start reading node");
        RH4N_CHECKERROR(rh4n_messaging_recvVarlistNode(recvSocket, newEntry, props));
        
        pnextFlags = nextFlags;
        rh4n_log_develop(props->logging, "Waiting for next flags");
        RH4N_CHECKERROR(rh4n_messaging_recvDataChunk(recvSocket, &nextFlags, sizeof(nextFlags), props));
        rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);


        rh4n_messaging_recvVarlistAddNode(pnextFlags, newEntry, lastNode, varlist);
        lastNode = newEntry;

        rh4n_log_develop(props->logging, "Done reading Node");

        rh4n_log_develop(props->logging, "Waiting for seperator");
        RH4N_CHECKERROR(rh4n_messaging_recvDataChunk(recvSocket, &seperator, sizeof(seperator), props));
        if(seperator == ASCII_EOT) {
            break;
        }
    }

    return(0);
}

int rh4n_messaging_recvVarlistNode(int recvSocket, RH4nVarEntry_t *target, RH4nProperties *props) {
    uint32_t nameLength = 0;

    rh4n_log_develop(props->logging, "Waiting for var name length");
    RH4N_CHECKERROR(rh4n_messaging_recvDataChunk(recvSocket, &nameLength, sizeof(nameLength), props));

    if((target->name = malloc(nameLength+1)) == NULL) {
        rh4n_log_fatal(props->logging, "Could not allocate memory for var name");
        rh4n_messaging_sendAcknowledge(recvSocket, ASCII_NACK, props);
        return(-1);
    }
    memset(target->name, 0x00, nameLength+1);
    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

    rh4n_log_develop(props->logging, "Waiting for var name");
    RH4N_CHECKERROR(rh4n_messaging_recvDataChunk(recvSocket, target->name, nameLength, props));
    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

    rh4n_log_develop(props->logging, "God varname: %s", target->name);

    return(rh4n_messaging_recvVarlistValue(recvSocket, &target->var, props));
}

int rh4n_messaging_recvVarlistValue(int recvSocket, RH4nVarObj *target, RH4nProperties *props) {
    uint8_t vartype = 0;

    rh4n_log_develop(props->logging, "Waiting for vartype");
    RH4N_CHECKERROR(rh4n_messaging_recvDataChunk(recvSocket, &vartype, sizeof(vartype), props));
    target->type = vartype;
    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

    if(vartype == RH4NVARTYPEGROUP) {
        return(0);
    }

    rh4n_log_develop(props->logging, "Waiting for var value length");
    RH4N_CHECKERROR(rh4n_messaging_recvDataChunk(recvSocket, &target->length, sizeof(target->length), props));

    if(target->length == 0) {
        target->value = NULL;
        rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);
        return(0);
    }

    if(vartype == RH4NVARTYPEARRAY) {
        return(rh4n_messaging_recvVarlistArray(recvSocket, target, props));
    }

    
    if((target->value = malloc(target->length)) == NULL) {
        rh4n_log_fatal(props->logging, "Could not allocate memory for value");
        rh4n_messaging_sendAcknowledge(recvSocket, ASCII_NACK, props);
        return(-1);
    }
    memset(target->value, 0x00, target->length);
    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

    rh4n_log_develop(props->logging, "Waiting for var value");
    RH4N_CHECKERROR(rh4n_messaging_recvDataChunk(recvSocket, target->value, target->length, props));
    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

    return(0);
}

int rh4n_messaging_recvVarlistArray(int recvSocket, RH4nVarObj *target, RH4nProperties *props) {
    uint32_t i = 0;
    
    rh4n_log_develop(props->logging, "Start reading array");

   if((target->value = malloc(sizeof(RH4nVarObj)*target->length)) == NULL) {
       rh4n_log_fatal(props->logging, "Could not allocate memory for array");
       rh4n_messaging_sendAcknowledge(recvSocket, ASCII_NACK, props);
       return(-1);
   }
   memset(target->value, 0x00, sizeof(RH4nVarObj)*target->length);
   rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

    for(; i < target->length; i++) {
        RH4N_CHECKERROR(rh4n_messaging_recvVarlistValue(recvSocket, &((RH4nVarObj*)target->value)[i], props));
    }
    
    rh4n_log_develop(props->logging, "Done reading array");
    return(0);
}

void rh4n_messaging_recvVarlistAddNode(uint8_t nextFlags, RH4nVarEntry_t *newNode, RH4nVarEntry_t *lastNode, RH4nVarList *varlist) {
    RH4nVarEntry_t *hptr = NULL;

    if(lastNode == NULL) {
        varlist->anker = newNode;
        return;
    }
    
    if(nextFlags & RH4NLIBMESSAGINGFLAG_NEXTLVL) {
        newNode->prev = lastNode;
        lastNode->nextlvl = newNode;
    } else if(nextFlags & RH4NLIBMESSAGINGFLAG_NEXT) {
        newNode->prev = lastNode;
        lastNode->next = newNode;
    } else if(nextFlags == 0) {
        for(hptr = lastNode; hptr != NULL && (hptr->var.type != RH4NVARTYPEGROUP || hptr->next != NULL); hptr = hptr->prev);
        if(hptr == NULL) return;

        hptr->next = newNode;
        newNode->prev = hptr;
    }
}
