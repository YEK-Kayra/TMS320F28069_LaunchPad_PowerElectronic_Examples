/******************************************************************************
 * Brief: Handling ePWM Interrupts and PIE Acknowledge in C2000
 *
 * ePWM Interrupt Handling:
 * The line "EPwm1Regs.ETCLR.bit.INT = 1;" clears the interrupt flag for the
 * specified ePWM channel after an interrupt has occurred. By commenting out
 * this line, the ePWM signal will be blocked, as the interrupt flag remains
 * set, preventing new interrupts from being processed.
 * Note that this action only affects the ePWM1 signal, while other ePWM channels
 * (like ePWM2 and others) are unaffected by this change.
 *

 * PIE Acknowledge Handling:
 * The line "PieCtrlRegs.PIEACK.bit.ACK3 = 1;" acknowledges the interrupt for
 * group 3, allowing further interrupts from this group to be processed.
 * Commenting out this line will block all group 3 interrupts since the
 * processor remains occupied with the current interrupt and cannot handle new
 * ones. The other interrupt groups are not affected.
 *
 ******************************************************************************/


/** TO-DO LIST
 *
 * Configure ePWM1 and ePWM2
 * ePWM1 = 2000 Hz switching frequency
 * ePWM1 produce an interrupt toggle GPIO12
 *
 * ePWM2 = 4000 Hz switching frequency
 * ePWM2 produce an interrupt toggle GPIO20
 */

    //-->Function prototypes definition
__interrupt void ePWM1_ISR(void);
__interrupt void ePWM2_ISR(void);

#include "DSP28x_Project.h"
void main(void)
{

    //-->Initialize system
         InitSysCtrl();

    //-->Disable global interrupt
         DINT;

    //-->Disable interrupt IER and clear IFR
         IER = 0x0000;
         IFR = 0x0000;

    //-->Initialize peripherals
         InitPieCtrl();

    //-->Initialize PIE Vector Table
         InitPieVectTable();

    //-->Configure GPIO pins
        EALLOW;
        GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;    //General purpose input/output
        GpioCtrlRegs.GPADIR.bit.GPIO12  = 1;    //Configures the GPIO pin as an output

        GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 0;
        GpioCtrlRegs.GPADIR.bit.GPIO20  = 1;
        EDIS;

    //-->Assign ISR function prototypes(send it to the Vector Elements)
         EALLOW;
         PieVectTable.EPWM1_INT = &ePWM1_ISR;
         PieVectTable.EPWM2_INT = &ePWM2_ISR;
         EDIS;

    //-->Configure ePWM1 and ePWM2 peripherals

             //Enable ePWM clocks
             EALLOW;
             SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1;
             SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;
             EDIS;

             //Configure ePWM_1 Time Base & Event Trigger Sub Modules
             EPwm1Regs.TBCTL.bit.CTRMODE   = 0; //Sawtooth model,Up-count mode
             EPwm1Regs.TBCTL.bit.FREE_SOFT = 2; //Free
             EPwm1Regs.TBPRD = 22499;           //Setting 2000Hz time period
                                                //TBCLK = ( SYSCLKOUT / ( HSPCLKDIV * CLKDIV ) )
                                                //TBPRD = ( (Tpwm / TBCLK) - 1 )

             EPwm1Regs.ETSEL.bit.INTEN  = 1;     //Enable EPWMx_INT generation
             EPwm1Regs.ETSEL.bit.INTSEL = 1;     //Enable event time-base counter equal to zero. (TBCTR = 0x0000)
             EPwm1Regs.ETPS.bit.INTPRD  = 1;     //Generate an interrupt on the first event INTCNT = 01 (first event)


             //Configure ePWM_2 Time Base & Event Trigger Sub Modules
             EPwm2Regs.TBCTL.bit.CTRMODE   = 0; //Sawtooth model,Up-count mode
             EPwm2Regs.TBCTL.bit.FREE_SOFT = 2; //Free
             EPwm2Regs.TBPRD = 11249;           //Setting 4000Hz time period
                                                //TBCLK = ( SYSCLKOUT / ( HSPCLKDIV * CLKDIV ) )
                                                //TBPRD = ( (Tpwm / TBCLK) - 1 )

             EPwm2Regs.ETSEL.bit.INTEN  = 1;     //Enable EPWMx_INT generation
             EPwm2Regs.ETSEL.bit.INTSEL = 1;     //Enable event time-base counter equal to zero. (TBCTR = 0x0000)
             EPwm2Regs.ETPS.bit.INTPRD  = 1;     //Generate an interrupt on the first event INTCNT = 01 (first event)


    //-->Enable appropriate interrupts in the PIE module
         PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
         PieCtrlRegs.PIEIER3.bit.INTx2 = 1;

    //-->Enable interrupts for PIE Group in IER
         IER = M_INT3;

    //-->Enable the global interrupt (INTM = 0)
         EINT;

    //-->Start clocks (optional)
         //Since there is no start bit to initiate the ePWM, we skip it.

    //-->Infinite loop
         for(;;);

}


    //-->System ISR Functions

__interrupt void ePWM1_ISR(void){

    /*! BEGIN - ACTION - BEGIN */
    GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1;
    /*! END - ACTION - END */

    /*! BEGIN - CLEAN_UP - BEGIN */
    PieCtrlRegs.PIEACK.bit.ACK3 = 1;            //Acknowledge PIE Interrupt Group 3, Writing a 1 clears the bit(Local interrupt flag)
    EPwm1Regs.ETCLR.bit.INT = 1;                //Clears the ETFLG[INT] flag bit and enable further interrupts pulses to be generated
    /*! END - CLEAN_UP - END */

}
__interrupt void ePWM2_ISR(void){

    /*! BEGIN - ACTION - BEGIN */
    GpioDataRegs.GPATOGGLE.bit.GPIO20 = 1;
    /*! END - ACTION - END */

    /*! BEGIN - CLEAN_UP - BEGIN */
    PieCtrlRegs.PIEACK.bit.ACK3 = 1;            //Acknowledge PIE Interrupt Group 3, Writing a 1 clears the bit(Local interrupt flag)
    EPwm2Regs.ETCLR.bit.INT = 1;                //Clears the ETFLG[INT] flag bit and enable further interrupts pulses to be generated
    /*! END - CLEAN_UP - END */
}



