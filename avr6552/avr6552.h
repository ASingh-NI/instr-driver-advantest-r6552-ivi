/****************************************************************************
 *                       Advantest R6552 DMM                           
 *---------------------------------------------------------------------------
 *  (c) 1999-2013, National Instruments, Corporation.  All Rights Reserved.         
 *---------------------------------------------------------------------------
 *                                                                          
 * Title:    avr6552.h                                        
 * Purpose:  Advantest R6552 DMM                                       
 *           instrument driver declarations.                                
 *                                                                          
 ****************************************************************************/

#ifndef __AVR6552_HEADER
#define __AVR6552_HEADER

#include <ivi.h>
#include <ividmm.h>

#if defined(__cplusplus) || defined(__cplusplus__)
extern "C" {
#endif

/****************************************************************************
 *----------------- Instrument Driver Revision Information -----------------*
 ****************************************************************************/
#define AVR6552_MAJOR_VERSION                3       /* Instrument driver major version   */
#define AVR6552_MINOR_VERSION                0       /* Instrument driver minor version   */

#define AVR6552_CLASS_SPEC_MAJOR_VERSION     3     /* Class specification major version */
#define AVR6552_CLASS_SPEC_MINOR_VERSION     0     /* Class specification minor version */

#define AVR6552_SUPPORTED_INSTRUMENT_MODELS  "R6552,R6552T,R6552T-R"

#define AVR6552_DRIVER_VENDOR                "National Instruments"
    
#ifdef _IVI_mswin64_ 
#define AVR6552_DRIVER_DESCRIPTION           "Advantest R6552 Series Digital Multimeter [Compiled for 64-bit]"
#else
#define AVR6552_DRIVER_DESCRIPTION           "Advantest R6552 Series Digital Multimeter"
#endif

#define AVR6552_IO_SESSION_TYPE              "VISA"

/**************************************************************************** 
 *------------------------------ Useful Macros -----------------------------* 
 ****************************************************************************/


    /* Defined values for maxTime parameter to the Measure, Read, and Fetch functions */
#define AVR6552_VAL_MAX_TIME_INFINITE          IVIDMM_VAL_MAX_TIME_INFINITE
#define AVR6552_VAL_MAX_TIME_IMMEDIATE         IVIDMM_VAL_MAX_TIME_IMMEDIATE 

/**************************************************************************** 
 *---------------------------- Attribute Defines ---------------------------* 
 ****************************************************************************/

    /*- IVI Inherent Instrument Attributes ---------------------------------*/

        /* User Options */
#define AVR6552_ATTR_RANGE_CHECK                   IVI_ATTR_RANGE_CHECK                    /* ViBoolean */
#define AVR6552_ATTR_QUERY_INSTRUMENT_STATUS       IVI_ATTR_QUERY_INSTR_STATUS             /* ViBoolean */
#define AVR6552_ATTR_CACHE                         IVI_ATTR_CACHE                          /* ViBoolean */
#define AVR6552_ATTR_SIMULATE                      IVI_ATTR_SIMULATE                       /* ViBoolean */
#define AVR6552_ATTR_RECORD_COERCIONS              IVI_ATTR_RECORD_COERCIONS               /* ViBoolean */

        /* Instrument Capabilities */
#define AVR6552_ATTR_CHANNEL_COUNT                 IVI_ATTR_NUM_CHANNELS                   /* ViInt32,  read-only  */
#define AVR6552_ATTR_GROUP_CAPABILITIES            IVI_ATTR_GROUP_CAPABILITIES             /* ViString, read-only */

        /* Driver Information  */
#define AVR6552_ATTR_SPECIFIC_DRIVER_PREFIX        IVI_ATTR_SPECIFIC_DRIVER_PREFIX         /* ViString, read-only  */
#define AVR6552_ATTR_SUPPORTED_INSTRUMENT_MODELS   IVI_ATTR_SUPPORTED_INSTRUMENT_MODELS    /* ViString, read-only  */
#define AVR6552_ATTR_INSTRUMENT_MANUFACTURER       IVI_ATTR_INSTRUMENT_MANUFACTURER        /* ViString, read-only  */
#define AVR6552_ATTR_INSTRUMENT_MODEL              IVI_ATTR_INSTRUMENT_MODEL               /* ViString, read-only  */
#define AVR6552_ATTR_INSTRUMENT_FIRMWARE_REVISION  IVI_ATTR_INSTRUMENT_FIRMWARE_REVISION   /* ViString, read-only  */
#define AVR6552_ATTR_SPECIFIC_DRIVER_REVISION      IVI_ATTR_SPECIFIC_DRIVER_REVISION       /* ViString, read-only  */
#define AVR6552_ATTR_SPECIFIC_DRIVER_VENDOR        IVI_ATTR_SPECIFIC_DRIVER_VENDOR         /* ViString, read-only  */
#define AVR6552_ATTR_SPECIFIC_DRIVER_DESCRIPTION   IVI_ATTR_SPECIFIC_DRIVER_DESCRIPTION    /* ViString, read-only  */
#define AVR6552_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION /* ViInt32, read-only */
#define AVR6552_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION /* ViInt32, read-only */
#define AVR6552_ATTR_DRIVER_SETUP                  IVI_ATTR_DRIVER_SETUP
#define AVR6552_ATTR_INTERCHANGE_CHECK             IVI_ATTR_INTERCHANGE_CHECK

        /* Error Info */

        /* Advanced Session Information */
#define AVR6552_ATTR_LOGICAL_NAME                  IVI_ATTR_LOGICAL_NAME                   /* ViString, read-only  */
#define AVR6552_ATTR_IO_RESOURCE_DESCRIPTOR        IVI_ATTR_RESOURCE_DESCRIPTOR            /* ViString, read-only  */
    
    /*- Instrument-Specific Attributes -------------------------------------*/

    /*- Basic Instrument Operation -----------------------------------------*/

#define AVR6552_ATTR_FUNCTION                  IVIDMM_ATTR_FUNCTION                 /* ViInt32   */
#define AVR6552_ATTR_RANGE                     IVIDMM_ATTR_RANGE                    /* ViReal64  */
#define AVR6552_ATTR_RESOLUTION_ABSOLUTE       IVIDMM_ATTR_RESOLUTION_ABSOLUTE      /* ViReal64  */
#define AVR6552_ATTR_TRIGGER_SOURCE            IVIDMM_ATTR_TRIGGER_SOURCE           /* ViInt32   */
#define AVR6552_ATTR_TRIGGER_DELAY             IVIDMM_ATTR_TRIGGER_DELAY            /* ViReal64  */

    /*- AC Measurement -----------------------------------------------------*/
#define AVR6552_ATTR_AC_MIN_FREQ               IVIDMM_ATTR_AC_MIN_FREQ              /* ViReal64  */
#define AVR6552_ATTR_AC_MAX_FREQ               IVIDMM_ATTR_AC_MAX_FREQ              /* ViReal64  */

    /*- Frequency Measurement ----------------------------------------------*/
#define AVR6552_ATTR_FREQ_VOLTAGE_RANGE        IVIDMM_ATTR_FREQ_VOLTAGE_RANGE       /* ViReal64  */

    /*- Ripple Volts Measurement -------------------------------------------*/ 
#define AVR6552_ATTR_HIGH_PASS_FILTER          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 19L)/* ViInt32   */
#define AVR6552_ATTR_LOW_PASS_FILTER           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 20L)/* ViInt32   */

    /*- Auto Zero Mode -----------------------------------------------------*/
#define AVR6552_ATTR_AUTO_ZERO                 IVIDMM_ATTR_AUTO_ZERO                /* ViInt32   */

    /*- Powerline Frequency  -----------------------------------------------*/
#define AVR6552_ATTR_POWERLINE_FREQ            IVIDMM_ATTR_POWERLINE_FREQ           /* ViReal64  */

    
    /*- Trigger Operations -------------------------------------------------*/
#define AVR6552_ATTR_SAMPLE_MODE               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 2L) /* ViInt32   */ 
#define AVR6552_ATTR_DELAY                     (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 3L) /* ViReal64  */
#define AVR6552_ATTR_SAMPLE_RATE               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 4L) /* ViInt32   */
#define AVR6552_ATTR_INTEGRAL_TIME             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 5L) /* ViReal64  */ 

        /*- Hold Mode ----*/
#define AVR6552_ATTR_HOLD_SAMPLE_COUNT         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 8L) /* ViInt32   */

        /*- Burst Mode ---*/
#define AVR6552_ATTR_BURST_COUNT               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 6L) /* ViInt32   */
#define AVR6552_ATTR_DELAYED_TRIGGER           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 7L) /* ViReal64  */


    /*- Measurement Operation ----------------------------------------------*/
    
       /*- Null Operation ----*/
#define AVR6552_ATTR_NULL_OFFSET               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 11L)/* ViReal64  */

       /*- Smooth Operation --*/ 
#define AVR6552_ATTR_SMOOTH_COUNT              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 12L)/* ViInt32   */  

       /*- Compare Operation -*/ 
#define AVR6552_ATTR_COMPARE_HIGH_VAL          (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 13L)/* ViReal64  */
#define AVR6552_ATTR_COMPARE_LOW_VAL           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 14L)/* ViReal64  */

       /*- Scale Operation ---*/ 
#define AVR6552_ATTR_SCALE_A                   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 15L)/* ViReal64  */
#define AVR6552_ATTR_SCALE_B                   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 16L)/* ViReal64  */
#define AVR6552_ATTR_SCALE_C                   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 17L)/* ViReal64  */

       /*- DB/DBM Operation --*/ 
#define AVR6552_ATTR_DB_CONSTANT               (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 18L)/* ViReal64  */ 

    /*- Miscellaneous  -----------------------------------------------------*/
#define AVR6552_ATTR_ID_QUERY_RESPONSE         (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 1L) /* ViString (Read Only) */ 
#define AVR6552_ATTR_NULL_ENABLED              (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 22L)
#define AVR6552_ATTR_SMOOTH_ENABLED            (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 23L)
#define AVR6552_ATTR_COMPARE_ENABLED           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 24L)
#define AVR6552_ATTR_SCALE_ENABLED             (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 25L)
#define AVR6552_ATTR_MAX_MIN_ENABLED           (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 26L)
#define AVR6552_ATTR_DB_MODE                   (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 27L)
#define AVR6552_ATTR_RESOLUTION                (IVI_SPECIFIC_PUBLIC_ATTR_BASE + 28L)

/**************************************************************************** 
 *------------------------ Attribute Value Defines -------------------------* 
 ****************************************************************************/

    /*- Defined values for attribute AVR6552_ATTR_FUNCTION -*/

#define AVR6552_VAL_DC_VOLTS                   IVIDMM_VAL_DC_VOLTS    
#define AVR6552_VAL_AC_VOLTS                   IVIDMM_VAL_AC_VOLTS    
#define AVR6552_VAL_DC_CURRENT                 IVIDMM_VAL_DC_CURRENT  
#define AVR6552_VAL_AC_CURRENT                 IVIDMM_VAL_AC_CURRENT  
#define AVR6552_VAL_2_WIRE_RES                 IVIDMM_VAL_2_WIRE_RES  
#define AVR6552_VAL_4_WIRE_RES                 IVIDMM_VAL_4_WIRE_RES           
#define AVR6552_VAL_FREQ                       IVIDMM_VAL_FREQ                 
#define AVR6552_VAL_AC_PLUS_DC_VOLTS           IVIDMM_VAL_AC_PLUS_DC_VOLTS     
#define AVR6552_VAL_AC_PLUS_DC_CURRENT         IVIDMM_VAL_AC_PLUS_DC_CURRENT   
#define AVR6552_VAL_DIODE                      (IVIDMM_VAL_FUNC_SPECIFIC_EXT_BASE + 1)               
#define AVR6552_VAL_LP_2_WIRE_RES              (IVIDMM_VAL_FUNC_SPECIFIC_EXT_BASE + 2)
#define AVR6552_VAL_LP_4_WIRE_RES              (IVIDMM_VAL_FUNC_SPECIFIC_EXT_BASE + 3)
#define AVR6552_VAL_RIPPLE_VOLTS               (IVIDMM_VAL_FUNC_SPECIFIC_EXT_BASE + 4)  

    /*- Defined values for attribute AVR6552_ATTR_RANGE -*/
        
#define AVR6552_VAL_AUTO_RANGE_ON              IVIDMM_VAL_AUTO_RANGE_ON            

    /*- Defined values for attribute AVR6552_ATTR_RESOLUTION -*/

#define AVR6552_VAL_3_5_DIGITS                 3.5
#define AVR6552_VAL_4_DIGITS                   4.0
#define AVR6552_VAL_4_5_DIGITS                 4.5
#define AVR6552_VAL_5_DIGITS                   5.0
#define AVR6552_VAL_5_5_DIGITS                 5.5
#define AVR6552_VAL_6_DIGITS                   6.0

    /*- Defined values for attribute AVR6552_ATTR_TRIGGER_SOURCE -*/

#define AVR6552_VAL_IMMEDIATE                   IVIDMM_VAL_IMMEDIATE  

    /*- Defined values for attribute AVR6552_ATTR_AUTO_ZER0 -*/
    
#define AVR6552_VAL_AUTO_ZERO_OFF              IVIDMM_VAL_AUTO_ZERO_OFF    
#define AVR6552_VAL_AUTO_ZERO_ON               IVIDMM_VAL_AUTO_ZERO_ON     
#define AVR6552_VAL_AUTO_ZERO_ONCE             IVIDMM_VAL_AUTO_ZERO_ONCE   

    /*- Defined values for attribute AVR6552_ATTR_POWERLINE_FREQ -*/
    
#define AVR6552_VAL_50_HERTZ                   50.0 
#define AVR6552_VAL_60_HERTZ                   60.0 

   /*- Defined values for attribute AVR6552_ATTR_SAMPLE_MODE -*/ 
   
#define AVR6552_VAL_FREE_RUN                   0
#define AVR6552_VAL_HOLD                       1
#define AVR6552_VAL_BURST                      2

   /*- Defined values for attribute AVR6552_ATTR_SAMPLE_RATE -*/ 
   
#define AVR6552_VAL_FAST                       0
#define AVR6552_VAL_MED                        1
#define AVR6552_VAL_SLOW                       2
#define AVR6552_VAL_LONGIT                     3

   /*- Defined values for attribute AVR6552_ATTR_HIGH_PASS_FILTER -*/ 
#define AVR6552_VAL_FILTER_OFF                 -1     

   /*- Defined values for attribute AVR6552_ATTR_LOW_PASS_FILTER -*/ 
/* #define AVR6552_VAL_FILTER_OFF          DEFINED ABOVE */

   /*- Defined values for attribute AVR6552_ATTR_COMPARE_HIGH_VAL -*/ 
#define AVR6552_VAL_MEASURE_VALUE              9.9E+37

   /*- Defined values for attribute AVR6552_ATTR_DB_MODE -*/ 

#define AVR6552_VAL_DB_OFF      0
#define AVR6552_VAL_DB_ON       1
#define AVR6552_VAL_DBM_ON      2

/**************************************************************************** 
 *---------------- Instrument Driver Function Declarations -----------------* 
 ****************************************************************************/

    /*- Init and Close Functions -------------------------------------------*/

ViStatus _VI_FUNC  avr6552_init (ViRsrc resourceName, ViBoolean IDQuery,
                                  ViBoolean resetDevice, ViSession *vi);
ViStatus _VI_FUNC  avr6552_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                             ViBoolean resetDevice, ViConstString optionString, 
                                             ViSession *newVi);
