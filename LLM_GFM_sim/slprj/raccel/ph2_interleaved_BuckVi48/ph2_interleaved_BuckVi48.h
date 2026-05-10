#ifndef ph2_interleaved_BuckVi48_h_
#define ph2_interleaved_BuckVi48_h_
#ifndef ph2_interleaved_BuckVi48_COMMON_INCLUDES_
#define ph2_interleaved_BuckVi48_COMMON_INCLUDES_
#include <stdlib.h>
#include "sl_AsyncioQueue/AsyncioQueueCAPI.h"
#include "rtwtypes.h"
#include "sigstream_rtw.h"
#include "simtarget/slSimTgtSigstreamRTW.h"
#include "simtarget/slSimTgtSlioCoreRTW.h"
#include "simtarget/slSimTgtSlioClientsRTW.h"
#include "simtarget/slSimTgtSlioSdiRTW.h"
#include "simstruc.h"
#include "fixedpoint.h"
#include "raccel.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "rt_logging_simtarget.h"
#include "rt_nonfinite.h"
#include "math.h"
#include "dt_info.h"
#include "ext_work.h"
#endif
#include "ph2_interleaved_BuckVi48_types.h"
#include <stddef.h>
#include "rtw_modelmap_simtarget.h"
#include "rt_defines.h"
#include <string.h>
#include "rtGetInf.h"
#define MODEL_NAME ph2_interleaved_BuckVi48
#define NSAMPLE_TIMES (4) 
#define NINPUTS (0)       
#define NOUTPUTS (0)     
#define NBLOCKIO (30) 
#define NUM_ZC_EVENTS (0) 
#ifndef NCSTATES
#define NCSTATES (4)   
#elif NCSTATES != 4
#error Invalid specification of NCSTATES defined in compiler command
#endif
#ifndef rtmGetDataMapInfo
#define rtmGetDataMapInfo(rtm) (*rt_dataMapInfoPtr)
#endif
#ifndef rtmSetDataMapInfo
#define rtmSetDataMapInfo(rtm, val) (rt_dataMapInfoPtr = &val)
#endif
#ifndef IN_RACCEL_MAIN
#endif
typedef struct { real_T osxfsl0cym [ 11 ] ; real_T k2dlruxx5u [ 8 ] ; real_T
hsyb5sy2fc ; real_T djugjdenza ; real_T cgdfmnvyw5 ; real_T lcqe1riv5e ;
real_T fnw34rqsrd ; real_T ggnovrpbca ; real_T ln23cmyrrk ; real_T if2ke03cmg
; real_T osvfaydsul ; real_T gfgi4gjdtc ; real_T hvhda0al5i ; real_T
bdj2lr3e5n ; real_T howhprf3lv ; real_T aoadtzkehg ; real_T ejfa4qebmu ;
real_T jjy21yotzy ; real_T gmw5rkyhks ; real_T hqifyhyn5g ; real_T ofeouq3efa
; real_T c0fpagk2j4 ; real_T ha3c1iodke ; real_T ehbg2lyldn ; real_T
i0iuxica0n ; real_T bdsk5vrvux ; real_T pclhdfxm1d ; real_T gb2fvfaqkx ;
real_T g2ya2r5i0g ; real_T gf4nlt3vfv ; } B ; typedef struct { real_T
gwi4vt3wpl [ 4 ] ; struct { void * AS ; void * BS ; void * CS ; void * DS ;
void * DX_COL ; void * BD_COL ; void * TMP1 ; void * TMP2 ; void * XTMP ;
void * SWITCH_STATUS ; void * SWITCH_STATUS_INIT ; void * SW_CHG ; void *
G_STATE ; void * USWLAST ; void * XKM12 ; void * XKP12 ; void * XLAST ; void
* ULAST ; void * IDX_SW_CHG ; void * Y_SWITCH ; void * SWITCH_TYPES ; void *
IDX_OUT_SW ; void * SWITCH_TOPO_SAVED_IDX ; void * SWITCH_MAP ; } oi02hmlpyi
; struct { void * LoggedData [ 2 ] ; } lhus1uexmq ; struct { void *
LoggedData ; } fgy1lz20tg ; struct { void * LoggedData ; } gss0hbbl3l ;
struct { void * LoggedData ; } dtiqwrir45 ; struct { void * LoggedData ; }
gdqhx2icer ; struct { void * AQHandles ; } mrhxip3nk1 ; int_T ehenhsl3i5 [ 11
] ; boolean_T dwdcdvhiev ; boolean_T p0c3lv2aj4 ; } DW ; typedef struct {
real_T m411sasqn2 ; real_T ibkfpq1ykc ; real_T ilk5fcpmgd ; real_T ilikplwy3g
; } X ; typedef struct { real_T m411sasqn2 ; real_T ibkfpq1ykc ; real_T
ilk5fcpmgd ; real_T ilikplwy3g ; } XDot ; typedef struct { boolean_T
m411sasqn2 ; boolean_T ibkfpq1ykc ; boolean_T ilk5fcpmgd ; boolean_T
ilikplwy3g ; } XDis ; typedef struct { real_T m411sasqn2 ; real_T ibkfpq1ykc
; real_T ilk5fcpmgd ; real_T ilikplwy3g ; } CStateAbsTol ; typedef struct {
real_T m411sasqn2 ; real_T ibkfpq1ykc ; real_T ilk5fcpmgd ; real_T ilikplwy3g
; } CXPtMin ; typedef struct { real_T m411sasqn2 ; real_T ibkfpq1ykc ; real_T
ilk5fcpmgd ; real_T ilikplwy3g ; } CXPtMax ; typedef struct { real_T
ditqguoxg1 ; real_T ngadrcokn2 ; } ZCV ; typedef struct {
rtwCAPI_ModelMappingInfo mmi ; } DataMapInfo ; struct P_ { real_T
DC_Amplitude ; real_T PID_Controller_e_1_I ; real_T PID_Controller_e_2_I ;
real_T PID_Controller_i_1_I ; real_T PID_Controller_i_2_I ; real_T
PID_Controller_e_1_InitialConditionForIntegrator ; real_T
PID_Controller_i_1_InitialConditionForIntegrator ; real_T
PID_Controller_e_2_InitialConditionForIntegrator ; real_T
PID_Controller_i_2_InitialConditionForIntegrator ; real_T
PID_Controller_e_1_P ; real_T PID_Controller_i_1_P ; real_T
PID_Controller_e_2_P ; real_T PID_Controller_i_2_P ; real_T
StateSpace_AS_param [ 16 ] ; real_T StateSpace_BS_param [ 36 ] ; real_T
StateSpace_CS_param [ 44 ] ; real_T StateSpace_DS_param [ 99 ] ; real_T
StateSpace_X0_param [ 4 ] ; real_T donotdeletethisgain_Gain ; real_T
trigger_Value ; real_T Switch_Threshold ; real_T
donotdeletethisgain_Gain_czz01xtb3a ; real_T Constant3_Value ; real_T
Constant2_Value ; real_T uib1_Gain ; real_T Gain_Gain ; real_T
Switch1_Threshold ; real_T donotdeletethisgain_Gain_o0wxvljlan ; real_T
Constant3_Value_hw4zakmrfm ; real_T Constant2_Value_lsteepdquw ; real_T
uib1_Gain_bsvwuaxwpi ; real_T Gain1_Gain ; real_T SwitchCurrents_Value [ 8 ]
; real_T Constant_Value ; real_T Constant1_Value ; real_T
Constant2_Value_ahwqlmii2b ; real_T Vref1_Value ; real_T Vref2_Value ; } ;
extern const char_T * RT_MEMORY_ALLOCATION_ERROR ; extern B rtB ; extern X
rtX ; extern DW rtDW ; extern P rtP ; extern mxArray *
mr_ph2_interleaved_BuckVi48_GetDWork ( ) ; extern void
mr_ph2_interleaved_BuckVi48_SetDWork ( const mxArray * ssDW ) ; extern
mxArray * mr_ph2_interleaved_BuckVi48_GetSimStateDisallowedBlocks ( ) ;
extern const rtwCAPI_ModelMappingStaticInfo *
ph2_interleaved_BuckVi48_GetCAPIStaticMap ( void ) ; extern SimStruct * const
rtS ; extern DataMapInfo * rt_dataMapInfoPtr ; extern
rtwCAPI_ModelMappingInfo * rt_modelMapInfoPtr ; void MdlOutputs ( int_T tid )
; void MdlOutputsParameterSampleTime ( int_T tid ) ; void MdlUpdate ( int_T
tid ) ; void MdlTerminate ( void ) ; void MdlInitializeSizes ( void ) ; void
MdlInitializeSampleTimes ( void ) ; SimStruct * raccel_register_model (
ssExecutionInfo * executionInfo ) ;
#endif
