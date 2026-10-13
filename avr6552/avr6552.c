/***************************************************************************** 
 * (c) 1999-2013, National Instruments, Corporation.  All Rights Reserved.   * 
 *****************************************************************************/

/*****************************************************************************
 *  Advantest R6552 DMM Instrument Driver                               
 *  LabWindows/CVI 5.0 Instrument Driver                                     
 *  Original Release: 99-7-27                                  
 *  By: AL, National Instruments                              
 *      PH. 86-21-65557838   Fax 86-21-65556244                              
 *                                                                           
 *  Modification History:                                                    
 *                                                                           
 *       99-7-27 - Instrument Driver Created.
 *       00-11-8 - Bug fix (AVR6552_ATTR_ID_QUERY_RESPONSE)
 *                 Changed MINOR version to 100 
 *      04/03/01 - [KP] Driver updated to IVI-3.2 specification:
 *                      Added functions:
 *                         avr6552_InvalidateAllAttributes
 *                         avr6552_ResetWithDefaults
 *                         avr6552_Disable
 *                         avr6552_GetNextInterchangeWarning
 *                         avr6552_ResetInterchangeCheck
 *                         avr6552_ClearInterchangeWarning
 *                         avr6552_GetError
 *                         avr6552_ClearError
 *                      Added attributes:
 *                         AVR6552_ATTR_DRIVER_SETUP
 *                         AVR6552_ATTR_INTERCHANGE_CHECK
 *                      Renamed Attributes
 *                         AVR6552_ATTR_QUERY_INSTR_STATUS to AVR6552_ATTR_QUERY_INSTRUMENT_STATUS
 *                         AVR6552_ATTR_RESOURCE_DESCRIPTOR to AVR6552_ATTR_IO_RESOURCE_DESCRIPTOR
 *                         AVR6552_ATTR_NUM_CHANNELS to AVR6552_ATTR_CHANNEL_COUNT
 *                      Changed:
 *                         avr6552_InitWithOptions changed type of optionsString
 *                                                parameter to ViConstString
 *      01/07/03 - [VJ] Bug fix - removed trailing whitespace (avr6552AttrIdQueryResponse_ReadCallback)
 *                              - Changed MINOR version to 100 (2.1) 
 *      5/28/2003 - [AL] Set attribute IVI_ATTR_IO_RESOURCE_DESCRIPTOR 
 *                        in initialize time.
 *      03/12/2004 - [Qizhen] Bug fix.  Add codes in attribute RANGE callback functions.  
 *                                      Not allow to set range in frequency and diode measurement function.
 *      10/20/2013 - [YL] Port to 64bit.
 *****************************************************************************/

#include <ansi_c.h>  
#include <string.h>
#include <stdio.h>  
#include <formatio.h>
#include <stdlib.h>
#include "avr6552.h"

#if defined(_MSC_VER)
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif

/*****************************************************************************
 *--------------------- Hidden Attribute Declarations -----------------------*
 *****************************************************************************/

#define AVR6552_ATTR_OPC_TIMEOUT      (IVI_SPECIFIC_PRIVATE_ATTR_BASE + 1L)   /* ViInt32 */ 

/*****************************************************************************
 *---------------------------- Useful Macros --------------------------------*
 *****************************************************************************/

    /*- I/O buffer size -----------------------------------------------------*/
#define BUFFER_SIZE                             512L        

    /*- 488.2 Event Status Register (ESR) Bits ------------------------------*/
#define IEEE_488_2_DEVICE_DEPENDENT_ERROR_BIT   0x08
#define IEEE_488_2_EXECUTION_ERROR_BIT          0x10
#define IEEE_488_2_COMMAND_ERROR_BIT            0x20

    /*- List of channels passed to the Ivi_BuildChannelTable function -------*/ 
#define CHANNEL_LIST                            "1"

    /*- Family Variable -----------------------------------------------------*/
#define SETUP_MODEL_TAG                   "Model"      
#define AVR6552_VAL_MODEL_R6552           0
#define AVR6552_VAL_MODEL_R6552T          1
#define AVR6552_VAL_MODEL_R6552R          2

static IviStringValueTable avr6552_ModelTable =

{
        {AVR6552_VAL_MODEL_R6552,    "R6552"},
        {AVR6552_VAL_MODEL_R6552T,   "R6552T"},
        {AVR6552_VAL_MODEL_R6552R,   "R6552R"},
        {VI_NULL,    VI_NULL}
};

static IviStringValueTable avr6552_IdTable =

{
        {AVR6552_VAL_MODEL_R6552,    "ADVANTEST,R6552"},
        {AVR6552_VAL_MODEL_R6552T,   "ADVANTEST,R6552T"},
        {AVR6552_VAL_MODEL_R6552R,   "ADVANTEST,R6552R"},
        {VI_NULL,    VI_NULL}
};

/*****************************************************************************
 *-------------- Utility Function Declarations (Non-Exported) ---------------*
 *****************************************************************************/
static ViStatus avr6552_InitAttributes (ViSession vi, ViInt32 model);
static ViStatus avr6552_DefaultInstrSetup (ViSession openInstrSession);
static ViStatus avr6552_CheckStatus (ViSession vi);
static ViStatus avr6552_WaitForOPC (ViSession vi, ViInt32 maxTime);
static ViStatus avr6552_GetDriverSetupOption (ViSession vi,
                                               ViConstString optionTag,
                                               ViInt32 optionValueSize,
                                               ViChar optionValue[]);

static ViStatus avr6552_GetCalculationCommandString (ViAttr attributeId, ViString *cmd);
                                   
    /*- File I/O Utility Functions -*/
static ViStatus avr6552_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, ViInt32 *totalBytesWritten);
static ViStatus avr6552_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten);
static ViStatus avr6552_convertAbsolutetoDigits(ViSession vi, ViReal64 absResolution,
                                         ViReal64 *digits);
static ViStatus avr6552_convertDigitstoAbsolute(ViSession vi, ViReal64 digits, 
                                         ViReal64 *absResolution);

/*****************************************************************************
 *----------------- Callback Declarations (Non-Exported) --------------------*
 *****************************************************************************/

    /*- Global Session Callbacks --------------------------------------------*/
    
static ViStatus _VI_FUNC avr6552_CheckStatusCallback (ViSession vi, ViSession io);
static ViStatus _VI_FUNC avr6552_WaitForOPCCallback (ViSession vi, ViSession io);

    /*- Attribute callbacks -------------------------------------------------*/

static ViStatus _VI_FUNC avr6552AttrSpecificDriverRevision_ReadCallback (ViSession vi,
                                                                  ViSession io, 
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId, 
                                                                  const ViConstString cacheValue);
static ViStatus _VI_FUNC avr6552AttrIdQueryResponse_ReadCallback (ViSession vi,
                                                                   ViSession io, 
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId, 
                                                                   const ViConstString cacheValue);
static ViStatus _VI_FUNC avr6552AttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue);
static ViStatus _VI_FUNC avr6552AttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue);
static ViStatus _VI_FUNC avr6552AttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue);
static ViStatus _VI_FUNC avr6552AttrFunction_CheckCallback (ViSession vi,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrFunction_WriteCallback (ViSession vi,
                                                             ViSession io, 
                                                             ViConstString channelName, 
                                                             ViAttr attributeId, 
                                                             ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrFunction_ReadCallback (ViSession vi,
                                                            ViSession io, 
                                                            ViConstString channelName, 
                                                            ViAttr attributeId, 
                                                            ViInt32 *value);
static ViStatus _VI_FUNC avr6552AttrRange_ReadCallback (ViSession vi, 
                                                         ViSession io, 
                                                         ViConstString channelName, 
                                                         ViAttr attributeId, 
                                                         ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrRange_WriteCallback (ViSession vi, 
                                                          ViSession io, 
                                                          ViConstString channelName, 
                                                          ViAttr attributeId, 
                                                          ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrRange_CheckCallback (ViSession vi,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrResolutionAbsolute_CoerceCallback (ViSession vi,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 value,
                                                                     ViReal64 *coercedValue);
static ViStatus _VI_FUNC avr6552AttrResolutionAbsolute_CheckCallback (ViSession vi,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrResolutionAbsolute_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrResolutionAbsolute_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrAcMinFreq_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrAcMinFreq_ReadCallback (ViSession vi, 
                                                             ViSession io, 
                                                             ViConstString channelName, 
                                                             ViAttr attributeId, 
                                                             ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrAcMaxFreq_RangeTableCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  IviRangeTablePtr *rangeTablePtr);
static ViStatus _VI_FUNC avr6552AttrAutoZero_CheckCallback (ViSession vi,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrAutoZero_ReadCallback (ViSession vi, 
                                                            ViSession io, 
                                                            ViConstString channelName, 
                                                            ViAttr attributeId, 
                                                            ViInt32 *value);
static ViStatus _VI_FUNC avr6552AttrAutoZero_WriteCallback (ViSession vi, 
                                                             ViSession io, 
                                                             ViConstString channelName, 
                                                             ViAttr attributeId, 
                                                             ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrPowerlineFreq_ReadCallback (ViSession vi, 
                                                                 ViSession io, 
                                                                 ViConstString channelName, 
                                                                 ViAttr attributeId, 
                                                                 ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrHoldSampleCount_ReadCallback (ViSession vi, 
                                                               ViSession io, 
                                                               ViConstString channelName, 
                                                               ViAttr attributeId, 
                                                               ViInt32 *value);
static ViStatus _VI_FUNC avr6552AttrHoldSampleCount_WriteCallback (ViSession vi, 
                                                                ViSession io, 
                                                                ViConstString channelName, 
                                                                ViAttr attributeId, 
                                                                ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrTriggerDelay_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrSampleMode_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value);
static ViStatus _VI_FUNC avr6552AttrSampleMode_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrBurstCount_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value);
static ViStatus _VI_FUNC avr6552AttrBurstCount_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrDelayedTrigger_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);
static ViStatus _VI_FUNC avr6552AttrDelayedTrigger_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrSampleRate_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrSampleRate_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value);
static ViStatus _VI_FUNC avr6552AttrSampleRate_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrIntegralTime_CoerceCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value,
                                                                 ViReal64 *coercedValue);
static ViStatus _VI_FUNC avr6552AttrIntegralTime_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrIntegralTime_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrCalculationEnabled_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViBoolean *value);
static ViStatus _VI_FUNC avr6552AttrCalculationEnabled_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViBoolean value);
static ViStatus _VI_FUNC avr6552AttrNullOffset_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrNullOffset_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrNullOffset_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrSmoothCount_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value);
static ViStatus _VI_FUNC avr6552AttrSmoothCount_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrCompareHighVal_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrCompareHighVal_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrCompareLowVal_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrCompareLowVal_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrScaleA_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrScaleA_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrScaleB_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrScaleB_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrScaleC_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrScaleC_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrDbConstant_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrDbConstant_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrHighPassFilter_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value);
static ViStatus _VI_FUNC avr6552AttrHighPassFilter_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrLowPassFilter_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value);
static ViStatus _VI_FUNC avr6552AttrLowPassFilter_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value);
static ViStatus _VI_FUNC avr6552AttrResolution_RangeTableCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   IviRangeTablePtr *rangeTablePtr);
static ViStatus _VI_FUNC avr6552AttrResolution_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value);
static ViStatus _VI_FUNC avr6552AttrResolution_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrDelay_ReadCallback (ViSession vi,
                                                        ViSession io,
                                                        ViConstString channelName,
                                                        ViAttr attributeId,
                                                        ViReal64 *value);

static ViStatus _VI_FUNC avr6552AttrDelay_WriteCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 value);
static ViStatus _VI_FUNC avr6552AttrFreqVoltageRange_CoerceCallback (ViSession vi,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 value,
                                                                     ViReal64 *coercedValue);
static ViStatus _VI_FUNC avr6552AttrCalculationEnabled_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean *value);
static ViStatus _VI_FUNC avr6552AttrCalculationEnabled_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViBoolean value);
static ViStatus _VI_FUNC avr6552AttrDbMode_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViInt32 *value);
static ViStatus _VI_FUNC avr6552AttrDbMode_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViInt32 value);

static ViStatus _VI_FUNC Range_RangeTableCallback (ViSession vi,
                                                   ViConstString channelName,
                                                   ViAttr attributeId,
                                                   IviRangeTablePtr *rangeTablePtr);

static ViStatus _VI_FUNC SampleMode_CheckCallback (ViSession vi,
                                                   ViConstString channelName,
                                                   ViAttr attributeId,
                                                   ViInt32 value);

/*****************************************************************************
 *------------ User-Callable Functions (Exportable Functions) ---------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: avr6552_init   
 * Purpose:  VXIplug&play required function.  Calls the   
 *           avr6552_InitWithOptions function.   
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_init (ViRsrc resourceName, ViBoolean IDQuery,
                                 ViBoolean resetDevice, ViSession *newVi)
{   
    ViStatus    error = VI_SUCCESS;

    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER4, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }

    checkErr( avr6552_InitWithOptions (resourceName, IDQuery, resetDevice, 
                                        "", newVi));

Error:  
    return error;
}

/*****************************************************************************
 * Function: avr6552_InitWithOptions                                       
 * Purpose:  This function creates a new IVI session and calls the 
 *           IviInit function.                                     
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_InitWithOptions (ViRsrc resourceName, ViBoolean IDQuery,
                                            ViBoolean resetDevice, ViConstString optionString, 
                                            ViSession *newVi)
{   
    ViStatus    error = VI_SUCCESS;
    ViSession   vi = VI_NULL;
    ViChar      newResourceName[IVI_MAX_MESSAGE_BUF_SIZE];
    ViChar      newOptionString[IVI_MAX_MESSAGE_BUF_SIZE];
    ViBoolean   isLogicalName;
    
    if (newVi == VI_NULL)
        {
        Ivi_SetErrorInfo (VI_NULL, VI_FALSE, IVI_ERROR_INVALID_PARAMETER, 
                          VI_ERROR_PARAMETER5, "Null address for Instrument Handle");
        checkErr( IVI_ERROR_INVALID_PARAMETER);
        }

    *newVi = VI_NULL;
    
    checkErr( Ivi_GetInfoFromResourceName (resourceName, (ViString) optionString, newResourceName,
                                           newOptionString, &isLogicalName));
    
        /* create a new interchangeable driver */
    checkErr( Ivi_SpecificDriverNew ("avr6552", newOptionString, &vi));  
    if (!isLogicalName)
    {
        ViInt32 oldFlag = 0;
        
        checkErr (Ivi_GetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, &oldFlag));
        checkErr (Ivi_SetAttributeFlags (vi, IVI_ATTR_IO_RESOURCE_DESCRIPTOR, oldFlag & 0xfffb | 0x0010));
        checkErr (Ivi_SetAttributeViString (vi, "", IVI_ATTR_IO_RESOURCE_DESCRIPTOR, 0, newResourceName));
    }
        /* init the driver */
    checkErr( avr6552_IviInit (newResourceName, IDQuery, resetDevice, vi)); 
    
    if (isLogicalName)
        checkErr( Ivi_ApplyDefaultSetup (vi));
        
    *newVi = vi;
    
Error:
    if (error < VI_SUCCESS) 
        Ivi_Dispose (vi);
        
    return error;
}

/*****************************************************************************
 * Function: avr6552_IviInit                                                       
 * Purpose:  This function is called by avr6552_InitWithOptions  
 *           or by an IVI class driver.  This function initializes the I/O 
 *           interface, optionally resets the device, optionally performs an
 *           ID query, and sends a default setup to the instrument.                
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_IviInit (ViRsrc resourceName, ViBoolean IDQuery,
                                    ViBoolean reset, ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    ViSession   io = VI_NULL;
    ViChar    idQueryResponse[BUFFER_SIZE];
    ViChar    buffer[BUFFER_SIZE];
    ViInt32   model = AVR6552_VAL_MODEL_R6552;
    
    checkErr( Ivi_BuildChannelTable (vi, CHANNEL_LIST, VI_FALSE, VI_NULL));
    
    checkErr (Ivi_AddAttributeViString (vi, AVR6552_ATTR_ID_QUERY_RESPONSE,
                                        "AVR6552_ATTR_ID_QUERY_RESPONSE",
                                        "ADVANTEST,R6552", IVI_VAL_NOT_USER_WRITABLE,
                                        avr6552AttrIdQueryResponse_ReadCallback,
                                        VI_NULL));
    if (!Ivi_Simulating(vi))
        {
        ViSession   rmSession = VI_NULL;

        /* Open instrument session */
        checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_VISA_RM_SESSION, 0,
                                             &rmSession)); 
        viCheckErr( viOpen (rmSession, resourceName, VI_NULL, VI_NULL, &io));
        
        /* io session owned by driver now */
        checkErr( Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, io));  

        /* Configure VISA Formatted I/O */
        viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, 5000 ));
        viCheckErr( viSetBuf (io, VI_READ_BUF | VI_WRITE_BUF, 4000));
        viCheckErr( viSetAttribute (io, VI_ATTR_WR_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
        viCheckErr( viSetAttribute (io, VI_ATTR_RD_BUF_OPER_MODE, VI_FLUSH_ON_ACCESS));
        }
   
    
    if (!Ivi_Simulating(vi) && IDQuery)                               
        {
        checkErr( Ivi_GetAttributeViString (vi, VI_NULL, AVR6552_ATTR_ID_QUERY_RESPONSE, 0,
                                                BUFFER_SIZE, idQueryResponse));  

        /* If the response string from the id query does not match any value */
        /* in the IdTable, then the id query has failed                      */
        /* Extract the first part of the string (up to the second ',')       */
        
        error = Ivi_GetValueFromTable (avr6552_IdTable, idQueryResponse, &model);
        if (error == IVI_ERROR_INVALID_VALUE)
            viCheckErr (VI_ERROR_FAIL_ID_QUERY);
        }
    
    checkErr( avr6552_GetDriverSetupOption (vi, SETUP_MODEL_TAG, BUFFER_SIZE, buffer));
    
    if (buffer[0])
        {
        error = Ivi_GetValueFromTable (avr6552_ModelTable, buffer, &model);
        if (error == IVI_ERROR_INVALID_VALUE)
            viCheckErrElab( IVI_ERROR_BAD_OPTION_VALUE, SETUP_MODEL_TAG);
        }
        
    /* If the option string is not specified, and id query was true, then the correct */
    /* model is extracted from the id query. If not, model == AVR6552_VAL_MODEL_R6552 */
    /* by default.                                                                    */
    checkErr( avr6552_InitAttributes (vi, model));      
    
    if (reset) /*- Reset instrument -----------------------------------------*/ 
        checkErr( avr6552_reset (vi));
    else  /*- Send Default Instrument Setup ---------------------------------*/
        checkErr( avr6552_DefaultInstrSetup (vi));   

    checkErr( avr6552_CheckStatus (vi));

Error:
    if (error < VI_SUCCESS)
        {
        if (!Ivi_Simulating (vi) && io)
            viClose (io);
        }
    return error;
}

/*****************************************************************************
 * Function: avr6552_close                                                           
 * Purpose:  This function closes the instrument.                            
 *
 *           Note:  This function must unlock the session before calling
 *           Ivi_Dispose.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_close (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    checkErr( avr6552_IviClose (vi));

Error:    
    Ivi_UnlockSession (vi, VI_NULL);
    Ivi_Dispose (vi);  

    return error;
}

/*****************************************************************************
 * Function: avr6552_IviClose                                                        
 * Purpose:  This function performs all of the drivers clean-up operations   
 *           except for closing the IVI session.  This function is called by 
 *           avr6552_close or by an IVI class driver. 
 *
 *           Note:  This function must close the I/O session and set 
 *           IVI_ATTR_IO_SESSION to 0.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_IviClose (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    ViSession io = VI_NULL;

        /* Do not lock here.  The caller manages the lock. */

    checkErr( Ivi_GetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, &io));

Error:
    Ivi_SetAttributeViSession (vi, VI_NULL, IVI_ATTR_IO_SESSION, 0, VI_NULL);
    if(io)                                                      
        {
        viClose (io);
        }
    return error;   
}