ViStatus _VI_FUNC  avr6552_close (ViSession vi);   

    /*- Coercion Info Functions --------------------------------------------*/

ViStatus _VI_FUNC  avr6552_GetNextCoercionRecord (ViSession vi,
                                                   ViInt32 bufferSize,
                                                   ViChar record[]);

                                                   
    /*- Interchangeability Checking Functions ------------------------------*/
ViStatus _VI_FUNC avr6552_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[]);
ViStatus _VI_FUNC avr6552_ResetInterchangeCheck (ViSession vi);
ViStatus _VI_FUNC avr6552_ClearInterchangeWarnings (ViSession vi);                                                   

    /*- Locking Functions --------------------------------------------------*/

ViStatus _VI_FUNC  avr6552_LockSession (ViSession vi, ViBoolean *callerHasLock);   
ViStatus _VI_FUNC  avr6552_UnlockSession (ViSession vi, ViBoolean *callerHasLock);


    /*- Basic Instrument Operation -----------------------------------------*/
    
ViStatus _VI_FUNC  avr6552_ConfigureMeasurement (ViSession vi,
                                                 ViInt32 measFunction,                                       
                                                 ViReal64 range, 
                                                 ViReal64 resolution);
ViStatus _VI_FUNC  avr6552_ConfigureTrigger (ViSession vi, 
                                             ViInt32 triggerSource,
                                             ViReal64 triggerDelay);
