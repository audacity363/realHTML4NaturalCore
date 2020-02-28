#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

#include "rh4n.h"
#include "rh4n_messaging.h"


int rh4n_messaging_sendTextBlock(int sendSocket, char *text, RH4nProperties *props) {
    uint8_t ack = 0;

    if(strlen(text) == 0) { return(0); }

    RH4N_CHECKERROR(rh4n_messaging_sendHeader(sendSocket, RH4NLIBMESSAGING_TYPETEXTBLOCK, strlen(text), props));
    RH4N_CHECKERROR(rh4n_messaging_recvAcknowledge(sendSocket, &ack, props));

    RH4N_CHECKERROR(rh4n_messaging_sendDataChunk(sendSocket, text, strlen(text), props));
    return(0);
}

int rh4n_messaging_recvTextBlock(int recvSocket, char **text, RH4nProperties *props) {
    RH4nMessageingHeader_t header; memset(&header, 0x00, sizeof(header));

    RH4N_CHECKERROR(rh4n_messaging_recvHeader(recvSocket, &header, props));

    if((*text = calloc(header.dataLength+1, sizeof(char))) == NULL) {
        rh4n_messaging_sendAcknowledge(recvSocket, ASCII_NACK, props);
        rh4n_log_error(props->logging, "Could not allocate memory for text message - %s", strerror(errno));
        return(-1);
    }

    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

    RH4N_CHECKERROR(rh4n_messaging_recvDataChunk(recvSocket, *text, header.dataLength, props));
    rh4n_messaging_sendAcknowledge(recvSocket, ASCII_ACK, props);

    return(0);
}