/*****************************************************************************
 * Function: avr6552_reset                                                         
 * Purpose:  This function resets the instrument.                          
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_reset (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (!Ivi_Simulating(vi))                /* call only when locked */
        {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */
        
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        viCheckErr( viPrintf (io, "*RST"));
        }
    
    checkErr( avr6552_DefaultInstrSetup (vi));                                
    
    checkErr( avr6552_CheckStatus (vi));                                      

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_self_test                                                       
 * Purpose:  This function executes the instrument self-test and returns the 
 *           result.                                                         
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_self_test (ViSession vi, ViInt16 *testResult, 
                                      ViChar testMessage[])
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     testIndex;
    ViChar      tempMsg[BUFFER_SIZE];
    
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (testResult == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Test Result");
    if (testResult == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Test Message");

    if (!Ivi_Simulating(vi))                /* call only when locked */
        {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */
    
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        for (testIndex=1; testIndex<25; testIndex++)
          {
          viCheckErr( viPrintf (io, "TST%d", testIndex));
          viCheckErr( viPrintf (io, "*TST?"));
          viCheckErr( viScanf (io, "%256[^\n]", tempMsg));
          strcat(testMessage, tempMsg);
          if (strstr(tempMsg, "FAIL"))
             *testResult = 1; /*selt-test failed*/
          else
             *testResult = 0; /*selt-test passed*/
          }
        }
    else 
        {
            /* Simulate Self Test */
        *testResult = 0;
        strcpy (testMessage, "No error.");
        }
    
    checkErr( avr6552_CheckStatus (vi));

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_error_query                                                     
 * Purpose:  This function queries the instrument error queue and returns   
 *           the result.                                                     
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_error_query (ViSession vi, ViInt32 *errCode, 
                                        ViChar errMessage[])
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    if (errCode == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error Code");
    if (errMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

    if (!Ivi_Simulating(vi))                /* call only when locked */
        {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */
    
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
        viCheckErr( viPrintf(io, "*ESR?"));
    
        viCheckErr( viScanf (io, "%ld", errCode));
        
        errMessage[0] = 0;
    
        }
    else
        {
            /* Simulate Error Query */
        *errCode = 0;
        strcpy (errMessage, "No error.");
        }

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_error_message                                                  
 * Purpose:  This function translates the error codes returned by this       
 *           instrument driver into user-readable strings.  
 *
 *           Note:  The caller can pass VI_NULL for the vi parameter.  This 
 *           is useful if one of the init functions fail.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_error_message (ViSession vi, ViStatus errorCode,
                                          ViChar errorMessage[256])
{
    ViStatus    error = VI_SUCCESS;
    
    static      IviStringValueTable errorTable = 
        {
            IVIDMM_ERROR_CODES_AND_MSGS,
            {VI_NULL,                               VI_NULL}
        };
        
    if (vi)
        Ivi_LockSession(vi, VI_NULL);

        /* all VISA and IVI error codes are handled as well as codes in the table */
    if (errorMessage == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Error Message");

    checkErr( Ivi_GetSpecificDriverStatusDesc(vi, errorCode, errorMessage, errorTable));

Error:
    if (vi)
        Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_revision_query                                                  
 * Purpose:  This function returns the driver and instrument revisions.      
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_revision_query (ViSession vi, ViChar driverRev[], 
                                           ViChar instrRev[])
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (driverRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Driver Revision");
    if (instrRev == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Instrument Revision");

    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, AVR6552_ATTR_SPECIFIC_DRIVER_REVISION, 
                                        0, 256, driverRev));
    checkErr( Ivi_GetAttributeViString (vi, VI_NULL, AVR6552_ATTR_INSTRUMENT_FIRMWARE_REVISION, 
                                        0, 256, instrRev));
                                        
    checkErr( avr6552_CheckStatus (vi));

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}
/*****************************************************************************
 * Function: avr6552_ConfigureMeasurement                                                       
 * Purpose:  Configures the common attributes of the DMM.  These attributes 
 *           are AVR6552_ATTR_FUNCTION, 
 *               AVR6552_ATTR_RANGE, and
 *               AVR6552_ATTR_RESOLUTION_ABSOLUTE 
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureMeasurement (ViSession vi, ViInt32 measFunction, 
                                                ViReal64 range, ViReal64 resolution) 
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
        /* Set attributes: */
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION,   
                                          0, measFunction), 2, "Measurement Function");  
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_RANGE,      
                                           0, range), 3, "Range");  
 
    if (range != AVR6552_VAL_AUTO_RANGE_ON)
        viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_RESOLUTION_ABSOLUTE, 
                                               0, resolution), 4, "Resolution");  

    checkErr( avr6552_CheckStatus (vi));
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_ConfigureACBandwidth                                                       
 * Purpose:  Configures additional attributes for AC Voltage or AC current
 *           measurements.  These attributes are
 *           AVR6552_ATTR_AC_MIN_FREQ and 
 *           AVR6552_ATTR_AC_MAX_FREQ
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureACBandwidth (ViSession vi, ViReal64 minFreq,
                                                ViReal64 maxFreq) 
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
        /* Set attributes: */
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_AC_MIN_FREQ, 
                                           0, minFreq), 2, "AC Min Frequency");  
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_AC_MAX_FREQ, 
                                           0, maxFreq), 3, "AC Max Frequency");  

    checkErr( avr6552_CheckStatus (vi));
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_ConfigureFrequencyVoltageRange                                                       
 * Purpose:  Configures additional attributes for frequency
 *           measurements.  This attribute is  
 *           AVR6552_ATTR_FREQ_VOLTAGE_RANGE 
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureFrequencyVoltageRange (ViSession vi,
                                                          ViReal64 freqVoltageRange) 
{
      /* Set attribute: */
    return Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_FREQ_VOLTAGE_RANGE,
                                        IVI_VAL_DIRECT_USER_CALL, freqVoltageRange);  

}

/*****************************************************************************
 * Function: avr6552_ConfigureRippleVoltsFilter                                                         
 * Purpose:  This function configures the common attributes of ripple voltage
 *           measurement for the DMM.  These attributes are
 *           AVR6552_ATTR_HIGH_PASS_FILTER, and
 *           AVR6552_ATTR_LOW_PASS_FILTER
 *
 *           Note:  Only AVR6552R supports this function.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureRippleVoltsFilter (ViSession vi,
                                                      ViInt32 highPassFilter,
                                                      ViInt32 lowPassFilter)
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession(vi, VI_NULL));
    
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_HIGH_PASS_FILTER,
                                          0, highPassFilter), 2, "High Pass Filter");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_LOW_PASS_FILTER,
                                          0, lowPassFilter), 3, "Low Pass Filter");
   
    checkErr( avr6552_CheckStatus (vi)); 
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);  
    return error;
}

/*****************************************************************************
 * Function: avr6552_ConfigureTrigger                                                       
 * Purpose:  Configures the common DMM trigger attributes.  These attributes 
 *           are AVR6552_ATTR_TRIGGER_SOURCE and
 *               AVR6552_ATTR_TRIGGER_DELAY.                                             
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureTrigger (ViSession vi, ViInt32 triggerSource, 
                                            ViReal64 triggerDelay)
{
    ViStatus    error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
        /* Set attributes: */
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_TRIGGER_SOURCE, 
                                          0, triggerSource), 2, "Trigger Source");  
    viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_TRIGGER_DELAY,  
                                       0, triggerDelay), 3, "Trigger Delay");  

    checkErr( avr6552_CheckStatus (vi));
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_ConfigureAutoZeroMode                                                       
 * Purpose:  Configures the auto zero capability of the DMM. 
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureAutoZeroMode (ViSession vi,
                                                 ViInt32 autoZeroMode) 

{
        /* Set attribute: */
    return Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_AUTO_ZERO, 
                                          IVI_VAL_DIRECT_USER_CALL, autoZeroMode);  
}

/*****************************************************************************
 * Function: avr6552_ConfigureSampleMode                                               
 * Purpose:  This function configures the sample mode and the delay of the
 *           instrument.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureSampleMode (ViSession vi,
                                               ViInt32 sampleMode,
                                               ViReal64 delay)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession (vi, VI_NULL));
        /* Set attributes: */
    viCheckParm ( Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_SAMPLE_MODE,
                                    0, sampleMode), 2, "Sample Mode");
    viCheckParm ( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_DELAY,
                                    0, delay), 3, "Delay");
                                    
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_ConfigureHold                                                         
 * Purpose:  This function configures the sample count for the HOLD sample
 *           mode.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureHold (ViSession vi,
                                         ViInt32 sampleCount)
                                          
{
        /* Set attribute: */
    return Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_HOLD_SAMPLE_COUNT,
                                    IVI_VAL_DIRECT_USER_CALL, sampleCount);
}
/*****************************************************************************
 * Function: avr6552_ConfigureBurst                                                         
 * Purpose:  This function configures the attributes for measurements in
 *           burst sample mode. These attributes are AVR6552_ATTR_BURST_COUNT
 *           and AVR6552_ATTR_DELAYED_TRIGGER           
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureBurst (ViSession vi,
                                          ViInt32 burstCount,
                                          ViInt32 delayedTrigger)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));
    
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_BURST_COUNT,
                                          0, burstCount), 2, "Burst Count");
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_DELAYED_TRIGGER,
                                          0, delayedTrigger), 3, "Delayed Trigger"); 
                                          
    checkErr( avr6552_CheckStatus (vi)); 
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);  
    return error;
}

/*****************************************************************************
 * Function: avr6552_ConfigureNull                                                         
 * Purpose:  This function configures the attributes for the null calculation.
 *           These attributes are  AVR6552_ATTR_NULL_ENABLED and 
 *           AVR6552_ATTR_NULL_OFFSET.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureNull (ViSession vi,
                                         ViBoolean enable,
                                         ViReal64 nullOffset)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));
  
    viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, AVR6552_ATTR_NULL_ENABLED,
                                            0, enable), 2, "Enable");
    if (enable)
        viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_NULL_OFFSET,
                                               0, nullOffset), 3, "Null Offset");

    checkErr( avr6552_CheckStatus (vi)); 
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);  
    return error;
}

/*****************************************************************************
 * Function: avr6552_ConfigureSmooth                                                         
 * Purpose:  This function configures the attributes for the smoothing 
 *           calculation. These attributes are  AVR6552_ATTR_SMOOTH_ENABLED and 
 *           AVR6552_ATTR_SMOOTH_COUNT.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureSmooth (ViSession vi,
                                           ViBoolean enable,
                                           ViInt32 smoothCount)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));
  
    viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, AVR6552_ATTR_SMOOTH_ENABLED,
                                            0, enable), 2, "Enable");
    if (enable)
        viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_SMOOTH_COUNT,
                                               0, smoothCount), 3, "Smooth Count");

    checkErr( avr6552_CheckStatus (vi)); 
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);  
    return error;
}

/*****************************************************************************
 * Function: avr6552_ConfigureCompare                                                         
 * Purpose:  This function configures the attributes for the compare 
 *           calculation. These attributes are AVR6552_ATTR_COMPARE_ENABLED, 
 *           AVR6552_ATTR_COMPARE_HIGH_VAL, and
 *           AVR6552_ATTR_COMPARE_LOW_VAL  
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureCompare (ViSession vi,
                                            ViBoolean enable,
                                            ViReal64 highValue,
                                            ViReal64 lowValue)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));
    
    viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, AVR6552_ATTR_COMPARE_ENABLED,
                                            0, enable), 2, "Enable");
    if (enable)
        {
        viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_COMPARE_LOW_VAL,
                                              0, lowValue), 4, "Low Value");                                          
        viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_COMPARE_HIGH_VAL,
                                              0, highValue), 3, "High Value");
        }
        
    checkErr( avr6552_CheckStatus (vi)); 
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);  
    return error;
}

/*****************************************************************************
 * Function: avr6552_ConfigureScale                                                         
 * Purpose:  This function configures the attributes for the scale calculation.
 *           These attributes are AVR6552_ATTR_SCALE_A,
 *           AVR6552_ATTR_SCALE_B, and
 *           AVR6552_ATTR_SCALE_C
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureScale (ViSession vi,
                                          ViBoolean enable,
                                          ViReal64 constantA,
                                          ViReal64 constantB,
                                          ViReal64 constantC)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));
    
    viCheckParm( Ivi_SetAttributeViBoolean (vi, VI_NULL, AVR6552_ATTR_SCALE_ENABLED,
                                            0, enable), 2, "Enable");
    if (enable)
        {
        viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_SCALE_A,
                                              0, constantA), 3, "Constant A");
        viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_SCALE_B,
                                              0, constantB), 4, "Constant B");                                          
        viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_SCALE_C,
                                              0, constantC), 5, "Constant C"); 
        }                                      
    checkErr( avr6552_CheckStatus (vi)); 
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);  
    return error;
}

/*****************************************************************************
 * Function: avr6552_ConfigureMaxMin                                                         
 * Purpose:  This function configures the attributes for the MAX-MIN
 *           calculation. This attribute is AVR6552_ATTR_MAX_MIN_ENABLED.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureMaxMin (ViSession vi,
                                           ViBoolean enable)
{
    return Ivi_SetAttributeViBoolean (vi, VI_NULL, AVR6552_ATTR_MAX_MIN_ENABLED,
                                            0, enable);
}

/*****************************************************************************
 * Function: avr6552_ConfigureDecibel                                                         
 * Purpose:  This function configures the attributes for the dB/dBm 
 *           calculation. These attributes are the AVR6552_ATTR_DB_MODE and  
 *           the AVR6552_ATTR_DB_CONSTANT.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureDecibel (ViSession vi,
                                            ViInt32 mode,
                                            ViReal64 decibelValue)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_DB_MODE,
                                     0, mode), 3, "Mode");
    if (mode != AVR6552_VAL_DB_OFF)
        viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_DB_CONSTANT,
                                         0, decibelValue), 3, "Decibel Value");

    checkErr( avr6552_CheckStatus (vi)); 
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);  
    return error;
}

/*****************************************************************************
 * Function: avr6552_ConfigureSampleRate                                                         
 * Purpose:  This function configures the common attributes of sample rate
 *           for the DMM.  These attributes include the sample rate and
 *           integral time.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ConfigureSampleRate (ViSession vi,
                                               ViInt32 sampleRate,
                                               ViReal64 integralTime)
{
    ViStatus    error = VI_SUCCESS;
 
    checkErr( Ivi_LockSession(vi, VI_NULL));
    
    viCheckParm( Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_SAMPLE_RATE,
                                          0, sampleRate), 2, "Sample Rate");
    
    if (sampleRate == AVR6552_VAL_LONGIT)
        viCheckParm( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_INTEGRAL_TIME,
                                          0, integralTime), 3, "Integral Time");
   
    checkErr( avr6552_CheckStatus (vi)); 
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);  
    return error;
}

/*****************************************************************************
 * Function: avr6552_Read                                                            
 * Purpose:  Initiates a measurement and fetches the result.                 
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_Read (ViSession vi, ViInt32 maxTime, ViReal64 *reading)
{
    ViStatus    error = VI_SUCCESS;
    ViStatus    overRange = VI_SUCCESS;
    
    checkErr( Ivi_LockSession(vi, VI_NULL));
    
    IviDmm_InterchangeCheck(vi, "avr6552_Read");

    if (reading == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Reading");
    
    checkErr( avr6552_Initiate (vi));
    
    checkWarn (overRange = avr6552_Fetch (vi, maxTime, reading));
        
    checkErr( avr6552_CheckStatus (vi));
        
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    if (error < VI_SUCCESS)
        return error;
    else
        return overRange;
}
    
/*****************************************************************************
 * Function: avr6552_ReadHoldMeasurement                                                            
 * Purpose:  Initiates a measurement and fetches the result.                 
 *****************************************************************************/
ViStatus _VI_FUNC  avr6552_ReadHoldMeasurement (ViSession vi,
                                                ViInt32 maxTime,
                                                ViReal64 *reading)
{
    ViStatus    error = VI_SUCCESS;
    ViStatus    overRange = VI_SUCCESS;
    
    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (reading == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Reading");
    
    checkErr( avr6552_ForceTrigger (vi));
    
    checkWarn (overRange = avr6552_Fetch (vi, maxTime, reading));
        
    checkErr( avr6552_CheckStatus (vi));
        
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    if (error < VI_SUCCESS)
        return error;
    else
        return overRange;
}

/*****************************************************************************
 * Function: avr6552_Initiate                                                        
 * Purpose:  Initates a measurement.                                         
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_Initiate (ViSession vi)
{
    /* The acr6552 DMM doesn't support an idle state. */
    IviDmm_InterchangeCheck(vi, "avr6552_Initiate");
    return VI_SUCCESS;
}

/*****************************************************************************
 * Function: avr6552_Abort                                                           
 * Purpose:  Aborts a measurement.                                           
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_Abort (ViSession vi)
{
    /* The acr6552 DMM doesn't support an idle state. */
    return VI_SUCCESS;
}

/*****************************************************************************
 * Function: avr6552_ForceTrigger                                                   
 * Purpose:  Sends a command to trigger the instrument.                      
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ForceTrigger (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    checkErr( Ivi_LockSession(vi, VI_NULL));

    if (!Ivi_Simulating(vi))                /* call only when locked */
        {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */
    
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
                  
        viCheckErr( viWrite (io, "*TRG", 4, VI_NULL));                     
        }

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_Fetch                                                           
 * Purpose:  This function returns the measured value from a previously 
 *           initiated measurement.  This function does not trigger the 
 *           instrument.
 *
 *           After this function executes, the value in *readingRef 
 *           is an actual reading or a value indicating that an over-range 
 *           condition occurred.  If an over-range condition occurs, the
 *           function sets *readingRef to IVI_VAL_NAN and returns 
 *           AVR6552_WARN_OVER_RANGE.  
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_Fetch (ViSession vi, ViInt32 maxTime, ViReal64 *readingRef)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViReal64    reading;
    ViBoolean   overRange = VI_FALSE;
    ViSession   io = VI_NULL;
    ViUInt32    oldTimeout = 0;
    ViBoolean   needToRestoreTimeout = VI_FALSE;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (readingRef == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for Reading");
    
    if (!Ivi_Simulating(vi))    /* call only when locked */ 
        {
        io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

            /* Store the old timeout so that it can be restored later. */ 
        viCheckErr( viGetAttribute (io, VI_ATTR_TMO_VALUE, &oldTimeout));      
        viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, maxTime));
        needToRestoreTimeout = VI_TRUE;

        error = viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL);                               
        
        if (error == VI_ERROR_TMO)
            error = AVR6552_ERROR_MAX_TIME_EXCEEDED;
        viCheckErr(error);

        Scan (rdBuffer, "%s>%f", &reading);
            /* Test for over range. */
        if (reading == 9.900E+37) 
            {
            *readingRef = IVI_VAL_NAN;
            overRange = VI_TRUE;
            }
        else
            *readingRef = reading;
        }                                                                          
    else 
        {   /* Simulate measurement data */
        ViReal64    range;
        ViInt32     measFunc;

        checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION, 0, &measFunc));
        if ((measFunc != AVR6552_VAL_DIODE) && (measFunc != AVR6552_VAL_FREQ))
            {
            checkErr (Ivi_GetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_RANGE, 
                                            0, &range));
            if (range <= 0.0)       /* If auto-ranging, use the max value. */
            checkErr (Ivi_GetAttrMinMaxViReal64 (vi, VI_NULL, AVR6552_ATTR_RANGE, 
                                                 VI_NULL, &range, VI_NULL, VI_NULL));
            }
        else
            range = 1000;
            
        *readingRef = range * ((ViReal64)rand() / (ViReal64)RAND_MAX);
        }
    
Error:
    if (needToRestoreTimeout)
        {
            /* Restore the original timeout */
        viSetAttribute (io, VI_ATTR_TMO_VALUE, oldTimeout);                    
        }
    
    Ivi_UnlockSession (vi, VI_NULL);

    if (overRange && (error >= VI_SUCCESS))
        return AVR6552_WARN_OVER_RANGE;
    else
        return error;
}       

