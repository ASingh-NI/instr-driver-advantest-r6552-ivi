//:Macro CNiAvr6552
// This instrument driver provides programming support for the Advantest R6552 DMM.
//+The driver contains all the functions that IVI and VXIplug&play require.
//+It's supporting following models: R6552, R6552T, R6552T-R.
//:End Macro

//:Macro CNiAvr6552AdvancedCompare
// Additional configuration for comparison calculation.
//:End Macro

//:Macro CNiAvr6552AdvancedCompareEnabled
// Enable/Disable Comparison calculation.
//:End Macro

//:Macro CNiAvr6552AdvancedCompareHigh
// Specifies the high value for comparison calculation.
//:End Macro

//:Macro CNiAvr6552AdvancedCompareLow
// Specifies the low value for comparison calculation.
//:End Macro

//:Macro CNiAvr6552AdvancedCompareConfigureCompare
// This function enables or disables the comparator calculation and configures
// the comparator high value and comparator low value properties.
//+
//+
//+The comparator calculation works on all measurement functions. If the
//+MAX-MIN calaculation is enabled, the comparator calculation works on the
//+value in effect prior to the MAX-MIN calculation.
//:End Macro

//:Macro CNiAvr6552AdvancedCompareConfigureCompareEnable
// Enable or disable the compare calculation.
//+The driver uses this value to set the !!Advanced.Compare.Enabled!! property.
//+
//+
//+Valid Range:
//+
//+*!!true!!  (1) - Enable 
//+*!!false!! (0) - Disable 
//:End Macro

//:Macro CNiAvr6552AdvancedCompareConfigureCompareHighValue
// Pass the comparator high value you want the DMM to use.
//+The driver sets the property !!Advanced.Compare.High!! to this value.
//+
//+
//+Valid Range: -999999.0E+6 - +999999.0E+6
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::MeasureValue!! (9.9E+37) 
//+
//+
//+Note:
//+
//+(1) This property does not affect the behavior of the instrument if the value
//+of the !!Advanced.Compare.Enabled!! property is !!false!!.
//+
//+
//+(2) This parameter is ignored if the value of the 'Enable' parameter is
//+!!false!!.
//:End Macro

//:Macro CNiAvr6552AdvancedCompareConfigureCompareLowValue
// Pass the comparator low value you want the DMM to use.
//+The driver sets the property !!Advanced.Compare.Low!! to this value.
//+
//+
//+Valid Range: -999999.0E+6 - +999999.0E+6
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::MeasureValue!! (9.9E+37) 
//+
//+
//+Note:
//+
//+(1) This property does not affect the behavior of the instrument if the value
//+of the !!Advanced.Compare.Enabled!! property is !!false!!.
//+
//+
//+(2) This parameter is ignored if the value of the 'Enable' parameter is
//+!!false!!.
//:End Macro

//:Macro CNiAvr6552AdvancedDecibel
// 
//:End Macro

//:Macro CNiAvr6552AdvancedDecibelMode
// Specifies the decible conversion to perform.
//+
//+
//+Valid Values:
//+
//+! !!CNiAvr6552::DbOff!! - Disable the dB/dBm calculation
//+! !!CNiAvr6552::DbOn!!  - Enable the dB calculation
//+! !!CNiAvr6552::DbmOn!! - Enable the dBm calculation
//+
//+
//+Notes:
//+
//+
//+(1) The dB calculation executes a decibel converison. It is functional only
//+during voltage measurements. 
//+
//+
//+(2) The dBm calculation is useful for calculating the power gain  per mV. It
//+is functional only during voltage measurements.
//+
//+
//+(3) The db, dBm, and scaling calculations cannot be selected at the same
//+time. 
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::DbOff!! 
//+*!!CNiAvr6552::DbOn!!  
//+*!!CNiAvr6552::DbmOn!!
//:End Macro

//:Macro CNiAvr6552AdvancedDecibelConstant
// Specifies the constant for dB and dBm operation.
//+
//+
//+Valid Range:  0.00001E-9 - +999999.E+6
//:End Macro

//:Macro CNiAvr6552AdvancedDecibelConfigureDecibel
// This function configures the DMM to perform the dB/dBm calculations.
//+This calculation executes a decibel converison.
//+
//+
//+Notes:
//+
//+(1) This calculation is functional only during voltage measurements.
//:End Macro

//:Macro CNiAvr6552AdvancedDecibelConfigureDecibelMode
// Select the decible conversion to perform.
//+The driver uses this value to set the !!Advanced.Decibel.Mode!! property.
//+
//+
//+Valid Values:
//+
//+! !!CNiAvr6552::DbOff!! - Disable the dB/dBm calculation
//+! !!CNiAvr6552::DbOn!!  - Enable the dB calculation
//+! !!CNiAvr6552::DbmOn!! - Enable the dBm calculation
//+
//+
//+Notes:
//+
//+
//+(1) The dB calculation executes a decibel converison. It is functional only
//+during voltage measurements. 
//+
//+
//+(2) The dBm calculation is useful for calculating the power gain  per mV. It
//+is functional only during voltage measurements.
//+
//+
//+(3) The db, dBm, and scaling calculations cannot be selected at the same
//+time.
//:End Macro

//:Macro CNiAvr6552AdvancedDecibelConfigureDecibelDecibelValue
// Pass the decibel value you want the DMM to use.
//+The driver sets the property !!Advanced.Decibel.Constant!! to this value.
//+
//+
//+Valid Values:  0.00001E-9 - +999999.E+6
//+
//+
//+Note:
//+
//+(1) This property does not affect the behavior of the instrument if the value
//+of the !!Advanced.Decibel.Mode!! property is !!CNiAvr6552::DbOff!!.
//+
//+
//+(2) This parameter is ignored if the value of the 'Enable' parameter is
//+!!CNiAvr6552::DbOff!!.
//:End Macro

//:Macro CNiAvr6552AdvancedScale
// 
//:End Macro

//:Macro CNiAvr6552AdvancedScaleScaleC
// Specifies the third constant (constant C) for scale calculation.
//:End Macro

//:Macro CNiAvr6552AdvancedScaleScaleB
// Specifies the second constant (constant B) for scale calculation.
//:End Macro

//:Macro CNiAvr6552AdvancedScaleScaleA
// Specifies the first constant (constant A) for scale calculation.
//:End Macro

//:Macro CNiAvr6552AdvancedScaleEnabled
// 
//+Enable/Disable Scaling
//:End Macro

//:Macro CNiAvr6552AdvancedScaleConfigureScale
// This function enables or disables the scale calculation and configures
// additional properties for the calculation.
//+These properties include the scale constants A,B, and C.
//+
//+
//+The scaling equation is given below:
//+
//+!Result = (Measurement value - B) * C
//+!         -----------------------
//+!                    A
//:End Macro

