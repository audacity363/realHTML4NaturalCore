#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "rh4n.h"
#include "rh4n_natcaller.h"
#include "rh4n_messaging.h"

void rh4n_natcaller_logInternalError(RH4nProperties *props, char *error_str) {
    FILE *outputfile = NULL;

    if((outputfile = fopen(props->outputfile, "w")) == NULL) {
        rh4n_log_error(props->logging, "Could not open outputfile: [%s]", props->outputfile);
        rh4n_log_error(props->logging, "Response will be empty");
        return;
    }

    if(strcmp(props->errorrepresentation, "JSON") == 0) {
        fprintf(outputfile, "{\"error\":true,\"type\":\"internal\",msg:\"%s\"}", error_str);
    } else {
        fprintf(outputfile, "<html><head><title>Internal Error</title></head><body><h2>%s</h2></body>", error_str);
    }

    fclose(outputfile);
    return;
}

void rh4n_natcaller_handleNaturalError(RH4nProperties *props, int nniret, struct natural_exception natex) {
    char error_str[2048];

    rh4n_log_error(props->logging, "Error while executing natural program: %d", nniret);
    if(nniret > 0) {
        rh4n_log_error(props->logging, "Natural runtime error: %d", natex.natMessageNumber);
        if(props->mode == 0) {
            rh4n_natcaller_logNaturalError(props, natex);
        } else { 
            rh4n_natcaller_logNaturalErrorToSocket(props, natex);
        }
        return;
    }

    sprintf(error_str, "Could not run Natural program due to a NNI error. NNIret: %d", nniret);
    rh4n_natcaller_logInternalError(props, error_str);
    return;
}

void rh4n_natcaller_logNaturalError(RH4nProperties *props, struct natural_exception natex) {
    FILE *outputfile = NULL;

    if((outputfile = fopen(props->outputfile, "w")) == NULL) {
        rh4n_log_error(props->logging, "Could not open outputfile: [%s]", props->outputfile);
        rh4n_log_error(props->logging, "Response will be empty");
        return;
    }

    fprintf(outputfile, RH4N_TEMPLATE_NAT_JSON, natex.natMessageNumber, natex.natMessageText, 
            natex.natLibrary, natex.natMember, natex.natName, natex.natMethod, natex.natLine);

    fclose(outputfile);
    return;
}

void rh4n_natcaller_logNaturalErrorToSocket(RH4nProperties *props, struct natural_exception natex) {
    char errorstr[1024];

    rh4n_log_develop(props->logging, "Sending error to socket %d", props->udsClient);

    sprintf(errorstr, RH4N_TEMPLATE_NAT_JSON, natex.natMessageNumber, natex.natMessageText, 
            natex.natLibrary, natex.natMember, natex.natName, natex.natMethod, natex.natLine);


    rh4n_messaging_sendTextBlock(props->udsClient, errorstr, props);
}
