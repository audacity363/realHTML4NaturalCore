#ifndef RH4N_STD
#define RH4N_STD

#include <stdint.h>

#include "natuser.h"
#include "natni.h"
#include "rh4n_vars.h"
#include "rh4n_logging.h"

#define RH4N_RET_OK 0
#define RH4N_RET_PARM_MISSMATCH 1
#define RH4N_RET_SO_ERR 2
#define RH4N_RET_VAR_PTR_ERR 3
#define RH4N_RET_NNI_ERR 4
#define RH4N_RET_PARM_TYPE_MISSMATCH 5
#define RH4N_RET_UNKOWN_VAR 6
#define RH4N_RET_BUFFER_OVERFLOW 7
#define RH4N_RET_VAR_MISSMATCH 8
#define RH4N_RET_CONST_VAR 9
#define RH4N_RET_DIM1_TOO_SMALL 10
#define RH4N_RET_DIM2_TOO_SMALL 11
#define RH4N_RET_DIM3_TOO_SMALL 12
#define RH4N_RET_MIXED_XARRAY 13
#define RH4N_RET_MEMORY_ERR 14
#define RH4N_RET_UNICODE_ERR 15
#define RH4N_RET_USE_F8 16
#define RH4N_RET_NOT_SUPPORTED 17
#define RH4N_RET_MALFORMED_FORMAT_STR 18
#define RH4N_RET_LDA_PARSE_ERR 19
#define RH4N_RET_FORMAT_ERR 20
#define RH4N_RET_JNI_ERR 21
#define RH4N_RET_LOGGING_ERR 22
#define RH4N_RET_INTERNAL_ERR 23
#define RH4N_RET_PARM_ERR 24
#define RH4N_RET_VAR_EXISTS 25
#define RH4N_RET_VAR_NOT_ARRAY 26
#define RH4N_RET_VAR_BAD_DIM 27
#define RH4N_RET_VAR_BAD_LENGTH 28
#define RH4N_RET_VAR_BAD_INDEX 29
#define RH4N_RET_TARADITIONAL 30
#define RH4N_RET_LDA_NAT_MISSMATCH 31
#define RH4N_RET_STRUCT_ERR 32
#define RH4N_RET_NO_USER 33
#define RH4N_RET_FILE_ERR 34

#define RH4N_MODE_PLAIN 0
#define RH4N_MODE_WS 1

#define RH4N_CHECKERROR(function) {int rh4ncheckerrorret = 0; if((rh4ncheckerrorret = function) != 0) { return(rh4ncheckerrorret); }}

typedef struct {
    char natlibrary[NNI_LEN_LIBRARY+1];
    char natprogram[NNI_LEN_MEMBER+1];
    char *natparms;
    char c_loglevel[10];
    int  i_loglevel;
    char httpreqtype[10];
    RH4nVarList urlvars;
    RH4nVarList bodyvars;
    char *natsrcpath;
    char *outputfile;
    char *logpath;
    char errorrepresentation[20];

    RH4nLogrule *logging;

    char username[33];

    uint8_t mode;
    int udsClient;

} RH4nProperties;

#include "rh4n_vars_print.h"

#define RH4N_SONAME_NATURAL "libnatural.so"
#endif