//:Macro CNiAvr6552AdvancedScaleConfigureScaleEnable
// Enable or disable the scale calculation.
//+The driver uses this value to set the !!Advanced.Scale.Enabled!! property.
//+
//+
//+Valid Range:
//+
//+! !!true!!  (1) - Enable 
//+! !!false!! (0) - Disable 
//:End Macro

//:Macro CNiAvr6552AdvancedScaleConfigureScaleConstantA
// Pass the constant A value you want the DMM to use.
//+The driver sets the property !!Advanced.Scale.ScaleA!! to this value.
//+
//+
//+Valid Range: -999999.0E+6 - +999999.0E+6
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::MeasureValue!! (9.9E+37) 
//+           
//+
//+Note:
//+
//+(1) This property does not affect the behavior of the instrument if the value
//+of the !!Advanced.Scale.Enabled!! property is !!false!!.
//+
//+
//+(2) This parameter is ignored if the value of the 'Enable' parameter is
//+!!false!!.
//:End Macro

//:Macro CNiAvr6552AdvancedScaleConfigureScaleConstantB
// Pass the constant B value you want the DMM to use.
//+The driver sets the property !!Advanced.Scale.ScaleB!! to this value.
//+
//+
//+Valid Range: -999999.0E+6 - +999999.0E+6
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::MeasureValue!! (9.9E+37) 
//+           
//+
//+Notes:
//+
//+(1) This property does not affect the behavior of the instrument if the value
//+of the !!Advanced.Scale.Enabled!! property is !!false!!.
//+
//+
//+(2) This parameter is ignored if the value of the 'Enable' parameter is
//+!!false!!.
//:End Macro

//:Macro CNiAvr6552AdvancedScaleConfigureScaleConstantC
// Pass the constant C value you want the DMM to use.
//+The driver sets the property !!Advanced.Scale.ScaleC!! to this value.
//+
//+
//+Valid Range: -999999.0E+6 - +999999.0E+6
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::MeasureValue!! (9.9E+37) 
//+
//+
//+Notes:
//+
//+(1) This property does not affect the behavior of the instrument if the value
//+of the !!Advanced.Scale.Enabled!! property is !!false!!.
//+
//+
//+(2) This parameter is ignored if the value of the 'Enable' parameter is
//+!!false!!.
//:End Macro

//:Macro CNiAvr6552AdvancedMinMax
// 
//:End Macro

//:Macro CNiAvr6552AdvancedMinMaxEnabled
// Enable/Disable the MaxMin calculation.
//:End Macro

//:Macro CNiAvr6552AdvancedSmooth
// 
//:End Macro

//:Macro CNiAvr6552AdvancedSmoothEnabled
// 
//+Enable/Disable Smoothing
//:End Macro

//:Macro CNiAvr6552AdvancedSmoothCount
// Specifies the cycle count for smoothing calculation.
//:End Macro

//:Macro CNiAvr6552AdvancedSmoothConfigureSmooth
// This function enables or disables the smoothing calculation and configures
// the smooth count.
//+
//+
//+The smoothing calculation tests measurement signals that are superimposed
//+with noises. It determines the moving average of measurement values recorded
//+from a preset number of cycles of smoothing (smooth count), with the result
//+of narrower variations  among the measurement values.
//:End Macro

//:Macro CNiAvr6552AdvancedSmoothConfigureSmoothEnable
// Enable or disable the smoothing calculation.
//+The driver uses this value to set the !!Advanced.Smooth.Enabled!! property.
//+
//+
//+Valid Range:
//+
//+! !!true!!  (1) - Enable 
//+! !!false!! (0) - Disable 
//:End Macro

//:Macro CNiAvr6552AdvancedSmoothConfigureSmoothSmoothCount
// Pass the smooth count you want the DMM to use.
//+The driver sets the property !!Advanced.Smooth.Count!! to this value.
//+
//+
//+The smooth count specifies the number of cycles of smoothing that are used to
//+determine the moving average of measurement values. The smoothing calculation
//+is given below:
//+
//+
//+Result = (measurement 1 + measurement 2 .... measurement N)/N
//+
//+where N = smooth count.
//+
//+
//+Valid Values:  2 - 100
//+
//+
//+Note:
//+
//+(1) This property does not affect the behavior of the instrument if the value
//+of the !!Advanced.Smooth.Enabled!! property is !!false!!.
//+
//+
//+(2) This parameter is ignored if the value of the 'Enable' parameter is
//+!!false!!.
//:End Macro

//:Macro CNiAvr6552AdvancedNullCalculation
// 
//:End Macro

//:Macro CNiAvr6552AdvancedNullCalculationOffset
// Specifies the offset for null operation.
//:End Macro

//:Macro CNiAvr6552AdvancedNullCalculationEnabled
// 
//+Enable/Disable Null calculation
//:End Macro

//:Macro CNiAvr6552AdvancedNullCalculationConfigureNull
// This function enables or disables the null calculation and configures null
// offset.
//+
//+
//+The null calculation generates a measurement value less a null offset.
//:End Macro

//:Macro CNiAvr6552AdvancedNullCalculationConfigureNullEnable
// Enable or disable the null calculation.
//+The driver uses this value to set the !!Advanced.NullCalculation.Enabled!!
//+property.
//+
//+
//+Valid Range:
//+
//+! !!true!!  (1) - Enable 
//+! !!false!! (0) - Disable 
//:End Macro

//:Macro CNiAvr6552AdvancedNullCalculationConfigureNullNullOffset
// Pass the null offset you want the DMM to use.
//+The driver sets the !!Advanced.NullCalculation.Offset!! property to this
//+value.
//+
//+
//+The instrument subtracts the null offset form each subsequent measurement.
//+
//+
//+The null offset cannot be set if the null operation is not enabled. You must
//+set the value of the !!Advanced.NullCalculation.Enabled!!  to !!true!! before
//+setting this property.
//+
//+
//+Valid Values:  -999999.E+6 - +999999.E+6
//+
//+
//+Note:
//+
//+(1) This property does not affect the behavior of the instrument if the value
//+of the !!Advanced.NullCalculation.Enabled!! property is !!false!!.
//+
//+
//+(2) This parameter is ignored if the value of the 'Enable' parameter is
//+!!false!!.
//:End Macro

//:Macro CNiAvr6552RippleVoltageFilter
// 
//:End Macro

