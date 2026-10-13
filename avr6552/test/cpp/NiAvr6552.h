//==============================================================================
//
//:>  Title     : NiAvr6552.h
//:>  Copyright : National Instruments 2001.  All Rights Reserved.
//:>  Purpose   : Defines the following classes:
//  
//:>                  * CNiAvr6552
//
//==============================================================================
#pragma once
#pragma comment(lib, "avr6552.lib")

//==============================================================================
//:>  Includes
//==============================================================================
#include <avr6552.h>
#include "NiIviDriver.h"
#include "NiAvr6552Impl.h"

//==============================================================================
//:>  Macros
//==============================================================================
#define GET_PUT_PROPERTY(type, name, id)\
__declspec(property(put=Set##name,get=Get##name)) type name;\
void Set##name (const type& value) { impl.Set("",AVR6552_ATTR_##id,value); }\
type Get##name (void) const {\
    type value;\
    impl.Get("",AVR6552_ATTR_##id,value); \
    return value;\
}
#define GET_PROPERTY(type, name, id)\
__declspec(property(get=Get##name)) type name;\
type Get##name (void) const {\
    type value;\
    impl.Get("",AVR6552_ATTR_##id,value); \
    return value;\
}

//==============================================================================
//:>  Namespace
//==============================================================================
namespace NI {

//==============================================================================
//:>  Type Definitions
//==============================================================================
typedef CNiIviSubObject<CNiAvr6552Impl> CNiAvr6552SubObject;

//=============================================================================
//:>  CNiAvr6552AdvancedCompare
//=============================================================================
////@CNiAvr6552AdvancedCompare
class CNiAvr6552AdvancedCompare: CNiAvr6552SubObject {
public:
    //-------------------------------------------------------------------------
    //:> Constructor
    //-------------------------------------------------------------------------
    //:Ignore
    CNiAvr6552AdvancedCompare (CNiAvr6552Impl& impl) : CNiAvr6552SubObject(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Properties
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedCompareEnabled
    GET_PUT_PROPERTY(bool, Enabled, COMPARE_ENABLED);

    //@CNiAvr6552AdvancedCompareHigh
    GET_PUT_PROPERTY(double, High, COMPARE_HIGH_VAL);

    //@CNiAvr6552AdvancedCompareLow
    GET_PUT_PROPERTY(double, Low, COMPARE_LOW_VAL);


    //-------------------------------------------------------------------------
    //:> Methods
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedCompareConfigureCompare
    inline void ConfigureCompare(
        //@CNiAvr6552AdvancedCompareConfigureCompareEnable
        bool Enable,
        //@CNiAvr6552AdvancedCompareConfigureCompareHighValue
        double HighValue,
        //@CNiAvr6552AdvancedCompareConfigureCompareLowValue
        double LowValue);

};
//=============================================================================
//:>  CNiAvr6552AdvancedDecibel
//=============================================================================
////@CNiAvr6552AdvancedDecibel
class CNiAvr6552AdvancedDecibel: CNiAvr6552SubObject {
public:
    //-------------------------------------------------------------------------
    //:> Constructor
    //-------------------------------------------------------------------------
    //:Ignore
    CNiAvr6552AdvancedDecibel (CNiAvr6552Impl& impl) : CNiAvr6552SubObject(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Properties
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedDecibelMode
    GET_PUT_PROPERTY(long, Mode, DB_MODE);

    //@CNiAvr6552AdvancedDecibelConstant
    GET_PUT_PROPERTY(double, Constant, DB_CONSTANT);


    //-------------------------------------------------------------------------
    //:> Methods
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedDecibelConfigureDecibel
    inline void ConfigureDecibel(
        //@CNiAvr6552AdvancedDecibelConfigureDecibelMode
        long Mode,
        //@CNiAvr6552AdvancedDecibelConfigureDecibelDecibelValue
        double DecibelValue);

};
//=============================================================================
//:>  CNiAvr6552AdvancedScale
//=============================================================================
////@CNiAvr6552AdvancedScale
class CNiAvr6552AdvancedScale: CNiAvr6552SubObject {
public:
    //-------------------------------------------------------------------------
    //:> Constructor
    //-------------------------------------------------------------------------
    //:Ignore
    CNiAvr6552AdvancedScale (CNiAvr6552Impl& impl) : CNiAvr6552SubObject(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Properties
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedScaleScaleC
    GET_PUT_PROPERTY(double, ScaleC, SCALE_C);

    //@CNiAvr6552AdvancedScaleScaleB
    GET_PUT_PROPERTY(double, ScaleB, SCALE_B);

    //@CNiAvr6552AdvancedScaleScaleA
    GET_PUT_PROPERTY(double, ScaleA, SCALE_A);

    //@CNiAvr6552AdvancedScaleEnabled
    GET_PUT_PROPERTY(bool, Enabled, SCALE_ENABLED);


    //-------------------------------------------------------------------------
    //:> Methods
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedScaleConfigureScale
    inline void ConfigureScale(
        //@CNiAvr6552AdvancedScaleConfigureScaleEnable
        bool Enable,
        //@CNiAvr6552AdvancedScaleConfigureScaleConstantA
        double ConstantA,
        //@CNiAvr6552AdvancedScaleConfigureScaleConstantB
        double ConstantB,
        //@CNiAvr6552AdvancedScaleConfigureScaleConstantC
        double ConstantC);

};
//=============================================================================
//:>  CNiAvr6552AdvancedMinMax
//=============================================================================
////@CNiAvr6552AdvancedMinMax
class CNiAvr6552AdvancedMinMax: CNiAvr6552SubObject {
public:
    //-------------------------------------------------------------------------
    //:> Constructor
    //-------------------------------------------------------------------------
    //:Ignore
    CNiAvr6552AdvancedMinMax (CNiAvr6552Impl& impl) : CNiAvr6552SubObject(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Properties
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedMinMaxEnabled
    GET_PUT_PROPERTY(bool, Enabled, MAX_MIN_ENABLED);


};
//=============================================================================
//:>  CNiAvr6552AdvancedSmooth
//=============================================================================
////@CNiAvr6552AdvancedSmooth
class CNiAvr6552AdvancedSmooth: CNiAvr6552SubObject {
public:
    //-------------------------------------------------------------------------
    //:> Constructor
    //-------------------------------------------------------------------------
    //:Ignore
    CNiAvr6552AdvancedSmooth (CNiAvr6552Impl& impl) : CNiAvr6552SubObject(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Properties
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedSmoothEnabled
    GET_PUT_PROPERTY(bool, Enabled, SMOOTH_ENABLED);

    //@CNiAvr6552AdvancedSmoothCount
    GET_PUT_PROPERTY(long, Count, SMOOTH_COUNT);


    //-------------------------------------------------------------------------
    //:> Methods
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedSmoothConfigureSmooth
    inline void ConfigureSmooth(
        //@CNiAvr6552AdvancedSmoothConfigureSmoothEnable
        bool Enable,
        //@CNiAvr6552AdvancedSmoothConfigureSmoothSmoothCount
        long SmoothCount);

};
//=============================================================================
//:>  CNiAvr6552AdvancedNullCalculation
//=============================================================================
////@CNiAvr6552AdvancedNullCalculation
class CNiAvr6552AdvancedNullCalculation: CNiAvr6552SubObject {
public:
    //-------------------------------------------------------------------------
    //:> Constructor
    //-------------------------------------------------------------------------
    //:Ignore
    CNiAvr6552AdvancedNullCalculation (CNiAvr6552Impl& impl) : CNiAvr6552SubObject(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Properties
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedNullCalculationOffset
    GET_PUT_PROPERTY(double, Offset, NULL_OFFSET);

    //@CNiAvr6552AdvancedNullCalculationEnabled
    GET_PUT_PROPERTY(bool, Enabled, NULL_ENABLED);


    //-------------------------------------------------------------------------
    //:> Methods
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedNullCalculationConfigureNull
    inline void ConfigureNull(
        //@CNiAvr6552AdvancedNullCalculationConfigureNullEnable
        bool Enable,
        //@CNiAvr6552AdvancedNullCalculationConfigureNullNullOffset
        double NullOffset);

};
//=============================================================================
//:>  CNiAvr6552RippleVoltageFilter
//=============================================================================
////@CNiAvr6552RippleVoltageFilter
class CNiAvr6552RippleVoltageFilter: CNiAvr6552SubObject {
public:
    //-------------------------------------------------------------------------
    //:> Constructor
    //-------------------------------------------------------------------------
    //:Ignore
    CNiAvr6552RippleVoltageFilter (CNiAvr6552Impl& impl) : CNiAvr6552SubObject(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Properties
    //-------------------------------------------------------------------------
    //@CNiAvr6552RippleVoltageFilterLowPassFilter
    GET_PUT_PROPERTY(long, LowPassFilter, LOW_PASS_FILTER);

    //@CNiAvr6552RippleVoltageFilterHighPassFilter
    GET_PUT_PROPERTY(long, HighPassFilter, HIGH_PASS_FILTER);


    //-------------------------------------------------------------------------
    //:> Methods
    //-------------------------------------------------------------------------
    //@CNiAvr6552RippleVoltageFilterConfigureRippleVoltsFilter
    inline void ConfigureRippleVoltsFilter(
        //@CNiAvr6552RippleVoltageFilterConfigureRippleVoltsFilterHighPassFilter
        long HighPassFilter,
        //@CNiAvr6552RippleVoltageFilterConfigureRippleVoltsFilterLowPassFilter
        long LowPassFilter);

};


//==============================================================================
//:>  CNiAvr6552Ac
//==============================================================================
//@CNiAvr6552Ac
class CNiAvr6552Ac : CNiAvr6552SubObject {
public:
    //---------------------------------------------------------------------
    //:> Constructor
    //---------------------------------------------------------------------
    //:Ignore
    CNiAvr6552Ac (CNiAvr6552Impl& impl) : CNiAvr6552SubObject(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Properties
    //-------------------------------------------------------------------------
	//@CNiAvr6552AcFrequencyMax
    GET_PUT_PROPERTY(double, FrequencyMax, AC_MAX_FREQ);
	
	//@CNiAvr6552AcFrequencyMin
    GET_PUT_PROPERTY(double, FrequencyMin, AC_MIN_FREQ);
	

    //-------------------------------------------------------------------------
    //:> Functions
    //-------------------------------------------------------------------------
    //@CNiAvr6552AcConfigureBandwidth
    inline void ConfigureBandwidth (
		//@CNiAvr6552AcConfigureBandwidth1
        double min, 
        //@CNiAvr6552AcConfigureBandwidth2
        double max);
    
};

//==============================================================================
//:>  CNiAvr6552Advanced
//==============================================================================
//@CNiAvr6552Advanced
class CNiAvr6552Advanced : CNiAvr6552SubObject{
public:
    //---------------------------------------------------------------------
    //:> Constructor
    //---------------------------------------------------------------------
    //:Ignore
    CNiAvr6552Advanced (CNiAvr6552Impl& impl) : CNiAvr6552SubObject(impl), Compare(impl), Decibel(impl), Scale(impl), MinMax(impl), Smooth(impl), NullCalculation(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Properties
    //-------------------------------------------------------------------------
	    
	//@CNiAvr6552AdvancedAutoZero
    GET_PUT_PROPERTY(long, AutoZero, AUTO_ZERO);
	    
	//@CNiAvr6552AdvancedPowerlineFrequency
    GET_PUT_PROPERTY(double, PowerlineFrequency, POWERLINE_FREQ);
	
    //@CNiAvr6552AdvancedSampleRate
    GET_PUT_PROPERTY(long, SampleRate, SAMPLE_RATE);

    //@CNiAvr6552AdvancedIntegralTime
    GET_PUT_PROPERTY(double, IntegralTime, INTEGRAL_TIME);

    //@CNiAvr6552AdvancedSampleMode
    GET_PUT_PROPERTY(long, SampleMode, SAMPLE_MODE);

    //@CNiAvr6552AdvancedDelay
    GET_PUT_PROPERTY(double, Delay, DELAY);

    //@CNiAvr6552AdvancedHoldSampleCount
    GET_PUT_PROPERTY(long, HoldSampleCount, HOLD_SAMPLE_COUNT);

    //@CNiAvr6552AdvancedBurstCount
    GET_PUT_PROPERTY(long, BurstCount, BURST_COUNT);

    //@CNiAvr6552AdvancedDelayedTrigger
    GET_PUT_PROPERTY(long, DelayedTrigger, DELAYED_TRIGGER);

    //-------------------------------------------------------------------------
    //:> Functions
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedConfigureSampleRate
    inline void ConfigureSampleRate(
        //@CNiAvr6552AdvancedConfigureSampleRateSampleRate
        long SampleRate,
        //@CNiAvr6552AdvancedConfigureSampleRateIntegralTimemilliseconds
        double IntegralTimemilliseconds);
    //@CNiAvr6552AdvancedConfigureSampleMode
    inline void ConfigureSampleMode(
        //@CNiAvr6552AdvancedConfigureSampleModeSampleMode
        long SampleMode,
        //@CNiAvr6552AdvancedConfigureSampleModeDelaysec
        double Delaysec);


    //-------------------------------------------------------------------------
    //:> Subobjects
    //-------------------------------------------------------------------------
    //@CNiAvr6552AdvancedCompare
    CNiAvr6552AdvancedCompare Compare;
    //@CNiAvr6552AdvancedDecibel
    CNiAvr6552AdvancedDecibel Decibel;
    //@CNiAvr6552AdvancedScale
    CNiAvr6552AdvancedScale Scale;
    //@CNiAvr6552AdvancedMinMax
    CNiAvr6552AdvancedMinMax MinMax;
    //@CNiAvr6552AdvancedSmooth
    CNiAvr6552AdvancedSmooth Smooth;
    //@CNiAvr6552AdvancedNullCalculation
    CNiAvr6552AdvancedNullCalculation NullCalculation;

};

//==============================================================================
//:>  CNiAvr6552Frequency
//==============================================================================
//@CNiAvr6552Frequency
class CNiAvr6552Frequency : CNiAvr6552SubObject{
public:
    //---------------------------------------------------------------------
    //:> Constructor
    //---------------------------------------------------------------------
    //:Ignore
    CNiAvr6552Frequency (CNiAvr6552Impl& impl) : CNiAvr6552SubObject(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Properties
    //-------------------------------------------------------------------------
    //@CNiAvr6552FrequencyVoltageRange
    GET_PUT_PROPERTY(double, VoltageRange, FREQ_VOLTAGE_RANGE);
    
};


//==============================================================================
//:>  CNiAvr6552Measurement
//==============================================================================
//@CNiAvr6552Measurement
class CNiAvr6552Measurement : CNiAvr6552SubObject {
public:
    //-------------------------------------------------------------------------
    //:> Constructor
    //-------------------------------------------------------------------------
    //:Ignore
    CNiAvr6552Measurement(CNiAvr6552Impl& impl): CNiAvr6552SubObject(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Functions
    //-------------------------------------------------------------------------
    //@CNiAvr6552MeasurementRead
    inline double Read (
        //@CNiAvr6552MeasurementRead1
        long int maxTime);
	
    //@CNiAvr6552MeasurementFetch
    inline double Fetch (
	    //@CNiAvr6552MeasurementFetch1
		long int maxTime);
    
    //@CNiAvr6552MeasurementAbort
    inline void Abort (void);
	
	//@CNiAvr6552MeasurementInitiate
    inline void Initiate (void);
	
	//@CNiAvr6552MeasurementIsOverRange
    inline bool IsOverRange (
	//@CNiAvr6552MeasurementIsOverRange1
        double measurementValue);
	
    //@CNiAvr6552MeasurementReadHoldMeasurement
    inline void ReadHoldMeasurement(
        //@CNiAvr6552MeasurementReadHoldMeasurementMaximumTimems
        long MaximumTimems,
        //@CNiAvr6552MeasurementReadHoldMeasurementReading
        double* Reading);
    //@CNiAvr6552MeasurementReadBurstMultiPoint
    inline void ReadBurstMultiPoint(
        //@CNiAvr6552MeasurementReadBurstMultiPointMaximumTimems
        long MaximumTimems,
        //@CNiAvr6552MeasurementReadBurstMultiPointArraySize
        long ArraySize,
        //@CNiAvr6552MeasurementReadBurstMultiPointReadingArray
        double* ReadingArray,
        //@CNiAvr6552MeasurementReadBurstMultiPointActualNumberofPoints
        long* ActualNumberofPoints);
    //@CNiAvr6552MeasurementQueryMaxMin
    inline void QueryMaxMin(
        //@CNiAvr6552MeasurementQueryMaxMinMaxValue
        double* MaxValue,
        //@CNiAvr6552MeasurementQueryMaxMinMinValue
        double* MinValue,
        //@CNiAvr6552MeasurementQueryMaxMinAverageValue
        double* AverageValue,
        //@CNiAvr6552MeasurementQueryMaxMinCycleCount
        double* CycleCount);
    //@CNiAvr6552MeasurementFetchBurstMultiPoint
    inline void FetchBurstMultiPoint(
        //@CNiAvr6552MeasurementFetchBurstMultiPointMaximumTimems
        long MaximumTimems,
        //@CNiAvr6552MeasurementFetchBurstMultiPointArraySize
        long ArraySize,
        //@CNiAvr6552MeasurementFetchBurstMultiPointReadingArray
        double* ReadingArray,
        //@CNiAvr6552MeasurementFetchBurstMultiPointActualNumberofPoints
        long* ActualNumberofPoints);

};



//==============================================================================
//:>  CNiAvr6552Trigger
//==============================================================================
//@CNiAvr6552Trigger
class CNiAvr6552Trigger : CNiAvr6552SubObject {
public:
    //-------------------------------------------------------------------------
    //:> Constructor
    //-------------------------------------------------------------------------
    //:Ignore
    CNiAvr6552Trigger::CNiAvr6552Trigger(CNiAvr6552Impl& impl) :
        CNiAvr6552SubObject(impl){}
    //:End Ignore

    //-------------------------------------------------------------------------
    //:> Properties
    //-------------------------------------------------------------------------
        
    //@CNiAvr6552TriggerSource
    GET_PUT_PROPERTY (long, Source, TRIGGER_SOURCE);
	    
    //@CNiAvr6552TriggerDelay
    GET_PUT_PROPERTY (double, Delay, TRIGGER_DELAY);
	
    //-------------------------------------------------------------------------
    //:> Functions
    //-------------------------------------------------------------------------
    //@CNiAvr6552TriggerConfigure
	inline void Configure (
	    //@CNiAvr6552TriggerConfigure1
        long source,
        //@CNiAvr6552TriggerConfigure2
        double delay);
	
    //@CNiAvr6552TriggerForceTrigger
    inline void ForceTrigger(
);

};

//==============================================================================
//:>  CNiAvr6552
//==============================================================================
//@CNiAvr6552
class CNiAvr6552 : public CNiIviDriver {
public:
    //--------------------------------------------------------------------------
    //:> Static Constants
    //--------------------------------------------------------------------------
    static const long VoltsDcFunction;
    static const long VoltsAcFunction;
    static const long CurrentDcFunction;
    static const long CurrentAcFunction;
    static const long Resistance2WireFunction;
    static const long Resistance4WireFunction;
    static const long VoltsAcPlusDcFunction;
    static const long CurrentAcPlusDcFunction;
    static const long FrequencyFunction;
    static const long OffAutoZero;
    static const long OnAutoZero;
    static const long OnceAutoZero;
    static const long ImmediateSampleTrigger;
    static const long ImmediateTrigger;
    static const long InfiniteMaxTime;
    static const long ImmediateMaxTime;
    static const double OnAutoRange;
    static const long Diode;
    static const long Lp2WireRes;
    static const long Lp4WireRes;
    static const long RippleVolts;
    static const double Frequency50Hz;
    static const double Frequency60Hz;
    static const long FreeRun;
    static const long Hold;
    static const long Burst;
    static const long Fast;
    static const long Med;
    static const long Slow;
    static const long Longit;
    static const long FilterOff;
    static const double MeasureValue;
    static const long DbOff;
    static const long DbOn;
    static const long DbmOn;

    //--------------------------------------------------------------------------
    //:> Constructors
    //--------------------------------------------------------------------------
    //@IviDefaultConstructor
    inline CNiAvr6552 (void);
    
    //@IviConstructor
    inline CNiAvr6552 (
        //@CNiIviDriverSetSessionRsrcName
        const CString &resourceName,
        //@CNiIviDriverSetSessionIdQuery
        bool idQuery = true,
        //@CNiIviDriverSetSessionReset
        bool reset = true,
        //@CNiIviDriverSetSessionOptions
        const CString &options = "");

    //--------------------------------------------------------------------------
    //:> Properties
    //--------------------------------------------------------------------------
    //@CNiAvr6552Function
    GET_PUT_PROPERTY(long, Function, FUNCTION);
	    
    //@CNiAvr6552Range
    GET_PUT_PROPERTY(double, Range, RANGE);
	    
    //@CNiAvr6552Resolution
	GET_PUT_PROPERTY(double, Resolution, RESOLUTION_ABSOLUTE);
	
    //@CNiAvr6552IdQueryResponse
    GET_PROPERTY(CString, IdQueryResponse, ID_QUERY_RESPONSE);


    //-------------------------------------------------------------------------
    //:> Functions
    //-------------------------------------------------------------------------
	//@CNiAvr6552Configure
    inline void Configure (
	    //@CNiAvr6552Configure1
        long function,
        //@CNiAvr6552Configure2
        double range,
        //@CNiAvr6552Configure3
        double resolution);
	

    //-------------------------------------------------------------------------
    //:> Subobjects
    //-------------------------------------------------------------------------
    //@CNiAvr6552Ac
    CNiAvr6552Ac Ac;
    //@CNiAvr6552Advanced
    CNiAvr6552Advanced Advanced;
    //@CNiAvr6552Frequency
    CNiAvr6552Frequency Frequency;
    //@CNiAvr6552Measurement
    CNiAvr6552Measurement Measurement;
    //@CNiAvr6552Trigger
    CNiAvr6552Trigger Trigger;
    //@CNiAvr6552RippleVoltageFilter
    CNiAvr6552RippleVoltageFilter RippleVoltageFilter;

private:
    //-------------------------------------------------------------------------
    //:> Data
    //-------------------------------------------------------------------------
    CNiAvr6552Impl impl;

};  //  class CNiAvr6552

//==============================================================================
//:>  Inline includes
//==============================================================================
#include "NiAvr6552.inl"
}  //  namespace NI

//:Ignore
//==============================================================================
//:>  Macro Undefines
//==============================================================================
#undef GET_PUT_PROPERTY
#undef GET_PROPERTY
//:End Ignore