ViStatus _VI_FUNC  avr6552_Read (ViSession vi,
                                 ViInt32 maxTime,
                                 ViReal64 *reading);
ViStatus _VI_FUNC  avr6552_Fetch (ViSession vi,
                                  ViInt32 maxTime,
                                  ViReal64 *reading);
ViStatus _VI_FUNC  avr6552_Abort (ViSession vi);
ViStatus _VI_FUNC  avr6552_Initiate (ViSession vi);
ViStatus _VI_FUNC  avr6552_IsOverRange (ViSession vi,
                                        ViReal64 measurementValue,
                                        ViBoolean *isOverRange);

    /*- AC Measurement -----------------------------------------------------*/
ViStatus _VI_FUNC  avr6552_ConfigureACBandwidth (ViSession vi, 
                                                 ViReal64 minFreq, 
                                                 ViReal64 maxFreq);

    /*- Frequency Measurement ----------------------------------------------*/
ViStatus _VI_FUNC  avr6552_ConfigureFrequencyVoltageRange (ViSession vi, 
                                                           ViReal64 frequencyVoltageRange);

    /*- Ripple Volts Measurement -------------------------------------------*/
ViStatus _VI_FUNC  avr6552_ConfigureRippleVoltsFilter (ViSession vi,
                                                 ViInt32 highPassFilter,
                                                 ViInt32 lowPassFilter);

    /*- Auto Zero Mode -----------------------------------------------------*/