//:Macro CNiAvr6552RippleVoltageFilterLowPassFilter
// Specifies the frequency band of LPF (low-pass filter) the DMM uses to measure
// ripple voltage.
//+
//+
//+Valid Values:
//+*!!CNiAvr6552::FilterOff!!
//+*1000
//+*10000 
//+*100000
//+*1000000
//+
//+Note:  This property is supported only by AVR6552T-R.
//:End Macro

//:Macro CNiAvr6552RippleVoltageFilterHighPassFilter
// Specifies the frequency band of HPF (high-pass filter) the DMM uses to
// measure ripple voltage.
//+
//+
//+Valid Values:
//+*!!CNiAvr6552::FilterOff!!
//+*1000
//+*10000 
//+*100000
//+
//+Note:
//+This property is supported only by AVR6552T-R.
//:End Macro

//:Macro CNiAvr6552RippleVoltageFilterConfigureRippleVoltsFilter
// This function configures the common properties of ripple voltage measurement
// for the DMM.
//+These properties include the high pass filter and the low pass filter.
//+
//+
//+Notes:
//+
//+(1) This function is supported only by AVR6552T-R.
//:End Macro

//:Macro CNiAvr6552RippleVoltageFilterConfigureRippleVoltsFilterHighPassFilter
// Pass the high pass filter you want the DMM to use to measure ripple volts.
//+The unit for this property is hertz.  The driver sets the property
//+!!RippleVoltageFilter.HighPassFilter!! to this value.
//+
//+
//+Valid Range: 1000.0, 10000.0, 100000.0 Hz
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::FilterOff!! - Filter Off
//+
//+
//+Notes:
//+
//+(1) This property is supported only by AVR6552T-R.
//:End Macro

//:Macro CNiAvr6552RippleVoltageFilterConfigureRippleVoltsFilterLowPassFilter
// Pass the low pass filter you want the DMM to use to measure ripple volts.
//+The unit for this property is hertz.  The driver sets the property
//+!!RippleVoltageFilter.LowPassFilter!! to this value.
//+
//+
//+Valid Range: 1000.0, 10000.0, 100000.0, 1000000.0 Hz
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::FilterOff!! - Filter Off
//+
//+
//+Notes:
//+
//+(1) This property is supported only by AVR6552T-R.
//:End Macro

//:Macro CNiAvr6552Ac
// Additional settings for taking AC measurements.
//+These settings affect the behavior of the instrument only if the !!Function!!
//+property is set to an AC voltage or AC current measurement.
//:End Macro

//:Macro CNiAvr6552AcFrequencyMax
// Specifies the maximum frequency component of the input signal for AC
// measurements.
//+The value of this property affects instrument behavior only when you set the
//+!!Function!! property to one of the following values:
//+
//+
//+*!!CNiAvr6552::VoltsAcFunction!!
//+*!!CNiAvr6552::CurrentAcFunction!!
//+*!!CNiAvr6552::VoltsAcPlusDcFunction!!
//+*!!CNiAvr6552::CurrentAcPlusDcFunction!!
//:End Macro

//:Macro CNiAvr6552AcFrequencyMin
// Specifies the minimum frequency component of the input signal for AC
// measurements.
//+The value of this property affects instrument behavior only when you set the
//+!!Function!! property to one of the following values:
//+
//+
//+*!!CNiAvr6552::VoltsAcFunction!!
//+*!!CNiAvr6552::CurrentAcFunction!!
//+*!!CNiAvr6552::VoltsAcPlusDcFunction!!
//+*!!CNiAvr6552::CurrentAcPlusDcFunction!!
//+    
//+
//+Valid Values:
//+*300.0  
//+*20.0 
//:End Macro

//:Macro CNiAvr6552AcConfigureBandwidth
// This function configures the AC minimum and maximum frequency for AC voltage
// or AC current measurements.
//+
//+
//+This function affects the behavior of the instrument only if the
//+!!Function!! property is set to an AC voltage or AC current measurement.
//:End Macro

//:Macro CNiAvr6552AcConfigureBandwidth1
// Pass the minimum expected frequency component of the input signal.
//+The units are hertz.  The driver sets the !!Ac.FrequencyMin!! property to
//+this value. 
//+
//+
//+This parameter affects the DMM only when you set the Measurement Function
//+parameter to AC measurement. 
//+
//+
//+Valid Range: 20.0 - 300.0 Hz
//+
//:End Macro

//:Macro CNiAvr6552AcConfigureBandwidth2
// Pass the maximum expected frequency component of the input signal.
//+The units are hertz.  The driver sets the !!Ac.FrequencyMax!! property to
//+this value. 
//+
//+
//+This parameter affects the DMM only when you set the Measurement Function
//+parameter to AC measurement.
//+
//+
//+The maximum valid value of this property depends on the measurement
//+function.
//+
//+
//+Valid Range:  
//+
//+! For AC voltage measurements: 300.0 - 300000.0 Hz
//+! For AC current measurements: 300.0 - 5000.0 Hz
//:End Macro

//:Macro CNiAvr6552Advanced
// Advanced Dmm properties.
//:End Macro

//:Macro CNiAvr6552AdvancedAutoZero
// Specifies the auto-zero mode.
//+In general, the auto-zero capability of a DMM normalizes all measurements
//+based on a Zero Reading.  When auto-zeroing is enabled, the DMM internally
//+disconnects the input signal and takes a Zero Reading.  The DMM then
//+subtracts the Zero Reading from the measurement.  This prevents offset
//+voltages present in the DMM's input circuitry from affecting measurement
//+accuracy.
//+
//+    
//+Valid Values:
//+
//+*!!CNiAvr6552::OffAutoZero!! - Disables the DMM from taking any more Zero
//+Readings.  The DMM subtract the last Zero Reading from all subsequent values
//+it measures.
//+
//+*!!CNiAvr6552::OnAutoZero!! - Configures the DMM to take a Zero Reading for
//+each measurement.  The DMM subtracts the Zero Reading from the value it
//+measures.
//+
//+*!!CNiAvr6552::OnceAutoZero!! - Configures the DMM to take a Zero Reading
//+immediately.  The DMM then subtracts this Zero Reading from all subsequent
//+values it measures.
//:End Macro

//:Macro CNiAvr6552AdvancedPowerlineFrequency
// Specifies the power line frequency in Hertz.
//+
//+    
//+The instrument selects the powerline frequency automatically. This
//+property cannot be set by the user.
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::Frequency50Hz!! - 50 Hertz
//+*!!CNiAvr6552::Frequency60Hz!! - 60 Hertz
//:End Macro

//:Macro CNiAvr6552AdvancedSampleRate
// Specifies the sample rate the DMM uses to acquire a signal.
//:End Macro

