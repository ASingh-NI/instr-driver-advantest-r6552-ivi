/***************************************************************************** 
 *Copyright 1999-2013 National Instruments Corporation.  All Rights Reserved.* 
 *****************************************************************************/

/*****************************************************************************
 *  avr6552 Instrument Driver Sample Program                              
 *  Developed with LabWindows/CVI 5.1                                     
 *  Original Release:                                  
 *
 *  Purpose:  This sample program initializes the DMM, configures
 *            it to measure DC voltage, and displays the measurement data.
 *
 *            To run this sample program, do the following:
 *            1) Create a new project in LabWindows/CVI.
 *            2) Add this file to the project.  To add files to the project,
 *               select the Add Files To Project from the Edit menu of the 
 *               Project window.
 *            3) Add one of the following files to the project:
 *               avr6552.fp, avr6552.c, or avr6552.lib.
 *            4) Check that the resource name used in avr6552_InitWithOptions()
 *               is correct. If simulating, change flag to simulate.
 *            5) Run the project.
 *
 *****************************************************************************/

#include <userint.h> 
#include <stdio.h>
#include "avr6552.h"

main ()

{
    ViSession   avr6552;
    ViStatus    error = VI_SUCCESS;
    ViReal64    reading;
    ViChar      msgStr[256];
        /* 
            If you want to run this sample program and the instrument is not
            present, set the Simulate flag to 1.  (Example:  "Simulate = 1")
        */
              checkErr (avr6552_InitWithOptions ("GPIB0::2::INSTR", VI_TRUE, VI_TRUE,
                                         "Simulate=0,RangeCheck=1,QueryInstrStatus=0,Cache=1",
                                         &avr6552));
              checkErr (avr6552_ConfigureMeasurement (avr6552, AVR6552_VAL_DC_VOLTS, AVR6552_VAL_AUTO_RANGE_ON,
                                  0.0001));
              checkErr (avr6552_ConfigureACBandwidth (avr6552, 20.0, 300000.0));
              checkErr (avr6552_ConfigureTrigger (avr6552, AVR6552_VAL_IMMEDIATE, 0.0));
              checkErr (avr6552_Read (avr6552, 5000, &reading));
              sprintf (msgStr, "Reading = %f.\n", reading);
              MessagePopup ("Measurement", msgStr);
              
Error:
    if (error != VI_SUCCESS)
        {
            ViChar   errStr[2048];

            avr6552_GetError (avr6552, &error, 2048, errStr);
            MessagePopup ("Error!", errStr);
        }

    if (avr6552)
        avr6552_close (avr6552);
}