ViStatus _VI_FUNC  avr6552_ConfigureAutoZeroMode (ViSession vi, 
                                                  ViInt32 autoZeroMode);

    /*- Software Trigger ---------------------------------------------------*/
ViStatus _VI_FUNC  avr6552_ForceTrigger (ViSession vi);

    /*- Trigger Operations -------------------------------------------------*/
ViStatus _VI_FUNC  avr6552_ConfigureSampleMode (ViSession vi,
                                                ViInt32 sampleMode,
                                                ViReal64 delay); 
ViStatus _VI_FUNC  avr6552_ConfigureSampleRate (ViSession vi,
                                                ViInt32 sampleRate,
                                                ViReal64 integralTime);

        /*- Hold Mode ------------------------------------------------*/
ViStatus _VI_FUNC  avr6552_ConfigureHold (ViSession vi,
                                          ViInt32 sampleCount);
ViStatus _VI_FUNC  avr6552_ReadHoldMeasurement (ViSession vi,
                                                ViInt32 maxTime,
                                                ViReal64 *reading);
    
        /*- Burst Mode -----------------------------------------------*/
ViStatus _VI_FUNC  avr6552_ConfigureBurst (ViSession vi,
                                           ViInt32 burstCount,
                                           ViInt32 delayedTrigger);
