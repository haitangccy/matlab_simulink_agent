#ifndef ph2_interleaved_BuckVi48_cap_host_h__
#define ph2_interleaved_BuckVi48_cap_host_h__
#ifdef HOST_CAPI_BUILD
#include "rtw_capi.h"
#include "rtw_modelmap_simtarget.h"
typedef struct { rtwCAPI_ModelMappingInfo mmi ; }
ph2_interleaved_BuckVi48_host_DataMapInfo_T ;
#ifdef __cplusplus
extern "C" {
#endif
void ph2_interleaved_BuckVi48_host_InitializeDataMapInfo (
ph2_interleaved_BuckVi48_host_DataMapInfo_T * dataMap , const char * path ) ;
#ifdef __cplusplus
}
#endif
#endif
#endif