//:Macro CNiAvr6552AdvancedIntegralTime
// Specifies the integral time for long-it sample rate.
//+The units for this property are milliseconds.
//:End Macro

//:Macro CNiAvr6552AdvancedSampleMode
// Specifies the sample mode the DMM to use.
//+
//+
//+Valid Values:
//+*!!CNiAvr6552::FreeRun!! - Free run sample mode
//+*!!CNiAvr6552::Hold!!    - Hold sample mode
//+*!!CNiAvr6552::Burst!!   - Burst sample mode
//:End Macro

//:Macro CNiAvr6552AdvancedDelay
// Specifies the delay the DMM uses for the HOLD and BURST sample modes before
// it takes a measurement.
//+
//+
//+Valid Values:  0.0 - 99.999 seconds
//:End Macro

//:Macro CNiAvr6552AdvancedHoldSampleCount
// Specifies the number of measurements the DMM takes each time it receives a
// trigger in Hold sample mode.
//+
//+    
//+Valid Values:  1 - 9999
//+
//+Note:
//+
//+This property does not affect the behavior of the instrument if the  !!Advanced.SampleMode!! property is not
//+set to !!CNiAvr6552::Hold!!.
//:End Macro

//:Macro CNiAvr6552AdvancedBurstCount
// Specifies the measurement count to transfer to the internal memory in burst
// sample mode.
//+
//+
//+Valid Values:
//+
//+*1000
//+*2000
//+*3000
//+*4000
//+*5000
//+*6000
//+*7000
//+*8000
//+*9000
//+*10000
//+
//+
//+Notes:  
//+
//+(1)  This property does not affect the behavior of the instrument if the
//+sample mode is not set to burst mode.
//+
//+(2)  This property is supported only by AVR6552.
//:End Macro

//:Macro CNiAvr6552AdvancedDelayedTrigger
// Specifies the percentage of numbers of premeasurement cycles out of the total
// burst cycles.
//+
//+
//+Valid Values:
//+*0
//+*25
//+*50
//+*75
//+
//+Notes:  
//+(1)  This property does not affect
//+the behavior of the instrument if the sample mode is not set to burst mode.
//+
//+(2)  This property is supported only by AVR6552.
//:End Macro

//:Macro CNiAvr6552AdvancedConfigureSampleRate
// This function configures the common properties of sample rate for the DMM.
//+These properties include the sample rate and integral time.
//:End Macro

//:Macro CNiAvr6552AdvancedConfigureSampleRateSampleRate
// Pass the sample rate you want the DMM to use.
//+The driver sets the property !!Advanced.SampleRate!! to this value.
//+
//+
//+For AVR6552:
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::Fast!!
//+*!!CNiAvr6552::Med!!
//+*!!CNiAvr6552::Slow!!
//+*!!CNiAvr6552::Longit!!
//+
//+
//+For AVR6552T and AVR6552T-R:
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::Fast!!
//+*!!CNiAvr6552::Med!!
//+*!!CNiAvr6552::Slow!!    
//+
//+
//+Note:  This property can be set to !!CNiAvr6552::Longit!! only when the
//+measurement function is set to DC Volts or DC Current.
//:End Macro

//:Macro CNiAvr6552AdvancedConfigureSampleRateIntegralTimemilliseconds
// Pass the integral time you want the DMM to use for the long-it sample rate.
//+Express this value in milliseconds.  The driver sets the
//+!!Advanced.IntegralTime!! property to this value. 
//+
//+
//+Valid Manual Range:  100.0 - 60000.0 (milliseconds)
//+
//+
//+Notes: 
//+
//+ (1) This property is only supported by AVR6552.
//+
//+
//+ (2) This property has no effect on the behavior of the DMM if the sample
//+rate is not set to long-it.
//:End Macro

//:Macro CNiAvr6552AdvancedConfigureSampleMode
// This function configures the sample mode and the delay of the instrument.
//:End Macro

//:Macro CNiAvr6552AdvancedConfigureSampleModeSampleMode
// Pass the sample mode you want the DMM to use.
//+The driver sets the !!Advanced.SampleMode!! property to this value.
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::FreeRun!!
//+*!!CNiAvr6552::Hold!!
//+*!!CNiAvr6552::Burst!!
//+
//+
//+Notes:
//+
//+(1)  The burst sample mode is supported only by AVR6552. 
//+
//+(2)  If the sample mode is switched to !!CNiAvr6552::Burst!!, the auto-range
//+operation is automatically turned OFF.  When resetting the burst measurement
//+(switching to the other sample mode), the auto-range operation remains OFF.
//+
//+(3)  The auto zero setting is ignored if the sample mode is set to
//+!!CNiAvr6552::Burst!!.
//+
//+(4)  The measurement functions are limited to DC Volts, DC Current and 2-Wire
//+Resistance and LP 2-Wire Resistance if the sample mode is set to
//+!!CNiAvr6552::Burst!!.
//+
//+(5)  When the burst sample mode is reset, all settings for the calculation
//+functions are set to OFF.
//:End Macro

//:Macro CNiAvr6552AdvancedConfigureSampleModeDelaysec
// Pass the delay you want the DMM to use for the HOLD and BURST sample modes.
//+The driver sets the !!Advanced.Delay!! property to this value. 
//+
//+
//+After you call the !!Trigger.ForceTrigger!! function, the DMM waits the
//+amount of time specified by this property before it takes a measurement. 
//+
//+
//+Valid Values:  0.0 - 99.999 seconds
//+
//+
//+Notes:
//+
//+
//+(1) This property affects the behavior of the instrument only if the
//+!!Advanced.SampleMode!! property is set to the !!CNiAvr6552::Hold!! or the
//+!!CNiAvr6552::Burst!! defined values.
//:End Macro

//:Macro CNiAvr6552Frequency
// Additional settings for taking frequency and period measurements.
//+
//+These settings affect the behavior of the instrument only if the
//+!!Function!! property is set to a frequency or period measurement.
//:End Macro

//:Macro CNiAvr6552FrequencyVoltageRange
// Specifies the expected maximum value of the input signal for frequency
// measurements in volts RMS.
//+
//+
//+Valid Range: 0.1 - 700.0 volts RMS
//:End Macro

//:Macro CNiAvr6552Measurement
// Functions for acquiring and retrieving measurements.
//:End Macro

//:Macro CNiAvr6552MeasurementRead
// Call this function to obtain a measurement if the instrument is in the Free
// sample mode.
//+This function initiates a measurement, waits until the DMM has returned to
//+the Idle state, and returns the measured value.
//+
//+
//+Notes:
//+
//+
//+(1) After this function executes, the Reading parameter contains  an actual
//+reading or a value indicating that an over-range condition occurred.
//+
//+
//+(2) You can test the measurement value for over-range with the
//+!!Measurement.IsOverRange!! function.
//:End Macro

