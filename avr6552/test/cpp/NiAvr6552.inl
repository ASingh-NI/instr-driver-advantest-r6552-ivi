//==============================================================================
//:NoHelp
//  Title     : NiAvr6552.inl
//  Copyright : National Instruments 2000.  All Rights Reserved.
//  Purpose   : Implements the following class(es):
//
//						* CNiAvr6552
//
//==============================================================================

//==============================================================================
//:> CNiAvr6552AdvancedCompare
//==============================================================================

void CNiAvr6552AdvancedCompare::ConfigureCompare(
        bool Enable,
        double HighValue,
        double LowValue)
{
    impl.ConfigureCompare(Enable, HighValue, LowValue);
}
//==============================================================================
//:> CNiAvr6552AdvancedDecibel
//==============================================================================

void CNiAvr6552AdvancedDecibel::ConfigureDecibel(
        long Mode,
        double DecibelValue)
{
    impl.ConfigureDecibel(Mode, DecibelValue);
}
//==============================================================================
//:> CNiAvr6552AdvancedScale
//==============================================================================

void CNiAvr6552AdvancedScale::ConfigureScale(
        bool Enable,
        double ConstantA,
        double ConstantB,
        double ConstantC)
{
    impl.ConfigureScale(Enable, ConstantA, ConstantB, ConstantC);
}
//==============================================================================
//:> CNiAvr6552AdvancedSmooth
//==============================================================================

void CNiAvr6552AdvancedSmooth::ConfigureSmooth(
        bool Enable,
        long SmoothCount)
{
    impl.ConfigureSmooth(Enable, SmoothCount);
}
//==============================================================================
//:> CNiAvr6552AdvancedNullCalculation
//==============================================================================

void CNiAvr6552AdvancedNullCalculation::ConfigureNull(
        bool Enable,
        double NullOffset)
{
    impl.ConfigureNull(Enable, NullOffset);
}
void CNiAvr6552Advanced::ConfigureSampleRate(
        long SampleRate,
        double IntegralTimemilliseconds)
{
    impl.ConfigureSampleRate(SampleRate, IntegralTimemilliseconds);
}
void CNiAvr6552Advanced::ConfigureSampleMode(
        long SampleMode,
        double Delaysec)
{
    impl.ConfigureSampleMode(SampleMode, Delaysec);
}
//==============================================================================
//:> CNiAvr6552Measurement
//==============================================================================

void CNiAvr6552Measurement::ReadHoldMeasurement(
        long MaximumTimems,
        double* Reading)
{
    impl.ReadHoldMeasurement(MaximumTimems, Reading);
}
void CNiAvr6552Measurement::ReadBurstMultiPoint(
        long MaximumTimems,
        long ArraySize,
        double* ReadingArray,
        long* ActualNumberofPoints)
{
    impl.ReadBurstMultiPoint(MaximumTimems, ArraySize, ReadingArray, ActualNumberofPoints);
}
void CNiAvr6552Measurement::QueryMaxMin(
        double* MaxValue,
        double* MinValue,
        double* AverageValue,
        double* CycleCount)
{
    impl.QueryMaxMin(MaxValue, MinValue, AverageValue, CycleCount);
}
void CNiAvr6552Measurement::FetchBurstMultiPoint(
        long MaximumTimems,
        long ArraySize,
        double* ReadingArray,
        long* ActualNumberofPoints)
{
    impl.FetchBurstMultiPoint(MaximumTimems, ArraySize, ReadingArray, ActualNumberofPoints);
}

//==============================================================================
//:> CNiAvr6552RippleVoltageFilter
//==============================================================================

void CNiAvr6552RippleVoltageFilter::ConfigureRippleVoltsFilter(
        long HighPassFilter,
        long LowPassFilter)
{
    impl.ConfigureRippleVoltsFilter(HighPassFilter, LowPassFilter);
}


//==============================================================================
//:>  CNiAvr6552Ac
//==============================================================================
void CNiAvr6552Ac::ConfigureBandwidth (
    double min, 
    double max)
{
	impl.ConfigureACBandwidth (min, max);
}

//==============================================================================
//:>  CNiAvr6552Measurement
//==============================================================================
double CNiAvr6552Measurement::Read (
    long int maxTime)
{
    return impl.Read(maxTime);
}
double CNiAvr6552Measurement::Fetch (
	long int maxTime)
{
    return impl.Fetch(maxTime);
}
void CNiAvr6552Measurement::Abort (void)
{
    impl.Abort();
}
void CNiAvr6552Measurement::Initiate (void)
{
    impl.Initiate();
}
bool CNiAvr6552Measurement::IsOverRange (
    double value)
{
    return impl.IsOverRange(value);
}


//==============================================================================
//:>  CNiAvr6552Trigger
//==============================================================================
void CNiAvr6552Trigger::Configure (
		long source,
		double delay)
{
    impl.ConfigureTrigger(source, delay);
}
void CNiAvr6552Trigger::ForceTrigger(
)
{
    impl.ForceTrigger();
}
//==============================================================================
//:>  CNiAvr6552
//==============================================================================
//==============================================================================
//  Constructors/destructors
//==============================================================================
CNiAvr6552::CNiAvr6552 (void) :
    CNiIviDriver(impl), Ac(impl), Advanced(impl), Frequency(impl), Measurement(impl), Trigger(impl), RippleVoltageFilter(impl)
{
}

CNiAvr6552::CNiAvr6552 (
	const CString& resourceName,
	bool idQuery,
	bool reset,
    const CString& options) :
	CNiIviDriver(impl), Ac(impl), Advanced(impl), Frequency(impl), Measurement(impl), Trigger(impl), RippleVoltageFilter(impl)
{
    impl.SetSession(resourceName,idQuery,reset,options,true);
}

void CNiAvr6552::Configure (
	long function,
	double range,
	double resolution)
{
    impl.ConfigureMeasurement(function, range, resolution);
}
