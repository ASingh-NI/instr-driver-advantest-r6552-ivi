//==============================================================================
//:NoHelp
//:>  Title     : NiAvr6552Impl.h
//:>  Copyright : National Instruments 2001.  All Rights Reserved.
//:>  Purpose   : Defines the following classes:
//
//:>                  * CNiAvr6552Impl;
//==============================================================================
#pragma once

//==============================================================================
//:>  Includes
//==============================================================================
#include "ivisupport/DmmFlatClass.h"

//==============================================================================
//:>  Namespace
//==============================================================================
namespace Ivi {
namespace Avr6552 {
//==============================================================================
//:>  CFns
//==============================================================================
struct CFns {
    ViStatus (_VI_FUNC *ConfigureCompare) (
		ViSession,
		ViBoolean,
		ViReal64,
		ViReal64);
ViStatus (_VI_FUNC *ConfigureDecibel) (
		ViSession,
		ViInt32,
		ViReal64);
ViStatus (_VI_FUNC *ConfigureScale) (
		ViSession,
		ViBoolean,
		ViReal64,
		ViReal64,
		ViReal64);
ViStatus (_VI_FUNC *ConfigureSmooth) (
		ViSession,
		ViBoolean,
		ViInt32);
ViStatus (_VI_FUNC *ConfigureNull) (
		ViSession,
		ViBoolean,
		ViReal64);
ViStatus (_VI_FUNC *ConfigureSampleRate) (
		ViSession,
		ViInt32,
		ViReal64);
ViStatus (_VI_FUNC *ConfigureSampleMode) (
		ViSession,
		ViInt32,
		ViReal64);
ViStatus (_VI_FUNC *ReadHoldMeasurement) (
		ViSession,
		ViInt32,
		ViReal64*);
ViStatus (_VI_FUNC *ReadBurstMultiPoint) (
		ViSession,
		ViInt32,
		ViInt32,
		ViReal64*,
		ViInt32*);
ViStatus (_VI_FUNC *QueryMaxMin) (
		ViSession,
		ViReal64*,
		ViReal64*,
		ViReal64*,
		ViReal64*);
ViStatus (_VI_FUNC *FetchBurstMultiPoint) (
		ViSession,
		ViInt32,
		ViInt32,
		ViReal64*,
		ViInt32*);
ViStatus (_VI_FUNC *ForceTrigger) (
		ViSession);
ViStatus (_VI_FUNC *ConfigureRippleVoltsFilter) (
		ViSession,
		ViInt32,
		ViInt32);

}; // struct CFns

//==============================================================================
//:>  CImpl
//==============================================================================
class CImpl : public Ivi::Dmm::CImpl {
public:
    //Destructor
    virtual ~CImpl(void) {};
    
    inline void ConfigureCompare (
        ViBoolean Enable,
        ViReal64 HighValue,
        ViReal64 LowValue)
    {
        CheckStatus (m_avr6552Fns.ConfigureCompare (Session(), Enable, HighValue, LowValue));
    }
inline void ConfigureDecibel (
        ViInt32 Mode,
        ViReal64 DecibelValue)
    {
        CheckStatus (m_avr6552Fns.ConfigureDecibel (Session(), Mode, DecibelValue));
    }
inline void ConfigureScale (
        ViBoolean Enable,
        ViReal64 ConstantA,
        ViReal64 ConstantB,
        ViReal64 ConstantC)
    {
        CheckStatus (m_avr6552Fns.ConfigureScale (Session(), Enable, ConstantA, ConstantB, ConstantC));
    }
inline void ConfigureSmooth (
        ViBoolean Enable,
        ViInt32 SmoothCount)
    {
        CheckStatus (m_avr6552Fns.ConfigureSmooth (Session(), Enable, SmoothCount));
    }
inline void ConfigureNull (
        ViBoolean Enable,
        ViReal64 NullOffset)
    {
        CheckStatus (m_avr6552Fns.ConfigureNull (Session(), Enable, NullOffset));
    }
inline void ConfigureSampleRate (
        ViInt32 SampleRate,
        ViReal64 IntegralTimemilliseconds)
    {
        CheckStatus (m_avr6552Fns.ConfigureSampleRate (Session(), SampleRate, IntegralTimemilliseconds));
    }
inline void ConfigureSampleMode (
        ViInt32 SampleMode,
        ViReal64 Delaysec)
    {
        CheckStatus (m_avr6552Fns.ConfigureSampleMode (Session(), SampleMode, Delaysec));
    }
inline void ReadHoldMeasurement (
        ViInt32 MaximumTimems,
        ViReal64* Reading)
    {
        CheckStatus (m_avr6552Fns.ReadHoldMeasurement (Session(), MaximumTimems, Reading));
    }
inline void ReadBurstMultiPoint (
        ViInt32 MaximumTimems,
        ViInt32 ArraySize,
        ViReal64* ReadingArray,
        ViInt32* ActualNumberofPoints)
    {
        CheckStatus (m_avr6552Fns.ReadBurstMultiPoint (Session(), MaximumTimems, ArraySize, ReadingArray, ActualNumberofPoints));
    }
inline void QueryMaxMin (
        ViReal64* MaxValue,
        ViReal64* MinValue,
        ViReal64* AverageValue,
        ViReal64* CycleCount)
    {
        CheckStatus (m_avr6552Fns.QueryMaxMin (Session(), MaxValue, MinValue, AverageValue, CycleCount));
    }
inline void FetchBurstMultiPoint (
        ViInt32 MaximumTimems,
        ViInt32 ArraySize,
        ViReal64* ReadingArray,
        ViInt32* ActualNumberofPoints)
    {
        CheckStatus (m_avr6552Fns.FetchBurstMultiPoint (Session(), MaximumTimems, ArraySize, ReadingArray, ActualNumberofPoints));
    }
inline void ForceTrigger (
)
    {
        CheckStatus (m_avr6552Fns.ForceTrigger (Session()));
    }
inline void ConfigureRippleVoltsFilter (
        ViInt32 HighPassFilter,
        ViInt32 LowPassFilter)
    {
        CheckStatus (m_avr6552Fns.ConfigureRippleVoltsFilter (Session(), HighPassFilter, LowPassFilter));
    }

    
protected:
    //Constructor
    CImpl (const Ivi::CFns& inherent_fns, const Ivi::Dmm::CFns& dmm_fns, const CFns& avr6552_fns) :
         Ivi::Dmm::CImpl(inherent_fns, dmm_fns),
      m_avr6552Fns(avr6552_fns)
    {
    }

private:
    //Data
    const CFns& m_avr6552Fns;
}; //CImpl

}; //namespace Avr6552

}; //namespace Ivi

//==============================================================================
//:>  Namespace
//==============================================================================
namespace NI {

//==============================================================================
//:>  CNiAvr6552Impl
//==============================================================================
class CNiAvr6552Impl : public Ivi::Avr6552::CImpl {
public:
    //-------------------------------------------------------------------------
    //:> Constructors
    //-------------------------------------------------------------------------
    CNiAvr6552Impl (void) :
        Ivi::Avr6552::CImpl(inherentFns, classFns, specificFns){}

private:
    //-------------------------------------------------------------------------
    //:>  Data
    //-------------------------------------------------------------------------
    static const Ivi::CFns inherentFns;
    static const Ivi::Dmm::CFns classFns;
        static const Ivi::Avr6552::CFns specificFns;
};

}; // namespace NI