//:Macro CNiAvr6552MeasurementRead1
// Pass the maximum length of time in which to allow the read operation to
// complete.
//+Express this value in milliseconds.  
//+
//+
//+If the operation does not complete within this time interval, the function
//+throws an exception.  When this occurs, you can call !!Measurement.Abort!!
//+to cancel the read operation and return the instrument to the Idle state.  
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::InfiniteMaxTime!! (-1)           
//+*!!CNiAvr6552::ImmediateMaxTime!! (0)         
//+
//+
//+Notes:
//+
//+
//+(1) The Maximum Time parameter affects only this function.  It has no effect
//+on other timeout parameters or properties.
//:End Macro

//:Macro CNiAvr6552MeasurementFetch
// This function returns the value from a previously initiated measurement.
//+Use this function if the instrument is in the Free sample mode. 
//+
//+
//+You must first call the !!Measurement.Initiate!! function to initiate a
//+measurement before calling this function.
//+
//+
//+Notes:
//+
//+
//+(1) After this function executes, the Reading parameter contains  an actual
//+reading or a value indicating that an over-range condition occurred.
//+
//+
//+(2) You can test the measurement value for over-range with the
//+!!Measurement.IsOverRange!! function.
//+
//+
//+(3) This function does not check the instrument status.   Typically, you call
//+this function only in a sequence of calls to other low-level driver
//+functions.  The sequence performs one operation.  You use the low-level
//+functions to optimize one or more aspects of interaction with the instrument.
//+ If you want to check the instrument status, call the !!Utility.ErrorQuery!!
//+function at the conclusion of the sequence.
//:End Macro

//:Macro CNiAvr6552MeasurementFetch1
// Pass the maximum length of time in which to allow the fetch operation to
// complete.
//+Express this value in milliseconds.  
//+
//+
//+If the operation does not complete within this time interval, the function
//+throws an exception.  When this occurs, you can call !!Measurement.Abort!!
//+to cancel the fetch operation and return the instrument to the Idle state.  
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::InfiniteMaxTime!! (-1)           
//+*!!CNiAvr6552::ImmediateMaxTime!! (0)    
//+
//+
//+Notes:
//+
//+
//+(1) The Maximum Time parameter affects only this function.  It has no effect
//+on other timeout parameters or properties.
//:End Macro

//:Macro CNiAvr6552MeasurementAbort
// This function aborts a previously initiated measurement and returns the DMM
// to the Idle state.
//+
//+
//+Notes:
//+
//+(1) This function does not check the instrument status.  
//+Typically, you call this function only in a sequence of calls to other
//+low-level driver functions.  The sequence performs one operation.  You use
//+the low-level functions to optimize one or more aspects of interaction with
//+the instrument.  If you want to check the instrument status, call the
//+!!Utility.ErrorQuery!! function at the conclusion of the sequence.
//:End Macro

//:Macro CNiAvr6552MeasurementInitiate
// This function initiates a measurement.
//+After you call this function, the DMM leaves the Idle state and waits for a
//+trigger.
//+
//+
//+Notes:
//+
//+
//+(1) This function does not check the instrument status.   Typically, you call
//+this function only in a sequence of calls to other low-level driver
//+functions.  The sequence performs one operation.  You use the low-level
//+functions to optimize one or more aspects of interaction with the instrument.
//+ If you want to check the instrument status, call the !!Utility.ErrorQuery!!
//+function at the conclusion of the sequence.
//:End Macro

//:Macro CNiAvr6552MeasurementIsOverRange
// This function takes a measurement value that you obtain from one of the
// Measure, Read, or Fetch functions and determines if the value is a valid
// measurement value or a value indicating an over-range condition occurred.
//+
//+
//+Notes:
//+
//+(1) If an over-range condition occurs, the measurement value
//+contains an IEEE defined NaN (Not a Number) value.
//:End Macro

//:Macro CNiAvr6552MeasurementIsOverRange1
// Pass the measurement value that you obtain from one of the Measure, Read, or
// Fetch functions.
//+The driver tests this value to determine if the value is a valid measurement
//+value or a value indicating an over-range condition occurred.  
//+
//+
//+Notes:
//+
//+
//+(1) If an over-range condition occurs, the measurement value contains an IEEE
//+defined NaN (Not a Number) value indicating that an over-range occurred.
//:End Macro

//:Macro CNiAvr6552MeasurementReadHoldMeasurement
// Call this function to obtain a measurement if the instrument is in the Hold
// sample mode.
//+This function sends a software trigger to the instrument, waits until the DMM
//+has returned to the Idle state, and returns the measured value.
//+
//+
//+Notes:
//+
//+
//+(1) After this function executes, the Reading parameter contains  an actual
//+reading or a value indicating that an over-range condition occurred.
//+
//+
//+(2) You can test the measurement value for over-range with the
//+!!Measurement.IsOverRange!! function.
//:End Macro

//:Macro CNiAvr6552MeasurementReadHoldMeasurementMaximumTimems
// Pass the maximum length of time in which to allow the read operation to
// complete.
//+Express this value in milliseconds.  
//+
//+
//+If the operation does not complete within this time interval, the function
//+throws an exception.  When this occurs, you can call !!Measurement.Abort!!
//+to cancel the read operation and return the instrument to the Idle state.  
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::InfiniteMaxTime!! (-1)           
//+*!!CNiAvr6552::ImmediateMaxTime!! (0)         
//+
//+
//+Notes:
//+
//+
//+(1) The Maximum Time parameter affects only this function.  It has no effect
//+on other timeout parameters or properties.
//:End Macro

//:Macro CNiAvr6552MeasurementReadHoldMeasurementReading
// Returns the measured value.
//+The value you specify for the Measurement Function parameter of the
//+!!Configure!! function determines the units of this parameter as shown in
//+the following table.
//+
//+
//+!  DC Volts           - volts
//+!  AC Volts           - volts
//+!  DC Current         - amperes
//+!  AC Current         - amperes
//+!  2-Wire Resistance  - ohms
//+!  4-Wire Resistance  - ohms
//+!  LP 2-Wire Resistance - ohms
//+!  LP 4-Wire Resistance - ohms
//+!  AC plus DC Volts   - volts
//+!  AC plus DC Current - amperes
//+!  Frequency          - hertz
//+!  Diode              - none
//+!  Ripple Volts       - volts
//+
//+
//+Notes:
//+
//+
//+(1) You can test the measurement value for over-range with the
//+!!Measurement.IsOverRange!! function.
//:End Macro