ViStatus _VI_FUNC  avr6552_ReadBurstMultiPoint (ViSession vi,
                                                ViInt32 maximumTimems,
                                                ViInt32 arraySize,
                                                ViReal64 readingArray[],
                                                ViInt32 *actualNumberofPoints);
ViStatus _VI_FUNC  avr6552_FetchBurstMultiPoint (ViSession vi,
                                                 ViInt32 maximumTimems,
                                                 ViInt32 arraySize,
                                                 ViReal64 readingArray[],
                                                 ViInt32 *actualNumberofPoints);

    /*- Calculations -------------------------------------------------------*/

ViStatus _VI_FUNC  avr6552_ConfigureNull (ViSession vi,
                                          ViBoolean enable,
                                          ViReal64 nullOffset);
ViStatus _VI_FUNC  avr6552_ConfigureSmooth (ViSession vi,
                                          ViBoolean enable,
                                            ViInt32 smoothCount);
ViStatus _VI_FUNC  avr6552_ConfigureCompare (ViSession vi,
                                             ViBoolean enable,
                                             ViReal64 highValue,
                                             ViReal64 lowValue);
ViStatus _VI_FUNC  avr6552_ConfigureMaxMin (ViSession vi,
                                           ViBoolean enable);
ViStatus _VI_FUNC  avr6552_ConfigureScale (ViSession vi,
                                           ViBoolean enable,
                                           ViReal64 constantA, 
                                           ViReal64 constantB,
                                           ViReal64 constantC);