/*****************************************************************************
 * Function: avr6552_ReadBurstMultiPoint                                 
 * Purpose:  This reads multiple points from the instrument for burst sample.
 *           This function first triggers the DMM and then fetches the data
 *           from the instrument's internal memory.
 *****************************************************************************/ 
ViStatus _VI_FUNC avr6552_ReadBurstMultiPoint (ViSession vi,
                                               ViInt32 maxTime,
                                               ViInt32 arraySize,
                                               ViReal64 readingArray[],
                                               ViInt32 *actualPts)
{
    ViStatus    error = VI_SUCCESS;
    ViStatus    overRange = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    if (readingArray == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Reading Array");

    checkErr( avr6552_ForceTrigger (vi));     
    
    checkWarn (overRange = avr6552_FetchBurstMultiPoint (vi, maxTime, arraySize, 
                                                    readingArray, actualPts));

    checkErr( avr6552_CheckStatus (vi));
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    if (error < VI_SUCCESS)
        return error;
    else
        return overRange;
}

/*****************************************************************************
 * Function: avr6552_FetchBurstMultiPoint                                                 
 * Purpose:  Fetches multiple readings from a previously triggered burst 
 *           measurement.
 *           The number of measurements the DMM will take is determined by
 *           the value of the AVR6552_BURST_COUNT attribute.
 *           This function does not trigger the instrument.
 *           After this function executes, each value in readingArray 
 *           is an actual reading or a value indicating that an over-range 
 *           condition occurred.  If an over-range condition occurs, the
 *           function sets the corresponding value in readingArray to 
 *           IVI_VAL_NAN and returns AVR6552_WARN_OVER_RANGE.  
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_FetchBurstMultiPoint (ViSession vi,
                                                ViInt32 maxTime,
                                                ViInt32 arraySize,
                                                ViReal64 readingArray[],
                                                ViInt32 *actualPts)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   overRange = VI_FALSE;
    ViSession   io = VI_NULL;
    ViUInt32    oldTimeout = 0;
    ViBoolean   needToRestoreTimeout = VI_FALSE;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (readingArray == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Reading Array");

    if (!Ivi_Simulating(vi))                /* call only when locked */ 
        {
        ViInt32   i, burstCount;
        ViInt16   isReady = 0;
        ViReal64  tmpBuffer[10000];
        
        io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));

            /* Store the old timeout so that it can be restored later */
        viCheckErr( viGetAttribute (io, VI_ATTR_TMO_VALUE, &oldTimeout));      
        viCheckErr( viSetAttribute (io, VI_ATTR_TMO_VALUE, maxTime));
        needToRestoreTimeout = VI_TRUE;
        
        while (!(isReady & 0x01))  /*Wait until measurement is over */     
            viCheckErr (viQueryf (io, "*stb?", "%hd", &isReady));  
            
           /* Assumes there is at least one value in the instrument's memory to read */
        checkErr( Ivi_GetAttributeViInt32 (vi, "", AVR6552_ATTR_BURST_COUNT, VI_NULL, &burstCount));

        /* recalls the first burstCount-1 points from internal memory */
        viCheckErr( viPrintf (io, "IRD0,%d", burstCount-1));
        viCheckErr( viPrintf (io, "IRO?"));
        
        error = viScanf(io, "%,10000lf", tmpBuffer);
        if (error == VI_ERROR_TMO)
            error = AVR6552_ERROR_MAX_TIME_EXCEEDED;
        viCheckErr (error);

        viCheckErr( viQueryf (io, "IRPO?", "%*c%*c%*c%*c%d", actualPts));

        if (*actualPts > arraySize)
            *actualPts = arraySize ;
        
            /* Fill in array and check for overrange */
        for (i = 0; i < *actualPts; i++)
            {
            readingArray[i] = tmpBuffer[i];
            if ((-9.9E37 >= readingArray[i]) || (readingArray[i] >= 9.9E37))
                {   
                readingArray[i] = IVI_VAL_NAN;
                overRange = VI_TRUE;
                }
            }
        }
    else 
        {   /* Simulate measurement data */
        ViInt32 i;
        ViReal64 range;
        
        checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_RANGE, 0, &range));
        for (i=0; i < arraySize; i++)
            readingArray[i] = range * (rand() / RAND_MAX);
        }
    
        
Error:
    if (needToRestoreTimeout)
        {
            /* Restore the original timeout */
        viSetAttribute (io, VI_ATTR_TMO_VALUE, oldTimeout);                    
        }

    Ivi_UnlockSession(vi, VI_NULL);

    if (overRange && (error >= VI_SUCCESS))
        return AVR6552_WARN_OVER_RANGE;
    else
        return error;
}
    
/*****************************************************************************
 * Function: avr6552_IsOverRange
 * Purpose:  This function takes a measurement value that you obtain from one
 *           of the Measure, Read, or Fetch functions and determines if the
 *           value is a valid measurement value or a value indicating an
 *           over-range condition occurred.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_IsOverRange (ViSession vi, ViReal64 measurementValue,
                                        ViBoolean *isOverRange)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     type;
    
    checkErr( Ivi_LockSession(vi, VI_NULL));
    
    if (isOverRange == VI_NULL)
        viCheckParm( IVI_ERROR_INVALID_PARAMETER, 3, "Null address for OverRange status.");

    viCheckErr( Ivi_GetViReal64Type (measurementValue, &type));
    
    if (type == IVI_VAL_TYPE_NAN)
        *isOverRange = VI_TRUE;
    else
        *isOverRange = VI_FALSE;

Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_QueryMaxMin                                    
 * Purpose:  This function returns the maximum value, minimum value, average
 *           value and measurement cycle count while the DMM is performing
 *           the max-min calculation.                                        
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_QueryMaxMin (ViSession vi,
                                       ViReal64 *maxValue, ViReal64 *minValue,
                                       ViReal64 *averageValue,
                                       ViReal64 *cycleCount)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViSession   io = VI_NULL;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));

    if (!Ivi_Simulating(vi))    /* call only when locked */ 
        {
        io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
          /*Query max-min parameters*/
        viCheckErr (viQueryf (io, "MAX?", "%s", rdBuffer));
        Scan (rdBuffer, "%s[i3]>%f", maxValue);
        viCheckErr (viQueryf (io, "MIN?", "%s", rdBuffer));
        Scan (rdBuffer, "%s[i3]>%f", minValue);
        viCheckErr (viQueryf (io, "AVE?", "%s", rdBuffer));
        Scan (rdBuffer, "%s[i3]>%f", averageValue);
        viCheckErr (viQueryf (io, "AVN?", "%s", rdBuffer));
        Scan (rdBuffer, "%s[i3]>%f", cycleCount);
        }                                                                          

Error:
    Ivi_UnlockSession(vi, VI_NULL);   
    return error;
}

/*****************************************************************************
 * Function: avr6552_GetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to get 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession attributes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_GetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 *value)
{                                                                                                           
    return Ivi_GetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC avr6552_GetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 *value)
{                                                                                                           
    return Ivi_GetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC avr6552_GetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViInt32 bufSize, 
                                                 ViChar value[]) 
{   
    return Ivi_GetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     bufSize, value);
}   
ViStatus _VI_FUNC avr6552_GetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean *value)
{                                                                                                           
    return Ivi_GetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC avr6552_GetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession *value)
{                                                                                                           
    return Ivi_GetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: avr6552_SetAttribute<type> Functions                                    
 * Purpose:  These functions enable the instrument driver user to set 
 *           attribute values directly.  There are typesafe versions for 
 *           ViInt32, ViReal64, ViString, ViBoolean, and ViSession datatypes.                                         
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_SetAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_SetAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                    value);
}                                                                                                           
ViStatus _VI_FUNC avr6552_SetAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_SetAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}                                                                                                           
ViStatus _VI_FUNC avr6552_SetAttributeViString (ViSession vi, ViConstString channelName, 
                                                 ViAttr attributeId, ViConstString value) 
{   
    return Ivi_SetAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                     value);
}   
ViStatus _VI_FUNC avr6552_SetAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_SetAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           
ViStatus _VI_FUNC avr6552_SetAttributeViSession (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_SetAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}                                                                                                           

/*****************************************************************************
 * Function: avr6552_CheckAttribute<type> Functions                                  
 * Purpose:  These functions enable the instrument driver user to check  
 *           attribute values directly.  These functions check the value you
 *           specify even if you set the AVR6552_ATTR_RANGE_CHECK 
 *           attribute to VI_FALSE.  There are typesafe versions for ViInt32, 
 *           ViReal64, ViString, ViBoolean, and ViSession datatypes.                         
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_CheckAttributeViInt32 (ViSession vi, ViConstString channelName, 
                                                  ViAttr attributeId, ViInt32 value)
{                                                                                                           
    return Ivi_CheckAttributeViInt32 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                      value);
}
ViStatus _VI_FUNC avr6552_CheckAttributeViReal64 (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViReal64 value)
{                                                                                                           
    return Ivi_CheckAttributeViReal64 (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}                                                                                                           
ViStatus _VI_FUNC avr6552_CheckAttributeViString (ViSession vi, ViConstString channelName, 
                                                   ViAttr attributeId, ViConstString value)  
{   
    return Ivi_CheckAttributeViString (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                       value);
}   
ViStatus _VI_FUNC avr6552_CheckAttributeViBoolean (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViBoolean value)
{                                                                                                           
    return Ivi_CheckAttributeViBoolean (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           
ViStatus _VI_FUNC avr6552_CheckAttributeViSession (ViSession vi, ViConstString channelName, 
                                                    ViAttr attributeId, ViSession value)
{                                                                                                           
    return Ivi_CheckAttributeViSession (vi, channelName, attributeId, IVI_VAL_DIRECT_USER_CALL, 
                                        value);
}                                                                                                           

/*****************************************************************************
 * Function: avr6552_GetNextCoercionRecord                        
 * Purpose:  This function enables the instrument driver user to obtain
 *           the coercion information associated with the IVI session.                                                              
 *           This function retrieves and clears the oldest instance in which 
 *           the instrument driver coerced a value the instrument driver user
 *           specified to another value.                     
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_GetNextCoercionRecord (ViSession vi,
                                                  ViInt32 bufferSize,
                                                  ViChar  record[])
{
    return Ivi_GetNextCoercionString (vi, bufferSize, record);
}

/*****************************************************************************
 * Function: avr6552_LockSession and avr6552_UnlockSession Functions                        
 * Purpose:  These functions enable the instrument driver user to lock the 
 *           session around a sequence of driver calls during which other
 *           execution threads must not disturb the instrument state.
 *                                                                          
 *           NOTE:  The callerHasLock parameter must be a local variable 
 *           initialized to VI_FALSE and passed by reference, or you can pass 
 *           VI_NULL.                     
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_LockSession (ViSession vi, ViBoolean *callerHasLock)  
{                                              
    return Ivi_LockSession(vi,callerHasLock);      
}                                              
ViStatus _VI_FUNC avr6552_UnlockSession (ViSession vi, ViBoolean *callerHasLock) 
{                                              
    return Ivi_UnlockSession(vi,callerHasLock);    
}   

/*****************************************************************************
 * Function: avr6552_GetErrorInfo and avr6552_ClearErrorInfo Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_GetErrorInfo (ViSession vi, ViStatus *primaryError, 
                                         ViStatus *secondaryError, ViChar errorElaboration[256])  
{                                                                                                           
    return Ivi_GetErrorInfo(vi, primaryError, secondaryError, errorElaboration);                                
}                                                                                                           
ViStatus _VI_FUNC avr6552_ClearErrorInfo (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}

/*****************************************************************************
 * Function: WriteInstrData and ReadInstrData Functions                      
 * Purpose:  These functions enable the instrument driver user to  
 *           write and read commands directly to and from the instrument.            
 *                                                                           
 *           Note:  These functions bypass the IVI attribute state caching.  
 *                  WriteInstrData invalidates the cached values for all 
 *                  attributes.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_WriteInstrData (ViSession vi, ViConstString writeBuffer)   
{   
    return Ivi_WriteInstrData (vi, writeBuffer);    
}   
ViStatus _VI_FUNC avr6552_ReadInstrData (ViSession vi, ViInt32 numBytes, 
                                          ViChar rdBuf[], ViInt32 *bytesRead)  
{   
    return Ivi_ReadInstrData (vi, numBytes, rdBuf, bytesRead);   
} 

/*****************************************************************************
 * Function: avr6552_GetError and avr6552_ClearError Functions                       
 * Purpose:  These functions enable the instrument driver user to  
 *           get or clear the error information the driver associates with the
 *           IVI session.                                                        
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_GetError (ViSession vi, 
                                     ViStatus *errorCode, 
                                     ViInt32 bufferSize,
                                     ViChar description[])  
{
    ViStatus error = VI_SUCCESS;
    ViStatus primary = VI_SUCCESS,
             secondary = VI_SUCCESS;
    ViChar   elaboration[256] = "";
    ViChar   errorMessage[1024] = "";
    ViChar  *appendPoint = errorMessage;
    ViInt32  remaining = 0;
    ViInt32  written = 0;
    ViInt32  actualSize = 0;
    ViBoolean locked = VI_FALSE;

    /* Lock Session */
    if (vi != 0)
        {
        checkErr( Ivi_LockSession(vi, &locked));
        }

    /* Test for nulls and acquire error data */
    if (bufferSize != 0)
        {
        if (errorCode == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 2, "Null address for Error");
            }
        if (description == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Description");
            }
        checkErr( Ivi_GetErrorInfo (vi, &primary, &secondary, elaboration));
        }

    else
        {
        checkErr( Ivi_GetAttributeViString(vi, VI_NULL, IVI_ATTR_ERROR_ELABORATION, 0, 256, elaboration));
        checkErr( Ivi_GetAttributeViInt32(vi, VI_NULL, IVI_ATTR_SECONDARY_ERROR, 0, &secondary));
        checkErr( Ivi_GetAttributeViInt32(vi, VI_NULL, IVI_ATTR_PRIMARY_ERROR, 0, &primary));
        }
        
    /* Format data */
    if (primary != VI_SUCCESS)
        {
        ViChar msg[256] = "";
        checkErr( avr6552_error_message (vi, primary, msg));
        remaining = (ViInt32)(sizeof(errorMessage) - (appendPoint - errorMessage));
        if (remaining > 0)
            {
            written = snprintf(appendPoint, (size_t)remaining, "Primary Error: (Hex 0x%08X) %s\n", primary, msg);
            if (written < 0)
                written = 0;
            else if (written >= remaining)
                written = remaining - 1;
            appendPoint += written;
            }
        }
    
    if (secondary != VI_SUCCESS)
        {
        ViChar msg[256] = "";
        checkErr( avr6552_error_message (vi, secondary, msg));
        remaining = (ViInt32)(sizeof(errorMessage) - (appendPoint - errorMessage));
        if (remaining > 0)
            {
            written = snprintf(appendPoint, (size_t)remaining, "Secondary Error: (Hex 0x%08X) %s\n", secondary, msg);
            if (written < 0)
                written = 0;
            else if (written >= remaining)
                written = remaining - 1;
            appendPoint += written;
            }
        }
    
    if (elaboration[0])
        {
        remaining = (ViInt32)(sizeof(errorMessage) - (appendPoint - errorMessage));
        if (remaining > 0)
            {
            written = snprintf(appendPoint, (size_t)remaining, "Elaboration: %s", elaboration);
            if (written < 0)
                written = 0;
            else if (written >= remaining)
                written = remaining - 1;
            appendPoint += written;
            }
        }
    
    actualSize = (ViInt32)(strlen(errorMessage) + 1);
    
    /* Prepare return values */
    if (bufferSize == 0)
        {
        error = actualSize;
        }
    else 
        {
        if (bufferSize > 0)
            {
            if (actualSize > bufferSize)
                {
                error = actualSize;
                actualSize = bufferSize;
                }
            }
        memcpy(description, errorMessage, actualSize-1);
        description[actualSize-1] = '\0';
        }
    
    if (errorCode) 
        {
        *errorCode = primary;
        }
    
Error:
    /* Unlock Session */
    Ivi_UnlockSession (vi, &locked);
    return error;
}   

ViStatus _VI_FUNC avr6552_ClearError (ViSession vi)                                                        
{                                                                                                           
    return Ivi_ClearErrorInfo (vi);                                                                             
}

/**************************************************************************** 
 *  Function: avr6552_GetNextInterchangeWarning,
 *            avr6552_ResetInterchangeCheck, and
 *            avr6552_ClearInterchangeWarnings
 *  Purpose:  These functions allow the user to retrieve interchangeability
 *            warnings, reset the driver's interchangeability checking 
 *            state, and clear all previously logged interchangeability warnings.
 ****************************************************************************/
ViStatus _VI_FUNC avr6552_GetNextInterchangeWarning (ViSession vi, 
                                                      ViInt32 bufferSize, 
                                                      ViChar warnString[])
{
    return Ivi_GetNextInterchangeCheckString (vi, bufferSize, warnString);
}

ViStatus _VI_FUNC avr6552_ResetInterchangeCheck (ViSession vi)
{
    return Ivi_ResetInterchangeCheck (vi);
}

ViStatus _VI_FUNC avr6552_ClearInterchangeWarnings (ViSession vi)
{
    return Ivi_ClearInterchangeWarnings (vi);
}

/*****************************************************************************
 * Function: avr6552_InvalidateAllAttributes
 * Purpose:  This function invalidates the cached value of all attributes.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_InvalidateAllAttributes (ViSession vi)
{
    return Ivi_InvalidateAllAttributes(vi);
}

/**************************************************************************** 
 *  Function: avr6552_GetChannelName
 *  Purpose:  This function returns the highest-level channel name that
 *            corresponds to the specific driver channel string that is in
 *            the channel table at an index you specify.
 ****************************************************************************/