//:Macro CNiAvr6552MeasurementReadBurstMultiPoint
// Call this function to obtain a measurement if the instrument is in the Burst
// sample mode.
//+This function sends a software trigger to the instrument, waits for the DMM
//+to return to the Idle state, and returns an array of measured values.  The
//+number of measurements the DMM takes is determined by the 
//+!!Advanced.BurstCount!! property.
//+
//+
//+Notes:
//+
//+
//+(1) After this function executes, each element in the Reading Array parameter
//+is an actual reading or a value indicating that an over-range condition
//+occurred.
//+
//+
//+(2) You can test each element in the Reading Array parameter for over-range
//+with the !!Measurement.IsOverRange!! function.
//+
//+
//+(3) This function is supported only by the AVR6552.
//:End Macro

//:Macro CNiAvr6552MeasurementReadBurstMultiPointMaximumTimems
// Pass the maximum length of time in which to allow the multi-point read
// operation to complete.
//+Express this value in milliseconds.  
//+
//+
//+If the operation does not complete within this time interval, the function
//+throws an exception.  When this occurs, you can call !!Measurement.Abort!!
//+to cancel the multi-point read operation and return the instrument to the Idle state.  
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::InfiniteMaxTime!! (-1)            
//+*!!CNiAvr6552::ImmediateMaxTime!! (0)           
//+
//+Notes:
//+
//+(1) The Maximum Time parameter affects only this function.  It has no effect
//+on other timeout parameters or properties.
//:End Macro

//:Macro CNiAvr6552MeasurementReadBurstMultiPointArraySize
// Pass the number of elements in the Reading Array parameter.
//+
//+
//:End Macro

//:Macro CNiAvr6552MeasurementReadBurstMultiPointReadingArray
// Returns an array of the most recent measurement values.
//+The value you specify for the Measurement Function parameter of the
//+!!Configure!! function determines the units of this parameter as shown in
//+the following table.
//+
//+
//+!  DC Volts           - volts
//+!  AC Volts           - volts
//+!  DC Current         - amperes
//+!  AC Current         - amperes
//+!  2-Wire Resistance  - ohms
//+!  4-Wire Resistance  - ohms
//+!  LP 2-Wire Resistance - ohms
//+!  LP 4-Wire Resistance - ohms
//+!  AC plus DC Volts   - volts
//+!  AC plus DC Current - amperes
//+!  Frequency          - hertz
//+!  Diode              - none
//+!  Ripple Volts       - volts
//+
//+
//+Notes:
//+
//+
//+(1) The size of the Reading Array must be at least the size you specify for
//+the Array Size parameter.
//+
//+
//+(2) You can test each element in the Reading Array parameter for over-range
//+with the !!Measurement.IsOverRange!! function.
//:End Macro

//:Macro CNiAvr6552MeasurementReadBurstMultiPointActualNumberofPoints
// Indicates the number of measured values the function places in the Reading
// Array parameter.
//:End Macro

//:Macro CNiAvr6552MeasurementQueryMaxMin
// This function returns the maximum value, minimum value, average value and
// measurement cycle count when the DMM is performing the max-min calculation.
//:End Macro

//:Macro CNiAvr6552MeasurementQueryMaxMinMaxValue
// Returns the maximum value when the DMM is performing the max-min operation.
//:End Macro

//:Macro CNiAvr6552MeasurementQueryMaxMinMinValue
// Returns the minimum value when the DMM is performing the max-min operation.
//:End Macro

//:Macro CNiAvr6552MeasurementQueryMaxMinAverageValue
// Returns the average value when the DMM is performing the max-min operation.
//:End Macro

//:Macro CNiAvr6552MeasurementQueryMaxMinCycleCount
// Returns the measurement cycle count when the DMM is performing the max-min
// operation.
//:End Macro

//:Macro CNiAvr6552MeasurementFetchBurstMultiPoint
// This function returns an array of values from a previously initiated
// multi-point measurement.
//+The number of measurements the DMM takes is determined by the
//+!!Advanced.BurstCount!! property.
//+
//+
//+Use this function if the instrument is in the Burst sample mode. 
//+
//+
//+You must first call the !!Measurement.Initiate!! function to initiate a
//+measurement before calling this function.
//+
//+
//+Notes:
//+
//+
//+(1) After this function executes, each element in the Reading Array parameter
//+is an actual reading or a value indicating that an over-range condition
//+occurred.
//+
//+
//+(2) You can test each element in the Reading Array parameter for over-range
//+with the !!Measurement.IsOverRange!! function.
//+
//+
//+(3) This function does not check the instrument status.   Typically, you call
//+this function only in a sequence of calls to other low-level driver
//+functions.  The sequence performs one operation.  You use the low-level
//+functions to optimize one or more aspects of interaction with the instrument.
//+ If you want to check the instrument status, call the !!Utility.ErrorQuery!!
//+function at the conclusion of the sequence.
//+
//+
//+(5) This function is supported only by the AVR6552.
//:End Macro

//:Macro CNiAvr6552MeasurementFetchBurstMultiPointMaximumTimems
// Pass the maximum length of time in which to allow the multi-point fetch
// operation to complete.
//+Express this value in milliseconds.  
//+
//+
//+If the operation does not complete within this time interval, the function
//+throws an exception.  When this occurs, you can call !!Measurement.Abort!!
//+to cancel the multi-point fetch operation and return the instrument to the
//+Idle state.  
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::InfiniteMaxTime!! (-1)
//+*!!CNiAvr6552::ImmediateMaxTime!! (0)
//+
//+
//+Notes:
//+
//+
//+(1) The Maximum Time parameter affects only this function.  It has no effect
//+on other timeout parameters or properties.
//:End Macro

//:Macro CNiAvr6552MeasurementFetchBurstMultiPointArraySize
// Pass the number of elements in the Reading Array parameter.
//+
//+
//:End Macro

//:Macro CNiAvr6552MeasurementFetchBurstMultiPointReadingArray
// Returns an array of the most recent measurement values.
//+The value you specify for the Measurement Function parameter of the
//+!!Configure!! function determines the units of this parameter as shown in
//+the following table.
//+
//+
//+!  DC Volts           - volts
//+!  AC Volts           - volts
//+!  DC Current         - amperes
//+!  AC Current         - amperes
//+!  2-Wire Resistance  - ohms
//+!  4-Wire Resistance  - ohms
//+!  LP 2-Wire Resistance - ohms
//+!  LP 4-Wire Resistance - ohms
//+!  AC plus DC Volts   - volts
//+!  AC plus DC Current - amperes
//+!  Frequency          - hertz
//+!  Diode              - none
//+!  Ripple Volts       - volts
//+
//+
//+Notes:
//+
//+
//+(1) The size of the Reading Array must be at least the size you specify for
//+the Array Size parameter.
//+
//+
//+(2) You can test each element in the Reading Array parameter for over-range
//+with the !!Measurement.IsOverRange!! function.
//:End Macro

