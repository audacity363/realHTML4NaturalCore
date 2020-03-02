#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_messaging.h"


int rh4n_messaging_sendHeader(int sendSocket, uint8_t messageType, uint32_t dataLength, RH4nProperties *props) {
    RH4nMessageingHeader_t header = { ASCII_STARTOFHEADER, messageType, dataLength };
    
    if(rh4n_messaging_writeToSocket(sendSocket, &header, sizeof(header), props) != 0) {
        rh4n_log_fatal(props->logging, "Could not write header to socket [%d]", sendSocket);
        return(-1);
    }

    return(0);
} 
int rh4n_messaging_recvHeader(int recvSocket, RH4nMessageingHeader_t *header, RH4nProperties *props) {
    int readRet = 0;

    RH4N_CHECKERROR(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props));
    RH4N_CHECKERROR(rh4n_messaging_readFromSocket(recvSocket, header, sizeof(RH4nMessageingHeader_t), props));

    if(header->preable != ASCII_STARTOFHEADER) {
        rh4n_log_fatal(props->logging, "Unkown header preamble: [0x%.2x]", header->preable);
        return(-1);
    }
    return(0);
}

int rh4n_messaging_writeToSocket(int sendSocket, void *data, int length, RH4nProperties *props) {
    int bytesWritten = 0;

    //rh4n_log_develop(props->logging, "Write %d bytes of data to socket [%d]", length, sendSocket);

    if((bytesWritten = write(sendSocket, data, length)) < 0) {
        rh4n_log_fatal(props->logging, "Could not write data to socket - %s", strerror(errno));
        return(-1);
    } else if(bytesWritten != length) {
        rh4n_log_fatal(props->logging, "Could not write data to socket. Bytes written: [%d] should be: [%d]", bytesWritten, length);
        return(-1);
    }

    //rh4n_log_develop(props->logging, "Written %d bytes to socket [%d]", bytesWritten, sendSocket);
    return(0);
}

int rh4n_messaging_readFromSocket(int recvSocket, void *data, int length, RH4nProperties *props) {
    int bytesRead = 0;

    //rh4n_log_develop(props->logging, "Read %d bytes of data from socket [%d]", length, recvSocket);

    if((bytesRead = read(recvSocket, data, length)) < 0) {
        if(errno == ECONNRESET) {
            rh4n_log_error(props->logging, "Socket was closed");
            return(1);
        }
        rh4n_log_fatal(props->logging, "Could not read from socket - %s", strerror(errno));
        return(-1);
    } else if(bytesRead == 0) {
        rh4n_log_error(props->logging, "Socket was closed");
        return(1);
    } else if(bytesRead != length) {
        rh4n_log_fatal(props->logging, "Could not read data from socket. Bytes read: [%d] should be: [%d]", bytesRead, length);
        return(-1);
    }

    //rh4n_log_develop(props->logging, "Read %d bytes from socket [%d]", bytesRead, recvSocket);

    return(0);
}

void rh4n_messaging_sendAcknowledge(int sendSocket, uint8_t ack, RH4nProperties *props) {
    rh4n_messaging_writeToSocket(sendSocket, &ack, sizeof(ack), props);
}

int rh4n_messaging_recvAcknowledge(int recvSocket, uint8_t *ack, RH4nProperties *props) {
    uint8_t responseMessage = 0;

    if(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props) != 0) {
        rh4n_log_fatal(props->logging, "Error while waiting for acknowledge");
        return(-1);
    }

    RH4N_CHECKERROR(rh4n_messaging_readFromSocket(recvSocket, &responseMessage, sizeof(responseMessage), props));

    if(responseMessage == ASCII_NACK) {
        rh4n_log_fatal(props->logging, "God negative acknowledge");
        return(-1);
    } else if(responseMessage != ASCII_ACK) {
        rh4n_log_fatal(props->logging, "God unkown response: [0x%.2x]", responseMessage);
        return(-1);
    }

    if(ack != NULL) { *ack = responseMessage; }

    return(0);
}

int rh4n_messaging_sendDataChunk(int sendSocket, void *data, int length, RH4nProperties *props) {
    RH4N_CHECKERROR(rh4n_messaging_writeToSocket(sendSocket, data, length, props)); 
    RH4N_CHECKERROR(rh4n_messaging_recvAcknowledge(sendSocket, NULL, props)); 
    return(0);
}

int rh4n_messaging_recvDataChunk(int recvSocket, void *data, int length, RH4nProperties *props) {
    RH4N_CHECKERROR(rh4n_messaging_waitForData(recvSocket, RH4NLIBMESSAGING_RESPONSETIMEOUT, 0, props));
    RH4N_CHECKERROR(rh4n_messaging_readFromSocket(recvSocket, data, length, props));
    return(0);
}