ViStatus _VI_FUNC avr6552_GetChannelName (ViSession vi, ViInt32 index,
                                           ViInt32 bufferSize,
                                           ViChar name[])
{
   ViStatus         error = VI_SUCCESS;
   ViConstString    channelName;
   ViInt32          actualSize = 0;

    checkErr( Ivi_LockSession (vi, VI_NULL));
    
    if (bufferSize != 0)
        {
        if (name == VI_NULL)
            {
            viCheckParm( IVI_ERROR_INVALID_PARAMETER, 4, "Null address for Channel Name");
            }
        }
    
    /* Get channel name */
    viCheckParm( Ivi_GetNthChannelString (vi, index, &channelName), 2, "Index");
    
    actualSize = (ViInt32)(strlen(channelName) + 1);
    
    /* Prepare return values */
    if (bufferSize == 0)
        {
        error = actualSize;
        }
    else 
        {
        if (bufferSize > 0)
            {
            if (actualSize > bufferSize)
                {
                error = actualSize;
                actualSize = bufferSize;
                }
            }
        memcpy(name, channelName, actualSize-1);
        name[actualSize-1] = '\0';
        }

Error:    
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_ResetWithDefaults
 * Purpose:  This function resets the instrument and applies default settings
 *           from the IVI Configuration Store based on the logical name
 *           from which the session was created.
 *****************************************************************************/
ViStatus _VI_FUNC avr6552_ResetWithDefaults (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    checkErr( avr6552_reset(vi));
    checkErr( Ivi_ApplyDefaultSetup(vi));
    
Error:
    Ivi_UnlockSession(vi, VI_NULL);
    return error;
}

/**************************************************************************** 
 *  Function: avr6552_Disable
 *  Purpose:  This function places the instrument in a quiescent state as 
 *            quickly as possible.
 ****************************************************************************/
ViStatus _VI_FUNC avr6552_Disable (ViSession vi)
{
    ViStatus error = VI_SUCCESS;
    
    checkErr( Ivi_LockSession (vi, VI_NULL));
    if (!Ivi_Simulating(vi)) {
        //Place appropriate code here to disable the instrument.
    }

Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 *-------------------- Utility Functions (Not Exported) ---------------------*
 *****************************************************************************/

/*****************************************************************************
 *  Function: avr6552_GetDriverSetupOption
 *  Purpose:  Get the value of a DriverSetup option.  The driver setup string
 *            is part of the option string the user passes to the
 *            avr6552_InitWithOptions function.  Specifically it is the part
 *            that follows "DriverSetup=".  This function assumes the 
 *            DriverSetup string has the following format:
 *              "OptionTag1 : OptionValue1; OptionTag2 : OptionValue2"
 *            Leading and trailing white space is removed from the option
 *            value and both the option tag and option value may contain
 *            white space.  The option tag is case sensitive.
 *
 *            Example:
 *
 *              "DriverSetup= Model : InstrX ; Special Option:Feature Y "
 *
 *              optionTag "Model"          returns optionValue "InstrX"
 *
 *              optionTag "Special Option" returns optionValue "Feature Y"
 *****************************************************************************/
static ViStatus avr6552_GetDriverSetupOption (ViSession vi,
                                               ViConstString optionTag,
                                               ViInt32 optionValueSize,
                                               ViChar optionValue[])
{
    ViStatus error = VI_SUCCESS;
    ViChar   setup[BUFFER_SIZE];
    ViChar*  searchOrigin;
    ViChar*  start;
    ViChar*  end;
    ViUInt32 n;

    checkErr( Ivi_LockSession (vi, VI_NULL));

    if ((optionValue != VI_NULL) && (optionValueSize > 0))
        optionValue[0] = 0;
    else
       viCheckErr (IVI_ERROR_INVALID_PARAMETER);
       
    if ((optionTag != VI_NULL) && (optionValue != VI_NULL) && (optionValueSize > 1))
        {
        checkErr( Ivi_GetAttributeViString (vi, "", IVI_ATTR_DRIVER_SETUP,
                                            0, BUFFER_SIZE, setup));
        searchOrigin = setup;
        while (searchOrigin != VI_NULL)
            {
            searchOrigin = strstr(searchOrigin, optionTag);
            if (searchOrigin)
                {
                searchOrigin += strlen(optionTag);
                start = searchOrigin;
                while (isspace(*start)) start++;
                if (*start == ':')
                    {
                    start++;
                    while (isspace(*start)) start++;
                    if (*start)
                        {
                        end = strchr (start, ';');
                        if (start != end)
                            {
                            end = (end) ? end-1 : &setup[strlen(setup)-1];
                            while (isspace(*end)) end--;
                            n = (ViInt32)(end - start + 1);
                            if (n > (unsigned int)optionValueSize - 1)
                                n =(unsigned int)optionValueSize - 1;
                            strncpy(optionValue, start, n);
                            optionValue[n] = 0;
                            break;
                            }
                        }
                    }
                }
            }
        }
    else
        viCheckErr (IVI_ERROR_INVALID_PARAMETER);
        
Error:
    Ivi_UnlockSession (vi, VI_NULL);
    return error;
}

/*****************************************************************************
 * Function: avr6552_CheckStatus                                                 
 * Purpose:  This function checks the status of the instrument to detect 
 *           whether the instrument has encountered an error.  This function  
 *           is called at the end of most exported driver functions.  If the    
 *           instrument reports an error, this function returns      
 *           IVI_ERROR_INSTR_SPECIFIC.  The user can set the 
 *           IVI_ATTR_QUERY_INSTR_STATUS attribute to VI_FALSE to disable this 
 *           check and increase execution speed.                                   
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus avr6552_CheckStatus (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;

    if (Ivi_QueryInstrStatus (vi) && Ivi_NeedToCheckStatus (vi) && !Ivi_Simulating (vi))
        {
        checkErr( avr6552_CheckStatusCallback (vi, Ivi_IOSession(vi)));
        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_FALSE));
        }
        
Error:
    return error;
}

/*****************************************************************************
 * Function: avr6552_WaitForOPC                                                  
 * Purpose:  This function waits for the instrument to complete the      
 *           execution of all pending operations.  This function is a        
 *           wrapper for the WaitForOPCCallback.  It can be called from 
 *           other instrument driver functions. 
 *
 *           The maxTime parameter specifies the maximum time to wait for
 *           operation complete in milliseconds.
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus avr6552_WaitForOPC (ViSession vi, ViInt32 maxTime) 
{
    ViStatus    error = VI_SUCCESS;

    if (!Ivi_Simulating(vi))
        {
        ViInt32  oldOPCTimeout; 
        
        checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_OPC_TIMEOUT, 
                                           0, &oldOPCTimeout));
        Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_OPC_TIMEOUT,        
                                 0, maxTime);

        error = avr6552_WaitForOPCCallback (vi, Ivi_IOSession(vi));

        Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_OPC_TIMEOUT, 
                                 0, oldOPCTimeout);
        viCheckErr( error);
        }
Error:
    return error;
}

/*****************************************************************************
 * Function: avr6552_DefaultInstrSetup                                               
 * Purpose:  This function sends a default setup to the instrument.  The    
 *           avr6552_reset function calls this function.  The 
 *           avr6552_IviInit function calls this function when the
 *           user passes VI_FALSE for the reset parameter.  This function is 
 *           useful for configuring settings that other instrument driver 
 *           functions require.    
 *
 *           Note:  Call this function only when the session is locked.
 *****************************************************************************/
static ViStatus avr6552_DefaultInstrSetup (ViSession vi)
{
    ViStatus    error = VI_SUCCESS;
        
    /* Invalidate all attributes */
    checkErr( Ivi_InvalidateAllAttributes (vi));
    

    if (!Ivi_Simulating(vi))
        {
        ViSession   io = Ivi_IOSession(vi); /* call only when locked */

        checkErr( Ivi_SetNeedToCheckStatus (vi, VI_TRUE));
    
        viCheckErr( viPrintf (io, "*CLS,*ESE 1,*SRE 32"));
        /*Set instrument to header off and free sample mode*/
        viCheckErr( viPrintf (io, "H0,M0"));
        }
    
    /* Disable unused extensions */
    checkErr( Ivi_SetAttributeViInt32 (vi, "", AVR6552_ATTR_AUTO_ZERO,
                                       IVI_VAL_DONT_MARK_AS_SET_BY_USER,
                                       AVR6552_VAL_AUTO_ZERO_OFF));
    
Error:
    return error;
}

/*****************************************************************************
 * Function: ReadToFile and WriteFromFile Functions                          
 * Purpose:  Functions for instrument driver developers to read/write        
 *           instrument data to/from a file.                                 
 *****************************************************************************/
static ViStatus avr6552_ReadToFile (ViSession vi, ViConstString filename, 
                                     ViInt32 maxBytesToRead, ViInt32 fileAction, 
                                     ViInt32 *totalBytesWritten)  
{   
    return Ivi_ReadToFile (vi, filename, maxBytesToRead, fileAction, totalBytesWritten);  
}   
static ViStatus avr6552_WriteFromFile (ViSession vi, ViConstString filename, 
                                        ViInt32 maxBytesToWrite, ViInt32 byteOffset, 
                                        ViInt32 *totalBytesWritten) 
{   
    return Ivi_WriteFromFile (vi, filename, maxBytesToWrite, byteOffset, totalBytesWritten); 
}

/*****************************************************************************
 * Function: avr6552_GetCalculationCommandString                          
 * Purpose:  This function returns the appropriate command string for the 
 *           appropriate calculation attributes.                                 
 *****************************************************************************/
static ViStatus avr6552_GetCalculationCommandString (ViAttr attributeId, ViString *cmd)
{
    switch (attributeId)
    {
    case AVR6552_ATTR_NULL_ENABLED:
        *cmd = "NL";
        break;
    case AVR6552_ATTR_SMOOTH_ENABLED:
        *cmd = "SM";
        break;
    case AVR6552_ATTR_COMPARE_ENABLED:
        *cmd = "CO";
        break;
    case AVR6552_ATTR_SCALE_ENABLED:
        *cmd = "SC";
        break;
    case AVR6552_ATTR_MAX_MIN_ENABLED:
        *cmd = "MN";
        break;
    default:
        return IVI_ERROR_INVALID_VALUE;
    }    
    return VI_SUCCESS; 
}

/*****************************************************************************
 * Function: avr6552_convertAbsolutetoDigits                                               
 * Purpose:  This function converts resolution in absolute units to digits of 
 *           precision based on the current manual range of the instrument. 
 *
 *           Note: Do not call this function if your instrument is set to the 
 *           value AVR6552_VAL_AUTO_RANGE_ON.
 *****************************************************************************/
static ViStatus avr6552_convertAbsolutetoDigits(ViSession vi, ViReal64 absResolution, 
                                         ViReal64 *digits)
{
    ViStatus    error = VI_SUCCESS;
    ViReal64    range;

    checkErr (Ivi_GetAttributeViReal64 (vi, "", AVR6552_ATTR_RANGE, 0, &range));

    range = pow (10, ceil (log10(range)));
    *digits = log10(range/absResolution);

Error:
    return error;
}

/*****************************************************************************
 * Function: avr6552_convertDigitstoAbsolute                                               
 * Purpose:  This function converts resolution in digits of precision to  
 *           absolute units.
 *
 *           Note: Do not call this function if your instrument is set to the 
 *           value AVR6552_VAL_AUTO_RANGE_ON.
 *****************************************************************************/
static ViStatus avr6552_convertDigitstoAbsolute(ViSession vi, ViReal64 digits, 
                                         ViReal64 *absResolution)
{
    ViStatus    error = VI_SUCCESS;
    ViReal64    range;

    checkErr (Ivi_GetAttributeViReal64 (vi, "", AVR6552_ATTR_RANGE, 0, &range));

    range = pow (10, ceil (log10(range)));
    *absResolution = (range / pow(10,digits));

Error:
    return error;
}

/*****************************************************************************
 *------------------------ Global Session Callbacks -------------------------*
 *****************************************************************************/

/*****************************************************************************
 * Function: avr6552_CheckStatusCallback                                               
 * Purpose:  This function queries the instrument to determine if it has 
 *           encountered an error.  If the instrument has encountered an 
 *           error, this function returns the IVI_ERROR_INSTRUMENT_SPECIFIC 
 *           error code.  This function is called by the 
 *           avr6552_CheckStatus utility function.  The 
 *           Ivi_SetAttribute and Ivi_GetAttribute functions invoke this 
 *           function when the optionFlags parameter includes the
 *           IVI_VAL_DIRECT_USER_CALL flag.
 *           
 *           The user can disable calls to this function by setting the 
 *           IVI_ATTR_QUERY_INSTR_STATUS attribute to VI_FALSE.  The driver can 
 *           disable the check status callback for a particular attribute by 
 *           setting the IVI_VAL_DONT_CHECK_STATUS flag.
 *****************************************************************************/
static ViStatus _VI_FUNC avr6552_CheckStatusCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;
    
    
    /* Query instrument status */
        ViInt16     esr = 0; 
        viCheckErr( viWrite (io, "*ESR?", 5, VI_NULL));
    
        viCheckErr( viScanf (io, "%hd", &esr));
    
        if ((esr & IEEE_488_2_DEVICE_DEPENDENT_ERROR_BIT) ||
            (esr & IEEE_488_2_EXECUTION_ERROR_BIT) ||
            (esr & IEEE_488_2_COMMAND_ERROR_BIT))
                {     
                viCheckErr( IVI_ERROR_INSTR_SPECIFIC);
                }
        
Error:
    return error;
}

/*****************************************************************************
 * Function: avr6552_WaitForOPCCallback                                               
 * Purpose:  This function waits until the instrument has finished processing 
 *           all pending operations.  This function is called by the 
 *           avr6552_WaitForOPC utility function.  The IVI engine invokes
 *           this function in the following two cases:
 *           - Before invoking the read callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_BEFORE_READS flag is set.
 *           - After invoking the write callback for attributes for which the 
 *             IVI_VAL_WAIT_FOR_OPC_AFTER_WRITES flag is set.
 *****************************************************************************/
static ViStatus _VI_FUNC avr6552_WaitForOPCCallback (ViSession vi, ViSession io)
{
    ViStatus    error = VI_SUCCESS;
    
    ViInt32     opcTimeout;
    ViUInt16    statusByte;

    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_OPC_TIMEOUT, 
                                       0, &opcTimeout));

    viCheckErr( viEnableEvent (io, VI_EVENT_SERVICE_REQ, VI_QUEUE, VI_NULL));

    viCheckErr( viPrintf (io, "*OPC"));

        /* wait for SRQ */
    viCheckErr( viWaitOnEvent (io, VI_EVENT_SERVICE_REQ, opcTimeout, VI_NULL, VI_NULL));
    viCheckErr( viDisableEvent (io, VI_EVENT_SERVICE_REQ, VI_QUEUE));

        /* clean up after SRQ */
    viCheckErr( viBufWrite (io, "*CLS", 4, VI_NULL));
    viCheckErr( viReadSTB (io, &statusByte));
    
Error:
    viDiscardEvents (io, VI_EVENT_SERVICE_REQ, VI_QUEUE);
    return error;
}

/*****************************************************************************
 *----------------- Attribute Range Tables and Callbacks --------------------*
 *****************************************************************************/

/*- AVR6552_ATTR_ID_QUERY_RESPONSE -*/

static ViStatus _VI_FUNC avr6552AttrIdQueryResponse_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    ViUInt32    retCnt;
    char*       firstComma;
    char*       secondComma = VI_NULL;
    
    viCheckErr( viPrintf (io, "*IDN?"));
    viCheckErr( viRead (io, rdBuffer, BUFFER_SIZE-1, &retCnt));
    
    /* Extract the first part of the string (up to the second ',')       */
    
    firstComma = strchr (rdBuffer, ',');
    if (firstComma)
        {
        secondComma = strchr (firstComma+1, ',');
        }
    if (secondComma)
        {
        *secondComma = '\0';
        }
    else
        {
        viCheckErr (VI_ERROR_FAIL_ID_QUERY);
        }
    // remove trailing whitespace after second comma
    while ((secondComma > rdBuffer) && isspace(*--secondComma))
        {
        *secondComma = '\0';
        }

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}
    
/*- AVR6552_ATTR_SPECIFIC_DRIVER_REVISION -*/

static ViStatus _VI_FUNC avr6552AttrSpecificDriverRevision_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      driverRevision[256];
    
    
    snprintf (driverRevision,
              sizeof(driverRevision),
              "Driver: avr6552 %d.%d, Compiler: %s %3.2f, "
              "Components: IVIEngine %.2f, VISA-Spec %.2f",
              AVR6552_MAJOR_VERSION , AVR6552_MINOR_VERSION,
              IVI_COMPILER_NAME, IVI_COMPILER_VER_NUM,
              IVI_ENGINE_MAJOR_VERSION + IVI_ENGINE_MINOR_VERSION/1000.0,
              Ivi_ConvertVISAVer(VI_SPEC_VERSION));

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, driverRevision));    
Error:
    return error;
}

/*- AVR6552_ATTR_INSTRUMENT_FIRMWARE_REVISION -*/

static ViStatus _VI_FUNC avr6552AttrFirmwareRevision_ReadCallback (ViSession vi, 
                                                                    ViSession io,
                                                                    ViConstString channelName, 
                                                                    ViAttr attributeId,
                                                                    const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      idQ[BUFFER_SIZE], rdBuffer[BUFFER_SIZE];
    
    if (!Ivi_Simulating(vi))
        {
        checkErr( Ivi_GetAttributeViString (vi, "", AVR6552_ATTR_ID_QUERY_RESPONSE,
                                            0, BUFFER_SIZE, idQ));
        if (sscanf (idQ, "%*[^,],%*[^,],%*[^,],%256[^\n]", rdBuffer) != 1)
            {
            rdBuffer[0] = '\0';
            }
        }
    else
        snprintf(rdBuffer, sizeof(rdBuffer), "Firmware revision not available while simulating");
        
    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));

Error:
    return error;
}
    
/*- AVR6552_ATTR_INSTRUMENT_MANUFACTURER -*/

static ViStatus _VI_FUNC avr6552AttrInstrumentManufacturer_ReadCallback (ViSession vi, 
                                                                          ViSession io,
                                                                          ViConstString channelName, 
                                                                          ViAttr attributeId,
                                                                          const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      idQ[BUFFER_SIZE], rdBuffer[BUFFER_SIZE];
    
    checkErr( Ivi_GetAttributeViString (vi, "", AVR6552_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));

    if (sscanf (idQ, "%256[^,]", rdBuffer) != 1)
        {
        rdBuffer[0] = '\0';
        }

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}
    
/*- AVR6552_ATTR_INSTRUMENT_MODEL -*/

static ViStatus _VI_FUNC avr6552AttrInstrumentModel_ReadCallback (ViSession vi, 
                                                                   ViSession io,
                                                                   ViConstString channelName, 
                                                                   ViAttr attributeId,
                                                                   const ViConstString cacheValue)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      idQ[BUFFER_SIZE], rdBuffer[BUFFER_SIZE];
    
    checkErr( Ivi_GetAttributeViString (vi, "", AVR6552_ATTR_ID_QUERY_RESPONSE,
                                        0, BUFFER_SIZE, idQ));

    if (sscanf (idQ, "%*[^,],%256[^,]", rdBuffer) != 1)
        {
        rdBuffer[0] = '\0';
        }

    checkErr( Ivi_SetValInStringCallback (vi, attributeId, rdBuffer));
    
Error:
    return error;
}

/*- AVR6552_ATTR_FUNCTION -*/

static IviRangeTableEntry attrFunctionRangeTableEntries[] =
    {
        {AVR6552_VAL_DC_VOLTS, 0, 0, "F1", 0},
        {AVR6552_VAL_AC_VOLTS, 0, 0, "F2", 0},
        {AVR6552_VAL_DC_CURRENT, 0, 0, "F5", 0},
        {AVR6552_VAL_AC_CURRENT, 0, 0, "F6", 0},
        {AVR6552_VAL_2_WIRE_RES, 0, 0, "F3", 0},
        {AVR6552_VAL_4_WIRE_RES, 0, 0, "F4", 0},
        {AVR6552_VAL_DIODE, 0, 0, "F13", 0},
        {AVR6552_VAL_FREQ, 0, 0, "F50", 0},
        {AVR6552_VAL_AC_PLUS_DC_VOLTS, 0, 0, "F7", 0},
        {AVR6552_VAL_AC_PLUS_DC_CURRENT, 0, 0, "F8", 0},
        {AVR6552_VAL_LP_2_WIRE_RES, 0, 0, "F20", 0},
        {AVR6552_VAL_LP_4_WIRE_RES, 0, 0, "F21", 0},
        {AVR6552_VAL_RIPPLE_VOLTS, 0, 0, "F15", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrFunctionRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrFunctionRangeTableEntries,
    };

static IviRangeTableEntry attrFunctionRangeTable1Entries[] =
    {
        {AVR6552_VAL_DC_VOLTS, 0, 0, "F1", 0},
        {AVR6552_VAL_AC_VOLTS, 0, 0, "F2", 0},
        {AVR6552_VAL_DC_CURRENT, 0, 0, "F5", 0},
        {AVR6552_VAL_AC_CURRENT, 0, 0, "F6", 0},
        {AVR6552_VAL_2_WIRE_RES, 0, 0, "F3", 0},
        {AVR6552_VAL_4_WIRE_RES, 0, 0, "F4", 0},
        {AVR6552_VAL_DIODE, 0, 0, "F13", 0},
        {AVR6552_VAL_FREQ, 0, 0, "F50", 0},
        {AVR6552_VAL_AC_PLUS_DC_VOLTS, 0, 0, "F7", 0},
        {AVR6552_VAL_AC_PLUS_DC_CURRENT, 0, 0, "F8", 0},
        {AVR6552_VAL_LP_2_WIRE_RES, 0, 0, "F20", 0},
        {AVR6552_VAL_LP_4_WIRE_RES, 0, 0, "F21", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };
   
static IviRangeTable attrFunctionRangeTable1 =
    {
        IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrFunctionRangeTable1Entries,
    };

static IviRangeTableEntry attrFunctionRangeTable2Entries[] =
    {
        {AVR6552_VAL_DC_VOLTS, 0, 0, "F1", 0},
        {AVR6552_VAL_2_WIRE_RES, 0, 0, "F3", 0},
        {AVR6552_VAL_4_WIRE_RES, 0, 0, "F4", 0},
        {AVR6552_VAL_LP_2_WIRE_RES, 0, 0, "F20", 0},
        {AVR6552_VAL_LP_4_WIRE_RES, 0, 0, "F21", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrFunctionRangeTable2 =
    {
        IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrFunctionRangeTable2Entries,
    };

static IviRangeTableEntry attrFunctionRangeTable3Entries[] =
    {
        {AVR6552_VAL_DC_VOLTS, 0, 0, "F1", 0},
        {AVR6552_VAL_2_WIRE_RES, 0, 0, "F3", 0},
        {AVR6552_VAL_4_WIRE_RES, 0, 0, "F4", 0},
        {AVR6552_VAL_RIPPLE_VOLTS, 0, 0, "F15", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrFunctionRangeTable3 =
    {
        IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrFunctionRangeTable3Entries,
    };

static ViStatus _VI_FUNC avr6552AttrFunction_CheckCallback (ViSession vi,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     sampleRate, sampleMode;
    
    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_SAMPLE_RATE, VI_NULL, &sampleRate));
    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_SAMPLE_MODE, VI_NULL, &sampleMode));
    
    if ((sampleRate == AVR6552_VAL_LONGIT) &&
        (value != AVR6552_VAL_DC_VOLTS) &&
        (value != AVR6552_VAL_DC_CURRENT))
        viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION,
                        "Only DC Volts or DC Current measurements are valid for long-it sample rate."); 
    
    if ((sampleMode == AVR6552_VAL_BURST) &&
        (value != AVR6552_VAL_DC_VOLTS) &&
        (value != AVR6552_VAL_DC_CURRENT) &&
        (value != AVR6552_VAL_2_WIRE_RES) &&
        (value != AVR6552_VAL_LP_2_WIRE_RES))
        viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION,
                        "Only DC Volts, DC Current, 2-Wire resistance, "
                        "or LP 2-Wire resistance measurements are valid for burst sample mode.");
                        
    checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, VI_NULL, attributeId, value));                       
              