//:Macro CNiAvr6552MeasurementFetchBurstMultiPointActualNumberofPoints
// Indicates the number of measured values the function places in the Reading
// Array parameter.
//:End Macro

//:Macro CNiAvr6552Trigger
// Additional settings for taking measurements after a trigger event occurs.
//:End Macro

//:Macro CNiAvr6552TriggerSource
// This property specifies the trigger source.
//+
//+ 
//+Valid Values:
//+
//+*!!CNiAvr6552::ImmediateSampleTrigger!! - Immediate
//:End Macro

//:Macro CNiAvr6552TriggerDelay
// Specifies the length of time the DMM waits after it receives the trigger and
// before it takes a measurement.
//+Use positive values to set the trigger delay in seconds. 
//+
//+
//+Valid Range:  0.0 seconds
//+
//+
//+Note:
//+
//+This property does not affect the behavior of the DMM. It exists only
//+for class compliance.
//:End Macro

//:Macro CNiAvr6552TriggerConfigure
// This function configures the common DMM trigger properties.
//+These properties include the trigger source and trigger delay.
//:End Macro

//:Macro CNiAvr6552TriggerConfigure1
// Specify the trigger source.
//+The driver sets the !!Trigger.Source!! property to this value.  
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::ImmediateSampleTrigger!!   - Internal Trigger
//+
//+
//+Note:
//+
//+(1) This property affects the behavior of the instrument only when the
//+!!Advanced.SampleMode!! property is set to !!CNiAvr6552::FreeRun!!. For other
//+sample modes, the instrument will only trigger when the
//+!!Trigger.ForceTrigger!! function is called.
//:End Macro

//:Macro CNiAvr6552TriggerConfigure2
// The trigger delay specifies the length of time the DMM waits after it
// receives the trigger and before it takes a measurement.
//+
//+
//+Valid Manual Range:  0.0
//+
//+Note:  
//+
//+This property does not affect the behavior of the DMM and exists only for class
//+compliance.
//:End Macro

//:Macro CNiAvr6552TriggerForceTrigger
// This function sends a command to trigger the DMM.
//+
//+
//+Notes:
//+
//+(1) This function does not check the instrument status.  
//+Typically, you call this function only in a sequence of calls to other
//+low-level driver functions.  The sequence performs one operation.  You use
//+the low-level functions to optimize one or more aspects of interaction with
//+the instrument.  If you want to check the instrument status, call the
//+!!Utility.ErrorQuery!! function at the conclusion of the sequence.
//:End Macro

//:Macro CNiAvr6552Function
// Specifies the measurement function.
//:End Macro

//:Macro CNiAvr6552Range
// Specifies the measurement range.
//+You use positive values to represent the absolute value of the maximum
//+expected measurement.  The value is in units appropriate for the current
//+value of the !!Function!! property.  For example, when you set the
//+!!Function!! property to !!CNiAvr6552::VoltsDcFunction!!, the units are
//+volts.  For example, setting this property to 10.0 configures the DMM to
//+measure DC voltages from -10.0 to +10.0 volts.
//+
//+    Negative values are reserved for controlling the DMM's auto-ranging
//+capability.
//+
//+- !!CNiAvr6552::OnAutoRange!! (-1.0) - Configures the DMM to calculate the
//+range before each measurement automatically.
//+
//+    For AVR6552, When you set this property to !!CNiAvr6552::OnAutoRange!!,
//+you cannot obtain the actual range the DMM is currently using.
//+
//+
//+Note:  
//+
//+(1)  For frequency or diode measurements, this property must be set to
//+!!CNiAvr6552::OnAutoRange!!.    
//+
//+
//+(2) This property must be set to  !!CNiAvr6552::OnAutoRange!! in burst
//+measurement mode.
//:End Macro

//:Macro CNiAvr6552Resolution
// This property configures the measurement resolution of the DMM in absolute
// units.
//+
//+
//+Setting this property to lower values increases the measurement accuracy. 
//+Setting this property to higher values increases the measurement speed.
//+
//+
//+Valid Values:
//+
//+*10000  
//+*1000  
//+*100  
//+*10  
//+*1  
//+*.1  
//+*.01  
//+*.001  
//+*.0001 
//+*.00001 
//+*.000001  
//+*.0000001  
//+*.00000001 
//:End Macro

//:Macro CNiAvr6552IdQueryResponse
// Returns the ID Query response string.
//+The instrument driver gets the value of this property when you pass !!true!!
//+for the ID Query parameter to the constructor.
//:End Macro

//:Macro CNiAvr6552Configure
// This function configures the common properties of the DMM.
//+These properties include the measurement function, maximum range, and the
//+absolute resolution.
//:End Macro

//:Macro CNiAvr6552Configure1
// Pass the measurement function you want the DMM to perform.
//+The driver sets the !!Function!! property to this value.  
//+
//+
//+Valid Values:
//+
//+
//+Model AVR6552:
//+
//+! !!CNiAvr6552::VoltsDcFunction!!         - DC Volts 
//+! !!CNiAvr6552::VoltsAcFunction!!         - AC Volts
//+! !!CNiAvr6552::CurrentDcFunction!!       - DC Current
//+! !!CNiAvr6552::CurrentAcFunction!!       - AC Current
//+! !!CNiAvr6552::Resistance2WireFunction!! - 2-Wire Resistance
//+! !!CNiAvr6552::Lp2WireRes!!              - Low Power 2-Wire Resistance
//+! !!CNiAvr6552::Resistance4WireFunction!! - 4-Wire Resistance
//+! !!CNiAvr6552::Lp4WireRes!!              - Low Power 4-Wire Resistance
//+! !!CNiAvr6552::VoltsAcPlusDcFunction!!   - AC plus DC Volts
//+! !!CNiAvr6552::CurrentAcPlusDcFunction!! - AC plus DC Current
//+! !!CNiAvr6552::FrequencyFunction!!       - Frequency
//+! !!CNiAvr6552::Diode!!                   - Diode
//+
//+
//+Model AVR6552T:
//+
//+! !!CNiAvr6552::VoltsDcFunction!!          - DC Volts 
//+! !!CNiAvr6552::Resistance2WireFunction!!  - 2-Wire Resistance
//+! !!CNiAvr6552::Resistance4WireFunction!!  - 4-Wire Resistance
//+! !!CNiAvr6552::Lp2WireRes!!               - Low Power 2-Wire Resistance
//+! !!CNiAvr6552::Lp4WireRes!!               - Low Power 4-Wire Resistance
//+
//+
//+Model AVR6552T-R:
//+
//+! !!CNiAvr6552::VoltsDcFunction!!          - DC Volts 
//+! !!CNiAvr6552::Resistance2WireFunction!!  - 2-Wire Resistance
//+! !!CNiAvr6552::Resistance4WireFunction!!  - 4-Wire Resistance
//+! !!CNiAvr6552::RippleVolts!!              - Ripple Volts
//+
//+
//+Notes:  
//+
//+(1)  This property can only be set to DC Volts or DC Current when the sample
//+rate is !!CNiAvr6552::Longit!!.
//+
//+
//+(2)  This property can only be set to DC Volts, DC Current, 2-Wire
//+resistance, or LP 2-Wire resistance if the sample mode is
//+!!CNiAvr6552::Burst!!.
//:End Macro