ViStatus _VI_FUNC  avr6552_ConfigureDecibel (ViSession vi,
                                             ViInt32 mode,
                                             ViReal64 decibelValue);
ViStatus _VI_FUNC  avr6552_QueryMaxMin (ViSession vi,
                                        ViReal64 *maxValue,
                                        ViReal64 *minValue,
                                        ViReal64 *averageValue,
                                        ViReal64 *cycleCount);

    /*- Channel Info Functions ---------------------------------------------*/
ViStatus _VI_FUNC avr6552_GetChannelName (ViSession vi, ViInt32 index, 
                                   ViInt32 bufferSize, ViChar name[]);

    /*- Error Functions ----------------------------------------------------*/
ViStatus _VI_FUNC  avr6552_error_query (ViSession vi, ViInt32 *errorCode,
                                         ViChar errorMessage[]);
ViStatus _VI_FUNC  avr6552_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                          ViStatus *secondaryError, 
                                          ViChar errorElaboration[256]);
ViStatus _VI_FUNC  avr6552_ClearErrorInfo (ViSession vi);
ViStatus _VI_FUNC  avr6552_error_message (ViSession vi, ViStatus errorCode,
                                           ViChar errorMessage[256]);
ViStatus _VI_FUNC avr6552_GetError (ViSession vi, ViStatus *errorCode, 
                                     ViInt32 bufferSize, ViChar description[]);  
ViStatus _VI_FUNC  avr6552_ClearError (ViSession vi);                                           
    
    /*- Utility Functions --------------------------------------------------*/

ViStatus _VI_FUNC  avr6552_reset (ViSession vi);
ViStatus _VI_FUNC  avr6552_self_test (ViSession vi, ViInt16 *selfTestResult,
                                       ViChar selfTestMessage[]);
ViStatus _VI_FUNC  avr6552_revision_query (ViSession vi, 
                                            ViChar instrumentDriverRevision[],
                                            ViChar firmwareRevision[]);
ViStatus _VI_FUNC  avr6552_WriteInstrData (ViSession vi, ViConstString writeBuffer); 
ViStatus _VI_FUNC  avr6552_ReadInstrData  (ViSession vi, ViInt32 numBytes, 
                                            ViChar rdBuf[], ViInt32 *bytesRead);
ViStatus _VI_FUNC avr6552_InvalidateAllAttributes (ViSession vi);
ViStatus _VI_FUNC avr6552_ResetWithDefaults (ViSession vi);
ViStatus _VI_FUNC avr6552_Disable (ViSession vi);                                            

    /*- Set, Get, and Check Attribute Functions ----------------------------*/

