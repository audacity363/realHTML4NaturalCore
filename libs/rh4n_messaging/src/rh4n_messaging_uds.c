#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>
#include <fcntl.h>

#include "rh4n.h"
#include "rh4n_messaging.h"

int rh4n_messaging_createUDSServer(const char *path, uint16_t flags, RH4nProperties *props) {
    struct stat fileInformations;
    struct sockaddr_un addr; memset(&addr, 0x00, sizeof(addr));
    int serverSocket = 0, socketOptions = 0;

    if(stat(path, &fileInformations) == 0) {
        //To force remove the existing file it has to be type "socket" and the remove flag has to be true
        if((S_ISSOCK(fileInformations.st_mode) && !(flags & RH4NLIBMESSAGINGFLAG_OVERRIDE)) || !S_ISSOCK(fileInformations.st_mode)) {
            rh4n_log_fatal(props->logging, "Could not create UDS Server. Filepath [%s] does already exist", path);
            return(-1);
        }

        if(unlink(path) < 0) {
            rh4n_log_fatal(props->logging, "Could not remove file [%s] - %s", path, strerror(errno));
            return(-1);
        }
    }
    
    if((serverSocket = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
        rh4n_log_fatal(props->logging, "Could not create new socket - %s", strerror(errno));
        return(-1);
    }

    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, path);

    if(bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        rh4n_log_fatal(props->logging, "Could not bind socket - %s", strerror(errno));
        return(-1);
    }

    if(listen(serverSocket, RH4NLIBMESSAGINGMAXCLIENTS) < 0) {
        rh4n_log_fatal(props->logging, "Could not listen on socket - %s", strerror(errno));
        return(-1);
    }

    if((flags & RH4NLIBMESSAGINGFLAG_NONBLOCKING)) {
        socketOptions = fcntl(serverSocket, F_GETFL);
        fcntl(serverSocket, F_SETFL, socketOptions | O_NONBLOCK);
    }

    return(serverSocket);
}

int rh4n_messaging_waitForClient(int serverSocket, RH4nProperties *props) {
    struct sockaddr_un remoteHost; memset(&remoteHost, 0x00, sizeof(remoteHost));
    int clientSocket = 0;

    //We don't need the client informations here
    if((clientSocket = accept(serverSocket, NULL, NULL)) > -1 ) {
        return(clientSocket);
    }

    if(errno == EWOULDBLOCK) {
        return(-2);
    }

    rh4n_log_fatal(props->logging, "Could not accept new clients - %s\n", strerror(errno));
    return(-1);
}

int rh4n_messaging_connectToUDSServer(const char *path, RH4nProperties *props) {
    struct stat fileInformations;
    struct sockaddr_un address;
    int clientSocket = 0;

    if(stat(path, &fileInformations) < 0) {
        rh4n_log_fatal(props->logging, "Could not get information about [%s] - %s", path, strerror(errno));
        return(-1);
    }

    if(!S_ISSOCK(fileInformations.st_mode)) {
        rh4n_log_fatal(props->logging, "File [%s] is not a socket", path);
        return(-1);
    }

    if((clientSocket = socket(PF_LOCAL, SOCK_STREAM, 0)) < 0) {
        rh4n_log_fatal(props->logging, "Could not create new socket - %s", strerror(errno));
        return(-1);
    }

    address.sun_family = AF_LOCAL;
    strcpy(address.sun_path, path);

    if(connect(clientSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        rh4n_log_fatal(props->logging, "Could not connect to server [%s] - %s", path, strerror(errno));
        return(-1);
    }

    return(clientSocket);
}

int rh4n_messaging_waitForData(int socket, int secondsTimeout, int usecondsTimeout, RH4nProperties *props) {
    int selectRet = 0;
    struct timeval tv;
    fd_set socketSet;

    tv.tv_sec = secondsTimeout;
    tv.tv_usec = usecondsTimeout;

    FD_ZERO(&socketSet);
    FD_SET(socket, &socketSet);

    if((selectRet = select(socket+1, &socketSet, NULL, NULL, &tv)) < 0) {
        rh4n_log_fatal(props->logging, "Could not wait for new data - %s", strerror(errno));
        return(-1);
    }

    if(selectRet == 0) { return(1); }
    if(FD_ISSET(socket, &socketSet)) { return(0); }
    return(-1);
}
