#include "rtw_capi.h"
#ifdef HOST_CAPI_BUILD
#include "ph2_interleaved_BuckVi48_capi_host.h"
#define sizeof(s) ((size_t)(0xFFFF))
#undef rt_offsetof
#define rt_offsetof(s,el) ((uint16_T)(0xFFFF))
#define TARGET_CONST
#define TARGET_STRING(s) (s)
#ifndef SS_UINT64
#define SS_UINT64 20
#endif
#ifndef SS_INT64
#define SS_INT64 21
#endif
#else
#include "builtin_typeid_types.h"
#include "ph2_interleaved_BuckVi48.h"
#include "ph2_interleaved_BuckVi48_capi.h"
#include "ph2_interleaved_BuckVi48_private.h"
#ifdef LIGHT_WEIGHT_CAPI
#define TARGET_CONST
#define TARGET_STRING(s)               ((NULL))
#else
#define TARGET_CONST                   const
#define TARGET_STRING(s)               (s)
#endif
#endif
static const rtwCAPI_Signals rtBlockSignals [ ] = { { 0 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/trigger" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 ,
0 } , { 1 , 0 , TARGET_STRING ( "ph2_interleaved_BuckVi48/Gain" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 2 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Gain1" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1
} , { 3 , 0 , TARGET_STRING ( "ph2_interleaved_BuckVi48/Add4" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 4 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Add6" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2
} , { 5 , 0 , TARGET_STRING ( "ph2_interleaved_BuckVi48/Switch" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 6 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Switch1" ) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 ,
2 } , { 7 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Current/do not delete this gain" ) , TARGET_STRING
( "" ) , 0 , 0 , 0 , 0 , 1 } , { 8 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Current1/do not delete this gain" ) , TARGET_STRING
( "" ) , 0 , 0 , 0 , 0 , 1 } , { 9 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Voltage/do not delete this gain" ) , TARGET_STRING
( "" ) , 0 , 0 , 0 , 0 , 1 } , { 10 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/powergui/EquivalentModel1/State-Space" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 1 , 0 , 1 } , { 11 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/powergui/EquivalentModel1/State-Space" ) ,
TARGET_STRING ( "" ) , 1 , 0 , 2 , 0 , 1 } , { 12 , 0 , TARGET_STRING (
 "ph2_interleaved_BuckVi48/PID_Controller_e_1/I Gain/Internal Parameters/Integral Gain"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 13 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_e_1/Integrator/Continuous/Integrator"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 14 , 0 , TARGET_STRING (
 "ph2_interleaved_BuckVi48/PID_Controller_e_1/Parallel P Gain/Internal Parameters/Proportional Gain"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 15 , 2 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_e_1/Sum/Sum_PI/Sum" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 16 , 0 , TARGET_STRING (
 "ph2_interleaved_BuckVi48/PID_Controller_e_2/I Gain/Internal Parameters/Integral Gain"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 17 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_e_2/Integrator/Continuous/Integrator"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 18 , 0 , TARGET_STRING (
 "ph2_interleaved_BuckVi48/PID_Controller_e_2/Parallel P Gain/Internal Parameters/Proportional Gain"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 1 } , { 19 , 1 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_e_2/Sum/Sum_PI/Sum" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 20 , 0 , TARGET_STRING (
 "ph2_interleaved_BuckVi48/PID_Controller_i_1/I Gain/Internal Parameters/Integral Gain"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 21 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_i_1/Integrator/Continuous/Integrator"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 22 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_i_1/Sum/Sum_PI/Sum" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 23 , 0 , TARGET_STRING (
 "ph2_interleaved_BuckVi48/PID_Controller_i_2/I Gain/Internal Parameters/Integral Gain"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 24 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_i_2/Integrator/Continuous/Integrator"
) , TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 25 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_i_2/Sum/Sum_PI/Sum" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 2 } , { 26 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/S1/Ideal Switch/Model/Data Type Conversion" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 27 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/S2/Ideal Switch/Model/Data Type Conversion" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 28 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/S3/Ideal Switch/Model/Data Type Conversion" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 29 , 0 , TARGET_STRING (
"ph2_interleaved_BuckVi48/S4/Ideal Switch/Model/Data Type Conversion" ) ,
TARGET_STRING ( "" ) , 0 , 0 , 0 , 0 , 0 } , { 0 , 0 , ( NULL ) , ( NULL ) ,
0 , 0 , 0 , 0 , 0 } } ; static const rtwCAPI_BlockParameters
rtBlockParameters [ ] = { { 30 , TARGET_STRING (
"ph2_interleaved_BuckVi48/DC" ) , TARGET_STRING ( "Amplitude" ) , 0 , 0 , 0 }
, { 31 , TARGET_STRING ( "ph2_interleaved_BuckVi48/PID_Controller_e_1" ) ,
TARGET_STRING ( "P" ) , 0 , 0 , 0 } , { 32 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_e_1" ) , TARGET_STRING ( "I" ) , 0 ,
0 , 0 } , { 33 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_e_1" ) , TARGET_STRING (
"InitialConditionForIntegrator" ) , 0 , 0 , 0 } , { 34 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_e_2" ) , TARGET_STRING ( "P" ) , 0 ,
0 , 0 } , { 35 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_e_2" ) , TARGET_STRING ( "I" ) , 0 ,
0 , 0 } , { 36 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_e_2" ) , TARGET_STRING (
"InitialConditionForIntegrator" ) , 0 , 0 , 0 } , { 37 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_i_1" ) , TARGET_STRING ( "P" ) , 0 ,
0 , 0 } , { 38 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_i_1" ) , TARGET_STRING ( "I" ) , 0 ,
0 , 0 } , { 39 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_i_1" ) , TARGET_STRING (
"InitialConditionForIntegrator" ) , 0 , 0 , 0 } , { 40 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_i_2" ) , TARGET_STRING ( "P" ) , 0 ,
0 , 0 } , { 41 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_i_2" ) , TARGET_STRING ( "I" ) , 0 ,
0 , 0 } , { 42 , TARGET_STRING (
"ph2_interleaved_BuckVi48/PID_Controller_i_2" ) , TARGET_STRING (
"InitialConditionForIntegrator" ) , 0 , 0 , 0 } , { 43 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Constant" ) , TARGET_STRING ( "Value" ) , 0 , 0 , 0
} , { 44 , TARGET_STRING ( "ph2_interleaved_BuckVi48/Constant1" ) ,
TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 45 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Constant2" ) , TARGET_STRING ( "Value" ) , 0 , 0 ,
0 } , { 46 , TARGET_STRING ( "ph2_interleaved_BuckVi48/Vref1" ) ,
TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 47 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Vref2" ) , TARGET_STRING ( "Value" ) , 0 , 0 , 0 }
, { 48 , TARGET_STRING ( "ph2_interleaved_BuckVi48/trigger" ) , TARGET_STRING
( "Value" ) , 0 , 0 , 0 } , { 49 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Gain" ) , TARGET_STRING ( "Gain" ) , 0 , 0 , 0 } ,
{ 50 , TARGET_STRING ( "ph2_interleaved_BuckVi48/Gain1" ) , TARGET_STRING (
"Gain" ) , 0 , 0 , 0 } , { 51 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Switch" ) , TARGET_STRING ( "Threshold" ) , 0 , 0 ,
0 } , { 52 , TARGET_STRING ( "ph2_interleaved_BuckVi48/Switch1" ) ,
TARGET_STRING ( "Threshold" ) , 0 , 0 , 0 } , { 53 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Current/do not delete this gain" ) , TARGET_STRING
( "Gain" ) , 0 , 0 , 0 } , { 54 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Current1/do not delete this gain" ) , TARGET_STRING
( "Gain" ) , 0 , 0 , 0 } , { 55 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Voltage/do not delete this gain" ) , TARGET_STRING
( "Gain" ) , 0 , 0 , 0 } , { 56 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Sawtooth Generator/Model/Constant2" ) ,
TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 57 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Sawtooth Generator/Model/Constant3" ) ,
TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 58 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Sawtooth Generator/Model/1\\ib1" ) , TARGET_STRING
( "Gain" ) , 0 , 0 , 0 } , { 59 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Sawtooth Generator1/Model/Constant2" ) ,
TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 60 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Sawtooth Generator1/Model/Constant3" ) ,
TARGET_STRING ( "Value" ) , 0 , 0 , 0 } , { 61 , TARGET_STRING (
"ph2_interleaved_BuckVi48/Sawtooth Generator1/Model/1\\ib1" ) , TARGET_STRING
( "Gain" ) , 0 , 0 , 0 } , { 62 , TARGET_STRING (
"ph2_interleaved_BuckVi48/powergui/EquivalentModel1/State-Space" ) ,
TARGET_STRING ( "AS_param" ) , 0 , 3 , 0 } , { 63 , TARGET_STRING (
"ph2_interleaved_BuckVi48/powergui/EquivalentModel1/State-Space" ) ,
TARGET_STRING ( "BS_param" ) , 0 , 4 , 0 } , { 64 , TARGET_STRING (
"ph2_interleaved_BuckVi48/powergui/EquivalentModel1/State-Space" ) ,
TARGET_STRING ( "CS_param" ) , 0 , 5 , 0 } , { 65 , TARGET_STRING (
"ph2_interleaved_BuckVi48/powergui/EquivalentModel1/State-Space" ) ,
TARGET_STRING ( "DS_param" ) , 0 , 6 , 0 } , { 66 , TARGET_STRING (
"ph2_interleaved_BuckVi48/powergui/EquivalentModel1/State-Space" ) ,
TARGET_STRING ( "X0_param" ) , 0 , 7 , 0 } , { 67 , TARGET_STRING (
"ph2_interleaved_BuckVi48/powergui/EquivalentModel1/Sources/SwitchCurrents" )
, TARGET_STRING ( "Value" ) , 0 , 2 , 0 } , { 0 , ( NULL ) , ( NULL ) , 0 , 0
, 0 } } ; static int_T rt_LoggedStateIdxList [ ] = { - 1 } ; static const
rtwCAPI_Signals rtRootInputs [ ] = { { 0 , 0 , ( NULL ) , ( NULL ) , 0 , 0 ,
0 , 0 , 0 } } ; static const rtwCAPI_Signals rtRootOutputs [ ] = { { 0 , 0 ,
( NULL ) , ( NULL ) , 0 , 0 , 0 , 0 , 0 } } ; static const
rtwCAPI_ModelParameters rtModelParameters [ ] = { { 0 , ( NULL ) , 0 , 0 , 0
} } ;
#ifndef HOST_CAPI_BUILD
static void * rtDataAddrMap [ ] = { & rtB . lcqe1riv5e , & rtB . gfgi4gjdtc ,
& rtB . ofeouq3efa , & rtB . ln23cmyrrk , & rtB . jjy21yotzy , & rtB .
fnw34rqsrd , & rtB . aoadtzkehg , & rtB . ggnovrpbca , & rtB . ejfa4qebmu , &
rtB . hsyb5sy2fc , & rtB . osxfsl0cym [ 0 ] , & rtB . k2dlruxx5u [ 0 ] , &
rtB . i0iuxica0n , & rtB . cgdfmnvyw5 , & rtB . djugjdenza , & rtB .
g2ya2r5i0g , & rtB . bdsk5vrvux , & rtB . howhprf3lv , & rtB . bdj2lr3e5n , &
rtB . gf4nlt3vfv , & rtB . pclhdfxm1d , & rtB . if2ke03cmg , & rtB .
osvfaydsul , & rtB . gb2fvfaqkx , & rtB . gmw5rkyhks , & rtB . hqifyhyn5g , &
rtB . hvhda0al5i , & rtB . c0fpagk2j4 , & rtB . ha3c1iodke , & rtB .
ehbg2lyldn , & rtP . DC_Amplitude , & rtP . PID_Controller_e_1_P , & rtP .
PID_Controller_e_1_I , & rtP .
PID_Controller_e_1_InitialConditionForIntegrator , & rtP .
PID_Controller_e_2_P , & rtP . PID_Controller_e_2_I , & rtP .
PID_Controller_e_2_InitialConditionForIntegrator , & rtP .
PID_Controller_i_1_P , & rtP . PID_Controller_i_1_I , & rtP .
PID_Controller_i_1_InitialConditionForIntegrator , & rtP .
PID_Controller_i_2_P , & rtP . PID_Controller_i_2_I , & rtP .
PID_Controller_i_2_InitialConditionForIntegrator , & rtP . Constant_Value , &
rtP . Constant1_Value , & rtP . Constant2_Value_ahwqlmii2b , & rtP .
Vref1_Value , & rtP . Vref2_Value , & rtP . trigger_Value , & rtP . Gain_Gain
, & rtP . Gain1_Gain , & rtP . Switch_Threshold , & rtP . Switch1_Threshold ,
& rtP . donotdeletethisgain_Gain_czz01xtb3a , & rtP .
donotdeletethisgain_Gain_o0wxvljlan , & rtP . donotdeletethisgain_Gain , &
rtP . Constant2_Value , & rtP . Constant3_Value , & rtP . uib1_Gain , & rtP .
Constant2_Value_lsteepdquw , & rtP . Constant3_Value_hw4zakmrfm , & rtP .
uib1_Gain_bsvwuaxwpi , & rtP . StateSpace_AS_param [ 0 ] , & rtP .
StateSpace_BS_param [ 0 ] , & rtP . StateSpace_CS_param [ 0 ] , & rtP .
StateSpace_DS_param [ 0 ] , & rtP . StateSpace_X0_param [ 0 ] , & rtP .
SwitchCurrents_Value [ 0 ] , } ; static int32_T * rtVarDimsAddrMap [ ] = { (
NULL ) } ;
#endif
static TARGET_CONST rtwCAPI_DataTypeMap rtDataTypeMap [ ] = { { "double" ,
"real_T" , 0 , 0 , sizeof ( real_T ) , ( uint8_T ) SS_DOUBLE , 0 , 0 , 0 } }
;
#ifdef HOST_CAPI_BUILD
#undef sizeof
#endif
static TARGET_CONST rtwCAPI_ElementMap rtElementMap [ ] = { { ( NULL ) , 0 ,
0 , 0 , 0 } , } ; static const rtwCAPI_DimensionMap rtDimensionMap [ ] = { {
rtwCAPI_SCALAR , 0 , 2 , 0 } , { rtwCAPI_VECTOR , 2 , 2 , 0 } , {
rtwCAPI_VECTOR , 4 , 2 , 0 } , { rtwCAPI_MATRIX_COL_MAJOR , 6 , 2 , 0 } , {
rtwCAPI_MATRIX_COL_MAJOR , 8 , 2 , 0 } , { rtwCAPI_MATRIX_COL_MAJOR , 10 , 2
, 0 } , { rtwCAPI_MATRIX_COL_MAJOR , 12 , 2 , 0 } , { rtwCAPI_VECTOR , 14 , 2
, 0 } } ; static const uint_T rtDimensionArray [ ] = { 1 , 1 , 11 , 1 , 8 , 1
, 4 , 4 , 4 , 9 , 11 , 4 , 11 , 9 , 4 , 1 } ; static const real_T
rtcapiStoredFloats [ ] = { 0.0 , 1.0 , 1.0E-7 } ; static const
rtwCAPI_FixPtMap rtFixPtMap [ ] = { { ( NULL ) , ( NULL ) ,
rtwCAPI_FIX_RESERVED , 0 , 0 , ( boolean_T ) 0 } , } ; static const
rtwCAPI_SampleTimeMap rtSampleTimeMap [ ] = { { ( const void * ) &
rtcapiStoredFloats [ 0 ] , ( const void * ) & rtcapiStoredFloats [ 1 ] , (
int8_T ) 1 , ( uint8_T ) 0 } , { ( const void * ) & rtcapiStoredFloats [ 2 ]
, ( const void * ) & rtcapiStoredFloats [ 0 ] , ( int8_T ) 2 , ( uint8_T ) 0
} , { ( const void * ) & rtcapiStoredFloats [ 0 ] , ( const void * ) &
rtcapiStoredFloats [ 0 ] , ( int8_T ) 0 , ( uint8_T ) 0 } } ; static
rtwCAPI_ModelMappingStaticInfo mmiStatic = { { rtBlockSignals , 30 ,
rtRootInputs , 0 , rtRootOutputs , 0 } , { rtBlockParameters , 38 ,
rtModelParameters , 0 } , { ( NULL ) , 0 } , { rtDataTypeMap , rtDimensionMap
, rtFixPtMap , rtElementMap , rtSampleTimeMap , rtDimensionArray } , "float"
, { 768363794U , 1091937133U , 483167757U , 2605760807U } , ( NULL ) , 0 , (
boolean_T ) 0 , rt_LoggedStateIdxList } ; const
rtwCAPI_ModelMappingStaticInfo * ph2_interleaved_BuckVi48_GetCAPIStaticMap (
void ) { return & mmiStatic ; }
#ifndef HOST_CAPI_BUILD
void ph2_interleaved_BuckVi48_InitializeDataMapInfo ( void ) {
rtwCAPI_SetVersion ( ( * rt_dataMapInfoPtr ) . mmi , 1 ) ;
rtwCAPI_SetStaticMap ( ( * rt_dataMapInfoPtr ) . mmi , & mmiStatic ) ;
rtwCAPI_SetLoggingStaticMap ( ( * rt_dataMapInfoPtr ) . mmi , ( NULL ) ) ;
rtwCAPI_SetDataAddressMap ( ( * rt_dataMapInfoPtr ) . mmi , rtDataAddrMap ) ;
rtwCAPI_SetVarDimsAddressMap ( ( * rt_dataMapInfoPtr ) . mmi ,
rtVarDimsAddrMap ) ; rtwCAPI_SetInstanceLoggingInfo ( ( * rt_dataMapInfoPtr )
. mmi , ( NULL ) ) ; rtwCAPI_SetChildMMIArray ( ( * rt_dataMapInfoPtr ) . mmi
, ( NULL ) ) ; rtwCAPI_SetChildMMIArrayLen ( ( * rt_dataMapInfoPtr ) . mmi ,
0 ) ; }
#else
#ifdef __cplusplus
extern "C" {
#endif
void ph2_interleaved_BuckVi48_host_InitializeDataMapInfo (
ph2_interleaved_BuckVi48_host_DataMapInfo_T * dataMap , const char * path ) {
rtwCAPI_SetVersion ( dataMap -> mmi , 1 ) ; rtwCAPI_SetStaticMap ( dataMap ->
mmi , & mmiStatic ) ; rtwCAPI_SetDataAddressMap ( dataMap -> mmi , ( NULL ) )
; rtwCAPI_SetVarDimsAddressMap ( dataMap -> mmi , ( NULL ) ) ;
rtwCAPI_SetPath ( dataMap -> mmi , path ) ; rtwCAPI_SetFullPath ( dataMap ->
mmi , ( NULL ) ) ; rtwCAPI_SetChildMMIArray ( dataMap -> mmi , ( NULL ) ) ;
rtwCAPI_SetChildMMIArrayLen ( dataMap -> mmi , 0 ) ; }
#ifdef __cplusplus
}
#endif
#endif