Error:
    return error;
}


static ViStatus _VI_FUNC avr6552AttrFunction_WriteCallback (ViSession vi, 
                                                             ViSession io, 
                                                             ViConstString channelName, 
                                                             ViAttr attributeId, 
                                                             ViInt32 value)
{
    ViStatus      error = VI_SUCCESS;
    ViString      cmd;                                                                                                                     
                                                                                                                                
    viCheckErr( Ivi_GetViInt32EntryFromValue (value, &attrFunctionRangeTable, 
                                              VI_NULL, VI_NULL, VI_NULL, VI_NULL, 
                                              &cmd, VI_NULL));         
    viCheckErr( viPrintf (io, "%s", cmd));                                                                                              
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrFunction_ReadCallback (ViSession vi, 
                                                            ViSession io, 
                                                            ViConstString channelName, 
                                                            ViAttr attributeId, 
                                                            ViInt32 *value)
{
    ViStatus error = VI_SUCCESS;
    ViChar   rdBuffer[BUFFER_SIZE];
    ViInt32  temp;
    
    viCheckErr( viPrintf (io, "F?"));
    viCheckErr( viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i1]>%d", &temp);
    
    Fmt (rdBuffer, "F%d", temp);
    viCheckErr( Ivi_GetViInt32EntryFromString (rdBuffer, &attrFunctionRangeTable, 
                                               value, VI_NULL, VI_NULL, VI_NULL, 
                                               VI_NULL));
     
Error:
    return error;
}

/*- AVR6552_ATTR_RANGE -*/

static IviRangeTableEntry VDCRangeTableR6552Entries[] =
    {
        {AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, "R0", 0},
        {0, 0.03, 0.03, "R2", 0},
        {0.03, 0.3, 0.3, "R3", 0},
        {0.3, 3.0, 3.0, "R4", 0},
        {3.0, 30.0, 30.0, "R5", 0},
        {30.0, 300.0, 300.0, "R6", 0},
        {300.0, 1000.0, 1000.0, "R7", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };
   
static IviRangeTable VDCRangeTableR6552 =
    {
        IVI_VAL_COERCED,
        VI_FALSE,
        VI_TRUE,
        VI_NULL,
        VDCRangeTableR6552Entries,
    };

static IviRangeTableEntry VDCRangeTableR6552TEntries[] =
    {
        {AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, "R0", 0},
        {0, 0.03, 0.03, "R2", 0},
        {0.03, 0.3, 0.3, "R3", 0},
        {0.3, 3.0, 3.0, "R4", 0},
        {3.0, 30.0, 30.0, "R5", 0},
        {30.0, 200.0, 200.0, "R6", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable VDCRangeTableR6552T =
    {
        IVI_VAL_COERCED,
        VI_FALSE,
        VI_TRUE,
        VI_NULL,
        VDCRangeTableR6552TEntries,
    };

static IviRangeTableEntry VDCRangeTableR6552TREntries[] =
    {
        {AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, "R0", 0},
        {0, 0.3, 0.3, "R3", 0},
        {0.3, 3.0, 3.0, "R4", 0},
        {3.0, 30.0, 30.0, "R5", 0},
        {30.0, 300.0, 300.0, "R6", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable VDCRangeTableR6552TR =
    {
        IVI_VAL_COERCED,
        VI_FALSE,
        VI_TRUE,
        VI_NULL,
        VDCRangeTableR6552TREntries,
    };

static IviRangeTableEntry VACRangeTableEntries[] =
    {
        {AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, "R0", 0},
        {0.0, 0.3, 0.3, "R3", 0},
        {0.3, 3.0, 3.0, "R4", 0},
        {3.0, 30.0, 30.0, "R5", 0},
        {30.0, 300.0, 300.0, "R6", 0},
        {300.0, 700.0, 700.0, "R7", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };
   
static IviRangeTable VACRangeTable =
    {
        IVI_VAL_COERCED,
        VI_FALSE,
        VI_TRUE,
        VI_NULL,
        VACRangeTableEntries,
    };

static IviRangeTableEntry CurrentRangeTableEntries[] =
    {
        {AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, "R0", 0},
        {0, 0.003, 0.003, "R4", 0},
        {0.003, 0.03, 0.03, "R5", 0},
        {0.03, 0.3, 0.3, "R6", 0},
        {0.3, 3.0, 3.0, "R7", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable CurrentRangeTable =
    {
        IVI_VAL_COERCED,
        VI_FALSE,
        VI_TRUE,
        VI_NULL,
        CurrentRangeTableEntries,
    };

static IviRangeTableEntry ohmsRangeTable1Entries[] =
    {
        {AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, "R0", 0},
        {0.0, 30.0, 30.0, "R2", 0},
        {30.0, 300.0, 300.0, "R3", 0},
        {300.0, 3000.0, 3000.0, "R4", 0},
        {3000.0, 30000.0, 30000.0, "R5", 0},
        {30000.0, 300000.0, 300000.0, "R6", 0},
        {300000.0, 3000000.0, 3000000.0, "R7", 0},
        {3000000.0, 30000000.0, 30000000.0, "R8", 0},
        {30000000.0, 300000000.0, 300000000.0, "R9", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };
   
static IviRangeTable ohmsRangeTable1 =
    {
        IVI_VAL_COERCED,
        VI_FALSE,
        VI_TRUE,
        VI_NULL,
        ohmsRangeTable1Entries,
    };

static IviRangeTableEntry ohmsRangeTable1R6552TREntries[] =
    {
        {AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, "R0", 0},
        {0.0, 30.0, 30.0, "R2", 0},
        {30.0, 300.0, 300.0, "R3", 0},
        {300.0, 3000.0, 3000.0, "R4", 0},
        {3000.0, 30000.0, 30000.0, "R5", 0},
        {30000.0, 300000.0, 300000.0, "R6", 0},
        {300000.0, 3000000.0, 3000000.0, "R7", 0},
        {3000000.0, 30000000.0, 30000000.0, "R8", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable ohmsRangeTable1R6552TR =
    {
        IVI_VAL_COERCED,
        VI_FALSE,
        VI_TRUE,
        VI_NULL,
        ohmsRangeTable1R6552TREntries,
    };

static IviRangeTableEntry ohmsRangeTable2Entries[] =
    {
        {AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, "R0", 0},
        {0.0, 300.0, 300.0, "R3", 0},
        {300.0, 3000.0, 3000.0, "R4", 0},
        {3000.0, 30000.0, 30000.0, "R5", 0},
        {30000.0, 300000.0, 300000.0, "R6", 0},
        {300000.0, 3000000.0, 3000000.0, "R7", 0},
        {3000000.0, 30000000.0, 30000000.0, "R8", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable ohmsRangeTable2 =
    {
        IVI_VAL_COERCED,
        VI_FALSE,
        VI_TRUE,
        VI_NULL,
        ohmsRangeTable2Entries,
    };

static IviRangeTableEntry RippleVoltsRangeTableEntries[] =
    {
        {AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, "R0", 0},
        {0, 0.03, 0.03, "R2", 0},
        {0.03, 0.3, 0.3, "R3", 0},
        {0.3, 3.0, 3.0, "R4", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable RippleVoltsRangeTable =
    {
        IVI_VAL_COERCED,
        VI_FALSE,
        VI_TRUE,
        VI_NULL,
        RippleVoltsRangeTableEntries,
    };
static IviRangeTableEntry FreqDiodeRangeTableEntries[] =
    {
        {AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, AVR6552_VAL_AUTO_RANGE_ON, "R0", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable FreqDiodeRangeTable =
    {
        IVI_VAL_COERCED,
        VI_FALSE,
        VI_TRUE,
        VI_NULL,
        FreqDiodeRangeTableEntries,
    };


static ViStatus _VI_FUNC avr6552AttrRange_RangeTableCallback1 (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               IviRangeTablePtr *rangeTablePtr)
{
    ViStatus         error = VI_SUCCESS;
    IviRangeTablePtr tblPtr = VI_NULL;
    ViInt32          function;
              
    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION, 0, &function));
    
    switch (function) 
        {
        case AVR6552_VAL_DC_VOLTS:
            tblPtr = &VDCRangeTableR6552; 
            break;
        case AVR6552_VAL_AC_VOLTS:
        case AVR6552_VAL_AC_PLUS_DC_VOLTS:
            tblPtr = &VACRangeTable;        
            break;
        case AVR6552_VAL_2_WIRE_RES:
        case AVR6552_VAL_4_WIRE_RES:
            tblPtr = &ohmsRangeTable1;  
            break;
        case AVR6552_VAL_LP_2_WIRE_RES:
        case AVR6552_VAL_LP_4_WIRE_RES:
            tblPtr = &ohmsRangeTable2;       
            break;
        case AVR6552_VAL_DC_CURRENT:
        case AVR6552_VAL_AC_CURRENT:
        case AVR6552_VAL_AC_PLUS_DC_CURRENT:
            tblPtr = &CurrentRangeTable;       
            break;    
        case AVR6552_VAL_RIPPLE_VOLTS:
            tblPtr = &RippleVoltsRangeTable;       
            break;    
        case AVR6552_VAL_FREQ:
        case AVR6552_VAL_DIODE:
            tblPtr = &FreqDiodeRangeTable;       
            break; 
        default:
            viCheckErr (IVI_ERROR_INVALID_VALUE);
            break;
        }
   
Error:
    *rangeTablePtr = tblPtr;
    return error;
}

static ViStatus _VI_FUNC avr6552AttrRange_RangeTableCallback2 (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               IviRangeTablePtr *rangeTablePtr)
{
    ViStatus         error = VI_SUCCESS;
    IviRangeTablePtr tblPtr = VI_NULL;
    ViInt32          function;
              
    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION, 0, &function));
    
    switch (function) 
        {
        case AVR6552_VAL_DC_VOLTS:
            tblPtr = &VDCRangeTableR6552T; 
            break;
        case AVR6552_VAL_AC_VOLTS:
        case AVR6552_VAL_AC_PLUS_DC_VOLTS:
            tblPtr = &VACRangeTable;        
            break;
        case AVR6552_VAL_2_WIRE_RES:
        case AVR6552_VAL_4_WIRE_RES:
            tblPtr = &ohmsRangeTable1;  
            break;
        case AVR6552_VAL_LP_2_WIRE_RES:
        case AVR6552_VAL_LP_4_WIRE_RES:
            tblPtr = &ohmsRangeTable2;       
            break;
        case AVR6552_VAL_DC_CURRENT:
        case AVR6552_VAL_AC_CURRENT:
        case AVR6552_VAL_AC_PLUS_DC_CURRENT:
            tblPtr = &CurrentRangeTable;       
            break;    
        case AVR6552_VAL_RIPPLE_VOLTS:
            tblPtr = &RippleVoltsRangeTable;       
            break;    
        case AVR6552_VAL_FREQ:
        case AVR6552_VAL_DIODE:
            tblPtr = &FreqDiodeRangeTable;       
            break; 
        default:
            viCheckErr (IVI_ERROR_INVALID_VALUE);
            break;
        }
   
Error:
    *rangeTablePtr = tblPtr;
    return error;
}

static ViStatus _VI_FUNC avr6552AttrRange_RangeTableCallback3 (ViSession vi,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               IviRangeTablePtr *rangeTablePtr)
{
    ViStatus         error = VI_SUCCESS;
    IviRangeTablePtr tblPtr = VI_NULL;
    ViInt32          function;
              
    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION, 0, &function));
   
    switch (function) 
        {
        case AVR6552_VAL_DC_VOLTS:
            tblPtr = &VDCRangeTableR6552TR;    
            break;
        case AVR6552_VAL_AC_VOLTS:
        case AVR6552_VAL_AC_PLUS_DC_VOLTS:
            tblPtr = &VACRangeTable;        
            break;
        case AVR6552_VAL_2_WIRE_RES:
        case AVR6552_VAL_4_WIRE_RES:
            tblPtr = &ohmsRangeTable1R6552TR;     
            break;
        case AVR6552_VAL_LP_2_WIRE_RES:
        case AVR6552_VAL_LP_4_WIRE_RES:
            tblPtr = &ohmsRangeTable2;       
            break;
        case AVR6552_VAL_DC_CURRENT:
        case AVR6552_VAL_AC_CURRENT:
        case AVR6552_VAL_AC_PLUS_DC_CURRENT:
            tblPtr = &CurrentRangeTable;       
            break;    
        case AVR6552_VAL_RIPPLE_VOLTS:
            tblPtr = &RippleVoltsRangeTable;       
            break;    
        case AVR6552_VAL_FREQ:
        case AVR6552_VAL_DIODE:
            tblPtr = &FreqDiodeRangeTable;       
            break; 
        default:
            viCheckErr (IVI_ERROR_INVALID_VALUE);
            break;
        }
   
Error:
    *rangeTablePtr = tblPtr;
    return error;
}

static ViStatus _VI_FUNC avr6552AttrRange_CheckCallback (ViSession vi,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     sampleMode;
    ViInt32     function;

    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_SAMPLE_MODE, 0, &sampleMode));

    if (sampleMode == AVR6552_VAL_BURST)
          viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION,
                      "Cannot set range to a manual value in Burst sample mode.");
    
    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION, VI_NULL, &function));
                                          
    if (function == AVR6552_VAL_FREQ)
    {
        ViInt32 equal;
        viCheckErr( Ivi_CompareWithPrecision (6, value, -1.0, &equal));
        if ( equal != 0 )
            viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION,
                            "Cannot set range to a manual value in Frequency measurement function.");
    }

    if (function == AVR6552_VAL_DIODE)
    {
        ViInt32 equal;
        viCheckErr( Ivi_CompareWithPrecision (6, value, -1.0, &equal));
        if ( equal != 0 )
            viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION,
                            "Cannot set range to a manual value in Diode measurement function.");
    }

Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrRange_WriteCallback (ViSession vi, 
                                                          ViSession io, 
                                                          ViConstString channelName, 
                                                          ViAttr attributeId, 
                                                          ViReal64 value)
{
    ViStatus error = VI_SUCCESS;
    ViString cmd;                                                                                                                     
    IviRangeTablePtr rangeTablePtr = VI_NULL;   
    ViInt32 function;
    
    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION, VI_NULL, &function));
    
    if ( (function != AVR6552_VAL_FREQ) && (function != AVR6552_VAL_DIODE) )
    {
        checkErr( Ivi_GetAttrRangeTable (vi, "", AVR6552_ATTR_RANGE, &rangeTablePtr));

        viCheckErr( Ivi_GetViReal64EntryFromValue (value, rangeTablePtr, VI_NULL, 
                                          VI_NULL, VI_NULL, VI_NULL, 
                                          &cmd, VI_NULL));
        viCheckErr (viPrintf (io, "%s", cmd));   
    }
Error:          
    return error;

}

static ViStatus _VI_FUNC avr6552AttrRange_ReadCallback (ViSession vi, 
                                                         ViSession io, 
                                                         ViConstString channelName, 
                                                         ViAttr attributeId, 
                                                         ViReal64 *value)
{
    ViStatus error = VI_SUCCESS;
    ViChar     autoRange[BUFFER_SIZE];
    IviRangeTablePtr rangeTablePtr = VI_NULL; 
    ViInt32          function;
              
    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION, 0, &function));
    
    if (function == AVR6552_VAL_DIODE)
    {
       *value = 3.0;
    }
    else if (function == AVR6552_VAL_FREQ)
    {
       *value = -1;
    }
    else
    {
        /* Determine if the DMM is auto ranging. */
    
       viCheckErr (viQueryf (io, "R?", "%s", autoRange));
    
       if (!strcmp (autoRange, "R0"))
          *value = AVR6552_VAL_AUTO_RANGE_ON;
       else
       {
          checkErr( Ivi_GetAttrRangeTable (vi, "", AVR6552_ATTR_RANGE, &rangeTablePtr));
          Ivi_GetViReal64EntryFromString (autoRange, rangeTablePtr, VI_NULL, VI_NULL, value,
                                      VI_NULL, VI_NULL);
       }
    }   
   
Error:
    return error;
}

/*- AVR6552_ATTR_RESOLUTION -*/

static IviRangeTableEntry attrResolutionLowRangeTableEntries[] =
    {
        {0.0, AVR6552_VAL_3_5_DIGITS, AVR6552_VAL_3_5_DIGITS, "", AVR6552_VAL_FAST},
        {AVR6552_VAL_3_5_DIGITS, AVR6552_VAL_4_5_DIGITS, AVR6552_VAL_4_5_DIGITS, "", AVR6552_VAL_MED},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrResolutionLowRangeTable =
    {
        IVI_VAL_COERCED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrResolutionLowRangeTableEntries,
    };

static IviRangeTableEntry attrResolutionHighRangeTableEntries[] =
    {
        {0.0, AVR6552_VAL_4_5_DIGITS, AVR6552_VAL_4_5_DIGITS, "", AVR6552_VAL_FAST},
        {AVR6552_VAL_4_5_DIGITS, AVR6552_VAL_5_5_DIGITS, AVR6552_VAL_5_5_DIGITS,"", AVR6552_VAL_MED},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrResolutionHighRangeTable =
    {
        IVI_VAL_COERCED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrResolutionHighRangeTableEntries,
    };

static IviRangeTableEntry attrResolutionRangeTableFreqEntries[] =
    {
        {0.0, AVR6552_VAL_4_DIGITS, AVR6552_VAL_4_DIGITS, "", AVR6552_VAL_FAST},
        {AVR6552_VAL_4_DIGITS, AVR6552_VAL_5_DIGITS, AVR6552_VAL_5_DIGITS, "", AVR6552_VAL_MED},
        {AVR6552_VAL_5_DIGITS, AVR6552_VAL_6_DIGITS, AVR6552_VAL_6_DIGITS,"", AVR6552_VAL_SLOW},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrResolutionRangeTableFreq =
    {
        IVI_VAL_COERCED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrResolutionRangeTableFreqEntries,
    };

static IviRangeTableEntry attrResolutionRangeTableRippleVoltsEntries[] =
    {
        {AVR6552_VAL_3_5_DIGITS, 0, 0, "", AVR6552_VAL_FAST},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrResolutionRangeTableRippleVolts =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrResolutionRangeTableRippleVoltsEntries,
    };

static ViStatus _VI_FUNC avr6552AttrResolution_RangeTableCallback (ViSession vi,
                                                                   ViConstString channelName,
                                                                   ViAttr attributeId,
                                                                   IviRangeTablePtr *rangeTablePtr)
{
    ViStatus    error = VI_SUCCESS;
    IviRangeTablePtr    tblPtr = VI_NULL;
    ViInt32     measFunc;
    
    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION, 0, &measFunc));
    
    switch (measFunc)
    {
    case AVR6552_VAL_DC_VOLTS:
    case AVR6552_VAL_DC_CURRENT:
    case AVR6552_VAL_AC_CURRENT:
    case AVR6552_VAL_2_WIRE_RES:
    case AVR6552_VAL_4_WIRE_RES:           
    case AVR6552_VAL_DIODE:              
    case AVR6552_VAL_LP_2_WIRE_RES:
    case AVR6552_VAL_LP_4_WIRE_RES:
    case AVR6552_VAL_AC_VOLTS: 
         tblPtr = &attrResolutionHighRangeTable;
         break;
    case AVR6552_VAL_AC_PLUS_DC_VOLTS:
    case AVR6552_VAL_AC_PLUS_DC_CURRENT:
         tblPtr = &attrResolutionLowRangeTable;
         break;
    case AVR6552_VAL_FREQ:
         tblPtr = &attrResolutionRangeTableFreq;
         break;
    case AVR6552_VAL_RIPPLE_VOLTS:
         tblPtr = &attrResolutionRangeTableRippleVolts;
         break;
    }

Error:
    *rangeTablePtr = tblPtr;
    return error;
}

static ViStatus _VI_FUNC avr6552AttrResolution_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     cmdVal;
    IviRangeTablePtr rangeTablePtr = VI_NULL;
    
    checkErr( Ivi_GetAttrRangeTable (vi, VI_NULL, AVR6552_ATTR_RESOLUTION,
                                    &rangeTablePtr));
    viCheckErr( Ivi_GetViReal64EntryFromValue (value, rangeTablePtr, VI_NULL, VI_NULL,
                                               VI_NULL, VI_NULL, VI_NULL, &cmdVal));
                
    checkErr( Ivi_SetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_SAMPLE_RATE, 
                                       IVI_VAL_DONT_MARK_AS_SET_BY_USER, cmdVal));
       
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrResolution_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    IviRangeTablePtr rangeTablePtr = VI_NULL;
    ViInt32     sampleRate;
    
    checkErr( Ivi_GetAttrRangeTable (vi, VI_NULL, AVR6552_ATTR_RESOLUTION,
                                    &rangeTablePtr));
    
    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_SAMPLE_RATE, 
                                       IVI_VAL_DONT_MARK_AS_SET_BY_USER, &sampleRate));
    if (sampleRate != AVR6552_VAL_LONGIT)                                   
       viCheckErr(Ivi_GetViReal64EntryFromCmdValue (sampleRate, rangeTablePtr, value, VI_NULL,
                                                  VI_NULL, VI_NULL, VI_NULL));
    else
       viCheckErr(Ivi_GetViReal64EntryFromCmdValue (AVR6552_VAL_MED, rangeTablePtr, value, VI_NULL,
                                                  VI_NULL, VI_NULL, VI_NULL));
