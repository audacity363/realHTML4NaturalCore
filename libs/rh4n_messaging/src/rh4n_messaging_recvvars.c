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

    if(rh4n_messaging_recvHeader(recvSocket, &header, props) < 0) { return(-1); }

    if(header.messageType != RH4NLIBMESSAGING_TYPEVARLIST) {
        rh4n_log_fatal(props->logging, "Wrong messageType. Expected: [0x%.2x] Is: [0x%.2x]", 
                RH4NLIBMESSAGING_TYPEVARLIST, header.messageType);
        return(-1);
    }

    while(1) {
        if((newEntry = malloc(sizeof(RH4nVarEntry_t))) == NULL) {
            rh4n_log_fatal(props->logging, "Could not allocate memory for new node");
            return(-1);
        }
        memset(newEntry, 0x00, sizeof(RH4nVarEntry_t));

        rh4n_log_develop(props->logging, "Start reading node");
        if(rh4n_messaging_recvVarlistNode(recvSocket, newEntry, props) < 0) { return(-1); }
        
        rh4n_log_develop(props->logging, "Waiting for next flags");
        if(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props) < 0) { 
            rh4n_log_fatal(props->logging, "Timeout while waiting for next flags");
            return(-1);
        }
   
        pnextFlags = nextFlags;
        rh4n_log_develop(props->logging, "Reading next flags");
        if(rh4n_messaging_readFromSocket(recvSocket, &nextFlags, sizeof(nextFlags), props) < 0) { return(-1); }
        rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);
        rh4n_log_develop(props->logging, "Done reading Node");

        rh4n_log_develop(props->logging, "next flag: [0x%.2x]", nextFlags);

        rh4n_messaging_recvVarlistAddNode(pnextFlags, newEntry, lastNode, varlist);
        lastNode = newEntry;

        rh4n_log_develop(props->logging, "Waiting for seperator");
        if(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props) < 0) { 
            rh4n_log_fatal(props->logging, "Timeout while waiting for seperator");
            return(-1);
        }
        rh4n_log_develop(props->logging, "Reading seperator");
        if(rh4n_messaging_readFromSocket(recvSocket, &seperator, sizeof(seperator), props) < 0) { return(-1); }
        if(seperator == ASCII_EOT) {
            break;
        }
    }

    return(0);
}

int rh4n_messaging_recvVarlistNode(int recvSocket, RH4nVarEntry_t *target, RH4nProperties *props) {
    uint32_t nameLength = 0;

    rh4n_log_develop(props->logging, "Waiting for var name length");
    if(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props) < 0) { 
        rh4n_log_fatal(props->logging, "Timeout while waiting for var name length");
        return(-1);
    }
   
    rh4n_log_develop(props->logging, "Reading var name length");
    if(rh4n_messaging_readFromSocket(recvSocket, &nameLength, sizeof(nameLength), props) < 0) { return(-1); }
    if((target->name = malloc(nameLength+1)) == NULL) {
        rh4n_log_fatal(props->logging, "Could not allocate memory for var name");
        rh4n_messaging_sendAcknowledge(recvSocket, ASCII_NACK, props);
        return(-1);
    }
    memset(target->name, 0x00, nameLength+1);
    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

    rh4n_log_develop(props->logging, "Waiting for var name");
    if(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props) < 0) {
        rh4n_log_fatal(props->logging, "Timeout while waiting for var name");
        return(-1);
    }

    rh4n_log_develop(props->logging, "Reading varname");
    if(rh4n_messaging_readFromSocket(recvSocket, target->name, nameLength, props) < 0) { 
    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_NACK, props);
        return(-1); 
    }
    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);
    rh4n_log_develop(props->logging, "God varname: %s", target->name);

    return(rh4n_messaging_recvVarlistValue(recvSocket, &target->var, props));
}

int rh4n_messaging_recvVarlistValue(int recvSocket, RH4nVarObj *target, RH4nProperties *props) {
    uint8_t vartype = 0;

    rh4n_log_develop(props->logging, "Waiting for vartype");
    if(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props) < 0) {
        rh4n_log_fatal(props->logging, "Timeout while waiting for var type");
        return(-1);
    }
    rh4n_log_develop(props->logging, "Reading vartype");
    if(rh4n_messaging_readFromSocket(recvSocket, &vartype, sizeof(vartype), props) < 0) { return(-1); }
    target->type = vartype;
    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

    if(vartype == RH4NVARTYPEGROUP) {
        return(0);
    }

    rh4n_log_develop(props->logging, "Waiting for var value length");
    if(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props) < 0) {
        rh4n_log_fatal(props->logging, "Timeout while waiting for var length");
        return(-1);
    }
    rh4n_log_develop(props->logging, "Reading var value length");
    if(rh4n_messaging_readFromSocket(recvSocket, &target->length, sizeof(target->length), props) < 0) { return(-1); }


    if(vartype == RH4NVARTYPEARRAY) {
        return(rh4n_messaging_recvVarlistArray(recvSocket, target, props));
    }


    if(target->length == 0) {
        target->value = NULL;
        rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);
        return(0);
    }

    if((target->value = malloc(target->length)) == NULL) {
        rh4n_log_fatal(props->logging, "Could not allocate memory for value");
        rh4n_messaging_sendAcknowledge(recvSocket, ASCII_NACK, props);
        return(-1);
    }
    memset(target->value, 0x00, target->length);
    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

    rh4n_log_develop(props->logging, "Waiting for var value");
    if(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props) < 0) {
        rh4n_log_fatal(props->logging, "Timeout while waiting for var value");
        return(-1);
    }

    rh4n_log_develop(props->logging, "Reading var value");
    if(rh4n_messaging_readFromSocket(recvSocket, target->value, target->length, props) < 0) {
        rh4n_messaging_sendAcknowledge(recvSocket, ASCII_NACK, props);
        return(-1);
    }
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
        if(rh4n_messaging_recvVarlistValue(recvSocket, &((RH4nVarObj*)target->value)[i], props) < 0) { return(-1); }
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
