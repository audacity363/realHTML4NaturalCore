#ifndef RH4NNATCALLER
#define RH4NNATCALLER

#define RH4N_TEMPLATE_NAT_JSON "{" \
                               "\"error\":true," \
                               "\"type\":\"natural\"," \
                               "\"naterror\":{"\
                               "\"natMessageNumber\":%d," \
                               "\"natMessageText\":\"%s\"," \
                               "\"natLibrary\":\"%s\","\
                               "\"natMember\":\"%s\"," \
                               "\"natName\":\"%s\"," \
                               "\"natMethod\":\"%s\"," \
                               "\"natLine\":%d" \
                               "}}"



int rh4n_main_loadSessionInformations(RH4nProperties *props, int recvSocket);
int rh4n_natcaller_init_plain(RH4nProperties *props, int recvSocket);
int rh4n_natcaller_callNatural(RH4nProperties *props);
void rh4n_natcaller_logInternalError(RH4nProperties *props, char *error_str);
void rh4n_natcaller_handleNaturalError(RH4nProperties *props, int nniret, struct natural_exception natex);
void rh4n_natcaller_logNaturalError(RH4nProperties *props, struct natural_exception natex);
void rh4n_natcaller_logNaturalErrorToSocket(RH4nProperties *props, struct natural_exception natex);
void rh4n_natcaller_cleanup(int udsServer, int udsClient, char *udsServerPath, RH4nProperties *props);
//int rh4n_natcaller_parseArgs(int argc, char *argv[], struct RH4nCallArguments *args);

#ifdef natni_h
struct parameter_description* rh4nNaturalParamsGeneration(pnni_611_functions nnifuncs, RH4nProperties *properties, int *pnnierr);
#endif

#endif