Error:
    return error;
}

/*- AVR6552_ATTR_TRIGGER_SOURCE -*/

static IviRangeTableEntry attrTriggerSourceRangeTableEntries[] =
    {
        {AVR6552_VAL_IMMEDIATE, 0, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrTriggerSourceRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrTriggerSourceRangeTableEntries,
    };

/*- AVR6552_ATTR_AC_MIN_FREQ -*/

static IviRangeTableEntry attrAcMinFreqRangeTableEntries[] =
    {
        {300.0, 300.0, 300.0, "FL0", 0},
        {20.0, 300.0, 20.0, "FL1", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAcMinFreqRangeTable =
    {
        IVI_VAL_COERCED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAcMinFreqRangeTableEntries,
    };


static ViStatus _VI_FUNC avr6552AttrAcMinFreq_WriteCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    cmd;
    
    viCheckErr (Ivi_GetViReal64EntryFromValue (value, &attrAcMinFreqRangeTable, 
                                              VI_NULL, VI_NULL, VI_NULL, VI_NULL, 
                                              &cmd, VI_NULL));

    viCheckErr (viPrintf (io, "%s", cmd));
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrAcMinFreq_ReadCallback (ViSession vi, 
                                                              ViSession io, 
                                                              ViConstString channelName, 
                                                              ViAttr attributeId, 
                                                              ViReal64 *value)
{
    ViStatus error = VI_SUCCESS;
    ViChar   rdBuffer[BUFFER_SIZE];

    viCheckErr( viQueryf (io, "FL?", "%s", rdBuffer));
    viCheckErr( Ivi_GetViReal64EntryFromString (rdBuffer, &attrAcMinFreqRangeTable,
                                                VI_NULL, VI_NULL, value, VI_NULL,
                                                VI_NULL));
                                                
Error:
    return error;
}

/*- AVR6552_ATTR_AC_MAX_FREQ -*/

static IviRangeTableEntry attrAcMaxFreqRangeTable2Entries[] =
    {
        {0.0, 5000.0, 5000.0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAcMaxFreqRangeTable2 =
    {
        IVI_VAL_COERCED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAcMaxFreqRangeTable2Entries,
    };

static IviRangeTableEntry attrAcMaxFreqRangeTable1Entries[] =
    {
        {0.0, 300000.0, 300000.0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrAcMaxFreqRangeTable1 =
    {
        IVI_VAL_COERCED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrAcMaxFreqRangeTable1Entries,
    };

static ViStatus _VI_FUNC avr6552AttrAcMaxFreq_RangeTableCallback (ViSession vi,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  IviRangeTablePtr *rangeTablePtr)
{
    ViStatus    error = VI_SUCCESS;
    IviRangeTablePtr    tblPtr = VI_NULL;
    ViInt32     measFunc;
    
    checkErr( Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION, VI_NULL, &measFunc));
    switch (measFunc)
       {
       case AVR6552_VAL_AC_VOLTS:
       case AVR6552_VAL_AC_PLUS_DC_VOLTS:
            tblPtr = &attrAcMaxFreqRangeTable1;
            break;
       case AVR6552_VAL_AC_CURRENT:
       case AVR6552_VAL_AC_PLUS_DC_CURRENT:
            tblPtr = &attrAcMaxFreqRangeTable2;
            break;     
       }     
Error:
    *rangeTablePtr = tblPtr;
    return error;
}

/*- AVR6552_ATTR_SAMPLE_MODE -*/

static IviRangeTableEntry attrSampleModeRangeTableEntries[] =
    {
        {AVR6552_VAL_FREE_RUN, 0, 0, "M0", 0},
        {AVR6552_VAL_HOLD, 0, 0, "M1", 0},
        {AVR6552_VAL_BURST, 0, 0, "M2", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrSampleModeRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSampleModeRangeTableEntries,
    };

static ViStatus _VI_FUNC avr6552AttrSampleMode_CheckCallback1 (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     measFunc;
    
    if (value == AVR6552_VAL_BURST)
       checkErr (Ivi_InvalidateAttribute (vi, "", AVR6552_ATTR_RANGE));  
       
    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION, VI_NULL, &measFunc));
    switch (measFunc)
      {
      case AVR6552_VAL_DC_VOLTS:
      case AVR6552_VAL_DC_CURRENT:
      case AVR6552_VAL_2_WIRE_RES:
      case AVR6552_VAL_LP_2_WIRE_RES:
          checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, VI_NULL, attributeId, value));
          break;
      default:
         if (value == AVR6552_VAL_BURST)
            viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION,
                        "Burst mode is available only for "
                        "DC Volts, DC Current, 2-Wire Resistance and LP 2-Wire Resistance measurements.");
      }                                          
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrSampleMode_CheckCallback2 (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    
    if (value == AVR6552_VAL_BURST)
       viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION,
                     "Burst sample mode is supported only by AVR6552.");
    
    checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, VI_NULL, attributeId, value));
                                   
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrSampleMode_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    cmd;
    
    viCheckErr (Ivi_GetViInt32EntryFromValue (value, &attrSampleModeRangeTable, 
                                              VI_NULL, VI_NULL, VI_NULL, VI_NULL, 
                                              &cmd, VI_NULL));
    viCheckErr (viPrintf (io, "%s", cmd));
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrSampleMode_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viQueryf (io, "M?", "%s", rdBuffer));
    viCheckErr (Ivi_GetViInt32EntryFromString (rdBuffer, &attrSampleModeRangeTable, 
                                               value, VI_NULL, VI_NULL, VI_NULL, 
                                               VI_NULL));
Error:
    return error;
}

/*- AVR6552_ATTR_AUTO_ZERO -*/

static IviRangeTableEntry attrAutoZeroRangeTableEntries[] =
    { 
        {AVR6552_VAL_AUTO_ZERO_OFF,   0,   0,   "AZ0",  0},
        {AVR6552_VAL_AUTO_ZERO_ON,    0,   0,   "AZ1",   0},
        {AVR6552_VAL_AUTO_ZERO_ONCE,  0,   0,   "AZ2", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}  
    };   
static IviRangeTable  attrAutoZeroRangeTable =       
    {
        IVI_VAL_DISCRETE,                   
        VI_FALSE,                           
        VI_FALSE,                           
        VI_NULL,                            
        attrAutoZeroRangeTableEntries,      
    };

static ViStatus _VI_FUNC avr6552AttrAutoZero_CheckCallback (ViSession vi,
                                                            ViConstString channelName,
                                                            ViAttr attributeId,
                                                            ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32 sampleMode;
    
    checkErr (Ivi_DefaultCheckCallbackViInt32 (vi,"", attributeId, value));
    
    checkErr (Ivi_GetAttributeViInt32 (vi, "", AVR6552_ATTR_SAMPLE_MODE, 0, &sampleMode));
    if ( sampleMode == AVR6552_VAL_BURST)
        viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION, "Cannot configure auto zero mode in burst"
                                " sample mode.");
        
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrAutoZero_WriteCallback (ViSession vi, 
                                                             ViSession io, 
                                                             ViConstString channelName, 
                                                             ViAttr attributeId, 
                                                             ViInt32 value)
{
    ViStatus      error = VI_SUCCESS;
    ViString      cmd;

    viCheckErr (Ivi_GetViInt32EntryFromValue (value, &attrAutoZeroRangeTable, 
                                              VI_NULL, VI_NULL, VI_NULL, VI_NULL, 
                                              &cmd, VI_NULL));

    viCheckErr (viPrintf (io, "%s", cmd));
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrAutoZero_ReadCallback (ViSession vi, 
                                                            ViSession io, 
                                                            ViConstString channelName, 
                                                            ViAttr attributeId, 
                                                            ViInt32 *value)
{
    ViStatus error = VI_SUCCESS;
    ViChar   rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viQueryf (io, "AZ?", "%s", rdBuffer));
    viCheckErr (Ivi_GetViInt32EntryFromString (rdBuffer, &attrAutoZeroRangeTable, 
                                               value, VI_NULL, VI_NULL, VI_NULL, 
                                               VI_NULL));
    
Error:
    return error;
}

/*- AVR6552_ATTR_POWERLINE_FREQ -*/

static IviRangeTableEntry attrPowerlineFreqRangeTableEntries[] =
    {
        {AVR6552_VAL_50_HERTZ, 0, 0, "LF0", 0},
        {AVR6552_VAL_60_HERTZ, 0, 0, "LF1", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };
   
static IviRangeTable attrPowerlineFreqRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrPowerlineFreqRangeTableEntries,
    };


static ViStatus _VI_FUNC avr6552AttrPowerlineFreq_ReadCallback (ViSession vi, 
                                                                ViSession io, 
                                                                ViConstString channelName, 
                                                                ViAttr attributeId, 
                                                                ViReal64 *value)
{
    ViStatus error = VI_SUCCESS;
    ViChar   rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viQueryf (io, "LF?", "%s", rdBuffer));
    viCheckErr (Ivi_GetViReal64EntryFromString (rdBuffer, &attrPowerlineFreqRangeTable, 
                                               value, VI_NULL, VI_NULL, VI_NULL, 
                                               VI_NULL));
Error:
    return error;
}

/*- AVR6552_ATTR_SAMPLE_RATE  -*/

static IviRangeTableEntry attrSampleRateRangeTable1Entries[] =
    {
        {AVR6552_VAL_FAST, 0, 0, "PR1", 0},
        {AVR6552_VAL_MED, 0, 0, "PR2", 0},
        {AVR6552_VAL_SLOW, 0, 0, "PR3", 0},
        {AVR6552_VAL_LONGIT, 0, 0, "PR4", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrSampleRateRangeTable1 =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSampleRateRangeTable1Entries,
    };

static IviRangeTableEntry attrSampleRateRangeTable2Entries[] =
    {
        {AVR6552_VAL_FAST, 0, 0, "PR1", 0},
        {AVR6552_VAL_MED, 0, 0, "PR2", 0},
        {AVR6552_VAL_SLOW, 0, 0, "PR3", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrSampleRateRangeTable2 =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSampleRateRangeTable2Entries,
    };

static ViStatus _VI_FUNC avr6552AttrSampleRate_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     measFunc;
    
    checkErr (Ivi_GetAttributeViInt32 (vi, VI_NULL, AVR6552_ATTR_FUNCTION, VI_NULL, &measFunc));
    switch (measFunc)
      {
      case AVR6552_VAL_DC_VOLTS:
      case AVR6552_VAL_DC_CURRENT:
          checkErr (Ivi_DefaultCheckCallbackViInt32 (vi, VI_NULL, attributeId, value));
          break;
      default:
         if (value ==  AVR6552_VAL_LONGIT)
            viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION,
                        "Long-it sample rate is available only DC voltage "
                        "or current measurements");
         break;               
      } 
      
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrSampleRate_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
    ViStatus         error = VI_SUCCESS;
    ViString         cmd;
    IviRangeTablePtr rangeTablePtr = VI_NULL;   
    

    checkErr( Ivi_GetAttrRangeTable (vi, VI_NULL, AVR6552_ATTR_SAMPLE_RATE,
                                    &rangeTablePtr));
    viCheckErr (Ivi_GetViInt32EntryFromValue (value, rangeTablePtr, 
                                              VI_NULL, VI_NULL, VI_NULL, VI_NULL, 
                                              &cmd, VI_NULL));

    viCheckErr (viPrintf (io, "%s", cmd));
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrSampleRate_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
    ViStatus         error = VI_SUCCESS;
    IviRangeTablePtr rangeTablePtr = VI_NULL;       
    ViChar           rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viQueryf (io, "PR?", "%s", rdBuffer));
    
    checkErr( Ivi_GetAttrRangeTable (vi, VI_NULL, AVR6552_ATTR_SAMPLE_RATE,
                                    &rangeTablePtr));
    viCheckErr (Ivi_GetViInt32EntryFromString (rdBuffer, rangeTablePtr, value, VI_NULL, VI_NULL,
                                      VI_NULL, VI_NULL));
  
Error:
    return error;
}

/*- AVR6552_ATTR_INTEGRAL_TIME -*/

static IviRangeTableEntry attrIntegralTimeRangeTableEntries[] =
    {
        {100.0, 60000.0, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrIntegralTimeRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrIntegralTimeRangeTableEntries,
    };

static ViStatus _VI_FUNC avr6552AttrIntegralTime_CoerceCallback (ViSession vi,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value,
                                                                 ViReal64 *coercedValue)
{
    /*10 milliseconds step*/
    *coercedValue = value - fmod(value, 10);
    
    return VI_SUCCESS;
}

static ViStatus _VI_FUNC avr6552AttrIntegralTime_WriteCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr (viPrintf (io, "LIT%f", value));
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrIntegralTime_ReadCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar   rdBuffer[BUFFER_SIZE];

    viCheckErr( viPrintf (io, "LIT?"));
    viCheckErr( viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i3]>%f", value);
   
Error:
    return error;
}

/*- AVR6552_ATTR_HOLD_SAMPLE_COUNT -*/

static IviRangeTableEntry attrHoldSampleCountRangeTableEntries[] =
    {
        {1, 9999, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };
   
static IviRangeTable attrHoldSampleCountRangeTable =
    {
        IVI_VAL_RANGED,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        attrHoldSampleCountRangeTableEntries,
    };


static ViStatus _VI_FUNC avr6552AttrHoldSampleCount_WriteCallback (ViSession vi, 
                                                                ViSession io, 
                                                                ViConstString channelName, 
                                                                ViAttr attributeId, 
                                                                ViInt32 value)
{
    ViStatus error = VI_SUCCESS;

    viCheckErr (viPrintf (io, "SPN %d", value));
  

Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrHoldSampleCount_ReadCallback (ViSession vi, 
                                                               ViSession io, 
                                                               ViConstString channelName, 
                                                               ViAttr attributeId, 
                                                               ViInt32 *value)
{
    ViStatus error = VI_SUCCESS;
    ViChar   rdBuffer[BUFFER_SIZE];

    viCheckErr( viPrintf (io, "SPN?"));
    viCheckErr( viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i3]>%d", value);
   
Error:
    return error;
}


/*- AVR6552_ATTR_BURST_COUNT -*/

static IviRangeTableEntry attrBurstCountRangeTableEntries[] =
    {
        {1000, 10000, 1000, "", 0},
        {2000, 0, 0, "", 0},
        {3000, 0, 0, "", 0},
        {4000, 0, 0, "", 0},
        {5000, 0, 0, "", 0},
        {6000, 0, 0, "", 0},
        {7000, 0, 0, "", 0},
        {8000, 0, 0, "", 0},
        {9000, 0, 0, "", 0},
        {10000, 0, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrBurstCountRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrBurstCountRangeTableEntries,
    };



static ViStatus _VI_FUNC avr6552AttrBurstCount_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr (viPrintf (io, "BCN%d", value));
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrBurstCount_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];

    viCheckErr( viPrintf (io, "BCN?"));
    viCheckErr( viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i3]>%d", value);
    
Error:
    return error;
}

/*- AVR6552_ATTR_DELAYED_TRIGGER -*/

static IviRangeTableEntry attrDelayedTriggerRangeTableEntries[] =
    {
        {0, 0, 0, "PTL0", 0},
        {25, 0, 0, "PTL1", 0},
        {50, 0, 0, "PTL2", 0},
        {75, 0, 0, "PTL3", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrDelayedTriggerRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrDelayedTriggerRangeTableEntries,
    };

static ViStatus _VI_FUNC avr6552AttrDelayedTrigger_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    cmd;
    
    viCheckErr (Ivi_GetViInt32EntryFromValue (value, &attrDelayedTriggerRangeTable, 
                                              VI_NULL, VI_NULL, VI_NULL, VI_NULL, 
                                              &cmd, VI_NULL));
    viCheckErr (viPrintf (io, "%s", cmd));
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrDelayedTrigger_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viQueryf (io, "PTL?", "%s", rdBuffer));
    Ivi_GetViInt32EntryFromString (rdBuffer, &attrDelayedTriggerRangeTable, value, VI_NULL, VI_NULL,
                                      VI_NULL, VI_NULL);
Error:
    return error;
}

/*- AVR6552_ATTR_TRIGGER_DELAY -*/

static ViStatus _VI_FUNC avr6552AttrTriggerDelay_CheckCallback (ViSession vi,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    
    if (value > 0.0)
        viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION, "Trigger delay cannot be greater than 0.0");

    checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, "", attributeId, value));
    
Error:
    return error;
}

/*- AVR6552_ATTR_NULL_OFFSET -*/