//:Macro CNiAvr6552Configure2
// Pass the measurement range you want to use.
//+The driver sets the !!Range!! property to this value.  
//+
//+
//+Use positive values to represent the absolute value of the maximum expected
//+measurement.  The value must be in units appropriate for the Measurement
//+Function.  For example, when you set the Measurement Function to
//+!!CNiAvr6552::VoltsDcFunction!!, you must specify the Range in volts. 
//+Setting this parameter to 10.0 configures the DMM to measure DC voltages from
//+-10.0 to +10.0 volts.
//+
//+
//+The driver reserves special negative values for controlling the DMM's
//+auto-ranging capability.
//+
//+
//+Valid Values:
//+
//+*!!CNiAvr6552::OnAutoRange!!  (-1.0) - Auto-range On
//+
//+
//+Valid Manual Range:  The valid manual range depends on the measurement
//+function you select with the !!Function!! property.
//+
//+
//+AVR6552:
//+
//+! --------------------------------------------------------------
//+! |    Function    |             Valid Value           | Unit  |      
//+! |----------------|-----------------------------------|-------|
//+! | DC Volts       |0.03, 0.3, 3.0, 30.0, 300.0, 1000.0| volts |
//+! |----------------|-----------------------------------|-------|
//+! | AC Volts       |  0.3, 3.0, 30.0, 300.0, 700.0     | volts |
//+! | AC+DC Volts    |                                   |       |  
//+! |----------------|-----------------------------------|-------|
//+! | DC/AC Current  |  0.003, 0.03, 0.3, 3.0            | amps  |
//+! | AC+DC Current  |                                   |       |
//+! |----------------|-----------------------------------|-------|
//+! | 2/4-Wire Res   |  30,300,3K,30K,300K,3M,30M,300M   | ohms  |
//+! |----------------|-----------------------------------|-------|
//+! | LP 2/4-Wire Res|  300, 3K, 30K, 300K, 3M, 30M      | ohms  |
//+! --------------------------------------------------------------
//+
//+
//+AVR6552T:
//+
//+! --------------------------------------------------------------
//+! |    Function    |             Valid Value           | Unit  |      
//+! |----------------|-----------------------------------|-------|
//+! | DC Volts       |  0.03, 0.3, 3.0, 30.0, 200.0      | volts |
//+! |----------------|-----------------------------------|-------|
//+! | 2/4-Wire Res   |  30,300,3K,30K,300K,3M,30M,300M   | ohms  |
//+! |----------------|-----------------------------------|-------|
//+! | LP 2/4-Wire Res|  300, 3K, 30K, 300K, 3M, 30M      | ohms  |
//+! --------------------------------------------------------------
//+
//+
//+AVR6552T-R:
//+
//+! --------------------------------------------------------------
//+! |    Function    |             Valid Value           | Unit  |      
//+! |----------------|-----------------------------------|-------|
//+! | DC Volts       |  0.3, 3.0, 30.0, 300.0            | volts |
//+! |----------------|-----------------------------------|-------|
//+! | 2/4-Wire Res   |  30, 300, 3K, 30K, 300K, 3M, 30M  | ohms  |
//+! |----------------|-----------------------------------|-------|
//+! | Ripple Voltage |  0.03, 0.3, 3.0                   | volts |
//+! --------------------------------------------------------------
//+
//+
//+Notes:
//+
//+
//+(1) For AVR6552, the auto range is always on when the measurement function is
//+set to !!CNiAvr6552::FrequencyFunction!!, and the range value is fixed value
//+3000mv when the measurement function is set to !!CNiAvr6552::Diode!!.
//:End Macro

//:Macro CNiAvr6552Configure3
// Pass your desired measurement resolution in absolute units.
//+The driver sets the !!Resolution!! property to this value.
//+
//+
//+Setting this parameter to lower values increases the measurement accuracy. 
//+Setting this parameter to higher values increases the measurement speed.     
//+
//+
//+Valid Range:
//+
//+
//+The valid range depends on the current measurement function and the current
//+measurement range that you select with the !!Function!! and the !!Range!!
//+properties. The table below shows the minimum and maximum resolution values
//+for each measurement type. The minimum resolution values are based on the
//+maximum possible range for that measurement function. The maximum resolution
//+values are based on the minimum possible range. 
//+
//+  
//+! ---------------------------------------------------------------
//+! |    Function    | Min Resolution|  Max Resolution   | Unit   |      
//+! ---------------------------------------------------------------
//+! | DC Volts       |  0.1          |  100.0E-9         | volts  |
//+! |----------------|---------------|-------------------|--------| 
//+! | AC Volts       |  0.1          |  1.0E-6           | volts  |
//+! | AC+DC Volts    |               |                   |        |
//+! |----------------|---------------|-------------------|--------| 
//+! | DC Current     |  100.0E-6     |  10.0E-9          | amps   |
//+! |----------------|---------------|-------------------|--------| 
//+! | AC Current     |  100.0E-6     |  10.0E-9          | amps   |
//+! | AC+DC Current  |               |                   |        |
//+! |----------------|---------------|-------------------|--------| 
//+! | 2/4 Wire Res   |  10000.0      |  100.0E-6         | ohms   |
//+! | LP 2/4 Wire Res|               |                   | ohms   |
//+! |----------------|---------------|-------------------|--------| 
//+! | Ripple Volts   |  0.0001       |  10.0E-6          | volts  |
//+! --------------------------------------------------------------
//+
//+
//+Notes:
//+
//+
//+(1) This parameter is ignored if the 'Range' parameter is set to
//+!!CNiAvr6552::OnAutoRange!!.
//:End Macro

