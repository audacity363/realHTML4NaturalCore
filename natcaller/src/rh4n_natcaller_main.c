#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>

#include "rh4n.h"
#include "rh4n_utils.h"
#include "rh4n_messaging.h"
#include "rh4n_natcaller.h"

struct RH4nCallArguments {
    int loglevel;
    char *socketfile;
    char *library;
    char *program;
};

struct RH4nCallArguments args;
int g_rh4n_natcaller_udsServer = 0, g_rh4n_natcaller_udsClient = 0;
RH4nProperties *g_rh4n_natcaller_props = NULL;

int rh4n_natcaller_parseArgs(int argc, char *argv[], struct RH4nCallArguments *args);
void rh4n_natcaller_printUsage(char *binname);
void rh4n_natcaller_signalHandler(int signal);

int main(int argc, char *argv[]) {
    struct stat fileStatus;
    int initRet = 0, natRet = 0;
    RH4nProperties props; memset(&props, 0x00, sizeof(props));
    time_t start = 0, now = 0;

    memset(&args, 0x00, sizeof(args));

    if(argc < 2) {
        rh4n_natcaller_printUsage(argv[0]);
        return(-1);
    }

    if(rh4n_natcaller_parseArgs(argc, argv, &args) < 0) {
        return(-1);
    }

    g_rh4n_natcaller_props = &props;

    signal(9, rh4n_natcaller_signalHandler);

    if((props.logging = rh4nLoggingCreateStreamingRule(args.library, args.program, args.loglevel, "")) == NULL) {
        fprintf(stderr, "Could not initialize logging to stdout\n");
        return(-1);
    }

    rh4nUtilsLoadProperties(args.socketfile, &props);

    // if((g_rh4n_natcaller_udsServer = rh4n_messaging_createUDSServer(args.socketfile, RH4NLIBMESSAGINGFLAG_NONBLOCKING, &props)) < 0) {
    //     rh4n_natcaller_cleanup(g_rh4n_natcaller_udsServer, g_rh4n_natcaller_udsClient, args.socketfile, &props);
    //     exit(1);
    // }

    // rh4n_log_debug(props.logging, "Waiting for a new client.");
    // start = time(NULL);

    // while(1) {
    //     g_rh4n_natcaller_udsClient = rh4n_messaging_waitForClient(g_rh4n_natcaller_udsServer, &props);
    //     if(g_rh4n_natcaller_udsClient > 0) {
    //         rh4n_log_debug(props.logging, "Got new client %d", g_rh4n_natcaller_udsClient);
    //         break;
    //     } else if(g_rh4n_natcaller_udsClient == -1) {
    //         rh4n_log_fatal(props.logging, "Could not wait for new client");
    //         rh4n_natcaller_cleanup(g_rh4n_natcaller_udsServer, g_rh4n_natcaller_udsClient, args.socketfile, &props);
    //         exit(1);
    //     }

    //     now = time(NULL);
    //     if(now - start >= 5) { rh4n_log_fatal(props.logging, "Timeout while waiting for client on %s", args.socketfile);
    //         rh4n_natcaller_cleanup(g_rh4n_natcaller_udsClient, g_rh4n_natcaller_udsClient, args.socketfile, &props);
    //         exit(1);
    //     }
    // }
    // if(g_rh4n_natcaller_udsClient< 0) {
    //     rh4n_log_fatal(props.logging, "Timeout while waiting for a new client");
    //     exit(1);
    // }

    // RH4N_CHECKERROR(rh4n_main_loadSessionInformations(&props, g_rh4n_natcaller_udsClient));

    // initRet = rh4n_natcaller_init_plain(&props, g_rh4n_natcaller_udsClient);
    // if(initRet < 0) {
    //     rh4n_log_fatal(props.logging, "Something went wrong while initializing");
    //     rh4n_natcaller_cleanup(g_rh4n_natcaller_udsServer, g_rh4n_natcaller_udsClient, args.socketfile, &props);
    //     return(-1);
    // }

    // props.udsClient = g_rh4n_natcaller_udsClient;

    natRet = rh4n_natcaller_callNatural(&props);

    rh4n_natcaller_cleanup(g_rh4n_natcaller_udsServer, g_rh4n_natcaller_udsClient, args.socketfile, &props);
    return(natRet);
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

void rh4n_natcaller_cleanup(int udsServer, int udsClient, char *udsServerPath, RH4nProperties *props) {
    // if(udsClient > -1) {
    //     shutdown(udsClient, SHUT_RDWR);
    //     close(udsClient);
    // }
    // if(udsServer > -1) close(udsServer);
    // unlink(udsServerPath);

    rh4nUtilsFreeProperties(props);
}

int rh4n_natcaller_parseArgs(int argc, char *argv[], struct RH4nCallArguments *args) {
    int opt;

    args->loglevel = RH4N_ERROR;

    while((opt = getopt(argc, argv, "L:P:l:")) != EOF) {
        switch(opt) {
            case 'L':
                args->library = optarg;
                break;
            case 'P':
                args->program = optarg;
                break;
            case 'l':
                if((args->loglevel = rh4nLoggingConvertStrtoInt(optarg)) < 0) {
                    fprintf(stderr, "Unkown loglevel %s\n", optarg);
                    rh4n_natcaller_printUsage(argv[0]);
                    return(-1);
                }
                break;
        }
    }

    if(optind+1 != argc) {
        rh4n_natcaller_printUsage(argv[0]);
        return(-1);
    }

    if(args->library == NULL) {
        args->library = "";
    }

    if(args->program == NULL) {
        args->program = "";
    }

    args->socketfile = argv[optind];
    return(0);
}

void rh4n_natcaller_printUsage(char *binname) {
    printf("Usage: %s [-options] socketfile\n\n", binname);
    printf("options:\n");
    printf("\t-l <DEVELOP|DEBUG|INFO|WARN|ERROR|FATAL> default: ERROR\n");
    printf("\t\tset logging level\n");
    printf("\t-L <libname>\n");
    printf("\t\tset natural library which shows up in the logging\n");
    printf("\t-P <progname>\n");
    printf("\t\tset natural program which shows up in the logging\n");
}

void rh4n_natcaller_signalHandler(int signal) {
    rh4n_log_error(g_rh4n_natcaller_props->logging, "Recived signal %d. Cleaning up", signal);
    rh4n_natcaller_cleanup(g_rh4n_natcaller_udsServer, g_rh4n_natcaller_udsClient, args.socketfile, g_rh4n_natcaller_props);
}
