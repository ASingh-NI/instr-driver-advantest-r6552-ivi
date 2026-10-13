//:NoHelp
#include <avr6552.h>
#include "NiAvr6552Impl.h"

namespace NI {

//------------------------------------------------------------------------
//:> Static Constants
//------------------------------------------------------------------------
const Ivi::CFns CNiAvr6552Impl::inherentFns = {
    "avr6552",
    AVR6552_ATTR_SPECIFIC_DRIVER_PREFIX,
    avr6552_InitWithOptions, 
    avr6552_close, 
    avr6552_reset, 
    avr6552_ResetWithDefaults, 
    avr6552_self_test, 
    avr6552_error_query, 
    avr6552_error_message, 
    avr6552_Disable, 
    avr6552_LockSession, 
    avr6552_UnlockSession, 
    avr6552_GetNextInterchangeWarning, 
    avr6552_ClearInterchangeWarnings, 
    avr6552_ResetInterchangeCheck, 
    avr6552_GetNextCoercionRecord, 
    avr6552_SetAttributeViBoolean, 
    avr6552_SetAttributeViInt32, 
    avr6552_SetAttributeViReal64, 
    avr6552_SetAttributeViString, 
    avr6552_SetAttributeViSession, 
    avr6552_GetAttributeViBoolean, 
    avr6552_GetAttributeViInt32, 
    avr6552_GetAttributeViReal64, 
    avr6552_GetAttributeViString, 
    avr6552_GetAttributeViSession, 
    avr6552_InvalidateAllAttributes, 
    NULL, // IviDmm_GetSpecificDriverCHandle
    avr6552_GetChannelName, 
};


const Ivi::Dmm::CFns CNiAvr6552Impl::classFns = {
    avr6552_ConfigureMeasurement, 
    avr6552_ConfigureTrigger, 
    avr6552_Read, 
    avr6552_Fetch, 
    avr6552_Abort, 
    avr6552_Initiate, 
    avr6552_IsOverRange, 
    avr6552_ConfigureACBandwidth, 
    NULL, //avr6552_ConfigureThermocouple
    NULL, //avr6552_ConfigureRTD
    NULL, //avr6552_ConfigureMultiPoint
    NULL, //avr6552_ReadMultiPoint
    NULL, //avr6552_FetchMultiPoint
    NULL, //avr6552_SendSoftwareTrigger
};

const Ivi::Avr6552::CFns CNiAvr6552Impl::specificFns = {
    avr6552_ConfigureCompare,
avr6552_ConfigureDecibel,
avr6552_ConfigureScale,
avr6552_ConfigureSmooth,
avr6552_ConfigureNull,
avr6552_ConfigureSampleRate,
avr6552_ConfigureSampleMode,
avr6552_ReadHoldMeasurement,
avr6552_ReadBurstMultiPoint,
avr6552_QueryMaxMin,
avr6552_FetchBurstMultiPoint,
avr6552_ForceTrigger,
avr6552_ConfigureRippleVoltsFilter,

};

};