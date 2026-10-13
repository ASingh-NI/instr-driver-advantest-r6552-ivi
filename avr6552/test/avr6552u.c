#include <utility.h>
#include "avr6552.h"
#include <cvirte.h>     /* Needed if linking in external compiler; harmless otherwise */
#include <userint.h>
#include "avr6552u.h"

static int panelHandle;
ViSession vi;

int main (int argc, char *argv[])
{
    if (InitCVIRTE (0, argv, 0) == 0)   /* Needed if linking in external compiler; harmless otherwise */
        return -1;  /* out of memory */
    if ((panelHandle = LoadPanel (0, "avr6552u.uir", PANEL)) < 0)
        return -1;
    DisplayPanel (panelHandle);
    RunUserInterface ();
    return 0;
}

int CVICALLBACK quit (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    switch (event)
        {
        case EVENT_COMMIT:
            QuitUserInterface (0);
            break;
        }
    return 0;
}

int CVICALLBACK test (int panel, int control, int event,
        void *callbackData, int eventData1, int eventData2)
{
    ViInt32  seqNo, points;
    ViReal64 multiplier, offset, reading, readings[1000];
    ViStatus error = VI_SUCCESS;

    switch (event)
        {
        case EVENT_LEFT_CLICK:
            SetCtrlVal (panelHandle, PANEL_TEXTMSG, "    ");
            GetCtrlVal (panelHandle, PANEL_RING, &seqNo);
            switch (seqNo)
             {
             case 1:
             /*Sequence 1:DC Volts,single point measurement*/
              checkErr (avr6552_InitWithOptions ("GPIB0::2::INSTR", VI_TRUE, VI_TRUE,
                                         "Simulate=1,RangeCheck=1,QueryInstrStatus=1,Cache=1",
                                         &vi));
              checkErr (avr6552_ConfigureMeasurement (vi, AVR6552_VAL_DC_VOLTS, AVR6552_VAL_AUTO_RANGE_ON,
                                  0.001));
              checkErr (avr6552_ConfigureACBandwidth(vi, 20.0, 30000.0));
              checkErr (avr6552_ConfigureTrigger (vi, AVR6552_VAL_IMMEDIATE, 0.0));
              checkErr (avr6552_ConfigureNull (vi, VI_TRUE, 1));
              checkErr (avr6552_ForceTrigger (vi));
              checkErr (avr6552_Read (vi, 5000, &reading));
              checkErr (avr6552_ConfigureNull (vi, VI_FALSE, 0));
              checkErr (avr6552_close (vi));
              break;
             case 2:
               /*Sequence 2:AC Volts,multi point measurement*/
              checkErr (avr6552_InitWithOptions ("GPIB0::2::INSTR", VI_TRUE, VI_TRUE,
                                         "Simulate=1,RangeCheck=1,QueryInstrStatus=1,Cache=1",
                                         &vi));
              checkErr (avr6552_ConfigureMeasurement (vi, AVR6552_VAL_AC_VOLTS, 1, 0.001));
              checkErr (avr6552_ConfigureTrigger (vi, AVR6552_VAL_IMMEDIATE, 0.0));
              checkErr (avr6552_ConfigureSampleMode (vi, AVR6552_VAL_HOLD, 0.0));
              checkErr (avr6552_ConfigureScale (vi, VI_TRUE, 1.0, 1.0, 0.0));
              checkErr (avr6552_ReadHoldMeasurement (vi, 5000, &reading));
              checkErr (avr6552_ConfigureScale (vi, VI_FALSE, 1.0, 1.0, 0.0));
              checkErr (avr6552_close (vi));     
              break;
             case 3:
               /*Sequence 3:2-Wire Resistance,comparator burst sample mode measurement*/
               checkErr (avr6552_InitWithOptions ("GPIB0::2::INSTR", VI_TRUE, VI_TRUE,
                                         "Simulate=1,RangeCheck=1,QueryInstrStatus=1,Cache=1",
                                         &vi));
               checkErr (avr6552_ConfigureMeasurement (vi, AVR6552_VAL_2_WIRE_RES,
                                                        AVR6552_VAL_AUTO_RANGE_ON, 0.0001));
               checkErr (avr6552_ConfigureCompare (vi, VI_TRUE, 0.0, 0.0));
               checkErr (avr6552_ConfigureSampleRate (vi, AVR6552_VAL_MED, 100.0));
               checkErr (avr6552_ConfigureSampleMode (vi, AVR6552_VAL_BURST, 0.0));
               checkErr (avr6552_ConfigureBurst (vi, 1000, 0));
               checkErr (avr6552_ForceTrigger (vi));
               checkErr (avr6552_ReadBurstMultiPoint (vi, 5000, 1000, readings,&points));
               checkErr (avr6552_ConfigureCompare (vi, VI_FALSE, 0.0, 0.0));
               checkErr (avr6552_close (vi));    
               break;
             case 4:
               /*Sequence 4:Ripple Volts, simulate, smooth single point measurement*/
               checkErr (avr6552_InitWithOptions ("GPIB0::2::INSTR", VI_TRUE, VI_TRUE,
                                         "Simulate=1,RangeCheck=1,QueryInstrStatus=1,Cache=1",
                                         &vi));
               checkErr (avr6552_ConfigureSmooth (vi, VI_TRUE, 10));
               checkErr (avr6552_Read (vi, 5000, &reading));
               checkErr (avr6552_ConfigureSmooth (vi, VI_FALSE, 10));
               checkErr (avr6552_close (vi));
               break;
             } 
Error:  
  if (error != VI_SUCCESS)
     SetCtrlVal (panelHandle, PANEL_TEXTMSG, "FAIL");
  else
     SetCtrlVal (panelHandle, PANEL_TEXTMSG, "PASS");
            
            break;
        }
   return 0;    
    
}