static IviRangeTableEntry attrNullConstantRangeTableEntries[] =
    {
        {-999999.0E+6, 999999.0E+6, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrNullConstantRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrNullConstantRangeTableEntries,
    };


static ViStatus _VI_FUNC avr6552AttrNullOffset_CheckCallback (ViSession vi,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViBoolean   nullEnabled;
    
    checkErr (Ivi_GetAttributeViBoolean (vi, "", AVR6552_ATTR_NULL_ENABLED, 0, &nullEnabled));

    if (!nullEnabled)
       viCheckErrElab (IVI_ERROR_INVALID_CONFIGURATION,
                       "Cannot set Null Offset the null calculation is not enabled.");
                       
    checkErr (Ivi_DefaultCheckCallbackViReal64 (vi, VI_NULL, attributeId, value));
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrNullOffset_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      cmd[BUFFER_SIZE];
    
    Fmt (cmd, "KNL%f[e]", value);
    viCheckErr (viPrintf (io, "%s", cmd));
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrNullOffset_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viPrintf (io, "KNL?"));
    viCheckErr (viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i3]>%f", value);
    
    
Error:
    return error;
}

/*- AVR6552_ATTR_SMOOTH_COUNT  -*/

static IviRangeTableEntry attrSmoothCountRangeTableEntries[] =
    {
        {2, 100, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrSmoothCountRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrSmoothCountRangeTableEntries,
    };

static ViStatus _VI_FUNC avr6552AttrSmoothCount_WriteCallback (ViSession vi,
                                                               ViSession io,
                                                               ViConstString channelName,
                                                               ViAttr attributeId,
                                                               ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr (viPrintf (io, "TI%d", value));
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrSmoothCount_ReadCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viQueryf (io, "TI?", "%s", rdBuffer));
    Scan (rdBuffer, "%s[i2]>%d", value);
    
Error:
    return error;
}

/*- AVR6552_ATTR_COMPARE_HIGH_VAL   -*/

static IviRangeTableEntry attrConstantRangeTableEntries[] =
    {
        {AVR6552_VAL_MEASURE_VALUE, AVR6552_VAL_MEASURE_VALUE, 0, "", 0},
        {-999999.0E+6, 999999.0E+6, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrConstantRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrConstantRangeTableEntries,
    };

static ViStatus _VI_FUNC avr6552AttrCompareHighVal_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      cmd[BUFFER_SIZE];
    
    if (value == AVR6552_VAL_MEASURE_VALUE)
       {
       viCheckErr (viPrintf (io, "HIM"));
       checkErr (Ivi_InvalidateAttribute (vi, "", AVR6552_ATTR_COMPARE_HIGH_VAL));
       }
    else   
       {
       Fmt (cmd, "HI%f[e]", value); 
       viCheckErr (viPrintf (io, "%s", cmd));
       }
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrCompareHighVal_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viPrintf (io, "HI?"));
    viCheckErr (viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i2]>%f", value);
    
Error:
    return error;
}
/*- AVR6552_ATTR_COMPARE_LOW_VAL    -*/

static ViStatus _VI_FUNC avr6552AttrCompareLowVal_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      cmd[BUFFER_SIZE];
    if (value == AVR6552_VAL_MEASURE_VALUE)
       {
       viCheckErr (viPrintf (io, "LOM"));
       checkErr (Ivi_InvalidateAttribute (vi, "", AVR6552_ATTR_COMPARE_LOW_VAL));
       }
    else    
       {
       Fmt (cmd, "LO%f[e]", value); 
       viCheckErr (viPrintf (io, "%s", cmd));
       }
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrCompareLowVal_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viPrintf (io, "LO?"));
    viCheckErr (viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i2]>%f", value);
    
Error:
    return error;
}

/*- AVR6552_ATTR_SCALE_A  -*/

static ViStatus _VI_FUNC avr6552AttrScaleA_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      cmd[BUFFER_SIZE];
    
    if (value == AVR6552_VAL_MEASURE_VALUE)
       {
       viCheckErr (viPrintf (io, "KAM"));
       checkErr (Ivi_InvalidateAttribute (vi, "", AVR6552_ATTR_SCALE_A));
       }
    else  
       {
       Fmt (cmd, "KA%f[e]", value); 
       viCheckErr (viPrintf (io, "%s", cmd));
       }
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrScaleA_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viPrintf (io, "KA?"));
    viCheckErr (viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i2]>%f", value);
   
Error:
    return error;
}

/*- AVR6552_ATTR_SCALE_B   -*/

static ViStatus _VI_FUNC avr6552AttrScaleB_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      cmd[BUFFER_SIZE];
    
    if (value == AVR6552_VAL_MEASURE_VALUE)
       {
       viCheckErr (viPrintf (io, "KBM"));
       checkErr (Ivi_InvalidateAttribute (vi, "", AVR6552_ATTR_SCALE_B));
       }
    else  
       {
       Fmt (cmd, "KB%f[e]", value); 
       viCheckErr (viPrintf (io, "%s", cmd));
       }

Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrScaleB_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viPrintf (io, "KB?"));
    viCheckErr (viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i2]>%f", value);
    
    
Error:
    return error;
}

/*- AVR6552_ATTR_SCALE_C  -*/

static ViStatus _VI_FUNC avr6552AttrScaleC_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      cmd[BUFFER_SIZE];
    
    if (value == AVR6552_VAL_MEASURE_VALUE)
       {
       viCheckErr (viPrintf (io, "KCM"));
       checkErr (Ivi_InvalidateAttribute (vi, "", AVR6552_ATTR_SCALE_C));
       }
    else   
       {
       Fmt (cmd, "KC%f[e]", value); 
       viCheckErr (viPrintf (io, "%s", cmd));
       }

Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrScaleC_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viPrintf (io, "KC?"));
    viCheckErr (viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i2]>%f", value);
   
    
Error:
    return error;
}

/*- AVR6552_ATTR_DB_CONSTANT  -*/

static IviRangeTableEntry attrDbConstantRangeTableEntries[] =
    {
        {0.00001E-9, 999999.0E+6, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrDbConstantRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrDbConstantRangeTableEntries,
    };

static ViStatus _VI_FUNC avr6552AttrDbConstant_WriteCallback (ViSession vi,
                                                              ViSession io,
                                                              ViConstString channelName,
                                                              ViAttr attributeId,
                                                              ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      cmd[BUFFER_SIZE];
    
    Fmt (cmd, "KD%f[e]", value);
    viCheckErr (viPrintf (io, "%s", cmd)); 

Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrDbConstant_ReadCallback (ViSession vi,
                                                             ViSession io,
                                                             ViConstString channelName,
                                                             ViAttr attributeId,
                                                             ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viPrintf (io, "KD?"));
    viCheckErr (viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i2]>%f", value);
    
    
Error:
    return error;
}

/*- AVR6552_ATTR_HIGH_PASS_FILTER -*/

static IviRangeTableEntry attrHighPassFilterRangeTableEntries[] =
    {
        {AVR6552_VAL_FILTER_OFF, 0, 0, "HPF0", 0},
        {1000, 0, 0, "HPF1", 0},
        {10000, 0, 0, "HPF2", 0},
        {100000, 0, 0, "HPF3", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrHighPassFilterRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrHighPassFilterRangeTableEntries,
    };

static ViStatus _VI_FUNC avr6552AttrHighPassFilter_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    cmd;                                                                                                                     
                                                                                                                                
    viCheckErr( Ivi_GetViInt32EntryFromValue (value, &attrHighPassFilterRangeTable, 
                                              VI_NULL, VI_NULL, VI_NULL, VI_NULL, 
                                              &cmd, VI_NULL));         
    viCheckErr( viPrintf (io, "%s", cmd));   
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrHighPassFilter_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viQueryf (io, "HPF?", "%s", rdBuffer));
    
    Ivi_GetViInt32EntryFromString (rdBuffer, &attrHighPassFilterRangeTable, value, VI_NULL, VI_NULL,
                                      VI_NULL, VI_NULL);
                                      
Error:
    return error;
}

/*- AVR6552_ATTR_LOW_PASS_FILTER -*/

static IviRangeTableEntry attrLowPassFilterRangeTableEntries[] =
    {
        {AVR6552_VAL_FILTER_OFF, 0, 0, "LPF0", 0},
        {1000, 0, 0, "LPF1", 0},
        {10000, 0, 0, "LPF2", 0},
        {100000, 0, 0, "LPF3", 0},
        {1000000, 0, 0, "LPF4", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrLowPassFilterRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrLowPassFilterRangeTableEntries,
    };

static ViStatus _VI_FUNC avr6552AttrLowPassFilter_WriteCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    cmd;                                                                                                                     
                                                                                                                                
    viCheckErr( Ivi_GetViInt32EntryFromValue (value, &attrLowPassFilterRangeTable, 
                                              VI_NULL, VI_NULL, VI_NULL, VI_NULL, 
                                              &cmd, VI_NULL));         
    viCheckErr( viPrintf (io, "%s", cmd));   
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrLowPassFilter_ReadCallback (ViSession vi,
                                                                ViSession io,
                                                                ViConstString channelName,
                                                                ViAttr attributeId,
                                                                ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];
    
    viCheckErr (viQueryf (io, "LPF?", "%s", rdBuffer));
    
    Ivi_GetViInt32EntryFromString (rdBuffer, &attrLowPassFilterRangeTable, value, VI_NULL, VI_NULL,
                                      VI_NULL, VI_NULL);
                                      
Error:
    return error;
}

