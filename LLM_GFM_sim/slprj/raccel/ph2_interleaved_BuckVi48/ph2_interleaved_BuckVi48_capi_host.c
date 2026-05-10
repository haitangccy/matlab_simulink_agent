#include "ph2_interleaved_BuckVi48_capi_host.h"
static ph2_interleaved_BuckVi48_host_DataMapInfo_T root;
static int initialized = 0;
__declspec( dllexport ) rtwCAPI_ModelMappingInfo *getRootMappingInfo()
{
    if (initialized == 0) {
        initialized = 1;
        ph2_interleaved_BuckVi48_host_InitializeDataMapInfo(&(root), "ph2_interleaved_BuckVi48");
    }
    return &root.mmi;
}

rtwCAPI_ModelMappingInfo *mexFunction(){return(getRootMappingInfo());}
