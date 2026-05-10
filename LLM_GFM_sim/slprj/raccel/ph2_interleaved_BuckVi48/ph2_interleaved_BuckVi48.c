#include "ph2_interleaved_BuckVi48.h"
#include "rtwtypes.h"
#include "mwmathutil.h"
#include "ph2_interleaved_BuckVi48_private.h"
#include "rt_logging_mmi.h"
#include "ph2_interleaved_BuckVi48_capi.h"
#include "ph2_interleaved_BuckVi48_dt.h"
extern void * CreateDiagnosticAsVoidPtr_wrapper ( const char * id , int nargs
, ... ) ; extern ssExecutionInfo gblExecutionInfo ; RTWExtModeInfo *
gblRTWExtModeInfo = NULL ; void raccelForceExtModeShutdown ( boolean_T
extModeStartPktReceived ) { if ( ! extModeStartPktReceived ) { boolean_T
stopRequested = false ; rtExtModeWaitForStartPkt ( gblRTWExtModeInfo , 3 , &
stopRequested ) ; } rtExtModeShutdown ( 3 ) ; }
#include "slsv_diagnostic_codegen_c_api.h"
#include "slsa_engine_exec.h"
#ifdef RSIM_WITH_SOLVER_MULTITASKING
boolean_T gbl_raccel_isMultitasking = 1 ;
#else
boolean_T gbl_raccel_isMultitasking = 0 ;
#endif
boolean_T gbl_raccel_tid01eq = 0 ; int_T gbl_raccel_NumST = 4 ; const char_T
* gbl_raccel_Version = "24.1 (R2024a) 19-Nov-2023" ; void
raccel_setup_MMIStateLog ( SimStruct * S ) {
#ifdef UseMMIDataLogging
rt_FillStateSigInfoFromMMI ( ssGetRTWLogInfo ( S ) , & ssGetErrorStatus ( S )
) ;
#else
UNUSED_PARAMETER ( S ) ;
#endif
} static DataMapInfo rt_dataMapInfo ; DataMapInfo * rt_dataMapInfoPtr = &
rt_dataMapInfo ; rtwCAPI_ModelMappingInfo * rt_modelMapInfoPtr = & (
rt_dataMapInfo . mmi ) ; int_T enableFcnCallFlag [ ] = { 1 , 1 , 1 , 1 } ;
const char * raccelLoadInputsAndAperiodicHitTimes ( SimStruct * S , const
char * inportFileName , int * matFileFormat ) { return
rt_RAccelReadInportsMatFile ( S , inportFileName , matFileFormat ) ; }
#include "simstruc.h"
#include "fixedpoint.h"
#include "slsa_engine_exec.h"
#include "simtarget/slSimTgtSLExecSimBridge.h"
B rtB ; X rtX ; DW rtDW ; static SimStruct model_S ; SimStruct * const rtS =
& model_S ; void MdlInitialize ( void ) { { int32_T i , j ; real_T * As = (
real_T * ) rtDW . oi02hmlpyi . AS ; real_T * Bs = ( real_T * ) rtDW .
oi02hmlpyi . BS ; real_T * Cs = ( real_T * ) rtDW . oi02hmlpyi . CS ; real_T
* Ds = ( real_T * ) rtDW . oi02hmlpyi . DS ; real_T * X0 = ( real_T * ) &
rtDW . gwi4vt3wpl [ 0 ] ; for ( i = 0 ; i < 4 ; i ++ ) { X0 [ i ] = ( rtP .
StateSpace_X0_param [ i ] ) ; } for ( i = 0 ; i < 4 ; i ++ ) { for ( j = 0 ;
j < 4 ; j ++ ) As [ i * 4 + j ] = ( rtP . StateSpace_AS_param [ i + j * 4 ] )
; for ( j = 0 ; j < 9 ; j ++ ) Bs [ i * 9 + j ] = ( rtP . StateSpace_BS_param
[ i + j * 4 ] ) ; } for ( i = 0 ; i < 11 ; i ++ ) { for ( j = 0 ; j < 4 ; j
++ ) Cs [ i * 4 + j ] = ( rtP . StateSpace_CS_param [ i + j * 11 ] ) ; } for
( i = 0 ; i < 11 ; i ++ ) { for ( j = 0 ; j < 9 ; j ++ ) Ds [ i * 9 + j ] = (
rtP . StateSpace_DS_param [ i + j * 11 ] ) ; } { int_T * switch_status = (
int_T * ) rtDW . oi02hmlpyi . SWITCH_STATUS ; int_T * gState = ( int_T * )
rtDW . oi02hmlpyi . G_STATE ; real_T * yswitch = ( real_T * ) rtDW .
oi02hmlpyi . Y_SWITCH ; int_T * switchTypes = ( int_T * ) rtDW . oi02hmlpyi .
SWITCH_TYPES ; int_T * idxOutSw = ( int_T * ) rtDW . oi02hmlpyi . IDX_OUT_SW
; int_T * switch_status_init = ( int_T * ) rtDW . oi02hmlpyi .
SWITCH_STATUS_INIT ; switch_status [ 0 ] = 0 ; switch_status_init [ 0 ] = 0 ;
gState [ 0 ] = ( int_T ) 0.0 ; yswitch [ 0 ] = 1 / 0.1 ; switchTypes [ 0 ] =
( int_T ) 1.0 ; idxOutSw [ 0 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 1 ]
= 0 ; switch_status_init [ 1 ] = 0 ; gState [ 1 ] = ( int_T ) 0.0 ; yswitch [
1 ] = 1 / 0.1 ; switchTypes [ 1 ] = ( int_T ) 1.0 ; idxOutSw [ 1 ] = ( (
int_T ) 0.0 ) - 1 ; switch_status [ 2 ] = 0 ; switch_status_init [ 2 ] = 0 ;
gState [ 2 ] = ( int_T ) 0.0 ; yswitch [ 2 ] = 1 / 0.1 ; switchTypes [ 2 ] =
( int_T ) 1.0 ; idxOutSw [ 2 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 3 ]
= 0 ; switch_status_init [ 3 ] = 0 ; gState [ 3 ] = ( int_T ) 0.0 ; yswitch [
3 ] = 1 / 0.1 ; switchTypes [ 3 ] = ( int_T ) 1.0 ; idxOutSw [ 3 ] = ( (
int_T ) 0.0 ) - 1 ; switch_status [ 4 ] = 0 ; switch_status_init [ 4 ] = 0 ;
gState [ 4 ] = ( int_T ) 0.0 ; yswitch [ 4 ] = 1 / 0.01 ; switchTypes [ 4 ] =
( int_T ) 3.0 ; idxOutSw [ 4 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 5 ]
= 0 ; switch_status_init [ 5 ] = 0 ; gState [ 5 ] = ( int_T ) 0.0 ; yswitch [
5 ] = 1 / 0.01 ; switchTypes [ 5 ] = ( int_T ) 3.0 ; idxOutSw [ 5 ] = ( (
int_T ) 0.0 ) - 1 ; switch_status [ 6 ] = 0 ; switch_status_init [ 6 ] = 0 ;
gState [ 6 ] = ( int_T ) 0.0 ; yswitch [ 6 ] = 1 / 0.01 ; switchTypes [ 6 ] =
( int_T ) 3.0 ; idxOutSw [ 6 ] = ( ( int_T ) 0.0 ) - 1 ; switch_status [ 7 ]
= 0 ; switch_status_init [ 7 ] = 0 ; gState [ 7 ] = ( int_T ) 0.0 ; yswitch [
7 ] = 1 / 0.01 ; switchTypes [ 7 ] = ( int_T ) 3.0 ; idxOutSw [ 7 ] = ( (
int_T ) 0.0 ) - 1 ; } } rtX . m411sasqn2 = rtP .
PID_Controller_e_1_InitialConditionForIntegrator ; rtX . ibkfpq1ykc = rtP .
PID_Controller_i_1_InitialConditionForIntegrator ; rtX . ilk5fcpmgd = rtP .
PID_Controller_e_2_InitialConditionForIntegrator ; rtX . ilikplwy3g = rtP .
PID_Controller_i_2_InitialConditionForIntegrator ; } void MdlStart ( void ) {
{ bool externalInputIsInDatasetFormat = false ; void * pISigstreamManager =
rt_GetISigstreamManager ( rtS ) ;
rtwISigstreamManagerGetInputIsInDatasetFormat ( pISigstreamManager , &
externalInputIsInDatasetFormat ) ; if ( externalInputIsInDatasetFormat ) { }
} { { { bool isStreamoutAlreadyRegistered = false ; { sdiSignalSourceInfoU
srcInfo ; sdiLabelU loggedName = sdiGetLabelFromChars ( "PID_Controller_i_1"
) ; sdiLabelU origSigName = sdiGetLabelFromChars ( "" ) ; sdiLabelU propName
= sdiGetLabelFromChars ( "PID_Controller_i_1" ) ; sdiLabelU blockPath =
sdiGetLabelFromChars ( "ph2_interleaved_BuckVi48/To Workspace" ) ; sdiLabelU
blockSID = sdiGetLabelFromChars ( "" ) ; sdiLabelU subPath =
sdiGetLabelFromChars ( "" ) ; sdiDims sigDims ; sdiLabelU sigName =
sdiGetLabelFromChars ( "PID_Controller_i_1" ) ; sdiAsyncRepoDataTypeHandle
hDT = sdiAsyncRepoGetBuiltInDataTypeHandle ( DATA_TYPE_DOUBLE ) ; {
sdiComplexity sigComplexity = REAL ; sdiSampleTimeContinuity stCont =
SAMPLE_TIME_CONTINUOUS ; int_T sigDimsArray [ 2 ] = { 1 , 1 } ; sigDims .
nDims = 2 ; sigDims . dimensions = sigDimsArray ; srcInfo . numBlockPathElems
= 1 ; srcInfo . fullBlockPath = ( sdiFullBlkPathU ) & blockPath ; srcInfo .
SID = ( sdiSignalIDU ) & blockSID ; srcInfo . subPath = subPath ; srcInfo .
portIndex = 0 + 1 ; srcInfo . signalName = sigName ; srcInfo . sigSourceUUID
= 0 ; rtDW . mrhxip3nk1 . AQHandles = sdiStartAsyncioQueueCreation ( hDT , &
srcInfo , rt_dataMapInfo . mmi . InstanceMap . fullPath ,
"88a512c8-fe2f-454b-a190-3b9da8f256a5" , sigComplexity , & sigDims ,
DIMENSIONS_MODE_FIXED , stCont , "" ) ; sdiCompleteAsyncioQueueCreation (
rtDW . mrhxip3nk1 . AQHandles , hDT , & srcInfo ) ; if ( rtDW . mrhxip3nk1 .
AQHandles ) { sdiSetSignalSampleTimeString ( rtDW . mrhxip3nk1 . AQHandles ,
"&#x8FDE;&#x7EED;" , 0.0 , ssGetTFinal ( rtS ) ) ; sdiSetSignalRefRate ( rtDW
. mrhxip3nk1 . AQHandles , 0.0 ) ; sdiSetRunStartTime ( rtDW . mrhxip3nk1 .
AQHandles , ssGetTaskTime ( rtS , 0 ) ) ; sdiAsyncRepoSetSignalExportSettings
( rtDW . mrhxip3nk1 . AQHandles , 1 , 0 ) ; sdiAsyncRepoSetSignalExportName (
rtDW . mrhxip3nk1 . AQHandles , loggedName , origSigName , propName ) ;
sdiAsyncRepoSetBlockPathDomain ( rtDW . mrhxip3nk1 . AQHandles ) ; }
sdiFreeLabel ( sigName ) ; sdiFreeLabel ( loggedName ) ; sdiFreeLabel (
origSigName ) ; sdiFreeLabel ( propName ) ; sdiFreeLabel ( blockPath ) ;
sdiFreeLabel ( blockSID ) ; sdiFreeLabel ( subPath ) ; } } if ( !
isStreamoutAlreadyRegistered ) { { sdiLabelU varName = sdiGetLabelFromChars (
"Data" ) ; sdiRegisterWksVariable ( rtDW . mrhxip3nk1 . AQHandles , varName ,
"timeseries" ) ; sdiFreeLabel ( varName ) ; } } } } } { rtDW . oi02hmlpyi .
AS = ( real_T * ) calloc ( 4 * 4 , sizeof ( real_T ) ) ; rtDW . oi02hmlpyi .
BS = ( real_T * ) calloc ( 4 * 9 , sizeof ( real_T ) ) ; rtDW . oi02hmlpyi .
CS = ( real_T * ) calloc ( 11 * 4 , sizeof ( real_T ) ) ; rtDW . oi02hmlpyi .
DS = ( real_T * ) calloc ( 11 * 9 , sizeof ( real_T ) ) ; rtDW . oi02hmlpyi .
DX_COL = ( real_T * ) calloc ( 11 , sizeof ( real_T ) ) ; rtDW . oi02hmlpyi .
TMP2 = ( real_T * ) calloc ( 9 , sizeof ( real_T ) ) ; rtDW . oi02hmlpyi .
BD_COL = ( real_T * ) calloc ( 4 , sizeof ( real_T ) ) ; rtDW . oi02hmlpyi .
TMP1 = ( real_T * ) calloc ( 4 , sizeof ( real_T ) ) ; rtDW . oi02hmlpyi .
XTMP = ( real_T * ) calloc ( 4 , sizeof ( real_T ) ) ; rtDW . oi02hmlpyi .
SWITCH_STATUS = ( int_T * ) calloc ( 8 , sizeof ( int_T ) ) ; rtDW .
oi02hmlpyi . SW_CHG = ( int_T * ) calloc ( 8 , sizeof ( int_T ) ) ; rtDW .
oi02hmlpyi . G_STATE = ( int_T * ) calloc ( 8 , sizeof ( int_T ) ) ; rtDW .
oi02hmlpyi . Y_SWITCH = ( real_T * ) calloc ( 8 , sizeof ( real_T ) ) ; rtDW
. oi02hmlpyi . SWITCH_TYPES = ( int_T * ) calloc ( 8 , sizeof ( int_T ) ) ;
rtDW . oi02hmlpyi . IDX_OUT_SW = ( int_T * ) calloc ( 8 , sizeof ( int_T ) )
; rtDW . oi02hmlpyi . SWITCH_STATUS_INIT = ( int_T * ) calloc ( 8 , sizeof (
int_T ) ) ; rtDW . oi02hmlpyi . USWLAST = ( real_T * ) calloc ( 8 , sizeof (
real_T ) ) ; } MdlInitialize ( ) ; } void MdlOutputs ( int_T tid ) { real_T
j2tthkygy1 ; real_T jgg3ixt0id ; boolean_T pevn3ircv2 ; if ( ssIsSampleHit (
rtS , 2 , 0 ) ) { { real_T accum ; int_T * switch_status = ( int_T * ) rtDW .
oi02hmlpyi . SWITCH_STATUS ; int_T * switch_status_init = ( int_T * ) rtDW .
oi02hmlpyi . SWITCH_STATUS_INIT ; int_T * SwitchChange = ( int_T * ) rtDW .
oi02hmlpyi . SW_CHG ; int_T * gState = ( int_T * ) rtDW . oi02hmlpyi .
G_STATE ; real_T * yswitch = ( real_T * ) rtDW . oi02hmlpyi . Y_SWITCH ;
int_T * switchTypes = ( int_T * ) rtDW . oi02hmlpyi . SWITCH_TYPES ; int_T *
idxOutSw = ( int_T * ) rtDW . oi02hmlpyi . IDX_OUT_SW ; real_T * DxCol = (
real_T * ) rtDW . oi02hmlpyi . DX_COL ; real_T * tmp2 = ( real_T * ) rtDW .
oi02hmlpyi . TMP2 ; real_T * BDcol = ( real_T * ) rtDW . oi02hmlpyi . BD_COL
; real_T * tmp1 = ( real_T * ) rtDW . oi02hmlpyi . TMP1 ; real_T * uswlast =
( real_T * ) rtDW . oi02hmlpyi . USWLAST ; int_T newState ; int_T swChanged =
0 ; int loopsToDo = 20 ; real_T temp ; memcpy ( switch_status_init ,
switch_status , 8 * sizeof ( int_T ) ) ; memcpy ( uswlast , & rtB .
osxfsl0cym [ 0 ] , 8 * sizeof ( real_T ) ) ; do { if ( loopsToDo == 1 ) {
swChanged = 0 ; { int_T i1 ; for ( i1 = 0 ; i1 < 8 ; i1 ++ ) { swChanged = (
( SwitchChange [ i1 ] = switch_status_init [ i1 ] - switch_status [ i1 ] ) !=
0 ) ? 1 : swChanged ; switch_status [ i1 ] = switch_status_init [ i1 ] ; } }
} else { real_T * Cs = ( real_T * ) rtDW . oi02hmlpyi . CS ; real_T * Ds = (
real_T * ) rtDW . oi02hmlpyi . DS ; { int_T i1 ; real_T * y0 = & rtB .
osxfsl0cym [ 0 ] ; for ( i1 = 0 ; i1 < 11 ; i1 ++ ) { accum = 0.0 ; accum +=
* ( Cs ++ ) * rtDW . gwi4vt3wpl [ 0 ] ; accum += * ( Cs ++ ) * rtDW .
gwi4vt3wpl [ 1 ] ; accum += * ( Cs ++ ) * rtDW . gwi4vt3wpl [ 2 ] ; accum +=
* ( Cs ++ ) * rtDW . gwi4vt3wpl [ 3 ] ; { int_T i2 ; const real_T * u0 = &
rtP . SwitchCurrents_Value [ 0 ] ; for ( i2 = 0 ; i2 < 8 ; i2 ++ ) { accum +=
* ( Ds ++ ) * u0 [ i2 ] ; } accum += * ( Ds ++ ) * rtP . DC_Amplitude ; } y0
[ i1 ] = accum ; } } swChanged = 0 ; { int_T i1 ; real_T * y0 = & rtB .
osxfsl0cym [ 0 ] ; for ( i1 = 0 ; i1 < 8 ; i1 ++ ) { switch ( switchTypes [
i1 ] ) { case 1 : newState = gState [ i1 ] > 0 ? 1 : 0 ; break ; case 3 :
newState = y0 [ i1 ] > 0.0 ? 1 : ( ( y0 [ i1 ] < 0.0 ) ? 0 : switch_status [
i1 ] ) ; break ; } swChanged = ( ( SwitchChange [ i1 ] = newState -
switch_status [ i1 ] ) != 0 ) ? 1 : swChanged ; switch_status [ i1 ] =
newState ; } } } if ( swChanged ) { real_T * As = ( real_T * ) rtDW .
oi02hmlpyi . AS ; real_T * Cs = ( real_T * ) rtDW . oi02hmlpyi . CS ; real_T
* Bs = ( real_T * ) rtDW . oi02hmlpyi . BS ; real_T * Ds = ( real_T * ) rtDW
. oi02hmlpyi . DS ; real_T a1 ; { int_T i1 ; for ( i1 = 0 ; i1 < 8 ; i1 ++ )
{ if ( SwitchChange [ i1 ] != 0 ) { a1 = yswitch [ i1 ] * SwitchChange [ i1 ]
; temp = 1 / ( 1 - Ds [ i1 * 10 ] * a1 ) ; { int_T i2 ; for ( i2 = 0 ; i2 <
11 ; i2 ++ ) { DxCol [ i2 ] = Ds [ i2 * 9 + i1 ] * temp * a1 ; } } DxCol [ i1
] = temp ; BDcol [ 0 ] = Bs [ 0 + i1 ] * a1 ; BDcol [ 1 ] = Bs [ 9 + i1 ] *
a1 ; BDcol [ 2 ] = Bs [ 18 + i1 ] * a1 ; BDcol [ 3 ] = Bs [ 27 + i1 ] * a1 ;
memcpy ( tmp1 , & Cs [ i1 * 4 ] , 4 * sizeof ( real_T ) ) ; memset ( & Cs [
i1 * 4 ] , '\0' , 4 * sizeof ( real_T ) ) ; memcpy ( tmp2 , & Ds [ i1 * 9 ] ,
9 * sizeof ( real_T ) ) ; memset ( & Ds [ i1 * 9 ] , '\0' , 9 * sizeof (
real_T ) ) ; { int_T i2 ; for ( i2 = 0 ; i2 < 11 ; i2 ++ ) { a1 = DxCol [ i2
] ; Cs [ i2 * 4 + 0 ] += a1 * tmp1 [ 0 ] ; Cs [ i2 * 4 + 1 ] += a1 * tmp1 [ 1
] ; Cs [ i2 * 4 + 2 ] += a1 * tmp1 [ 2 ] ; Cs [ i2 * 4 + 3 ] += a1 * tmp1 [ 3
] ; { int_T i3 ; for ( i3 = 0 ; i3 < 9 ; i3 ++ ) { Ds [ i2 * 9 + i3 ] += a1 *
tmp2 [ i3 ] ; } } } } a1 = BDcol [ 0 ] ; As [ 0 + 0 ] += a1 * Cs [ i1 * 4 + 0
] ; As [ 0 + 1 ] += a1 * Cs [ i1 * 4 + 1 ] ; As [ 0 + 2 ] += a1 * Cs [ i1 * 4
+ 2 ] ; As [ 0 + 3 ] += a1 * Cs [ i1 * 4 + 3 ] ; { int_T i2 ; for ( i2 = 0 ;
i2 < 9 ; i2 ++ ) { Bs [ 0 + i2 ] += a1 * Ds [ i1 * 9 + i2 ] ; } } a1 = BDcol
[ 1 ] ; As [ 4 + 0 ] += a1 * Cs [ i1 * 4 + 0 ] ; As [ 4 + 1 ] += a1 * Cs [ i1
* 4 + 1 ] ; As [ 4 + 2 ] += a1 * Cs [ i1 * 4 + 2 ] ; As [ 4 + 3 ] += a1 * Cs
[ i1 * 4 + 3 ] ; { int_T i2 ; for ( i2 = 0 ; i2 < 9 ; i2 ++ ) { Bs [ 9 + i2 ]
+= a1 * Ds [ i1 * 9 + i2 ] ; } } a1 = BDcol [ 2 ] ; As [ 8 + 0 ] += a1 * Cs [
i1 * 4 + 0 ] ; As [ 8 + 1 ] += a1 * Cs [ i1 * 4 + 1 ] ; As [ 8 + 2 ] += a1 *
Cs [ i1 * 4 + 2 ] ; As [ 8 + 3 ] += a1 * Cs [ i1 * 4 + 3 ] ; { int_T i2 ; for
( i2 = 0 ; i2 < 9 ; i2 ++ ) { Bs [ 18 + i2 ] += a1 * Ds [ i1 * 9 + i2 ] ; } }
a1 = BDcol [ 3 ] ; As [ 12 + 0 ] += a1 * Cs [ i1 * 4 + 0 ] ; As [ 12 + 1 ] +=
a1 * Cs [ i1 * 4 + 1 ] ; As [ 12 + 2 ] += a1 * Cs [ i1 * 4 + 2 ] ; As [ 12 +
3 ] += a1 * Cs [ i1 * 4 + 3 ] ; { int_T i2 ; for ( i2 = 0 ; i2 < 9 ; i2 ++ )
{ Bs [ 27 + i2 ] += a1 * Ds [ i1 * 9 + i2 ] ; } } } } } } } while ( swChanged
> 0 && -- loopsToDo > 0 ) ; if ( loopsToDo == 0 ) { real_T * Cs = ( real_T *
) rtDW . oi02hmlpyi . CS ; real_T * Ds = ( real_T * ) rtDW . oi02hmlpyi . DS
; { int_T i1 ; real_T * y0 = & rtB . osxfsl0cym [ 0 ] ; for ( i1 = 0 ; i1 <
11 ; i1 ++ ) { accum = 0.0 ; accum += * ( Cs ++ ) * rtDW . gwi4vt3wpl [ 0 ] ;
accum += * ( Cs ++ ) * rtDW . gwi4vt3wpl [ 1 ] ; accum += * ( Cs ++ ) * rtDW
. gwi4vt3wpl [ 2 ] ; accum += * ( Cs ++ ) * rtDW . gwi4vt3wpl [ 3 ] ; { int_T
i2 ; const real_T * u0 = & rtP . SwitchCurrents_Value [ 0 ] ; for ( i2 = 0 ;
i2 < 8 ; i2 ++ ) { accum += * ( Ds ++ ) * u0 [ i2 ] ; } accum += * ( Ds ++ )
* rtP . DC_Amplitude ; } y0 [ i1 ] = accum ; } } } { int_T i1 ; real_T * y1 =
& rtB . k2dlruxx5u [ 0 ] ; for ( i1 = 0 ; i1 < 8 ; i1 ++ ) { y1 [ i1 ] = (
real_T ) switch_status [ i1 ] ; } } } rtB . hsyb5sy2fc = rtP .
donotdeletethisgain_Gain * rtB . osxfsl0cym [ 8 ] ; jgg3ixt0id = rtP .
Vref1_Value - rtB . hsyb5sy2fc ; rtB . djugjdenza = rtP .
PID_Controller_e_1_P * jgg3ixt0id ; } rtB . cgdfmnvyw5 = rtX . m411sasqn2 ;
if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { rtB . lcqe1riv5e = rtP . trigger_Value
; } if ( rtB . lcqe1riv5e > rtP . Switch_Threshold ) { rtB . g2ya2r5i0g = rtB
. djugjdenza + rtB . cgdfmnvyw5 ; rtB . fnw34rqsrd = rtB . g2ya2r5i0g ; }
else { rtB . fnw34rqsrd = rtP . Constant2_Value_ahwqlmii2b ; } if (
ssIsSampleHit ( rtS , 2 , 0 ) ) { rtB . ggnovrpbca = rtP .
donotdeletethisgain_Gain_czz01xtb3a * rtB . osxfsl0cym [ 9 ] ; rtB .
gfgi4gjdtc = ( ( muDoubleScalarRem ( ssGetTaskTime ( rtS , 2 ) + rtP .
Constant3_Value , rtP . Constant2_Value ) * rtP . uib1_Gain * 2.0 - 1.0 ) +
rtP . Constant_Value ) * rtP . Gain_Gain ; } rtB . ln23cmyrrk = rtB .
fnw34rqsrd - rtB . ggnovrpbca ; rtB . if2ke03cmg = rtX . ibkfpq1ykc ; rtB .
osvfaydsul = rtP . PID_Controller_i_1_P * rtB . ln23cmyrrk + rtB . if2ke03cmg
; if ( ssIsSampleHit ( rtS , 1 , 0 ) ) { if ( ssIsModeUpdateTimeStep ( rtS )
) { rtDW . dwdcdvhiev = ( rtB . osvfaydsul >= rtB . gfgi4gjdtc ) ; }
pevn3ircv2 = rtDW . dwdcdvhiev ; rtB . hvhda0al5i = rtDW . dwdcdvhiev ; } if
( ssIsSampleHit ( rtS , 2 , 0 ) ) { j2tthkygy1 = rtP . Vref2_Value - rtB .
hsyb5sy2fc ; rtB . bdj2lr3e5n = rtP . PID_Controller_e_2_P * j2tthkygy1 ; }
rtB . howhprf3lv = rtX . ilk5fcpmgd ; if ( rtB . lcqe1riv5e > rtP .
Switch1_Threshold ) { rtB . gf4nlt3vfv = rtB . bdj2lr3e5n + rtB . howhprf3lv
; rtB . aoadtzkehg = rtB . gf4nlt3vfv ; } else { rtB . aoadtzkehg = rtP .
Constant2_Value_ahwqlmii2b ; } if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { rtB .
ejfa4qebmu = rtP . donotdeletethisgain_Gain_o0wxvljlan * rtB . osxfsl0cym [
10 ] ; rtB . ofeouq3efa = ( ( muDoubleScalarRem ( ssGetTaskTime ( rtS , 2 ) +
rtP . Constant3_Value_hw4zakmrfm , rtP . Constant2_Value_lsteepdquw ) * rtP .
uib1_Gain_bsvwuaxwpi * 2.0 - 1.0 ) + rtP . Constant1_Value ) * rtP .
Gain1_Gain ; } rtB . jjy21yotzy = rtB . aoadtzkehg - rtB . ejfa4qebmu ; rtB .
gmw5rkyhks = rtX . ilikplwy3g ; rtB . hqifyhyn5g = rtP . PID_Controller_i_2_P
* rtB . jjy21yotzy + rtB . gmw5rkyhks ; if ( ssIsSampleHit ( rtS , 1 , 0 ) )
{ if ( ssIsModeUpdateTimeStep ( rtS ) ) { rtDW . p0c3lv2aj4 = ( rtB .
hqifyhyn5g >= rtB . ofeouq3efa ) ; } rtB . c0fpagk2j4 = rtDW . p0c3lv2aj4 ;
rtB . ha3c1iodke = ! pevn3ircv2 ; rtB . ehbg2lyldn = ! rtDW . p0c3lv2aj4 ; }
if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { } { if ( rtDW . mrhxip3nk1 . AQHandles
&& ssGetLogOutput ( rtS ) ) { sdiWriteSignal ( rtDW . mrhxip3nk1 . AQHandles
, ssGetTaskTime ( rtS , 0 ) , ( char * ) & rtB . osvfaydsul + 0 ) ; } } if (
ssIsSampleHit ( rtS , 2 , 0 ) ) { rtB . i0iuxica0n = rtP .
PID_Controller_e_1_I * jgg3ixt0id ; rtB . bdsk5vrvux = rtP .
PID_Controller_e_2_I * j2tthkygy1 ; } rtB . pclhdfxm1d = rtP .
PID_Controller_i_1_I * rtB . ln23cmyrrk ; rtB . gb2fvfaqkx = rtP .
PID_Controller_i_2_I * rtB . jjy21yotzy ; UNUSED_PARAMETER ( tid ) ; } void
MdlOutputsTID3 ( int_T tid ) { UNUSED_PARAMETER ( tid ) ; } void MdlUpdate (
int_T tid ) { if ( ssIsSampleHit ( rtS , 2 , 0 ) ) { { const real_T * As = (
real_T * ) rtDW . oi02hmlpyi . AS ; const real_T * Bs = ( real_T * ) rtDW .
oi02hmlpyi . BS ; real_T * xtmp = ( real_T * ) rtDW . oi02hmlpyi . XTMP ;
real_T accum ; accum = 0.0 ; accum += * ( As ++ ) * rtDW . gwi4vt3wpl [ 0 ] ;
accum += * ( As ++ ) * rtDW . gwi4vt3wpl [ 1 ] ; accum += * ( As ++ ) * rtDW
. gwi4vt3wpl [ 2 ] ; accum += * ( As ++ ) * rtDW . gwi4vt3wpl [ 3 ] ; { int_T
i1 ; const real_T * u0 = & rtP . SwitchCurrents_Value [ 0 ] ; for ( i1 = 0 ;
i1 < 8 ; i1 ++ ) { accum += * ( Bs ++ ) * u0 [ i1 ] ; } accum += * ( Bs ++ )
* rtP . DC_Amplitude ; } xtmp [ 0 ] = accum ; accum = 0.0 ; accum += * ( As
++ ) * rtDW . gwi4vt3wpl [ 0 ] ; accum += * ( As ++ ) * rtDW . gwi4vt3wpl [ 1
] ; accum += * ( As ++ ) * rtDW . gwi4vt3wpl [ 2 ] ; accum += * ( As ++ ) *
rtDW . gwi4vt3wpl [ 3 ] ; { int_T i1 ; const real_T * u0 = & rtP .
SwitchCurrents_Value [ 0 ] ; for ( i1 = 0 ; i1 < 8 ; i1 ++ ) { accum += * (
Bs ++ ) * u0 [ i1 ] ; } accum += * ( Bs ++ ) * rtP . DC_Amplitude ; } xtmp [
1 ] = accum ; accum = 0.0 ; accum += * ( As ++ ) * rtDW . gwi4vt3wpl [ 0 ] ;
accum += * ( As ++ ) * rtDW . gwi4vt3wpl [ 1 ] ; accum += * ( As ++ ) * rtDW
. gwi4vt3wpl [ 2 ] ; accum += * ( As ++ ) * rtDW . gwi4vt3wpl [ 3 ] ; { int_T
i1 ; const real_T * u0 = & rtP . SwitchCurrents_Value [ 0 ] ; for ( i1 = 0 ;
i1 < 8 ; i1 ++ ) { accum += * ( Bs ++ ) * u0 [ i1 ] ; } accum += * ( Bs ++ )
* rtP . DC_Amplitude ; } xtmp [ 2 ] = accum ; accum = 0.0 ; accum += * ( As
++ ) * rtDW . gwi4vt3wpl [ 0 ] ; accum += * ( As ++ ) * rtDW . gwi4vt3wpl [ 1
] ; accum += * ( As ++ ) * rtDW . gwi4vt3wpl [ 2 ] ; accum += * ( As ++ ) *
rtDW . gwi4vt3wpl [ 3 ] ; { int_T i1 ; const real_T * u0 = & rtP .
SwitchCurrents_Value [ 0 ] ; for ( i1 = 0 ; i1 < 8 ; i1 ++ ) { accum += * (
Bs ++ ) * u0 [ i1 ] ; } accum += * ( Bs ++ ) * rtP . DC_Amplitude ; } xtmp [
3 ] = accum ; rtDW . gwi4vt3wpl [ 0 ] = xtmp [ 0 ] ; rtDW . gwi4vt3wpl [ 1 ]
= xtmp [ 1 ] ; rtDW . gwi4vt3wpl [ 2 ] = xtmp [ 2 ] ; rtDW . gwi4vt3wpl [ 3 ]
= xtmp [ 3 ] ; { int_T * gState = ( int_T * ) rtDW . oi02hmlpyi . G_STATE ; *
( gState ++ ) = ( int_T ) rtB . hvhda0al5i ; * ( gState ++ ) = ( int_T ) rtB
. c0fpagk2j4 ; * ( gState ++ ) = ( int_T ) rtB . ha3c1iodke ; * ( gState ++ )
= ( int_T ) rtB . ehbg2lyldn ; * ( gState ++ ) = ( int_T ) 0.0 ; * ( gState
++ ) = ( int_T ) 0.0 ; * ( gState ++ ) = ( int_T ) 0.0 ; * ( gState ++ ) = (
int_T ) 0.0 ; } } } UNUSED_PARAMETER ( tid ) ; } void MdlUpdateTID3 ( int_T
tid ) { UNUSED_PARAMETER ( tid ) ; } void MdlDerivatives ( void ) { XDot *
_rtXdot ; _rtXdot = ( ( XDot * ) ssGetdX ( rtS ) ) ; _rtXdot -> m411sasqn2 =
rtB . i0iuxica0n ; _rtXdot -> ibkfpq1ykc = rtB . pclhdfxm1d ; _rtXdot ->
ilk5fcpmgd = rtB . bdsk5vrvux ; _rtXdot -> ilikplwy3g = rtB . gb2fvfaqkx ; }
void MdlProjection ( void ) { } void MdlZeroCrossings ( void ) { ZCV *
_rtZCSV ; _rtZCSV = ( ( ZCV * ) ssGetSolverZcSignalVector ( rtS ) ) ; _rtZCSV
-> ditqguoxg1 = rtB . osvfaydsul - rtB . gfgi4gjdtc ; _rtZCSV -> ngadrcokn2 =
rtB . hqifyhyn5g - rtB . ofeouq3efa ; } void MdlTerminate ( void ) { { free (
rtDW . oi02hmlpyi . AS ) ; free ( rtDW . oi02hmlpyi . BS ) ; free ( rtDW .
oi02hmlpyi . CS ) ; free ( rtDW . oi02hmlpyi . DS ) ; free ( rtDW .
oi02hmlpyi . DX_COL ) ; free ( rtDW . oi02hmlpyi . TMP2 ) ; free ( rtDW .
oi02hmlpyi . BD_COL ) ; free ( rtDW . oi02hmlpyi . TMP1 ) ; free ( rtDW .
oi02hmlpyi . XTMP ) ; free ( rtDW . oi02hmlpyi . G_STATE ) ; free ( rtDW .
oi02hmlpyi . SWITCH_STATUS ) ; free ( rtDW . oi02hmlpyi . SW_CHG ) ; free (
rtDW . oi02hmlpyi . SWITCH_STATUS_INIT ) ; } { if ( rtDW . mrhxip3nk1 .
AQHandles ) { sdiTerminateStreaming ( & rtDW . mrhxip3nk1 . AQHandles ) ; } }
} static void mr_ph2_interleaved_BuckVi48_cacheDataAsMxArray ( mxArray *
destArray , mwIndex i , int j , const void * srcData , size_t numBytes ) ;
static void mr_ph2_interleaved_BuckVi48_cacheDataAsMxArray ( mxArray *
destArray , mwIndex i , int j , const void * srcData , size_t numBytes ) {
mxArray * newArray = mxCreateUninitNumericMatrix ( ( size_t ) 1 , numBytes ,
mxUINT8_CLASS , mxREAL ) ; memcpy ( ( uint8_T * ) mxGetData ( newArray ) , (
const uint8_T * ) srcData , numBytes ) ; mxSetFieldByNumber ( destArray , i ,
j , newArray ) ; } static void
mr_ph2_interleaved_BuckVi48_restoreDataFromMxArray ( void * destData , const
mxArray * srcArray , mwIndex i , int j , size_t numBytes ) ; static void
mr_ph2_interleaved_BuckVi48_restoreDataFromMxArray ( void * destData , const
mxArray * srcArray , mwIndex i , int j , size_t numBytes ) { memcpy ( (
uint8_T * ) destData , ( const uint8_T * ) mxGetData ( mxGetFieldByNumber (
srcArray , i , j ) ) , numBytes ) ; } static void
mr_ph2_interleaved_BuckVi48_cacheBitFieldToMxArray ( mxArray * destArray ,
mwIndex i , int j , uint_T bitVal ) ; static void
mr_ph2_interleaved_BuckVi48_cacheBitFieldToMxArray ( mxArray * destArray ,
mwIndex i , int j , uint_T bitVal ) { mxSetFieldByNumber ( destArray , i , j
, mxCreateDoubleScalar ( ( real_T ) bitVal ) ) ; } static uint_T
mr_ph2_interleaved_BuckVi48_extractBitFieldFromMxArray ( const mxArray *
srcArray , mwIndex i , int j , uint_T numBits ) ; static uint_T
mr_ph2_interleaved_BuckVi48_extractBitFieldFromMxArray ( const mxArray *
srcArray , mwIndex i , int j , uint_T numBits ) { const uint_T varVal = (
uint_T ) mxGetScalar ( mxGetFieldByNumber ( srcArray , i , j ) ) ; return
varVal & ( ( 1u << numBits ) - 1u ) ; } static void
mr_ph2_interleaved_BuckVi48_cacheDataToMxArrayWithOffset ( mxArray *
destArray , mwIndex i , int j , mwIndex offset , const void * srcData ,
size_t numBytes ) ; static void
mr_ph2_interleaved_BuckVi48_cacheDataToMxArrayWithOffset ( mxArray *
destArray , mwIndex i , int j , mwIndex offset , const void * srcData ,
size_t numBytes ) { uint8_T * varData = ( uint8_T * ) mxGetData (
mxGetFieldByNumber ( destArray , i , j ) ) ; memcpy ( ( uint8_T * ) & varData
[ offset * numBytes ] , ( const uint8_T * ) srcData , numBytes ) ; } static
void mr_ph2_interleaved_BuckVi48_restoreDataFromMxArrayWithOffset ( void *
destData , const mxArray * srcArray , mwIndex i , int j , mwIndex offset ,
size_t numBytes ) ; static void
mr_ph2_interleaved_BuckVi48_restoreDataFromMxArrayWithOffset ( void *
destData , const mxArray * srcArray , mwIndex i , int j , mwIndex offset ,
size_t numBytes ) { const uint8_T * varData = ( const uint8_T * ) mxGetData (
mxGetFieldByNumber ( srcArray , i , j ) ) ; memcpy ( ( uint8_T * ) destData ,
( const uint8_T * ) & varData [ offset * numBytes ] , numBytes ) ; } static
void mr_ph2_interleaved_BuckVi48_cacheBitFieldToCellArrayWithOffset ( mxArray
* destArray , mwIndex i , int j , mwIndex offset , uint_T fieldVal ) ; static
void mr_ph2_interleaved_BuckVi48_cacheBitFieldToCellArrayWithOffset ( mxArray
* destArray , mwIndex i , int j , mwIndex offset , uint_T fieldVal ) {
mxSetCell ( mxGetFieldByNumber ( destArray , i , j ) , offset ,
mxCreateDoubleScalar ( ( real_T ) fieldVal ) ) ; } static uint_T
mr_ph2_interleaved_BuckVi48_extractBitFieldFromCellArrayWithOffset ( const
mxArray * srcArray , mwIndex i , int j , mwIndex offset , uint_T numBits ) ;
static uint_T
mr_ph2_interleaved_BuckVi48_extractBitFieldFromCellArrayWithOffset ( const
mxArray * srcArray , mwIndex i , int j , mwIndex offset , uint_T numBits ) {
const uint_T fieldVal = ( uint_T ) mxGetScalar ( mxGetCell (
mxGetFieldByNumber ( srcArray , i , j ) , offset ) ) ; return fieldVal & ( (
1u << numBits ) - 1u ) ; } mxArray * mr_ph2_interleaved_BuckVi48_GetDWork ( )
{ static const char_T * ssDWFieldNames [ 3 ] = { "rtB" , "rtDW" ,
"NULL_PrevZCX" , } ; mxArray * ssDW = mxCreateStructMatrix ( 1 , 1 , 3 ,
ssDWFieldNames ) ; mr_ph2_interleaved_BuckVi48_cacheDataAsMxArray ( ssDW , 0
, 0 , ( const void * ) & ( rtB ) , sizeof ( rtB ) ) ; { static const char_T *
rtdwDataFieldNames [ 4 ] = { "rtDW.gwi4vt3wpl" , "rtDW.ehenhsl3i5" ,
"rtDW.dwdcdvhiev" , "rtDW.p0c3lv2aj4" , } ; mxArray * rtdwData =
mxCreateStructMatrix ( 1 , 1 , 4 , rtdwDataFieldNames ) ;
mr_ph2_interleaved_BuckVi48_cacheDataAsMxArray ( rtdwData , 0 , 0 , ( const
void * ) & ( rtDW . gwi4vt3wpl ) , sizeof ( rtDW . gwi4vt3wpl ) ) ;
mr_ph2_interleaved_BuckVi48_cacheDataAsMxArray ( rtdwData , 0 , 1 , ( const
void * ) & ( rtDW . ehenhsl3i5 ) , sizeof ( rtDW . ehenhsl3i5 ) ) ;
mr_ph2_interleaved_BuckVi48_cacheDataAsMxArray ( rtdwData , 0 , 2 , ( const
void * ) & ( rtDW . dwdcdvhiev ) , sizeof ( rtDW . dwdcdvhiev ) ) ;
mr_ph2_interleaved_BuckVi48_cacheDataAsMxArray ( rtdwData , 0 , 3 , ( const
void * ) & ( rtDW . p0c3lv2aj4 ) , sizeof ( rtDW . p0c3lv2aj4 ) ) ;
mxSetFieldByNumber ( ssDW , 0 , 1 , rtdwData ) ; } return ssDW ; } void
mr_ph2_interleaved_BuckVi48_SetDWork ( const mxArray * ssDW ) { ( void ) ssDW
; mr_ph2_interleaved_BuckVi48_restoreDataFromMxArray ( ( void * ) & ( rtB ) ,
ssDW , 0 , 0 , sizeof ( rtB ) ) ; { const mxArray * rtdwData =
mxGetFieldByNumber ( ssDW , 0 , 1 ) ;
mr_ph2_interleaved_BuckVi48_restoreDataFromMxArray ( ( void * ) & ( rtDW .
gwi4vt3wpl ) , rtdwData , 0 , 0 , sizeof ( rtDW . gwi4vt3wpl ) ) ;
mr_ph2_interleaved_BuckVi48_restoreDataFromMxArray ( ( void * ) & ( rtDW .
ehenhsl3i5 ) , rtdwData , 0 , 1 , sizeof ( rtDW . ehenhsl3i5 ) ) ;
mr_ph2_interleaved_BuckVi48_restoreDataFromMxArray ( ( void * ) & ( rtDW .
dwdcdvhiev ) , rtdwData , 0 , 2 , sizeof ( rtDW . dwdcdvhiev ) ) ;
mr_ph2_interleaved_BuckVi48_restoreDataFromMxArray ( ( void * ) & ( rtDW .
p0c3lv2aj4 ) , rtdwData , 0 , 3 , sizeof ( rtDW . p0c3lv2aj4 ) ) ; } }
mxArray * mr_ph2_interleaved_BuckVi48_GetSimStateDisallowedBlocks ( ) {
mxArray * data = mxCreateCellMatrix ( 7 , 3 ) ; mwIndex subs [ 2 ] , offset ;
{ static const char_T * blockType [ 7 ] = { "S-Function" , "Scope" , "Scope"
, "Scope" , "Scope" , "Scope" , "S-Function" , } ; static const char_T *
blockPath [ 7 ] = {
"ph2_interleaved_BuckVi48/powergui/EquivalentModel1/State-Space" ,
"ph2_interleaved_BuckVi48/Scope" , "ph2_interleaved_BuckVi48/Scope1" ,
"ph2_interleaved_BuckVi48/Scope2" , "ph2_interleaved_BuckVi48/Scope3" ,
"ph2_interleaved_BuckVi48/Scope4" ,
"ph2_interleaved_BuckVi48/powergui/EquivalentModel1/State-Space" , } ; static
const int reason [ 7 ] = { 0 , 0 , 0 , 0 , 0 , 0 , 2 , } ; for ( subs [ 0 ] =
0 ; subs [ 0 ] < 7 ; ++ ( subs [ 0 ] ) ) { subs [ 1 ] = 0 ; offset =
mxCalcSingleSubscript ( data , 2 , subs ) ; mxSetCell ( data , offset ,
mxCreateString ( blockType [ subs [ 0 ] ] ) ) ; subs [ 1 ] = 1 ; offset =
mxCalcSingleSubscript ( data , 2 , subs ) ; mxSetCell ( data , offset ,
mxCreateString ( blockPath [ subs [ 0 ] ] ) ) ; subs [ 1 ] = 2 ; offset =
mxCalcSingleSubscript ( data , 2 , subs ) ; mxSetCell ( data , offset ,
mxCreateDoubleScalar ( ( real_T ) reason [ subs [ 0 ] ] ) ) ; } } return data
; } void MdlInitializeSizes ( void ) { ssSetNumContStates ( rtS , 4 ) ;
ssSetNumPeriodicContStates ( rtS , 0 ) ; ssSetNumY ( rtS , 0 ) ; ssSetNumU (
rtS , 0 ) ; ssSetDirectFeedThrough ( rtS , 0 ) ; ssSetNumSampleTimes ( rtS ,
3 ) ; ssSetNumBlocks ( rtS , 122 ) ; ssSetNumBlockIO ( rtS , 30 ) ;
ssSetNumBlockParams ( rtS , 239 ) ; } void MdlInitializeSampleTimes ( void )
{ ssSetSampleTime ( rtS , 0 , 0.0 ) ; ssSetSampleTime ( rtS , 1 , 0.0 ) ;
ssSetSampleTime ( rtS , 2 , 1.0E-7 ) ; ssSetOffsetTime ( rtS , 0 , 0.0 ) ;
ssSetOffsetTime ( rtS , 1 , 1.0 ) ; ssSetOffsetTime ( rtS , 2 , 0.0 ) ; }
void raccel_set_checksum ( ) { ssSetChecksumVal ( rtS , 0 , 768363794U ) ;
ssSetChecksumVal ( rtS , 1 , 1091937133U ) ; ssSetChecksumVal ( rtS , 2 ,
483167757U ) ; ssSetChecksumVal ( rtS , 3 , 2605760807U ) ; }
#if defined(_MSC_VER)
#pragma optimize( "", off )
#endif
SimStruct * raccel_register_model ( ssExecutionInfo * executionInfo ) {
static struct _ssMdlInfo mdlInfo ; static struct _ssBlkInfo2 blkInfo2 ;
static struct _ssBlkInfoSLSize blkInfoSLSize ; rt_modelMapInfoPtr = & (
rt_dataMapInfo . mmi ) ; executionInfo -> gblObjects_ . numToFiles = 0 ;
executionInfo -> gblObjects_ . numFrFiles = 0 ; executionInfo -> gblObjects_
. numFrWksBlocks = 0 ; executionInfo -> gblObjects_ . numModelInputs = 0 ;
executionInfo -> gblObjects_ . numRootInportBlks = 0 ; executionInfo ->
gblObjects_ . inportDataTypeIdx = NULL ; executionInfo -> gblObjects_ .
inportDims = NULL ; executionInfo -> gblObjects_ . inportComplex = NULL ;
executionInfo -> gblObjects_ . inportInterpoFlag = NULL ; executionInfo ->
gblObjects_ . inportContinuous = NULL ; ( void ) memset ( ( char_T * ) rtS ,
0 , sizeof ( SimStruct ) ) ; ( void ) memset ( ( char_T * ) & mdlInfo , 0 ,
sizeof ( struct _ssMdlInfo ) ) ; ( void ) memset ( ( char_T * ) & blkInfo2 ,
0 , sizeof ( struct _ssBlkInfo2 ) ) ; ( void ) memset ( ( char_T * ) &
blkInfoSLSize , 0 , sizeof ( struct _ssBlkInfoSLSize ) ) ; ssSetBlkInfo2Ptr (
rtS , & blkInfo2 ) ; ssSetBlkInfoSLSizePtr ( rtS , & blkInfoSLSize ) ;
ssSetMdlInfoPtr ( rtS , & mdlInfo ) ; ssSetExecutionInfo ( rtS ,
executionInfo ) ; slsaAllocOPModelData ( rtS ) ; { static time_T mdlPeriod [
NSAMPLE_TIMES ] ; static time_T mdlOffset [ NSAMPLE_TIMES ] ; static time_T
mdlTaskTimes [ NSAMPLE_TIMES ] ; static int_T mdlTsMap [ NSAMPLE_TIMES ] ;
static int_T mdlSampleHits [ NSAMPLE_TIMES ] ; static boolean_T
mdlTNextWasAdjustedPtr [ NSAMPLE_TIMES ] ; static int_T mdlPerTaskSampleHits
[ NSAMPLE_TIMES * NSAMPLE_TIMES ] ; static time_T mdlTimeOfNextSampleHit [
NSAMPLE_TIMES ] ; { int_T i ; for ( i = 0 ; i < NSAMPLE_TIMES ; i ++ ) {
mdlPeriod [ i ] = 0.0 ; mdlOffset [ i ] = 0.0 ; mdlTaskTimes [ i ] = 0.0 ;
mdlTsMap [ i ] = i ; mdlSampleHits [ i ] = 1 ; } } ssSetSampleTimePtr ( rtS ,
& mdlPeriod [ 0 ] ) ; ssSetOffsetTimePtr ( rtS , & mdlOffset [ 0 ] ) ;
ssSetSampleTimeTaskIDPtr ( rtS , & mdlTsMap [ 0 ] ) ; ssSetTPtr ( rtS , &
mdlTaskTimes [ 0 ] ) ; ssSetSampleHitPtr ( rtS , & mdlSampleHits [ 0 ] ) ;
ssSetTNextWasAdjustedPtr ( rtS , & mdlTNextWasAdjustedPtr [ 0 ] ) ;
ssSetPerTaskSampleHitsPtr ( rtS , & mdlPerTaskSampleHits [ 0 ] ) ;
ssSetTimeOfNextSampleHitPtr ( rtS , & mdlTimeOfNextSampleHit [ 0 ] ) ; }
ssSetSolverMode ( rtS , SOLVER_MODE_SINGLETASKING ) ; { ssSetBlockIO ( rtS ,
( ( void * ) & rtB ) ) ; ( void ) memset ( ( ( void * ) & rtB ) , 0 , sizeof
( B ) ) ; } { real_T * x = ( real_T * ) & rtX ; ssSetContStates ( rtS , x ) ;
( void ) memset ( ( void * ) x , 0 , sizeof ( X ) ) ; } { void * dwork = (
void * ) & rtDW ; ssSetRootDWork ( rtS , dwork ) ; ( void ) memset ( dwork ,
0 , sizeof ( DW ) ) ; } { static DataTypeTransInfo dtInfo ; ( void ) memset (
( char_T * ) & dtInfo , 0 , sizeof ( dtInfo ) ) ; ssSetModelMappingInfo ( rtS
, & dtInfo ) ; dtInfo . numDataTypes = 26 ; dtInfo . dataTypeSizes = &
rtDataTypeSizes [ 0 ] ; dtInfo . dataTypeNames = & rtDataTypeNames [ 0 ] ;
dtInfo . BTransTable = & rtBTransTable ; dtInfo . PTransTable = &
rtPTransTable ; dtInfo . dataTypeInfoTable = rtDataTypeInfoTable ; }
ph2_interleaved_BuckVi48_InitializeDataMapInfo ( ) ;
ssSetIsRapidAcceleratorActive ( rtS , true ) ; ssSetRootSS ( rtS , rtS ) ;
ssSetVersion ( rtS , SIMSTRUCT_VERSION_LEVEL2 ) ; ssSetModelName ( rtS ,
"ph2_interleaved_BuckVi48" ) ; ssSetPath ( rtS , "ph2_interleaved_BuckVi48" )
; ssSetTStart ( rtS , 0.0 ) ; ssSetTFinal ( rtS , 1.0 ) ; { static RTWLogInfo
rt_DataLoggingInfo ; rt_DataLoggingInfo . loggingInterval = ( NULL ) ;
ssSetRTWLogInfo ( rtS , & rt_DataLoggingInfo ) ; } { { static int_T
rt_LoggedStateWidths [ ] = { 1 , 1 , 1 , 1 , 4 } ; static int_T
rt_LoggedStateNumDimensions [ ] = { 1 , 1 , 1 , 1 , 1 } ; static int_T
rt_LoggedStateDimensions [ ] = { 1 , 1 , 1 , 1 , 4 } ; static boolean_T
rt_LoggedStateIsVarDims [ ] = { 0 , 0 , 0 , 0 , 0 } ; static BuiltInDTypeId
rt_LoggedStateDataTypeIds [ ] = { SS_DOUBLE , SS_DOUBLE , SS_DOUBLE ,
SS_DOUBLE , SS_DOUBLE } ; static int_T rt_LoggedStateComplexSignals [ ] = { 0
, 0 , 0 , 0 , 0 } ; static RTWPreprocessingFcnPtr
rt_LoggingStatePreprocessingFcnPtrs [ ] = { ( NULL ) , ( NULL ) , ( NULL ) ,
( NULL ) , ( NULL ) } ; static const char_T * rt_LoggedStateLabels [ ] = {
"CSTATE" , "CSTATE" , "CSTATE" , "CSTATE" , "DSTATE" } ; static const char_T
* rt_LoggedStateBlockNames [ ] = {
"ph2_interleaved_BuckVi48/PID_Controller_e_1/Integrator/Continuous/Integrator"
,
"ph2_interleaved_BuckVi48/PID_Controller_i_1/Integrator/Continuous/Integrator"
,
"ph2_interleaved_BuckVi48/PID_Controller_e_2/Integrator/Continuous/Integrator"
,
"ph2_interleaved_BuckVi48/PID_Controller_i_2/Integrator/Continuous/Integrator"
, "ph2_interleaved_BuckVi48/powergui/EquivalentModel1/State-Space" } ; static
const char_T * rt_LoggedStateNames [ ] = { "" , "" , "" , "" , "" } ; static
boolean_T rt_LoggedStateCrossMdlRef [ ] = { 0 , 0 , 0 , 0 , 0 } ; static
RTWLogDataTypeConvert rt_RTWLogDataTypeConvert [ ] = { { 0 , SS_DOUBLE ,
SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0
, 0 , 1.0 , 0 , 0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 ,
0.0 } , { 0 , SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } , { 0 ,
SS_DOUBLE , SS_DOUBLE , 0 , 0 , 0 , 1.0 , 0 , 0.0 } } ; static int_T
rt_LoggedStateIdxList [ ] = { 0 , 1 , 2 , 3 , 0 } ; static RTWLogSignalInfo
rt_LoggedStateSignalInfo = { 5 , rt_LoggedStateWidths ,
rt_LoggedStateNumDimensions , rt_LoggedStateDimensions ,
rt_LoggedStateIsVarDims , ( NULL ) , ( NULL ) , rt_LoggedStateDataTypeIds ,
rt_LoggedStateComplexSignals , ( NULL ) , rt_LoggingStatePreprocessingFcnPtrs
, { rt_LoggedStateLabels } , ( NULL ) , ( NULL ) , ( NULL ) , {
rt_LoggedStateBlockNames } , { rt_LoggedStateNames } ,
rt_LoggedStateCrossMdlRef , rt_RTWLogDataTypeConvert , rt_LoggedStateIdxList
} ; static void * rt_LoggedStateSignalPtrs [ 5 ] ; rtliSetLogXSignalPtrs (
ssGetRTWLogInfo ( rtS ) , ( LogSignalPtrsType ) rt_LoggedStateSignalPtrs ) ;
rtliSetLogXSignalInfo ( ssGetRTWLogInfo ( rtS ) , & rt_LoggedStateSignalInfo
) ; rt_LoggedStateSignalPtrs [ 0 ] = ( void * ) & rtX . m411sasqn2 ;
rt_LoggedStateSignalPtrs [ 1 ] = ( void * ) & rtX . ibkfpq1ykc ;
rt_LoggedStateSignalPtrs [ 2 ] = ( void * ) & rtX . ilk5fcpmgd ;
rt_LoggedStateSignalPtrs [ 3 ] = ( void * ) & rtX . ilikplwy3g ;
rt_LoggedStateSignalPtrs [ 4 ] = ( void * ) rtDW . gwi4vt3wpl ; } rtliSetLogT
( ssGetRTWLogInfo ( rtS ) , "tmp_raccel_tout" ) ; rtliSetLogX (
ssGetRTWLogInfo ( rtS ) , "tmp_raccel_xout" ) ; rtliSetLogXFinal (
ssGetRTWLogInfo ( rtS ) , "xFinal" ) ; rtliSetLogVarNameModifier (
ssGetRTWLogInfo ( rtS ) , "none" ) ; rtliSetLogFormat ( ssGetRTWLogInfo ( rtS
) , 2 ) ; rtliSetLogMaxRows ( ssGetRTWLogInfo ( rtS ) , 0 ) ;
rtliSetLogDecimation ( ssGetRTWLogInfo ( rtS ) , 1 ) ; rtliSetLogY (
ssGetRTWLogInfo ( rtS ) , "" ) ; rtliSetLogYSignalInfo ( ssGetRTWLogInfo (
rtS ) , ( NULL ) ) ; rtliSetLogYSignalPtrs ( ssGetRTWLogInfo ( rtS ) , ( NULL
) ) ; } { static struct _ssStatesInfo2 statesInfo2 ; ssSetStatesInfo2 ( rtS ,
& statesInfo2 ) ; } { static ssPeriodicStatesInfo periodicStatesInfo ;
ssSetPeriodicStatesInfo ( rtS , & periodicStatesInfo ) ; } { static
ssJacobianPerturbationBounds jacobianPerturbationBounds ;
ssSetJacobianPerturbationBounds ( rtS , & jacobianPerturbationBounds ) ; } {
static ssSolverInfo slvrInfo ; static boolean_T contStatesDisabled [ 4 ] ;
static real_T absTol [ 4 ] = { 1.0E-6 , 1.0E-6 , 1.0E-6 , 1.0E-6 } ; static
uint8_T absTolControl [ 4 ] = { 0U , 0U , 0U , 0U } ; static real_T
contStateJacPerturbBoundMinVec [ 4 ] ; static real_T
contStateJacPerturbBoundMaxVec [ 4 ] ; static uint8_T zcAttributes [ 2 ] = {
( ZC_EVENT_ALL ) , ( ZC_EVENT_ALL ) } ; static ssNonContDerivSigInfo
nonContDerivSigInfo [ 6 ] = { { 1 * sizeof ( real_T ) , ( char * ) ( & rtB .
bdsk5vrvux ) , ( NULL ) } , { 1 * sizeof ( real_T ) , ( char * ) ( & rtB .
i0iuxica0n ) , ( NULL ) } , { 1 * sizeof ( real_T ) , ( char * ) ( & rtB .
ejfa4qebmu ) , ( NULL ) } , { 1 * sizeof ( real_T ) , ( char * ) ( & rtB .
bdj2lr3e5n ) , ( NULL ) } , { 1 * sizeof ( real_T ) , ( char * ) ( & rtB .
ggnovrpbca ) , ( NULL ) } , { 1 * sizeof ( real_T ) , ( char * ) ( & rtB .
djugjdenza ) , ( NULL ) } } ; { int i ; for ( i = 0 ; i < 4 ; ++ i ) {
contStateJacPerturbBoundMinVec [ i ] = 0 ; contStateJacPerturbBoundMaxVec [ i
] = rtGetInf ( ) ; } } ssSetSolverRelTol ( rtS , 0.001 ) ; ssSetStepSize (
rtS , 0.0 ) ; ssSetMinStepSize ( rtS , 0.0 ) ; ssSetMaxNumMinSteps ( rtS , -
1 ) ; ssSetMinStepViolatedError ( rtS , 0 ) ; ssSetMaxStepSize ( rtS , 1.0E-7
) ; ssSetSolverMaxOrder ( rtS , - 1 ) ; ssSetSolverRefineFactor ( rtS , 1 ) ;
ssSetOutputTimes ( rtS , ( NULL ) ) ; ssSetNumOutputTimes ( rtS , 0 ) ;
ssSetOutputTimesOnly ( rtS , 0 ) ; ssSetOutputTimesIndex ( rtS , 0 ) ;
ssSetZCCacheNeedsReset ( rtS , 1 ) ; ssSetDerivCacheNeedsReset ( rtS , 0 ) ;
ssSetNumNonContDerivSigInfos ( rtS , 6 ) ; ssSetNonContDerivSigInfos ( rtS ,
nonContDerivSigInfo ) ; ssSetSolverInfo ( rtS , & slvrInfo ) ;
ssSetSolverName ( rtS , "VariableStepAuto" ) ; ssSetVariableStepSolver ( rtS
, 1 ) ; ssSetSolverConsistencyChecking ( rtS , 0 ) ;
ssSetSolverAdaptiveZcDetection ( rtS , 0 ) ; ssSetSolverRobustResetMethod (
rtS , 0 ) ; ssSetAbsTolVector ( rtS , absTol ) ; ssSetAbsTolControlVector (
rtS , absTolControl ) ; ssSetSolverAbsTol_Obsolete ( rtS , absTol ) ;
ssSetSolverAbsTolControl_Obsolete ( rtS , absTolControl ) ;
ssSetJacobianPerturbationBoundsMinVec ( rtS , contStateJacPerturbBoundMinVec
) ; ssSetJacobianPerturbationBoundsMaxVec ( rtS ,
contStateJacPerturbBoundMaxVec ) ; ssSetSolverStateProjection ( rtS , 0 ) ;
ssSetSolverMassMatrixType ( rtS , ( ssMatrixType ) 0 ) ;
ssSetSolverMassMatrixNzMax ( rtS , 0 ) ; ssSetModelOutputs ( rtS , MdlOutputs
) ; ssSetModelUpdate ( rtS , MdlUpdate ) ; ssSetModelDerivatives ( rtS ,
MdlDerivatives ) ; ssSetSolverZcSignalAttrib ( rtS , zcAttributes ) ;
ssSetSolverNumZcSignals ( rtS , 2 ) ; ssSetModelZeroCrossings ( rtS ,
MdlZeroCrossings ) ; ssSetSolverConsecutiveZCsStepRelTol ( rtS ,
2.8421709430404007E-13 ) ; ssSetSolverMaxConsecutiveZCs ( rtS , 1000 ) ;
ssSetSolverConsecutiveZCsError ( rtS , 2 ) ; ssSetSolverMaskedZcDiagnostic (
rtS , 1 ) ; ssSetSolverIgnoredZcDiagnostic ( rtS , 1 ) ;
ssSetSolverMaxConsecutiveMinStep ( rtS , 1 ) ;
ssSetSolverShapePreserveControl ( rtS , 2 ) ; ssSetTNextTid ( rtS , INT_MIN )
; ssSetTNext ( rtS , rtMinusInf ) ; ssSetSolverNeedsReset ( rtS ) ;
ssSetNumNonsampledZCs ( rtS , 2 ) ; ssSetContStateDisabled ( rtS ,
contStatesDisabled ) ; ssSetSolverMaxConsecutiveMinStep ( rtS , 1 ) ; }
ssSetChecksumVal ( rtS , 0 , 768363794U ) ; ssSetChecksumVal ( rtS , 1 ,
1091937133U ) ; ssSetChecksumVal ( rtS , 2 , 483167757U ) ; ssSetChecksumVal
( rtS , 3 , 2605760807U ) ; { static const sysRanDType rtAlwaysEnabled =
SUBSYS_RAN_BC_ENABLE ; static RTWExtModeInfo rt_ExtModeInfo ; static const
sysRanDType * systemRan [ 3 ] ; gblRTWExtModeInfo = & rt_ExtModeInfo ;
ssSetRTWExtModeInfo ( rtS , & rt_ExtModeInfo ) ;
rteiSetSubSystemActiveVectorAddresses ( & rt_ExtModeInfo , systemRan ) ;
systemRan [ 0 ] = & rtAlwaysEnabled ; systemRan [ 1 ] = & rtAlwaysEnabled ;
systemRan [ 2 ] = & rtAlwaysEnabled ; rteiSetModelMappingInfoPtr (
ssGetRTWExtModeInfo ( rtS ) , & ssGetModelMappingInfo ( rtS ) ) ;
rteiSetChecksumsPtr ( ssGetRTWExtModeInfo ( rtS ) , ssGetChecksums ( rtS ) )
; rteiSetTPtr ( ssGetRTWExtModeInfo ( rtS ) , ssGetTPtr ( rtS ) ) ; }
slsaDisallowedBlocksForSimTargetOP ( rtS ,
mr_ph2_interleaved_BuckVi48_GetSimStateDisallowedBlocks ) ;
slsaGetWorkFcnForSimTargetOP ( rtS , mr_ph2_interleaved_BuckVi48_GetDWork ) ;
slsaSetWorkFcnForSimTargetOP ( rtS , mr_ph2_interleaved_BuckVi48_SetDWork ) ;
rt_RapidReadMatFileAndUpdateParams ( rtS ) ; if ( ssGetErrorStatus ( rtS ) )
{ return rtS ; } executionInfo -> simulationOptions_ . stateSaveName_ =
rtliGetLogX ( ssGetRTWLogInfo ( rtS ) ) ; executionInfo -> simulationOptions_
. finalStateName_ = rtliGetLogXFinal ( ssGetRTWLogInfo ( rtS ) ) ;
executionInfo -> simulationOptions_ . outputSaveName_ = rtliGetLogY (
ssGetRTWLogInfo ( rtS ) ) ; return rtS ; }
#if defined(_MSC_VER)
#pragma optimize( "", on )
#endif
void MdlOutputsParameterSampleTime ( int_T tid ) { MdlOutputsTID3 ( tid ) ; }