/*- AVR6552_ATTR_DELAY -*/  
static IviRangeTableEntry attrDelayRangeTableEntries[] =
    {
        {0.0, 99.999, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };
   
static IviRangeTable attrDelayRangeTable =
    {
        IVI_VAL_RANGED,
        VI_FALSE,
        VI_TRUE,
        VI_NULL,
        attrDelayRangeTableEntries,
    };

static ViStatus _VI_FUNC avr6552AttrDelay_WriteCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr (viPrintf (io, "TRD %Lf", value));  
    
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrDelay_ReadCallback (ViSession vi,
                                                        ViSession io,
                                                        ViConstString channelName,
                                                        ViAttr attributeId,
                                                        ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViChar      rdBuffer[BUFFER_SIZE];

    viCheckErr( viPrintf (io, "TRD?"));
    viCheckErr( viRead (io, rdBuffer, BUFFER_SIZE, VI_NULL));
    Scan (rdBuffer, "%s[i3]>%f", value);
    
Error:
    return error;
}

/*- AVR6552_ATTR_FREQ_VOLTAGE_RANGE -*/

static IviRangeTableEntry attrFreqVoltageRangeRangeTableEntries[] =
    {
        {100e-3, 700.0, 0, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrFreqVoltageRangeRangeTable =
    {
        IVI_VAL_RANGED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrFreqVoltageRangeRangeTableEntries,
    };


static ViStatus _VI_FUNC avr6552AttrFreqVoltageRange_CoerceCallback (ViSession vi,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 value,
                                                                     ViReal64 *coercedValue)
{
    ViStatus    error = VI_SUCCESS;
    ViReal64    minVal;
    
    checkErr( Ivi_GetAttrMinMaxViReal64 (vi, "", attributeId, &minVal, VI_NULL,
                                            VI_NULL, VI_NULL));   

    *coercedValue = (value < minVal)? minVal: value;
    
Error:
    return error;
}

/*- AVR6552_ATTR_RESOLUTION_ABSOLUTE -*/
static IviRangeTableEntry attrResolutionAbsoluteRangeTableEntries[] =
    {
        {10000, 1000000, 10000, "", 0},
        {1000, 10000, 1000, "", 0},
        {100, 1000, 100, "", 0},
        {10, 100, 10, "", 0},
        {1, 10, 1, "", 0},
        {.1, 1, .1, "", 0},
        {.01, .1, .01, "", 0},
        {.001, .01, .001, "", 0},
        {.0001, .001, .0001, "", 0},
        {.00001, .0001, .00001, "", 0},
        {.000001, .00001, .000001, "", 0},
        {.0000001, .000001, .0000001, "", 0},
        {.00000001, .0000001, .00000001, "", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable attrResolutionAbsoluteRangeTable =
    {
        IVI_VAL_COERCED,
        VI_TRUE,
        VI_TRUE,
        VI_NULL,
        attrResolutionAbsoluteRangeTableEntries,
    };


    /* 
        This coerce callback is used to coerce values of ATTR_RESOLUTION_ABSOLUTE to correspond
        to the coercion that occurs when ATTR_RESOLUTION is set. This happens when the 
        desired digits of precision is between zero and the minimum value. ATTR_RESOLUTION
        coerces this value to the minimum possible digits of precision. Accordingly, the 
        absolute resolution must also be coerced to correspond to this value. This must
        be done before ATTR_RESOLUTION is set.
   */
static ViStatus _VI_FUNC avr6552AttrResolutionAbsolute_CoerceCallback (ViSession vi,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 value,
                                                                     ViReal64 *coercedValue)
{
    ViStatus    error = VI_SUCCESS;
    ViReal64    digits, coercedDigits;
    
    checkErr (Ivi_DefaultCoerceCallbackViReal64 (vi, "", attributeId, value, coercedValue));
    
    avr6552_convertAbsolutetoDigits(vi, *coercedValue, &digits);

    checkErr (Ivi_DefaultCoerceCallbackViReal64 (vi, VI_NULL, AVR6552_ATTR_RESOLUTION,
                                                    digits, &coercedDigits));
    if (digits != coercedDigits)
        avr6552_convertDigitstoAbsolute(vi, coercedDigits , coercedValue);
        
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrResolutionAbsolute_CheckCallback (ViSession vi,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViReal64    range, digits;
    
    checkErr (Ivi_DefaultCheckCallbackViReal64(vi,"", attributeId, value));
    checkErr (Ivi_GetAttributeViReal64 (vi, "", AVR6552_ATTR_RANGE, 0, &range));
    if (range == AVR6552_VAL_AUTO_RANGE_ON)
        viCheckErrElab (IVI_ERROR_INVALID_VALUE, "Cannot set absolute resolution while auto-ranging.");
    
    digits = log10(range/value);

    error = Ivi_CheckAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_RESOLUTION, 0, digits);
    
    if (error == 0xBFFA0010)   /* invalid value */
        viCheckErrElab(error,"Invalid resolution value based on current range setting.");
    else viCheckErr(error);
    
Error:
    return error;
}


static ViStatus _VI_FUNC avr6552AttrResolutionAbsolute_ReadCallback (ViSession vi,
                                                                     ViSession io,
                                                                     ViConstString channelName,
                                                                     ViAttr attributeId,
                                                                     ViReal64 *value)
{
    ViStatus    error = VI_SUCCESS;
    ViReal64    val, range, digit;

    *value = 0.0; 

    checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_RANGE, 
              0, &range));
    checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_RESOLUTION, 
              0, &digit));    
        
    val = (range / pow(10,digit));
    
    viCheckErr( Ivi_GetViReal64EntryFromValue (val, &attrResolutionAbsoluteRangeTable,
                                            VI_NULL, VI_NULL, value, VI_NULL,
                                            VI_NULL, VI_NULL));
Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrResolutionAbsolute_WriteCallback (ViSession vi,
                                                                      ViSession io,
                                                                      ViConstString channelName,
                                                                      ViAttr attributeId,
                                                                      ViReal64 value)
{
    ViStatus    error = VI_SUCCESS;
    ViReal64    range, digit;
    
    checkErr( Ivi_GetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_RANGE, 
              0, &range));
              
    digit = log10(range/value);
    
    checkErr( Ivi_SetAttributeViReal64 (vi, VI_NULL, AVR6552_ATTR_RESOLUTION, 
                 0, digit));
                 
Error:
    return error;
}

/*- AVR6552_ATTR_NULL_ENABLED -*/
/*- AVR6552_ATTR_SMOOTH_ENABLED -*/
/*- AVR6552_ATTR_COMPARE_ENABLED -*/
/*- AVR6552_ATTR_SCALE_ENABLED -*/
/*- AVR6552_ATTR_MAX_MIN_ENABLED -*/


static ViStatus _VI_FUNC avr6552AttrCalculationEnabled_WriteCallback (ViSession vi,
                                                                  ViSession io,
                                                                  ViConstString channelName,
                                                                  ViAttr attributeId,
                                                                  ViBoolean value)
{
    ViStatus    error = VI_SUCCESS;
    ViString    cmd;

    avr6552_GetCalculationCommandString (attributeId, &cmd);
    
    if (value == VI_FALSE)   
       viCheckErr (viPrintf (io, "%s0", cmd));
    else
       viCheckErr (viPrintf (io, "%s1", cmd));

Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrCalculationEnabled_ReadCallback (ViSession vi,
                                                                 ViSession io,
                                                                 ViConstString channelName,
                                                                 ViAttr attributeId,
                                                                 ViBoolean *value)
{
    ViStatus    error = VI_SUCCESS;
    ViInt32     result;
    ViString    cmd;

    avr6552_GetCalculationCommandString (attributeId, &cmd);
    
    viCheckErr (viQueryf (io, "%s?", "%*c%*c%d", cmd, &result));
    *value = result? VI_TRUE: VI_FALSE;
   
Error:
    return error;
}

/*- AVR6552_ATTR_DB_MODE -*/
static IviRangeTableEntry avr6552AttrDbModeRangeTableEntries[] =
    {
        {AVR6552_VAL_DB_OFF, 0, 0, "0", 0},
        {AVR6552_VAL_DB_ON, 0, 0, "1", 0},
        {AVR6552_VAL_DBM_ON, 0, 0, "2", 0},
        {IVI_RANGE_TABLE_LAST_ENTRY}
    };

static IviRangeTable avr6552AttrDbModeRangeTable =
    {
        IVI_VAL_DISCRETE,
        VI_FALSE,
        VI_FALSE,
        VI_NULL,
        avr6552AttrDbModeRangeTableEntries,
    };

static ViStatus _VI_FUNC avr6552AttrDbMode_WriteCallback (ViSession vi,
                                                          ViSession io,
                                                          ViConstString channelName,
                                                          ViAttr attributeId,
                                                          ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;

    viCheckErr (viPrintf (io, "DB%d", value));

Error:
    return error;
}

static ViStatus _VI_FUNC avr6552AttrDbMode_ReadCallback (ViSession vi,
                                                         ViSession io,
                                                         ViConstString channelName,
                                                         ViAttr attributeId,
                                                         ViInt32 *value)
{
    ViStatus    error = VI_SUCCESS;
    
    viCheckErr (viQueryf (io, "DB?", "%*c%*c%d", value));

Error:
    return error;
}


static ViStatus _VI_FUNC Range_RangeTableCallback (ViSession vi,
                                                   ViConstString channelName,
                                                   ViAttr attributeId,
                                                   IviRangeTablePtr *rangeTablePtr)
{
    ViStatus    error = VI_SUCCESS;
    IviRangeTablePtr    tblPtr = VI_NULL;

    /*
        NOTE:  Insert code here to select the correct range table
        or to modify a dynamic range table.  Set the tblPtr
        local variable to the address of the range table.
    */

    *rangeTablePtr = tblPtr;
    return error;
}

static ViStatus _VI_FUNC SampleMode_CheckCallback (ViSession vi,
                                                   ViConstString channelName,
                                                   ViAttr attributeId,
                                                   ViInt32 value)
{
    ViStatus    error = VI_SUCCESS;

    return error;
}

/*****************************************************************************
 * Function: avr6552_InitAttributes                                                  
 * Purpose:  This function adds attributes to the IVI session, initializes   
 *           instrument attributes, and sets attribute invalidation          
 *           dependencies.                                                   
 *****************************************************************************/
static ViStatus avr6552_InitAttributes (ViSession vi, ViInt32 model)
{
    ViStatus    error = VI_SUCCESS;
    
    /*- Initialize specific information for sertion model ----------------*/ 
    
    IviRangeTablePtr                avr6552AttrFunctionTblPtr = VI_NULL,
                                    avr6552AttrSampleRateTblPtr = VI_NULL;
    
    RangeTable_CallbackPtr          Range_RangeTableCallback = VI_NULL;
    CheckAttrViInt32_CallbackPtr    SampleMode_CheckCallback = VI_NULL;
    
    switch (model)
      {
      case AVR6552_VAL_MODEL_R6552:
           avr6552AttrFunctionTblPtr = &attrFunctionRangeTable1;
           Range_RangeTableCallback = avr6552AttrRange_RangeTableCallback1;
           SampleMode_CheckCallback = avr6552AttrSampleMode_CheckCallback1;
           avr6552AttrSampleRateTblPtr = &attrSampleRateRangeTable1;
           checkErr (Ivi_AddAttributeViInt32 (vi, AVR6552_ATTR_BURST_COUNT,
                                       "AVR6552_ATTR_BURST_COUNT", 1000, 0,
                                       avr6552AttrBurstCount_ReadCallback,
                                       avr6552AttrBurstCount_WriteCallback,
                                       &attrBurstCountRangeTable));
           checkErr (Ivi_AddAttributeViInt32 (vi, AVR6552_ATTR_DELAYED_TRIGGER,
                                       "AVR6552_ATTR_DELAYED_TRIGGER", 0, 0,
                                       avr6552AttrDelayedTrigger_ReadCallback,
                                       avr6552AttrDelayedTrigger_WriteCallback,
                                       &attrDelayedTriggerRangeTable));
           checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_INTEGRAL_TIME,
                                        "AVR6552_ATTR_INTEGRAL_TIME", 100.0, 0,
                                        avr6552AttrIntegralTime_ReadCallback,
                                        avr6552AttrIntegralTime_WriteCallback,
                                        &attrIntegralTimeRangeTable, 0));
           checkErr (Ivi_SetAttrCoerceCallbackViReal64 (vi, AVR6552_ATTR_INTEGRAL_TIME,
                                                 avr6552AttrIntegralTime_CoerceCallback));
           break;
      case AVR6552_VAL_MODEL_R6552T:
           avr6552AttrFunctionTblPtr = &attrFunctionRangeTable2;
           Range_RangeTableCallback = avr6552AttrRange_RangeTableCallback2;
           SampleMode_CheckCallback = avr6552AttrSampleMode_CheckCallback2;
           avr6552AttrSampleRateTblPtr = &attrSampleRateRangeTable2;
           break;
      case AVR6552_VAL_MODEL_R6552R:
           avr6552AttrFunctionTblPtr = &attrFunctionRangeTable3;
           Range_RangeTableCallback = avr6552AttrRange_RangeTableCallback3;
           SampleMode_CheckCallback = avr6552AttrSampleMode_CheckCallback2;
           avr6552AttrSampleRateTblPtr = &attrSampleRateRangeTable2;
           checkErr (Ivi_AddAttributeViInt32 (vi, AVR6552_ATTR_HIGH_PASS_FILTER,
                                       "AVR6552_ATTR_HIGH_PASS_FILTER",
                                       AVR6552_VAL_FILTER_OFF, 0,
                                       avr6552AttrHighPassFilter_ReadCallback,
                                       avr6552AttrHighPassFilter_WriteCallback,
                                       &attrHighPassFilterRangeTable));
           checkErr (Ivi_AddAttributeViInt32 (vi, AVR6552_ATTR_LOW_PASS_FILTER,
                                       "AVR6552_ATTR_LOW_PASS_FILTER",
                                       AVR6552_VAL_FILTER_OFF, 0,
                                       avr6552AttrLowPassFilter_ReadCallback,
                                       avr6552AttrLowPassFilter_WriteCallback,
                                       &attrLowPassFilterRangeTable));
           break;
      }    
        /*- Initialize instrument attributes --------------------------------*/            

    checkErr( Ivi_SetAttributeViInt32 (vi, "", AVR6552_ATTR_SPECIFIC_DRIVER_MAJOR_VERSION,
                                       0, AVR6552_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", AVR6552_ATTR_SPECIFIC_DRIVER_MINOR_VERSION,
                                       0, AVR6552_MINOR_VERSION));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, AVR6552_ATTR_SPECIFIC_DRIVER_REVISION,
                                               avr6552AttrSpecificDriverRevision_ReadCallback));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", AVR6552_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MAJOR_VERSION,
                                       0, AVR6552_CLASS_SPEC_MAJOR_VERSION));
    checkErr( Ivi_SetAttributeViInt32 (vi, "", AVR6552_ATTR_SPECIFIC_DRIVER_CLASS_SPEC_MINOR_VERSION,
                                       0, AVR6552_CLASS_SPEC_MINOR_VERSION));
    checkErr( Ivi_SetAttributeViString (vi, "", AVR6552_ATTR_IO_SESSION_TYPE,
                                        0, AVR6552_IO_SESSION_TYPE));
    checkErr( Ivi_SetAttributeViString (vi, "", AVR6552_ATTR_SUPPORTED_INSTRUMENT_MODELS,
                                        0, AVR6552_SUPPORTED_INSTRUMENT_MODELS));
    checkErr( Ivi_SetAttributeFlags (vi, AVR6552_ATTR_INSTRUMENT_FIRMWARE_REVISION,
                                     IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
    checkErr( Ivi_SetAttributeFlags (vi, AVR6552_ATTR_INSTRUMENT_MANUFACTURER,
                                     IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
    checkErr( Ivi_SetAttributeFlags (vi, AVR6552_ATTR_INSTRUMENT_MODEL,
                                     IVI_VAL_USE_CALLBACKS_FOR_SIMULATION));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, AVR6552_ATTR_INSTRUMENT_FIRMWARE_REVISION,
                                               avr6552AttrFirmwareRevision_ReadCallback));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, AVR6552_ATTR_INSTRUMENT_MANUFACTURER,
                                               avr6552AttrInstrumentManufacturer_ReadCallback));
    checkErr( Ivi_SetAttrReadCallbackViString (vi, AVR6552_ATTR_INSTRUMENT_MODEL,
                                               avr6552AttrInstrumentModel_ReadCallback));
    checkErr( Ivi_SetAttributeViString (vi, "", AVR6552_ATTR_SPECIFIC_DRIVER_VENDOR,
                                        0, AVR6552_DRIVER_VENDOR));
    checkErr( Ivi_SetAttributeViString (vi, "", AVR6552_ATTR_SPECIFIC_DRIVER_DESCRIPTION,
                                        0, AVR6552_DRIVER_DESCRIPTION));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_OPC_CALLBACK, 0,
                                      avr6552_WaitForOPCCallback));
    checkErr( Ivi_SetAttributeViAddr (vi, VI_NULL, IVI_ATTR_CHECK_STATUS_CALLBACK, 0,
                                      avr6552_CheckStatusCallback));
    checkErr( Ivi_SetAttributeViBoolean (vi, VI_NULL, IVI_ATTR_SUPPORTS_WR_BUF_OPER_MODE, 
                                         0, VI_FALSE));
    checkErr( Ivi_SetAttributeViString (vi, "", AVR6552_ATTR_GROUP_CAPABILITIES, 0,
                                        "IviDmmBase,"
                                        "IviDmmACMeasurement,"
                                        "IviDmmFrequencyMeasurement,"
                                        "IviDmmAutoZero"));
    
        /*- Add instrument-specific attributes ------------------------------*/            
    
    checkErr (Ivi_AddAttributeViInt32 (vi, AVR6552_ATTR_FUNCTION,
                                       "AVR6552_ATTR_FUNCTION",
                                       AVR6552_VAL_DC_VOLTS, 0,
                                       avr6552AttrFunction_ReadCallback,
                                       avr6552AttrFunction_WriteCallback,
                                       avr6552AttrFunctionTblPtr));
    checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, AVR6552_ATTR_FUNCTION,
                                               avr6552AttrFunction_CheckCallback));
    
    checkErr( Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_RANGE,
                                        "AVR6552_ATTR_RANGE",
                                        AVR6552_VAL_AUTO_RANGE_ON, 0,
                                        avr6552AttrRange_ReadCallback,
                                        avr6552AttrRange_WriteCallback,
                                        VI_NULL, 0));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AVR6552_ATTR_RANGE,
                                                avr6552AttrRange_CheckCallback));
    checkErr (Ivi_SetAttrRangeTableCallback (vi, AVR6552_ATTR_RANGE,
                                             Range_RangeTableCallback));
    
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_RESOLUTION_ABSOLUTE,
                                        "AVR6552_ATTR_RESOLUTION_ABSOLUTE", .0001,
                                        IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
                                        avr6552AttrResolutionAbsolute_ReadCallback,
                                        avr6552AttrResolutionAbsolute_WriteCallback,
                                        &attrResolutionAbsoluteRangeTable, 0));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AVR6552_ATTR_RESOLUTION_ABSOLUTE,
                                                avr6552AttrResolutionAbsolute_CheckCallback));
    checkErr (Ivi_SetAttrCoerceCallbackViReal64 (vi, AVR6552_ATTR_RESOLUTION_ABSOLUTE,
                                                avr6552AttrResolutionAbsolute_CoerceCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AVR6552_ATTR_TRIGGER_SOURCE,
                                       "AVR6552_ATTR_TRIGGER_SOURCE",
                                       AVR6552_VAL_IMMEDIATE,
                                       IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
                                       VI_NULL, VI_NULL,
                                       &attrTriggerSourceRangeTable));
    
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_AC_MIN_FREQ,
                                        "AVR6552_ATTR_AC_MIN_FREQ", 20.0, 0,
                                        avr6552AttrAcMinFreq_ReadCallback,
                                        avr6552AttrAcMinFreq_WriteCallback,
                                        &attrAcMinFreqRangeTable, 0));
    
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_AC_MAX_FREQ,
                                        "AVR6552_ATTR_AC_MAX_FREQ", 300000.0, 0,
                                        VI_NULL, VI_NULL, VI_NULL, 0));
    
    checkErr (Ivi_SetAttrRangeTableCallback (vi, AVR6552_ATTR_AC_MAX_FREQ,
                                             avr6552AttrAcMaxFreq_RangeTableCallback));
    
    checkErr( Ivi_AddAttributeViInt32  (vi, AVR6552_ATTR_AUTO_ZERO,
                                        "AVR6552_ATTR_AUTO_ZERO",
                                        AVR6552_VAL_AUTO_ZERO_OFF, 0,
                                        avr6552AttrAutoZero_ReadCallback,
                                        avr6552AttrAutoZero_WriteCallback,
                                        &attrAutoZeroRangeTable));
    checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, AVR6552_ATTR_AUTO_ZERO,
                                               avr6552AttrAutoZero_CheckCallback));
    
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_POWERLINE_FREQ,
                                        "AVR6552_ATTR_POWERLINE_FREQ",
                                        AVR6552_VAL_60_HERTZ,
                                        IVI_VAL_NOT_USER_WRITABLE,
                                        avr6552AttrPowerlineFreq_ReadCallback,
                                        VI_NULL, &attrPowerlineFreqRangeTable, 0));
    
    checkErr( Ivi_AddAttributeViInt32  (vi, AVR6552_ATTR_HOLD_SAMPLE_COUNT,
                                        "AVR6552_ATTR_HOLD_SAMPLE_COUNT", 1, 0,
                                        avr6552AttrHoldSampleCount_ReadCallback,
                                        avr6552AttrHoldSampleCount_WriteCallback,
                                        &attrHoldSampleCountRangeTable));
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_TRIGGER_DELAY,
                                        "AVR6552_ATTR_TRIGGER_DELAY", 0.0,
                                        0, VI_NULL, VI_NULL,
                                        VI_NULL, 0));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AVR6552_ATTR_TRIGGER_DELAY,
                                                avr6552AttrTriggerDelay_CheckCallback));
    checkErr( Ivi_AddAttributeViInt32 (vi, AVR6552_ATTR_OPC_TIMEOUT,
                                       "AVR6552_ATTR_OPC_TIMEOUT",
                                       5000, IVI_VAL_HIDDEN | IVI_VAL_DONT_CHECK_STATUS,
                                       VI_NULL, VI_NULL, VI_NULL)); 
    
    checkErr (Ivi_AddAttributeViInt32 (vi, AVR6552_ATTR_SAMPLE_MODE,
                                       "AVR6552_ATTR_SAMPLE_MODE",
                                       AVR6552_VAL_FREE_RUN, 0,
                                       avr6552AttrSampleMode_ReadCallback,
                                       avr6552AttrSampleMode_WriteCallback,
                                       &attrSampleModeRangeTable));
    checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, AVR6552_ATTR_SAMPLE_MODE,
                                               SampleMode_CheckCallback));
   
    checkErr (Ivi_AddAttributeViInt32 (vi, AVR6552_ATTR_SAMPLE_RATE,
                                       "AVR6552_ATTR_SAMPLE_RATE", AVR6552_VAL_SLOW,
                                       0, avr6552AttrSampleRate_ReadCallback,
                                       avr6552AttrSampleRate_WriteCallback, 
                                       avr6552AttrSampleRateTblPtr));
    checkErr (Ivi_SetAttrCheckCallbackViInt32 (vi, AVR6552_ATTR_SAMPLE_RATE,
                                               avr6552AttrSampleRate_CheckCallback));

    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_NULL_OFFSET,
                                        "AVR6552_ATTR_NULL_OFFSET", 0.0, 0,
                                        avr6552AttrNullOffset_ReadCallback,
                                        avr6552AttrNullOffset_WriteCallback,
                                        &attrNullConstantRangeTable, 0));
    checkErr (Ivi_SetAttrCheckCallbackViReal64 (vi, AVR6552_ATTR_NULL_OFFSET,
                                                avr6552AttrNullOffset_CheckCallback));
    
    checkErr (Ivi_AddAttributeViInt32 (vi, AVR6552_ATTR_SMOOTH_COUNT,
                                       "AVR6552_ATTR_SMOOTH_COUNT", 10, 0,
                                       avr6552AttrSmoothCount_ReadCallback,
                                       avr6552AttrSmoothCount_WriteCallback,
                                       &attrSmoothCountRangeTable));
    
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_COMPARE_HIGH_VAL,
                                        "AVR6552_ATTR_COMPARE_HIGH_VAL", 0.0, 0,
                                        avr6552AttrCompareHighVal_ReadCallback,
                                        avr6552AttrCompareHighVal_WriteCallback,
                                        &attrConstantRangeTable, 0));
    
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_COMPARE_LOW_VAL,
                                        "AVR6552_ATTR_COMPARE_LOW_VAL", 0.0, 0,
                                        avr6552AttrCompareLowVal_ReadCallback,
                                        avr6552AttrCompareLowVal_WriteCallback,
                                        &attrConstantRangeTable, 0));
    
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_SCALE_A,
                                        "AVR6552_ATTR_SCALE_A", 0.0, 0,
                                        avr6552AttrScaleA_ReadCallback,
                                        avr6552AttrScaleA_WriteCallback,
                                        &attrConstantRangeTable, 0));
    
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_FREQ_VOLTAGE_RANGE,
                                        "AVR6552_ATTR_FREQ_VOLTAGE_RANGE", 700.0, 0,
                                        VI_NULL, VI_NULL,
                                        &attrFreqVoltageRangeRangeTable, 0));
    checkErr (Ivi_SetAttrCoerceCallbackViReal64 (vi, AVR6552_ATTR_FREQ_VOLTAGE_RANGE,
                                                 avr6552AttrFreqVoltageRange_CoerceCallback));
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_SCALE_B,
                                        "AVR6552_ATTR_SCALE_B", 0.0, 0,
                                        avr6552AttrScaleB_ReadCallback,
                                        avr6552AttrScaleB_WriteCallback,
                                        &attrConstantRangeTable, 0));
    
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_SCALE_C,
                                        "AVR6552_ATTR_SCALE_C", 0.0, 0,
                                        avr6552AttrScaleC_ReadCallback,
                                        avr6552AttrScaleC_WriteCallback,
                                        &attrConstantRangeTable, 0));
    
    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_DB_CONSTANT,
                                        "AVR6552_ATTR_DB_CONSTANT", 1.0, 0,
                                        avr6552AttrDbConstant_ReadCallback,
                                        avr6552AttrDbConstant_WriteCallback,
                                        &attrDbConstantRangeTable, 0));

    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_RESOLUTION,
                                        "AVR6552_ATTR_RESOLUTION",
                                        AVR6552_VAL_5_5_DIGITS,
                                        IVI_VAL_USE_CALLBACKS_FOR_SIMULATION,
                                        avr6552AttrResolution_ReadCallback,
                                        avr6552AttrResolution_WriteCallback, VI_NULL,
                                        0));
    checkErr (Ivi_SetAttrRangeTableCallback (vi, AVR6552_ATTR_RESOLUTION,
                                             avr6552AttrResolution_RangeTableCallback));
    

    checkErr (Ivi_AddAttributeViReal64 (vi, AVR6552_ATTR_DELAY, "AVR6552_ATTR_DELAY",
                                        0.0, 0, avr6552AttrDelay_ReadCallback,
                                        avr6552AttrDelay_WriteCallback,
                                        &attrDelayRangeTable, 0));
    checkErr (Ivi_AddAttributeViBoolean (vi, AVR6552_ATTR_NULL_ENABLED,
                                         "AVR6552_ATTR_NULL_ENABLED", VI_FALSE, 0,
                                         avr6552AttrCalculationEnabled_ReadCallback,
                                         avr6552AttrCalculationEnabled_WriteCallback));
    checkErr (Ivi_AddAttributeViBoolean (vi, AVR6552_ATTR_SMOOTH_ENABLED,
                                         "AVR6552_ATTR_SMOOTH_ENABLED", VI_FALSE, 0,
                                         avr6552AttrCalculationEnabled_ReadCallback,
                                         avr6552AttrCalculationEnabled_WriteCallback));
    checkErr (Ivi_AddAttributeViBoolean (vi, AVR6552_ATTR_COMPARE_ENABLED,
                                         "AVR6552_ATTR_COMPARE_ENABLED", VI_FALSE, 0,
                                         avr6552AttrCalculationEnabled_ReadCallback,
                                         avr6552AttrCalculationEnabled_WriteCallback));
    checkErr (Ivi_AddAttributeViBoolean (vi, AVR6552_ATTR_SCALE_ENABLED,
                                         "AVR6552_ATTR_SCALE_ENABLED", VI_FALSE, 0,
                                         avr6552AttrCalculationEnabled_ReadCallback,
                                         avr6552AttrCalculationEnabled_WriteCallback));
    checkErr (Ivi_AddAttributeViBoolean (vi, AVR6552_ATTR_MAX_MIN_ENABLED,
                                         "AVR6552_ATTR_MAX_MIN_ENABLED", VI_FALSE, 0,
                                         avr6552AttrCalculationEnabled_ReadCallback,
                                         avr6552AttrCalculationEnabled_WriteCallback));
    checkErr (Ivi_AddAttributeViInt32 (vi, AVR6552_ATTR_DB_MODE,
                                       "AVR6552_ATTR_DB_MODE", AVR6552_VAL_DB_OFF, 0,
                                       avr6552AttrDbMode_ReadCallback,
                                       avr6552AttrDbMode_WriteCallback,
                                       &avr6552AttrDbModeRangeTable));

        /*- Setup attribute invalidations -----------------------------------*/            
    checkErr( Ivi_AddAttributeInvalidation (vi, AVR6552_ATTR_FUNCTION,
                                            AVR6552_ATTR_RANGE, VI_TRUE));
    checkErr( Ivi_AddAttributeInvalidation (vi, AVR6552_ATTR_FUNCTION,
                                            AVR6552_ATTR_RESOLUTION, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AVR6552_ATTR_FUNCTION,
                                            AVR6552_ATTR_RESOLUTION_ABSOLUTE,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AVR6552_ATTR_RANGE,
                                            AVR6552_ATTR_RESOLUTION_ABSOLUTE,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AVR6552_ATTR_RANGE,
                                            AVR6552_ATTR_RESOLUTION, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AVR6552_ATTR_SAMPLE_RATE,
                                            AVR6552_ATTR_RESOLUTION, VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AVR6552_ATTR_SAMPLE_RATE,
                                            AVR6552_ATTR_RESOLUTION_ABSOLUTE,
                                            VI_TRUE));
    checkErr (Ivi_AddAttributeInvalidation (vi, AVR6552_ATTR_SAMPLE_MODE,
                                            AVR6552_ATTR_RANGE, VI_TRUE));

Error:
    return error;
}

/*****************************************************************************
 *------------------- End Instrument Driver Source Code ---------------------*
 *****************************************************************************/