ViStatus _VI_FUNC  avr6552_GetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 *value);
ViStatus _VI_FUNC  avr6552_GetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 *value);
ViStatus _VI_FUNC  avr6552_GetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 bufSize, ViChar value[]); 
ViStatus _VI_FUNC  avr6552_GetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession *value);
ViStatus _VI_FUNC  avr6552_GetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean *value);

ViStatus _VI_FUNC  avr6552_SetAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  avr6552_SetAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  avr6552_SetAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  avr6552_SetAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  avr6552_SetAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

ViStatus _VI_FUNC  avr6552_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, ViAttr attribute, ViInt32 value);
ViStatus _VI_FUNC  avr6552_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, ViAttr attribute, ViReal64 value);
ViStatus _VI_FUNC  avr6552_CheckAttributeViString (ViSession vi, ViConstString channelName, ViAttr attribute, ViConstString value); 
ViStatus _VI_FUNC  avr6552_CheckAttributeViSession (ViSession vi, ViConstString channelName, ViAttr attribute, ViSession value);
ViStatus _VI_FUNC  avr6552_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, ViAttr attribute, ViBoolean value);

    /*********************************************************
        Functions reserved for class driver use only.
        End-users should not call these functions.  
     *********************************************************/

ViStatus _VI_FUNC  avr6552_IviInit (ViRsrc resourceName, ViBoolean IDQuery, 
                                     ViBoolean reset, ViSession vi);
ViStatus _VI_FUNC  avr6552_IviClose (ViSession vi);   

/****************************************************************************
 *------------------------ Error And Completion Codes ----------------------*
 ****************************************************************************/

#define AVR6552_WARN_OVER_RANGE                           IVIDMM_WARN_OVER_RANGE
#define AVR6552_ERROR_MAX_TIME_EXCEEDED                   IVIDMM_ERROR_MAX_TIME_EXCEEDED                  

    /*- Obsolete Inherent Instrument Attributes and functions -*/
    /*- These attributes and functions have been deprecated and may not
        be supported in future versions of this driver. 
    -*/
        /*- Driver Information -*/
#define AVR6552_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION  // ViInt32,  read-only  
#define AVR6552_ATTR_SPECIFIC_DRIVER_MINOR_VERSION IVI_ATTR_SPECIFIC_DRIVER_MINOR_VERSION  // ViInt32,  read-only  

        /*- Error Info -*/
#define AVR6552_ATTR_PRIMARY_ERROR                 IVI_ATTR_PRIMARY_ERROR                  // ViInt32 
#define AVR6552_ATTR_SECONDARY_ERROR               IVI_ATTR_SECONDARY_ERROR                // ViInt32
#define AVR6552_ATTR_ERROR_ELABORATION             IVI_ATTR_ERROR_ELABORATION              // ViString

        /*- Advanced Session Information -*/
#define AVR6552_ATTR_IO_SESSION_TYPE               IVI_ATTR_IO_SESSION_TYPE                // ViString, read-only  
#define AVR6552_ATTR_IO_SESSION                    IVI_ATTR_IO_SESSION                     // ViSession, read-only

        /*- Deprecated Attribute Identifiers for Renamed Attributes -*/
#define AVR6552_ATTR_NUM_CHANNELS                  AVR6552_ATTR_CHANNEL_COUNT
#define AVR6552_ATTR_QUERY_INSTR_STATUS            AVR6552_ATTR_QUERY_INSTRUMENT_STATUS            
#define AVR6552_ATTR_RESOURCE_DESCRIPTOR           AVR6552_ATTR_IO_RESOURCE_DESCRIPTOR

        /*- Deprecated Error Information functions -*/
ViStatus _VI_FUNC  avr6552_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                          ViStatus *secondaryError, 
                                          ViChar errorElaboration[256]);
ViStatus _VI_FUNC  avr6552_ClearErrorInfo (ViSession vi);

/**************************************************************************** 
 *---------------------------- End Include File ----------------------------* 
 ****************************************************************************/

#if defined(__cplusplus) || defined(__cplusplus__)
}
#endif
#endif /* __AVR6552_HEADER */




