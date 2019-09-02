#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_nat.h"
#include "rh4n_nat_varhandling.h"

int rh4nnatIntegerHandler(RH4nNatVarHandleParms *args) {
    int varlibret = 0, nniret = 0;
    int tmpbuff = 0;

    if(args->desc->dimensions > 0) {
        return(rh4nnatIntegerArrayHandler(args));
    }
    
    if((nniret = args->nnifuncs->pf_nni_get_parm(args->nnifuncs, args->parmindex, args->parmhandle, 
        args->desc->length, &tmpbuff)) != NNI_RC_OK) {
        sprintf(args->errorstr, "Could not get parm %d. NNI ret: %d", args->parmindex, nniret);
        return(RH4N_RET_NNI_ERR);
    }

    switch(args->desc->length) {
        case 1:
            tmpbuff = tmpbuff >> 24; 
            break;
        case 2:
            tmpbuff = tmpbuff >> 16; 
            break;
    }

    rh4n_log_debug(args->props->logging, "Got int: %d", tmpbuff);

    if((varlibret = rh4nvarCreateNewInt(args->varlist, NULL, args->tmpname, tmpbuff)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not create new string: %d", varlibret);
        return(varlibret);
    }

    return(RH4N_RET_OK);
}

int rh4nnatIntegerArrayHandler(RH4nNatVarHandleParms *args) {
    int x = 0, y = 0, z = 0, varlibret = 0, arrindex[3] = {0, 0, 0};

    if((varlibret = rh4nvarCreateNewIntArray(args->varlist, NULL, args->tmpname, args->desc->dimensions, 
                args->desc->occurrences)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Error while creating String Array: %d", varlibret);
        return(varlibret);
    }

    for(; x < args->desc->occurrences[0]; x++) {
        arrindex[0] = x; arrindex[1] = arrindex[2] = 0;
        if(args->desc->dimensions == 1) {
            if((varlibret = rh4nnatSaveIntegerArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
        } else {
            for(y = 0; y < args->desc->occurrences[1]; y++) {
                arrindex[1] = y; arrindex[2] = 0;
                if(args->desc->dimensions == 2) {
                    if((varlibret = rh4nnatSaveIntegerArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
                } else {
                    for(z = 0; z < args->desc->occurrences[2]; z++) {
                        arrindex[2] = z;
                        if((varlibret = rh4nnatSaveIntegerArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
                    }
                }
            }
        }
    }

    return(RH4N_RET_OK);
}

int rh4nnatSaveIntegerArrayEntry(RH4nNatVarHandleParms *args, int index[3]) {
    int nniret = 0, varlibret = 0, varlibindex[3] = { -1, -1, -1};
    int tmpbuff = 0;


    if((nniret = args->nnifuncs->pf_nni_get_parm_array(args->nnifuncs, args->parmindex, args->parmhandle, args->desc->length, 
            &tmpbuff, index)) != NNI_RC_OK) {
        sprintf(args->errorstr, "Could not get array entry from parm %d (x: %d, y:%d, z: %d) NNI err: %d", 
            args->parmindex, index[0], index[1], index[2], nniret);
        rh4n_log_error(args->props->logging, "%s", args->errorstr);
        return(RH4N_RET_NNI_ERR);

    }

    switch(args->desc->length) {
        case 1:
            tmpbuff = tmpbuff >> 24; 
            break;
        case 2:
            tmpbuff = tmpbuff >> 16; 
            break;
    }

    if(args->desc->dimensions == 1) varlibindex[0] = index[0];
    else if(args->desc->dimensions == 2) { varlibindex[0] = index[0]; varlibindex[1] = index[1]; }
    else if(args->desc->dimensions == 3) { varlibindex[0] = index[0]; varlibindex[1] = index[1]; varlibindex[2] = index[2]; }


    if((varlibret = rh4nvarSetIntArrayEntry(args->varlist, NULL, args->tmpname, varlibindex, tmpbuff)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not set Array Entry x: %d y: %d z: %d", varlibindex[0], varlibindex[1], varlibindex[2]);
        return(varlibret);
    }

    return(RH4N_RET_OK);
}
