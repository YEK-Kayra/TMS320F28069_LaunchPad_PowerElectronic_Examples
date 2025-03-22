

/**
 * TO DO List
 * Example for configurating and using Timer 0, 1 and 2
 * With Timer 0, toggle GPIO12
 * With Timer 1, toggle GPIO20
 * With Timer 2, toggle GPIO32
 *
 * SYSCLKOUT  : 90MHz, (ref. to InitSysCtrl() func.)
 * Timer0_PRD : 0.01s
 * Timer1_PRD : 1.5s
 * Timer2_PRD : 60s
 *
 */
#include "DSP28x_Project.h"

__interrupt void timer0_ISR(void);
__interrupt void timer1_ISR(void);
__interrupt void timer2_ISR(void);


void main(void)
{

    /*! Initializing system */
    InitSysCtrl();

    /*! Disable global interrupt mask bit */
    DINT;

    /*! Disable all interrupts and clear all flag */
    IER = 0x0000;
    IFR = 0x0000;

    /*! Initializing PIE module */
    InitPieCtrl();

    /*! Initializing PIE Vector Table */
    InitPieVectTable();
    EALLOW;
        PieVectTable.TINT0 = &timer0_ISR;
        PieVectTable.TINT1 = &timer1_ISR;
        PieVectTable.TINT2 = &timer2_ISR;
    EDIS;

    /*! Configure Timer 0 */
    EALLOW;
        SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; //Enable timer clock
    EDIS;
        CpuTimer0Regs.TCR.bit.TSS = 1;              //Stop the timer
        CpuTimer0Regs.PRD.all = 900000;             //Load the period value
        CpuTimer0Regs.TCR.bit.TRB = 1;              //Load PRDH:PRD into TIMH:TIM(32bit counter), TDDRH:TDDR into PSCH:PSC
        CpuTimer0Regs.TCR.bit.TIE = 1;              //Enable timer interrupt
        CpuTimer0Regs.TCR.bit.FREE= 1;              //Enable free mode(Let the timer keep running in the background during debugging)
        CpuTimer0Regs.TPRH.bit.TDDRH = 0;           //Set pre-scale value to o, we don't need to use it
        CpuTimer0Regs.TPR.bit.TDDR   = 0;


    /*! Configure Timer 1 */
    EALLOW;
        SysCtrlRegs.PCLKCR3.bit.CPUTIMER1ENCLK = 1;
    EDIS;
        CpuTimer1Regs.TCR.bit.TSS = 1;
        CpuTimer1Regs.PRD.all = 135000000;
        CpuTimer1Regs.TCR.bit.TRB = 1;
        CpuTimer1Regs.TCR.bit.TIE = 1;
        CpuTimer1Regs.TCR.bit.FREE= 1;
        CpuTimer1Regs.TPRH.bit.TDDRH = 0;
        CpuTimer1Regs.TPR.bit.TDDR   = 0;

    /*! Configure Timer 2 */
    EALLOW;
        SysCtrlRegs.PCLKCR3.bit.CPUTIMER2ENCLK = 1;
    EDIS;
        CpuTimer2Regs.TCR.bit.TSS = 1;
        CpuTimer2Regs.PRD.all = 1350000000;
        CpuTimer2Regs.TCR.bit.TRB = 1;
        CpuTimer2Regs.TCR.bit.TIE = 1;
        CpuTimer2Regs.TCR.bit.FREE= 1;
        CpuTimer2Regs.TPRH.bit.TDDRH = 0;
        CpuTimer2Regs.TPR.bit.TDDR   = 1;

    /*! Configure GPIO pins */
    EALLOW;
        GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0;
        GpioCtrlRegs.GPADIR.bit.GPIO12  = 1;

        GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 0;
        GpioCtrlRegs.GPADIR.bit.GPIO20  = 1;

        GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;
        GpioCtrlRegs.GPBDIR.bit.GPIO32  = 1;
    EDIS;

    /*! Set up Interrupts and PIE module */
        PieCtrlRegs.PIEIER1.bit.INTx7 = 1;          //Set up interrupt of Timer0 at the PIE module
        /*
         * Timer_1 and Timer_2 are not connected PIE Module
         */

    /*! Enable interrupts in IER */
        IER |= M_INT1;                              //Allow the processor to handle the interrupts coming from this line_1(For Timer_0)
        IER |= M_INT13;                             //                                                              line_13(For Timer_1)
        IER |= M_INT14;                             //                                                              line_14(For Timer_2)

    /*! Enable global interrupt */
        EINT;

    /*! Start the timer */
        CpuTimer0Regs.TCR.bit.TSS = 0;
        CpuTimer1Regs.TCR.bit.TSS = 0;
        CpuTimer2Regs.TCR.bit.TSS = 0;

    /*! Infinite loop*/
        for(;;);


}

__interrupt void timer0_ISR(void){
    /*! BEGIN - ACTION - BEGIN */
    GpioDataRegs.GPATOGGLE.bit.GPIO12 = 1;      //GPIO_LED_12 will be toggled for each ISR
    /*! END - ACTION - END */

    /*! BEGIN - CLEAN_UP - BEGIN */
    CpuTimer0Regs.TCR.bit.TIF = 1;              //Clear timer0 interrupt flag(optional)
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;            //Acknowledge PIE Interrupt Group 1, Writing a 1 clears the bit(Local interrupt flag)
    /*! END - CLEAN_UP - END */


}
__interrupt void timer1_ISR(void){
    /*! BEGIN - ACTION - BEGIN */
    GpioDataRegs.GPATOGGLE.bit.GPIO20 = 1;      //GPIO_LED_20 will be toggled for each ISR
    /*! END - ACTION - END */

    /*! BEGIN - CLEAN_UP - BEGIN */
    CpuTimer1Regs.TCR.bit.TIF = 1;              //Clear timer1 interrupt flag(optional)
    /*! END - CLEAN_UP - END */

}
__interrupt void timer2_ISR(void){
    /*! BEGIN - ACTION - BEGIN */
    GpioDataRegs.GPBTOGGLE.bit.GPIO32 = 1;      //GPIO_LED_32 will be toggled for each ISR
    /*! END - ACTION - END */

    /*! BEGIN - CLEAN_UP - BEGIN */
    CpuTimer2Regs.TCR.bit.TIF = 1;              //Clear timer2 interrupt flag(optional)
    /*! END - CLEAN_UP - END */

}








