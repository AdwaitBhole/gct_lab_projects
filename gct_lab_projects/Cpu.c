//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************

//#include "../Util/UtilInc.h"
//#include "CpuInc.h"
#include "../include/CmnInc.h"
#include "../include/Config.h"

#define CPU_ISR_RST()           (gCpu.m_IsrMsk = 0)
#define CPU_SET_ISR(IsrMsk)     (gCpu.m_IsrMsk |= IsrMsk)
#define CPU_CLR_ISR(IsrMsk)     (gCpu.m_IsrMsk &= (~IsrMsk))
#define IS_ISR_SET(IsrMsk)      (gCpu.m_IsrMsk & IsrMsk)

// ====================================================================
// CPU Osc/Ppi Config
// ====================================================================
void CCpu_InitOsc()
{
    // Configure Oscillator 40MHz
    // Fosc = Fin*M/(N1*N2), Fcy=Fosc/2
    // Fosc = 8MHz*40/(2*2)=80Mhz for 8M input clock => Fcy = 40MHz
    RCONbits.SWDTEN     = 0;    // Disable Watch Dog Timer
    PLLFBD              = 38;   // M=40
    CLKDIVbits.PLLPOST  = 0;    // N1=2
    CLKDIVbits.PLLPRE   = 0;    // N2=2
    OSCTUN = 0;                 // Tune FRC oscillator, if FRC is used
    // clock switching to incorporate PLL
    __builtin_write_OSCCONH( 0x03 );    // Initiate Clock Switch to Primary
    // Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONL( 0x01 );    // Start clock switching
    while( OSCCONbits.COSC != 0b011 );
    // Wait for Clock switch to occur
    // Wait for PLL to lock
    while( OSCCONbits.LOCK != 1 )
    { };
}

// ====================================================================
// CPU ISR Initialisation
// ====================================================================
void CCpu_ClrIsrMsk()
{
    CPU_ISR_RST();
}

void CCpu_UpdIsr()
{
    if (IS_ISR_SET(CPU_ISR_TMR1))
       IEC0bits.T1IE = 1;   // Enable Timer1 interrupt
    if (IS_ISR_SET(CPU_ISR_COM1TX))
        IEC0bits.U1TXIE = 1;    // Enable UART TX interrupt
    if (IS_ISR_SET(CPU_ISR_COM1RX))
        IEC0bits.U1RXIE = 1;    // Enable UART RX interrupt
     if (IS_ISR_SET(CPU_ISR_COM2TX))
        IEC1bits.U2TXIE = 1;    // Enable UART TX interrupt
    if (IS_ISR_SET(CPU_ISR_COM2RX))
        IEC1bits.U2RXIE = 1;    // Enable UART RX interrupt
    if (IS_ISR_SET(CPU_ISR_DMA0))  // Enable DMA0 transfer interrupt
        IEC0bits.DMA0IE = 1;
    if (IS_ISR_SET(CPU_ISR_DMA1)) // Enable DMA1 transfer interrupt
        IEC0bits.DMA1IE = 1;
    if (IS_ISR_SET(CPU_ISR_ECAN)) // Enable CAN interrupt
        IEC2bits.C1IE = 1;
    if (IS_ISR_SET(CPU_ISR_SPI1))   // Enable SPI1 isr
        IEC0bits.SPI1IE = 1;
    if (IS_ISR_SET(CPU_ISR_I2C1))   // Enable I2C1 isr
        IEC1bits.MI2C1IE = 1;
    if (IS_ISR_SET(CPU_ISR_SPI2))   // Enable SPI2 isr
        IEC2bits.SPI2IE = 1;
}

void CCpu_InitTmr1Isr()
{
    TMR1 = 0x0000;
    PR1 = TMR1_PR;
    T1CONbits.TCKPS = TMR1_PRESCL_CODE;     // Pre scaler of 1 => 8
    IFS0bits.T1IF = 0;                      // Clear Timer1 interrupt
    IPC0bits.T1IP = 6;                      // Set Timer1 interrupt priority to 6 (7 is highest)
    T1CONbits.TON = 1;                      // Start Timer1
    CPU_SET_ISR(CPU_ISR_TMR1);
}

// ====================================================================
// CPU UART
// ====================================================================
// Private Proto
// =============
void CCpu_Rs232ChnSetBaudrate(int iChn, int Baud);
void CCpu_Rs232ChnSetDbitsParity(int iChn, int Dbits, int Parity);
void CCpu_Rs232ChnSetSbits(int iChn, int Sbits);

void CCpu_Rs232ChnSetBaudrate(int iChn, int Baud)
{
    if (Baud <= 0)
        return;
    int baudval = ((FCY/Baud)/16)-1;
    switch (iChn) {
    case CPU_RS232_CHN1: U1BRG = baudval;   break;
    case CPU_RS232_CHN2: U2BRG = baudval;   break;
    }
}

void CCpu_Rs232ChnSetDbitsParity(int iChn, int Dbits, int Parity)
{
    int pdsel;
    switch(Parity)
    {
    case PAR_EVEN :
        pdsel =0x1;
        break;
    case PAR_ODD    :
        pdsel =0x2;
        break;
    case PAR_NONE   :
    default         :
        if(Dbits == DBIT_9)
             pdsel =0x3;
        else pdsel =0x0;
        break;
    }
    // Set CPU sfr
    switch (iChn) {
    case CPU_RS232_CHN1 : U1MODEbits.PDSEL = pdsel; break;
    case CPU_RS232_CHN2 : U2MODEbits.PDSEL = pdsel; break;
    }
}

void CCpu_Rs232ChnSetSbits(int iChn, int Sbits)
{
    int stsel=0;
    if (Sbits == SBIT_2)
         stsel = 1;
    else stsel = 0;
    switch (iChn) {
    case CPU_RS232_CHN1 : U1MODEbits.STSEL = stsel; break;
    case CPU_RS232_CHN2 : U2MODEbits.STSEL = stsel; break;
    }
}

void CCpu_Rs232ChnInit(int iChn, uInt2 Baud, int Dbits, int Parity, int Sbits)
{
    CCpu_Rs232ChnSetBaudrate(iChn, Baud);
    CCpu_Rs232ChnSetDbitsParity(iChn, Dbits, Parity);
    CCpu_Rs232ChnSetSbits(iChn, Sbits);
    switch (iChn) {
    case CPU_RS232_CHN1:
        U1MODEbits.ABAUD = 0;   // Auto-Baud disabled
        U1MODEbits.BRGH = 0;    // Standard-Speed mode
        U1STAbits.UTXISEL0 = 0; // Interrupt after one TX character is transmitted
        U1STAbits.UTXISEL1 = 1;
        U1STAbits.URXISEL = 0;  // Interrupt after one RX character is received;
        U1MODEbits.UARTEN = 1;  // Enable UART
        U1STAbits.UTXEN = 1;    // Enable UART TX
        CPU_SET_ISR(CPU_ISR_COM1TX);
        CPU_SET_ISR(CPU_ISR_COM1RX);
        break;
    case CPU_RS232_CHN2:
        U2MODEbits.ABAUD = 0;   // Auto-Baud disabled
        U2MODEbits.BRGH = 0;    // Standard-Speed mode
        U2STAbits.UTXISEL0 = 0; // Interrupt after one TX character is transmitted
        U2STAbits.UTXISEL1 = 1;
        U2STAbits.URXISEL = 0;  // Interrupt after one RX character is received;
        U2MODEbits.UARTEN = 1;  // Enable UART
        U2STAbits.UTXEN = 1;    // Enable UART TX
        CPU_SET_ISR(CPU_ISR_COM2TX);
        CPU_SET_ISR(CPU_ISR_COM2RX);
        break;
    }
}

void CCpu_Rs232ChnTxD(int iChn, Byte Ch)
{
    switch (iChn) {
    case CPU_RS232_CHN1: U1TXREG = Ch;      break;
    case CPU_RS232_CHN2: U2TXREG = Ch;      break;
    }
}

// ====================================================================
// CPU ECAN
// ====================================================================

void CCpu_CanReset()
{
    CCpu_CanSelBufWin();
    C1RXFUL1 = 0;
    C1RXFUL2 = 0;
    C1RXOVF1 = 0;
    C1RXOVF2 = 0;
    C1INTE   = 0;
}

void CCpu_CanSetIsr()
{
    CCpu_CanSelBufWin();

    C1INTEbits.TBIE = 1;        // Transmit interrupt enable
    C1INTEbits.RBIE = 1;        // Receive interrupt enable
    C1INTEbits.FIFOIE = 1;      // Fifo isr enable
    C1INTEbits.ERRIE = 1;       // Error interrupt enable
    C1INTEbits.RBOVIE = 1;      // RxBuf Overflow interrupt enable
    C1INTEbits.IVRIE  = 1;      // Invalid message interrupt enable

    CPU_SET_ISR(CPU_ISR_ECAN);
}

BOOL CCpu_CanIsValidFltNum(int FltNum)
{
    return ((FltNum >= 0) && (FltNum < CAN_FLT_N)) ? TRUE : FALSE;
}

BOOL CCpu_CanIsValidFltMskNum(int FltMskNum)
{
    return ((FltMskNum >= 0) && (FltMskNum < CAN_FLTMSK_N)) ? TRUE : FALSE;
}

BOOL CCpu_CanIsValidOpMode(int OpMode)
{
    BOOL flag = FALSE;
    switch (OpMode) {
    case CAN_OPMODE_NORM   :
    case CAN_OPMODE_DSB    :
    case CAN_OPMODE_LPBK   :
    case CAN_OPMODE_LSN    :
    case CAN_OPMODE_CNF    :
    case CAN_OPMODE_ALLMSG : flag = TRUE;  break;
    default                 : flag = FALSE; break;
    }
    return flag;
}

BOOL CCpu_CanIsFltWin()
{
    return (C1CTRL1bits.WIN == 1);
}

BOOL CCpu_CanIsBufWin()
{
    return (C1CTRL1bits.WIN == 0);
}

void CCpu_CanSelFltWin()
{
    if (CCpu_CanIsFltWin())
        return;
    C1CTRL1bits.WIN = 1;
}

void CCpu_CanSelBufWin()
{
    if (CCpu_CanIsBufWin())
        return;
    C1CTRL1bits.WIN = 0;
}

BOOL CCpu_CanIsOpMode(int OpMode)
{
    CCpu_CanSelBufWin();
    return (C1CTRL1bits.OPMODE == OpMode);
}

int CCpu_CanSetOpMode(int OpMode)
{
    if (!CCpu_CanIsValidOpMode(OpMode))
        return ERR;
    // Chk if already in same OpMode
    if (CCpu_CanIsOpMode(OpMode))
        return OK;

    CCpu_CanSelBufWin();
    // Set req opmode and wait
    C1CTRL1bits.REQOP = OpMode;
    while (C1CTRL1bits.OPMODE != OpMode);
    return OK;
}

void CCpu_CanBaudInit()
{
//    Bit Time = (Sync Segment + Propagation Delay +
//                Phase Segment 1 + Phase Segment 2) = 20*TQ
//    Phase Segment 1 = 8TQ
//    Phase Segment 2 = 6Tq
//    Propagation Delay = 5Tq
//    Sync Segment = 1TQ
//    CiCFG1<BRP> =(FCAN /(2 ×N×FBAUD)) 1

    CCpu_CanSelBufWin();
    // Fcan [0:Fcan=Fp; 1: Fcan=2*Fp]
    C1CTRL1bits.CANCKS  = 0;
    //Sync Jump Wd [0x03=4*TQ;0x02=3*TQ;..;0x00=1*TQ]
    C1CFG1bits.SJW      = 0x03;
    // See SysDef.h
    C1CFG1bits.BRP      = ECAN_BRP;
    // PhaseSegment1 time[0x7=8TQ;0x6=7TQ;..;0x0=1TQ]
    C1CFG2bits.SEG1PH=0x7;
    // PhaseSegment2 time select
    // [0x1=freely programmable;0x0=max of SEG1PH or IPT whichever is greater]
    C1CFG2bits.SEG2PHTS = 0x1;
    // PhaseSegment2 time [0x7=8TQ;0x6=7TQ;0x5=6TQ;..;0x0=1TQ]
    C1CFG2bits.SEG2PH = 0x5;
    // PropagationSegment time[0x7=8TQ;0x6=7TQ;0x5=6TQ;..;0x0=1TQ]
    C1CFG2bits.PRSEG = 0x4;
    // Bus line is sampled [0x0=once;0x1=three times] at the sample point
    C1CFG2bits.SAM = 0x1;
}

void CCpu_CanEnbFlt(int FltNum, BOOL bEnb)
{// To enable/disable filter
    if (!CCpu_CanIsValidFltNum(FltNum))
        return;
    
    CCpu_CanSelFltWin();
    
    switch (FltNum) {
        case CAN_FLT_00: C1FEN1bits.FLTEN0 = bEnb; break;
        case CAN_FLT_01: C1FEN1bits.FLTEN1 = bEnb; break;
        case CAN_FLT_02: C1FEN1bits.FLTEN2 = bEnb; break;
        case CAN_FLT_03: C1FEN1bits.FLTEN3 = bEnb; break;
        case CAN_FLT_04: C1FEN1bits.FLTEN4 = bEnb; break;
        case CAN_FLT_05: C1FEN1bits.FLTEN5 = bEnb; break;
        case CAN_FLT_06: C1FEN1bits.FLTEN6 = bEnb; break;
        case CAN_FLT_07: C1FEN1bits.FLTEN7 = bEnb; break;
        case CAN_FLT_08: C1FEN1bits.FLTEN8 = bEnb; break;
        case CAN_FLT_09: C1FEN1bits.FLTEN9 = bEnb; break;
        case CAN_FLT_10: C1FEN1bits.FLTEN10 = bEnb; break;
        case CAN_FLT_11: C1FEN1bits.FLTEN11 = bEnb; break;
        case CAN_FLT_12: C1FEN1bits.FLTEN12 = bEnb; break;
        case CAN_FLT_13: C1FEN1bits.FLTEN13 = bEnb; break;
        case CAN_FLT_14: C1FEN1bits.FLTEN14 = bEnb; break;
        case CAN_FLT_15: C1FEN1bits.FLTEN15 = bEnb; break;
    }
}

void CCpu_CanSetFltSidReg(int FltNum, uInt2 Sid, BOOL bExIde, uInt2 Eid)
{// CiRXFnSID: ECAN Acceptance Filter Standard Identifier Register n (n = 0-15)
 // Program Acpt Flt Standard Identifier Register

    if (!CCpu_CanIsValidFltNum(FltNum))
        return;
    CCpu_CanSelFltWin();
    switch (FltNum) {
        case CAN_FLT_00:
            C1RXF0SIDbits.SID = Sid;
            C1RXF0SIDbits.EXIDE = bExIde;
            C1RXF0SIDbits.EID = Eid;
            break;
        case CAN_FLT_01:
            C1RXF1SIDbits.SID = Sid;
            C1RXF1SIDbits.EXIDE = bExIde;
            C1RXF1SIDbits.EID = Eid;
            break;
        case CAN_FLT_02:
            C1RXF2SIDbits.SID = Sid;
            C1RXF2SIDbits.EXIDE = bExIde;
            C1RXF2SIDbits.EID = Eid;
            break;
        case CAN_FLT_03:
            C1RXF3SIDbits.SID = Sid;
            C1RXF3SIDbits.EXIDE = bExIde;
            C1RXF3SIDbits.EID = Eid;
            break;
        case CAN_FLT_04:
            C1RXF4SIDbits.SID = Sid;
            C1RXF4SIDbits.EXIDE = bExIde;
            C1RXF4SIDbits.EID = Eid;
            break;
        case CAN_FLT_05:
            C1RXF5SIDbits.SID = Sid;
            C1RXF5SIDbits.EXIDE = bExIde;
            C1RXF5SIDbits.EID = Eid;
            break;
        case CAN_FLT_06:
            C1RXF6SIDbits.SID = Sid;
            C1RXF6SIDbits.EXIDE = bExIde;
            C1RXF6SIDbits.EID = Eid;
            break;
        case CAN_FLT_07:
            C1RXF7SIDbits.SID = Sid;
            C1RXF7SIDbits.EXIDE = bExIde;
            C1RXF7SIDbits.EID = Eid;
            break;
        case CAN_FLT_08:
            C1RXF8SIDbits.SID = Sid;
            C1RXF8SIDbits.EXIDE = bExIde;
            C1RXF8SIDbits.EID = Eid;
            break;
        case CAN_FLT_09:
            C1RXF9SIDbits.SID = Sid;
            C1RXF9SIDbits.EXIDE = bExIde;
            C1RXF9SIDbits.EID = Eid;
            break;
        case CAN_FLT_10:
            C1RXF10SIDbits.SID = Sid;
            C1RXF10SIDbits.EXIDE = bExIde;
            C1RXF10SIDbits.EID = Eid;
            break;
        case CAN_FLT_11:
            C1RXF11SIDbits.SID = Sid;
            C1RXF11SIDbits.EXIDE = bExIde;
            C1RXF11SIDbits.EID = Eid;
            break;
        case CAN_FLT_12:
            C1RXF12SIDbits.SID = Sid;
            C1RXF12SIDbits.EXIDE = bExIde;
            C1RXF12SIDbits.EID = Eid;
            break;
        case CAN_FLT_13:
            C1RXF13SIDbits.SID = Sid;
            C1RXF13SIDbits.EXIDE = bExIde;
            C1RXF13SIDbits.EID = Eid;
            break;
        case CAN_FLT_14:
            C1RXF14SIDbits.SID = Sid;
            C1RXF14SIDbits.EXIDE = bExIde;
            C1RXF14SIDbits.EID = Eid;
            break;
        case CAN_FLT_15:
            C1RXF15SIDbits.SID = Sid;
            C1RXF15SIDbits.EXIDE = bExIde;
            C1RXF15SIDbits.EID = Eid;
            break;
        default         : return;
    }
}

void CCpu_CanSetFltEidReg(int FltNum, uInt2 Eid)
{// CiRXFnEID: ECAN Acceptance Filter Extended Identifier Register n (n = 0-15)
 // Program Acpt Flt Extended Identifier Register
    if (!CCpu_CanIsValidFltNum(FltNum))
        return;
    CCpu_CanSelFltWin();
    switch (FltNum) {
        case CAN_FLT_00: C1RXF0EID = Eid; break;
        case CAN_FLT_01: C1RXF1EID = Eid; break;
        case CAN_FLT_02: C1RXF2EID = Eid; break;
        case CAN_FLT_03: C1RXF3EID = Eid; break;
        case CAN_FLT_04: C1RXF4EID = Eid; break;
        case CAN_FLT_05: C1RXF5EID = Eid; break;
        case CAN_FLT_06: C1RXF6EID = Eid; break;
        case CAN_FLT_07: C1RXF7EID = Eid; break;
        case CAN_FLT_08: C1RXF8EID = Eid; break;
        case CAN_FLT_09: C1RXF9EID = Eid; break;
        case CAN_FLT_10: C1RXF10EID = Eid; break;
        case CAN_FLT_11: C1RXF11EID = Eid; break;
        case CAN_FLT_12: C1RXF12EID = Eid; break;
        case CAN_FLT_13: C1RXF13EID = Eid; break;
        case CAN_FLT_14: C1RXF14EID = Eid; break;
        case CAN_FLT_15: C1RXF15EID = Eid; break;
        default         : return;
    }
}

void CCpu_CanSetFltMskSidReg(int FltMskNum, uInt2 SidMsk, BOOL bMide, uInt2 EidMsk)
{// CiRXMnSID: ECAN Acceptance Filter Mask Standard Identifier Register n (n = 0-2)
 // Program Acpt FltMsk Standard Identifier Register
    if (!CCpu_CanIsValidFltMskNum(FltMskNum))
        return;
    CCpu_CanSelFltWin();
    switch (FltMskNum) {
        case CAN_FLTMSK_0:
            C1RXM0SIDbits.SID = SidMsk;
            C1RXM0SIDbits.MIDE= bMide;
            C1RXM0SIDbits.EID = EidMsk;
            break;
        case CAN_FLTMSK_1: 
            C1RXM1SIDbits.SID = SidMsk;
            C1RXM1SIDbits.MIDE= bMide;
            C1RXM1SIDbits.EID = EidMsk;
            break;
        case CAN_FLTMSK_2: 
            C1RXM2SIDbits.SID = SidMsk;
            C1RXM2SIDbits.MIDE= bMide;
            C1RXM2SIDbits.EID = EidMsk;
            break;
        default           : return;
    }
}

void CCpu_CanSetFltMskEidReg(int FltMskNum, uInt2 EidMsk)
{// CiRXMnEID: ECAN Acceptance Filter Mask Extended Identifier Register n (n = 0-2)
 // Program Acpt Flt Extended Identifier Register
    if (!CCpu_CanIsValidFltMskNum(FltMskNum))
        return;
    CCpu_CanSelFltWin();
    switch (FltMskNum) {
        case CAN_FLTMSK_0: C1RXM0EID = EidMsk; break;
        case CAN_FLTMSK_1: C1RXM1EID = EidMsk; break;
        case CAN_FLTMSK_2: C1RXM2EID = EidMsk; break;
        default           : return;
    }
}

void CCpu_CanFltAtchMskNum(int FltNum, int FltMsk)
{// CiFMSKSEL1: ECAN Filter 7-0 Mask Selection Register
 // CiFMSKSEL2: ECAN Filter 15-8 Mask Selection Register
    if (!CCpu_CanIsValidFltNum(FltNum))
        return;
    CCpu_CanSelFltWin();
    switch (FltNum) {
    case CAN_FLT_00: C1FMSKSEL1bits.F0MSK = FltMsk;    break;
    case CAN_FLT_01: C1FMSKSEL1bits.F1MSK = FltMsk;    break;
    case CAN_FLT_02: C1FMSKSEL1bits.F2MSK = FltMsk;    break;
    case CAN_FLT_03: C1FMSKSEL1bits.F3MSK = FltMsk;    break;
    case CAN_FLT_04: C1FMSKSEL1bits.F4MSK = FltMsk;    break;
    case CAN_FLT_05: C1FMSKSEL1bits.F5MSK = FltMsk;    break;
    case CAN_FLT_06: C1FMSKSEL1bits.F6MSK = FltMsk;    break;
    case CAN_FLT_07: C1FMSKSEL1bits.F7MSK = FltMsk;    break;
    case CAN_FLT_08: C1FMSKSEL2bits.F8MSK = FltMsk;    break;
    case CAN_FLT_09: C1FMSKSEL2bits.F9MSK = FltMsk;    break;
    case CAN_FLT_10: C1FMSKSEL2bits.F10MSK = FltMsk;    break;
    case CAN_FLT_11: C1FMSKSEL2bits.F11MSK = FltMsk;    break;
    case CAN_FLT_12: C1FMSKSEL2bits.F12MSK = FltMsk;    break;
    case CAN_FLT_13: C1FMSKSEL2bits.F13MSK = FltMsk;    break;
    case CAN_FLT_14: C1FMSKSEL2bits.F14MSK = FltMsk;    break;
    case CAN_FLT_15: C1FMSKSEL2bits.F15MSK = FltMsk;    break;
    default         : return;
    }
}

void CCpu_CanFltAtchBufPtr(int FltNum, int BufNum)
{// CiBUFPNT1, CiBUFPNT2, CiBUFPN3, CiBUFPNT4
    if (!CCpu_CanIsValidFltNum(FltNum))
        return;
    CCpu_CanSelFltWin();
    switch (FltNum) {
        case CAN_FLT_00: C1BUFPNT1bits.F0BP = BufNum;  break;
        case CAN_FLT_01: C1BUFPNT1bits.F1BP = BufNum;  break;
        case CAN_FLT_02: C1BUFPNT1bits.F2BP = BufNum;  break;
        case CAN_FLT_03: C1BUFPNT1bits.F3BP = BufNum;  break;
        case CAN_FLT_04: C1BUFPNT2bits.F4BP = BufNum;  break;
        case CAN_FLT_05: C1BUFPNT2bits.F5BP = BufNum;  break;
        case CAN_FLT_06: C1BUFPNT2bits.F6BP = BufNum;  break;
        case CAN_FLT_07: C1BUFPNT2bits.F7BP = BufNum;  break;
        case CAN_FLT_08: C1BUFPNT3bits.F8BP = BufNum;  break;
        case CAN_FLT_09: C1BUFPNT3bits.F9BP = BufNum;  break;
        case CAN_FLT_10: C1BUFPNT3bits.F10BP = BufNum;  break;
        case CAN_FLT_11: C1BUFPNT3bits.F11BP = BufNum;  break;
        case CAN_FLT_12: C1BUFPNT4bits.F12BP = BufNum;  break;
        case CAN_FLT_13: C1BUFPNT4bits.F13BP = BufNum;  break;
        case CAN_FLT_14: C1BUFPNT4bits.F14BP = BufNum;  break;
        case CAN_FLT_15: C1BUFPNT4bits.F15BP = BufNum;  break;
        default         : return;
    }
}

int CCpu_CanGetFltBufPtr(int FltNum)
{// CiBUFPNT1, CiBUFPNT2, CiBUFPN3, CiBUFPNT4
    if (!CCpu_CanIsValidFltNum(FltNum))
        return -1;
    CCpu_CanSelFltWin();
    int BufNum = -1;
    switch (FltNum) {
        case CAN_FLT_00: BufNum = C1BUFPNT1bits.F0BP;  break;
        case CAN_FLT_01: BufNum = C1BUFPNT1bits.F1BP;  break;
        case CAN_FLT_02: BufNum = C1BUFPNT1bits.F2BP;  break;
        case CAN_FLT_03: BufNum = C1BUFPNT1bits.F3BP;  break;
        case CAN_FLT_04: BufNum = C1BUFPNT2bits.F4BP;  break;
        case CAN_FLT_05: BufNum = C1BUFPNT2bits.F5BP;  break;
        case CAN_FLT_06: BufNum = C1BUFPNT2bits.F6BP;  break;
        case CAN_FLT_07: BufNum = C1BUFPNT2bits.F7BP;  break;
        case CAN_FLT_08: BufNum = C1BUFPNT3bits.F8BP;  break;
        case CAN_FLT_09: BufNum = C1BUFPNT3bits.F9BP;  break;
        case CAN_FLT_10: BufNum = C1BUFPNT3bits.F10BP;  break;
        case CAN_FLT_11: BufNum = C1BUFPNT3bits.F11BP;  break;
        case CAN_FLT_12: BufNum = C1BUFPNT4bits.F12BP;  break;
        case CAN_FLT_13: BufNum = C1BUFPNT4bits.F13BP;  break;
        case CAN_FLT_14: BufNum = C1BUFPNT4bits.F14BP;  break;
        case CAN_FLT_15: BufNum = C1BUFPNT4bits.F15BP;  break;
        default         : BufNum = -1;  break;
    }
    CCpu_CanSelBufWin();
    return BufNum;
}

int CCpu_CanVecGetBufNum()
{
    int bufnum = C1VEC;
    bufnum &= 0x1F;
    return bufnum;
}

int CCpu_CanVecGetIntCode()
{
    int intvec = C1VEC;
    intvec &= 0x3F;
    return intvec;
}

int CCpu_CanVecGetFltNum()
{
    int fltnum = C1VEC;
    fltnum >>= 8;
    fltnum &= 0x1F;
    return fltnum;
}

void CCpu_CanClrBufFullFlag(int BufNum)
{
    if ((BufNum < 0) || (BufNum >= CAN_MAX_BUF))
        return;
    uInt2 msk = 0;
    if (BufNum < 16) {
         msk = ~(1 << BufNum);
         C1RXFUL1 &= msk;
    }
    else {
        msk = ~(1 << (BufNum-16));
        C1RXFUL2 &= msk;
    }
}

int CCpu_CanGetRxFullBufId()
{
    int i = 0;
    int msk = 0;
    for (i = 0; i < 16; i++) {
        msk = 1 << i;
        if (C1RXFUL1 & msk)
            return i;
        if (C1RXFUL2 & msk)
            return (16+i);
    }
    //
    return -1;
}

BOOL CCpu_CanIsRxFullBufPending()
{
    if ((C1RXFUL1 == 0) && (C1RXFUL2 == 0))
        return FALSE;
    return TRUE;
}

BOOL CCpu_CanIsBufFull(int BufNum)
{
    if ((BufNum < 0) || (BufNum >= CAN_MAX_BUF))
        return FALSE;
    uInt2 bufsts = 0;
    CCpu_CanSelBufWin();
    if (BufNum < 16) 
        bufsts = C1RXFUL1;
    else {
        bufsts = C1RXFUL2;
        BufNum -= 16;
    }

    return (bufsts & (1 << BufNum));
}

void CCpu_CanClrBufOvrFlowFlag(int BufNum)
{
    if ((BufNum < 0) || (BufNum >= CAN_MAX_BUF))
        return;
    uInt2 msk = 0;
    if (BufNum < 16) {
         msk = ~(1 << BufNum);
         C1RXOVF1 &= msk;
    }
    else {
        msk = ~(1 << (BufNum-16));
        C1RXOVF2 &= msk;
    }
}

BOOL CCpu_CanIsBufOvrFlow(int BufNum)
{
    if ((BufNum < 0) || (BufNum >= CAN_MAX_BUF))
        return FALSE;
    uInt2 bufsts = 0;
    CCpu_CanSelBufWin();
    if (BufNum < 16) 
        bufsts = C1RXOVF1;
    else {
        bufsts = C1RXOVF2;
        BufNum -= 16;
    }

    return (bufsts & (1 << BufNum));
}

void CCpu_CanFifoCtrl(int nBuf, int StBuf)
{// Fifo Buf = StBuf ~ nBuf
    if ((nBuf >= 7) || (StBuf >= 32))
        return;
    CCpu_CanSelBufWin();
    C1FCTRLbits.DMABS = nBuf;
    C1FCTRLbits.FSA = StBuf;
}

int CCpu_CanGetDatRxCurFifoBuf()
{// Recent received data buffer
    CCpu_CanSelBufWin();
    return C1FIFObits.FBP;
}

int CCpu_CanGetDatRxNxtFifoBuf()
{// Recent next data buffer
    CCpu_CanSelBufWin();
    return C1FIFObits.FNRB;
}

uInt2 CCpu_CanGetIntSts()
{
    CCpu_CanSelBufWin();
    gCpu.m_EcanIntSts = C1INTF;
    return gCpu.m_EcanIntSts;
}

void CCpu_CanBufTrxMode(int BufNum, BOOL bTrxMode, int Prio)
{
    CCpu_CanSelBufWin();
    switch (BufNum) {
    case CAN_BUF_0: C1TR01CONbits.TXEN0  = bTrxMode; C1TR01CONbits.TX0PRI = Prio; break;
    case CAN_BUF_1: C1TR01CONbits.TXEN1  = bTrxMode; C1TR01CONbits.TX1PRI = Prio; break;
    case CAN_BUF_2: C1TR23CONbits.TXEN2  = bTrxMode; C1TR23CONbits.TX2PRI = Prio; break;
    case CAN_BUF_3: C1TR23CONbits.TXEN3  = bTrxMode; C1TR23CONbits.TX3PRI = Prio; break;
    case CAN_BUF_4: C1TR45CONbits.TXEN4  = bTrxMode; C1TR45CONbits.TX4PRI = Prio; break;
    case CAN_BUF_5: C1TR45CONbits.TXEN5  = bTrxMode; C1TR45CONbits.TX5PRI = Prio; break;
    case CAN_BUF_6: C1TR67CONbits.TXEN6  = bTrxMode; C1TR67CONbits.TX6PRI = Prio; break;
    case CAN_BUF_7: C1TR67CONbits.TXEN7  = bTrxMode; C1TR67CONbits.TX7PRI = Prio; break;
    }
}

int CCpu_CanBufTxReq(int BufNum, BOOL bEnb)
{
    int sts = OK;
    switch (BufNum) {
    case CAN_BUF_0: C1TR01CONbits.TXREQ0  = bEnb; break;
    case CAN_BUF_1: C1TR01CONbits.TXREQ1  = bEnb; break;
    case CAN_BUF_2: C1TR23CONbits.TXREQ2  = bEnb; break;
    case CAN_BUF_3: C1TR23CONbits.TXREQ3  = bEnb; break;
    case CAN_BUF_4: C1TR45CONbits.TXREQ4  = bEnb; break;
    case CAN_BUF_5: C1TR45CONbits.TXREQ5  = bEnb; break;
    case CAN_BUF_6: C1TR67CONbits.TXREQ6  = bEnb; break;
    case CAN_BUF_7: C1TR67CONbits.TXREQ7  = bEnb; break;
    default        : sts = ERR;
    }
    return sts;
}

BOOL CCpu_CanIsTxBufTxReqOvr(int BufNum)
{
    uInt2 bflag = false;
    switch (BufNum) {
    case CAN_BUF_0: bflag = C1TR01CONbits.TXREQ0; break;
    case CAN_BUF_1: bflag = C1TR01CONbits.TXREQ1; break;
    case CAN_BUF_2: bflag = C1TR23CONbits.TXREQ2; break;
    case CAN_BUF_3: bflag = C1TR23CONbits.TXREQ3; break;
    case CAN_BUF_4: bflag = C1TR45CONbits.TXREQ4; break;
    case CAN_BUF_5: bflag = C1TR45CONbits.TXREQ5; break;
    case CAN_BUF_6: bflag = C1TR67CONbits.TXREQ6; break;
    case CAN_BUF_7: bflag = C1TR67CONbits.TXREQ7; break;
    default        : bflag = FALSE;
    }
    return (bflag == 0) ? TRUE : FALSE;
}

uInt1 CCpu_CanGetTxErrCnt()
{
    return C1ECbits.TERRCNT;
}

uInt1 CCpu_CanGetRxErrCnt()
{
    return C1ECbits.RERRCNT;
}

// ====================================================================
// CPU DMA
// ====================================================================
void CCpu_DmaConEnb(int Chn, BOOL bEnb)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0CONbits.CHEN = bEnb;    break;
    case DMA_CHN_1: DMA1CONbits.CHEN = bEnb;    break;
    case DMA_CHN_2: DMA2CONbits.CHEN = bEnb;    break;
    case DMA_CHN_3: DMA3CONbits.CHEN = bEnb;    break;
    default :   return;
    }
}

void CCpu_DmaConSetTrxSz(int Chn, int TrxSz)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0CONbits.SIZE = TrxSz;    break;
    case DMA_CHN_1: DMA1CONbits.SIZE = TrxSz;    break;
    case DMA_CHN_2: DMA2CONbits.SIZE = TrxSz;    break;
    case DMA_CHN_3: DMA3CONbits.SIZE = TrxSz;    break;
    default :   return;
    }
}

void CCpu_DmaConSetTrxDrn(int Chn, int TrxDrn)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0CONbits.DIR = TrxDrn;    break;
    case DMA_CHN_1: DMA1CONbits.DIR = TrxDrn;    break;
    case DMA_CHN_2: DMA2CONbits.DIR = TrxDrn;    break;
    case DMA_CHN_3: DMA3CONbits.DIR = TrxDrn;    break;
    default :   return;
    }
}

void CCpu_DmaConSetIntMode(int Chn, int IntMode)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0CONbits.HALF = IntMode; break;
    case DMA_CHN_1: DMA1CONbits.HALF = IntMode; break;
    case DMA_CHN_2: DMA2CONbits.HALF = IntMode; break;
    case DMA_CHN_3: DMA3CONbits.HALF = IntMode; break;
    default :   return;
    }
}

void CCpu_DmaConSetWrMode(int Chn, int WrMode)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0CONbits.NULLW = WrMode; break;
    case DMA_CHN_1: DMA1CONbits.NULLW = WrMode; break;
    case DMA_CHN_2: DMA2CONbits.NULLW = WrMode; break;
    case DMA_CHN_3: DMA3CONbits.NULLW = WrMode; break;
    default :   return;
    }
}

void CCpu_DmaConSetAdrMode(int Chn, int AdrMode)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0CONbits.AMODE = AdrMode;    break;
    case DMA_CHN_1: DMA1CONbits.AMODE = AdrMode;    break;
    case DMA_CHN_2: DMA2CONbits.AMODE = AdrMode;    break;
    case DMA_CHN_3: DMA3CONbits.AMODE = AdrMode;    break;
    default :   return;
    }
}

void CCpu_DmaConSetOpMode(int Chn, int OpMode)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0CONbits.MODE = OpMode;  break;
    case DMA_CHN_1: DMA1CONbits.MODE = OpMode;  break;
    case DMA_CHN_2: DMA2CONbits.MODE = OpMode;  break;
    case DMA_CHN_3: DMA3CONbits.MODE = OpMode;  break;
    default :   return;
    }
}

void CCpu_DmaReqSetTrxMode(int Chn, int TrxMode)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0REQbits.FORCE = TrxMode;     break;
    case DMA_CHN_1: DMA1REQbits.FORCE = TrxMode;     break;
    case DMA_CHN_2: DMA2REQbits.FORCE = TrxMode;     break;
    case DMA_CHN_3: DMA3REQbits.FORCE = TrxMode;     break;
    default :   return;
    }
}

void CCpu_DmaReqSetIrq(int Chn, int Irq)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0REQbits.IRQSEL = Irq;   break;
    case DMA_CHN_1: DMA1REQbits.IRQSEL = Irq;   break;
    case DMA_CHN_2: DMA2REQbits.IRQSEL = Irq;   break;
    case DMA_CHN_3: DMA3REQbits.IRQSEL = Irq;   break;
    default :   return;
    }
}

void CCpu_DmaSetStartAdrA_(int Chn, unsigned int pAdrL, unsigned int pAdrH)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0STAH = pAdrH; DMA0STAL = pAdrL; break;
    case DMA_CHN_1: DMA1STAH = pAdrH; DMA1STAL = pAdrL; break;
    case DMA_CHN_2: DMA2STAH = pAdrH; DMA2STAL = pAdrL; break;
    case DMA_CHN_3: DMA3STAH = pAdrH; DMA3STAL = pAdrL; break;
    default :   return;
    }
}

void CCpu_DmaSetStartAdrA(int Chn, unsigned int* pAdr)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0STAH = 0x0000; DMA0STAL = (unsigned int) pAdr; break;
    case DMA_CHN_1: DMA1STAH = 0x0000; DMA1STAL = (unsigned int) pAdr; break;
    case DMA_CHN_2: DMA2STAH = 0x0000; DMA2STAL = (unsigned int) pAdr; break;
    case DMA_CHN_3: DMA3STAH = 0x0000; DMA3STAL = (unsigned int) pAdr; break;
    default :   return;
    }
}

void CCpu_DmaSetStartAdrB(int Chn, uInt2* pAdr)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0STBH = 0x0000; DMA0STBL = (unsigned int)pAdr; break;
    case DMA_CHN_1: DMA1STBH = 0x0000; DMA1STBL = (unsigned int)pAdr; break;
    case DMA_CHN_2: DMA2STBH = 0x0000; DMA2STBL = (unsigned int)pAdr; break;
    case DMA_CHN_3: DMA3STBH = 0x0000; DMA3STBL = (unsigned int)pAdr; break;
    default :   return;
    }
}

void CCpu_DmaSetPerAdr(int Chn, volatile unsigned int* pAdr)
{
    switch (Chn) {
    case DMA_CHN_0: DMA0PAD = (unsigned int)pAdr; break;
    case DMA_CHN_1: DMA1PAD = (unsigned int)pAdr; break;
    case DMA_CHN_2: DMA2PAD = (unsigned int)pAdr; break;
    case DMA_CHN_3: DMA3PAD = (unsigned int)pAdr; break;
    default :   return;
    }
}

void CCpu_DmaCntSetTrxCnt(int Chn, int TrxCnt)
{
    TrxCnt &= 0x3FFF;
    switch (Chn) {
    case DMA_CHN_0: DMA0CNT = TrxCnt; break;
    case DMA_CHN_1: DMA1CNT = TrxCnt; break;
    case DMA_CHN_2: DMA2CNT = TrxCnt; break;
    case DMA_CHN_3: DMA3CNT = TrxCnt; break;
    default :   return;
    }
}

void CCpu_DmaSetIsrMsk(int Chn, BOOL bFlag)
{
    uInt2 msk = 0;
    switch (Chn) {
    case DMA_CHN_0: msk = CPU_ISR_DMA0; break;
    case DMA_CHN_1: msk = CPU_ISR_DMA1; break;
    case DMA_CHN_2: msk = CPU_ISR_DMA2; break;
    case DMA_CHN_3: msk = CPU_ISR_DMA3; break;
    default :   return;
    }
    
    if (bFlag) 
         CPU_SET_ISR(msk);
    else CPU_CLR_ISR(msk);
}

// ====================================================================
// CPU SPI
// ====================================================================
void CCpu_SpiReset(int ChnId)
{
    switch (ChnId) {
        case CPU_SPI_CHN1: 
            SPI1BUF = 0;
            SPI1STATbits.SPIROV = 0;
            SPI1STATbits.SPITBF = 0;
            SPI1STATbits.SPIRBF = 0;
            break;
        case CPU_SPI_CHN2: 
            SPI2BUF = 0;
            SPI2STATbits.SPIROV = 0;
            SPI2STATbits.SPITBF = 0;
            SPI2STATbits.SPIRBF = 0;
            break;
    }
}

void CCpu_SpiSetIsrMsk(int ChnId)
{
    switch (ChnId) {
        case CPU_SPI_CHN1: CPU_SET_ISR(CPU_ISR_SPI1);   break;
        case CPU_SPI_CHN2: CPU_SET_ISR(CPU_ISR_SPI2);   break;
    }
}

void CCpu_SpiMode(int ChnId, int Mode)
{    
    if (Mode == SPI_MST_MODE) {
        switch (ChnId) {
            case CPU_SPI_CHN1: 
                SPI1CON1bits.MSTEN = 1;
                SPI1CON1bits.DISSCK = 0;
                SPI1CON1bits.DISSDO = 0;
                break;
            case CPU_SPI_CHN2: 
                SPI2CON1bits.MSTEN = 1;
                SPI2CON1bits.DISSCK = 0;
                SPI2CON1bits.DISSDO = 0;
                break;
        }
    }
    else {
        switch (ChnId) {
            case CPU_SPI_CHN1: 
                SPI1CON1bits.MSTEN = 0;
                break;
            case CPU_SPI_CHN2: 
                SPI2CON1bits.MSTEN = 0;
                break;
        }
    }
}

void CCpu_SpiDatSz(int ChnId, int DatSz)
{
    switch (ChnId) {
        case CPU_SPI_CHN1: 
            if (DatSz == SPI_DAT_WORD)
                 SPI1CON1bits.MODE16 = 1;
            else SPI1CON1bits.MODE16 = 0;
            break;
        case CPU_SPI_CHN2: 
            if (DatSz == SPI_DAT_WORD)
                 SPI2CON1bits.MODE16 = 1;
            else SPI2CON1bits.MODE16 = 0;
            break;
    }
}

void CCpu_SpiSmpAt(int ChnId, int SmpMode)
{
    switch (ChnId) {
        case CPU_SPI_CHN1: 
            if (SmpMode == SPI_SMP_END)
                 SPI1CON1bits.SMP = 1;
            else SPI1CON1bits.SMP = 0;
            break;
        case CPU_SPI_CHN2: 
            if (SmpMode == SPI_SMP_END)
                 SPI2CON1bits.SMP = 1;
            else SPI2CON1bits.SMP = 0;
            break;
    }
}

// Clock Edge Active-Low: Mode-0, Mode-1
// Clock Edge Active-High: Mode-2, Mode-3
// Mode-0: Data sampled on rising edge, shifted out on falling edge
// Mode-1: Data sampled on falling edge, shifted out on rising edge
// Mode-2: Data sampled on falling edge, shifted out on rising edge
// Mode-3: Data sampled on rising edge, shifted out on falling edge

void CCpu_SpiProtoMode(int ChnId, int ProtoMode)
{
    switch (ChnId) {
        case CPU_SPI_CHN1: 
            switch (ProtoMode) {
                case SPI_PROTO_MODE_0: SPI1CON1bits.CKP = 0; SPI1CON1bits.CKE = 1; break;
                case SPI_PROTO_MODE_1: SPI1CON1bits.CKP = 0; SPI1CON1bits.CKE = 0; break;
                case SPI_PROTO_MODE_2: SPI1CON1bits.CKP = 1; SPI1CON1bits.CKE = 1; break;
                case SPI_PROTO_MODE_3:
                default              : SPI1CON1bits.CKP = 1; SPI1CON1bits.CKE = 0; break;
            }
            break;
        case CPU_SPI_CHN2: 
            switch (ProtoMode) {
                case SPI_PROTO_MODE_0: SPI2CON1bits.CKP = 0; SPI2CON1bits.CKE = 1; break;
                case SPI_PROTO_MODE_1: SPI2CON1bits.CKP = 0; SPI2CON1bits.CKE = 0; break;
                case SPI_PROTO_MODE_2: SPI2CON1bits.CKP = 1; SPI2CON1bits.CKE = 1; break;
                case SPI_PROTO_MODE_3:
                default              : SPI2CON1bits.CKP = 1; SPI2CON1bits.CKE = 0; break;
            }
            break;
    }
    
}

void CCpu_SpiEnb(int ChnId)
{
    switch (ChnId) {
        case CPU_SPI_CHN1: 
            SPI1STATbits.SPIEN = 1;
            break;
        case CPU_SPI_CHN2: 
            SPI2STATbits.SPIEN = 1;
            break;
    }
}

void CCpu_SpiSetFrq(int ChnId, int Frq)
{
    int priscl, secscl;
    switch (Frq) {
    case SPI_FRQ_20MHz  : 
        priscl = SPI_PRI_PRESCL_1_1;
        secscl = SPI_SEC_PRESCL_2_1;
        break;
    case SPI_FRQ_5MHz   :
        priscl = SPI_PRI_PRESCL_1_1;
        secscl = SPI_SEC_PRESCL_8_1;
        break;
    case SPI_FRQ_2p5MHz :
        priscl = SPI_PRI_PRESCL_16_1;
        secscl = SPI_SEC_PRESCL_1_1;
        break;
    case SPI_FRQ_1p25MHz:
        priscl = SPI_PRI_PRESCL_4_1;
        secscl = SPI_SEC_PRESCL_8_1;
        break;
    case SPI_FRQ_10MHz  :
    default             :
        priscl = SPI_PRI_PRESCL_1_1;
        secscl = SPI_SEC_PRESCL_4_1;
        break;
    }
    
    switch (ChnId) {
        case CPU_SPI_CHN1: 
            SPI1CON1bits.PPRE = priscl;
            SPI1CON1bits.SPRE = secscl;
            break;
        case CPU_SPI_CHN2: 
            SPI2CON1bits.PPRE = priscl;
            SPI2CON1bits.SPRE = secscl;
            break;
    }
}

void CCpu_SpiTxd(int ChnId, Byte Ch)
{
    switch (ChnId) {
        case CPU_SPI_CHN1: SPI1BUF = Ch;    break;
        case CPU_SPI_CHN2: SPI2BUF = Ch;    break;
    }
}

// ====================================================================
// CPU I2C
// ====================================================================
void CCpu_I2c1Init(int Frq)
{
    ODCCbits.ODCC4 = 0;     // Enable open drain
    ODCCbits.ODCC5 = 0;     // Enable open drain

    I2C1CONbits.I2CSIDL = 0;    // operational in idle mode
    I2C1CONbits.A10M    = 0;       // 7-bit dev address
    I2C1CONbits.SCLREL  = 1;
    if (Frq == I2C_FRQ_100KHz)
         I2C1BRG = BAUD_100KHz;
    else I2C1BRG = BAUD_400KHz;
    I2C1ADD = 0;
    I2C1MSK = 0;

    I2C1CONbits.I2CEN = 1;  // Enable I2C
    IFS1bits.MI2C1IF = 0;   // Clear ISR flag
    //
    CPU_SET_ISR(CPU_ISR_I2C1);
}

void CCpu_I2c1Start()
{
    I2C1CONbits.SEN = 1;
}

void CCpu_I2c1Restart()
{
    I2C1CONbits.RSEN = 1;
}

void CCpu_I2c1Stop()
{
    I2C1CONbits.PEN = 1;
}

void CCpu_I2c1Txd(Byte Ch)
{
    I2C1TRN = Ch;
}

void CCpu_I2c1RxEnb()
{
    I2C1CONbits.RCEN = 1;
}

Byte CCpu_I2c1Rxd()
{
    return I2C1RCV & 0x00FF;
}

void CCpu_I2c1TxAck(BOOL bAck)
{
    I2C1CONbits.ACKEN = 1;
    if (bAck)
         I2C1CONbits.ACKDT = 1;
    else I2C1CONbits.ACKDT = 0;
}

BOOL CCpu_I2c1IsDatRdy()
{
    return I2C1STATbits.RBF;
}

BOOL CCpu_I2c1RdAck()
{
    return I2C1STATbits.ACKSTAT;
}

BOOL CCpu_I2c1IsTxBusy()
{
    return I2C1STATbits.TRSTAT;
}


// ====================================================================
// CPU GPIO
// ====================================================================

void CCpu_InitPPinA(int Pin, int Drn)
{
    switch(Pin){
    case PIN00:
        TRISAbits.TRISA0 = Drn;
        ANSELAbits.ANSA0 = 0;
        break;
    case PIN01:
        TRISAbits.TRISA1= Drn;
        ANSELAbits.ANSA1= 0;
        break;
    case PIN04:
        TRISAbits.TRISA4 = Drn;
        ANSELAbits.ANSA4 = 0;
        break;
    case PIN07:
        TRISAbits.TRISA7 = Drn;
        break;
    case PIN08:
        TRISAbits.TRISA8 = Drn;
        break;
    case PIN09:
        TRISAbits.TRISA9 = Drn;
        break;
    case PIN10:
        TRISAbits.TRISA10 = Drn;
        break;
    case PIN11:
        TRISAbits.TRISA11 = Drn;
        ANSELAbits.ANSA11 = 0;
        break;
    case PIN12:
        TRISAbits.TRISA12 = Drn;
        ANSELAbits.ANSA12 = 0;
        break;
    }
}

void CCpu_InitPPinB(int Pin, int Drn)
{
    switch(Pin){
    case PIN00:
        TRISBbits.TRISB0 = Drn;
        ANSELBbits.ANSB0 = 0;
        break;
    case PIN01:
        TRISBbits.TRISB1 = Drn;
        ANSELBbits.ANSB1 = 0;
        break;
    case PIN02:
        TRISBbits.TRISB2 = Drn;
        ANSELBbits.ANSB2 = 0;
        break;
    case PIN03:
        TRISBbits.TRISB3 = Drn;
        ANSELBbits.ANSB3 = 0;
        break;
    case PIN04:
        TRISBbits.TRISB4 = Drn;
        break;
    case PIN14:
        TRISBbits.TRISB14 = Drn;
        break;
    case PIN15:
        TRISBbits.TRISB15 = Drn;
        break;
    case PIN08:
        TRISBbits.TRISB8 = Drn;
        ANSELBbits.ANSB8 = 0;
        break;
    case PIN09:
        TRISBbits.TRISB9 = Drn;
        break;
    case PIN10:
        TRISBbits.TRISB10 = Drn;
        break;
   }
}

void CCpu_InitPPinC(int Pin, int Drn)
{
    switch(Pin){
    case PIN00:
        TRISCbits.TRISC0 = Drn;
        ANSELCbits.ANSC0 = 0;
        break;
    case PIN01:
        TRISCbits.TRISC1 = Drn;
        ANSELCbits.ANSC1 = 0;
        break;
    case PIN02:
        TRISCbits.TRISC2 = Drn;
        ANSELCbits.ANSC2 = 0;
        break;
    case PIN03:
        TRISCbits.TRISC3 = Drn;
        break;
    case PIN06:
        TRISCbits.TRISC6 = Drn;
        break;
    case PIN07:
        TRISCbits.TRISC7 = Drn;
        break;
    case PIN08:
        TRISCbits.TRISC8 = Drn;
        break;
    case PIN09:
        TRISCbits.TRISC9 = Drn;
        break;
    case PIN11:
        TRISCbits.TRISC11 = Drn;
        ANSELCbits.ANSC11 = 0;
        break;
    case PIN13:
        TRISCbits.TRISC13 = Drn;
        break;
    case PIN12:
        TRISCbits.TRISC12 = Drn;
        break;
    case PIN15:
        TRISCbits.TRISC15 = Drn;
        break;
    }
}

void CCpu_InitPPinD(int Pin, int Drn)
{
    switch (Pin) {
    case PIN05: TRISDbits.TRISD5 = Drn; break;
    case PIN06: TRISDbits.TRISD6 = Drn; break;
    case PIN08: TRISDbits.TRISD8 = Drn; break;
    }
}

void CCpu_InitPPinE(int Pin, int Drn)
{
    switch(Pin){
    case PIN12:
        TRISEbits.TRISE12 = Drn;
        ANSELEbits.ANSE12 = 0;
        break;
    case PIN13:
        TRISEbits.TRISE13 = Drn;
        ANSELEbits.ANSE13 = 0;
        break;
    case PIN14:
        TRISEbits.TRISE14 = Drn;
        ANSELEbits.ANSE14 = 0;
        break;
    case PIN15:
        TRISEbits.TRISE15 = Drn;
        ANSELEbits.ANSE15 = 0;
        break;
    }
}

void CCpu_InitPPinF(int Pin, int Drn)
{
    switch(Pin){
    case PIN00: TRISFbits.TRISF0 = Drn; break;
    case PIN01: TRISFbits.TRISF1 = Drn; break;
    }
}

// Set Ppi
// =======
void CCpu_SetPPinAVal(int Pin, Bool Val)
{
   switch (Pin) {
   case PIN00: LATAbits.LATA0 = Val; break;
   case PIN01: LATAbits.LATA1 = Val; break;
   case PIN04: LATAbits.LATA4 = Val; break;
   case PIN07: LATAbits.LATA7 = Val; break;
   case PIN08: LATAbits.LATA8 = Val; break;
   case PIN09: LATAbits.LATA9 = Val; break;
   case PIN10: LATAbits.LATA10 = Val; break;
   case PIN11: LATAbits.LATA11 = Val; break;
   case PIN12: LATAbits.LATA12 = Val; break;
   }
}

void CCpu_SetPPinBVal(int Pin, Bool Val)
{
   switch (Pin) {
   case PIN00: LATBbits.LATB0 = Val; break;
   case PIN01: LATBbits.LATB1 = Val; break;
   case PIN02: LATBbits.LATB2 = Val; break;
   case PIN03: LATBbits.LATB3 = Val; break;
   case PIN04: LATBbits.LATB4 = Val; break;
   case PIN08: LATBbits.LATB8 = Val; break;
   case PIN14: LATBbits.LATB14 = Val; break;
   case PIN15: LATBbits.LATB15 = Val; break;
   case PIN09: LATBbits.LATB9 = Val; break;
   case PIN10: LATBbits.LATB10 = Val; break;
   }
}

void CCpu_SetPPinCVal(int Pin, Bool Val)
{
    switch (Pin) {
    case PIN00: LATCbits.LATC0 = Val; break;
    case PIN01: LATCbits.LATC1 = Val; break;
    case PIN02: LATCbits.LATC2 = Val; break;
    case PIN03: LATCbits.LATC3 = Val; break;
    case PIN06: LATCbits.LATC6 = Val; break;
    case PIN07: LATCbits.LATC7 = Val; break;
    case PIN08: LATCbits.LATC8 = Val; break;
    case PIN09: LATCbits.LATC9 = Val; break;
    case PIN11: LATCbits.LATC11 = Val; break;
    case PIN12: LATCbits.LATC12 = Val; break;
    case PIN13: LATCbits.LATC13 = Val; break;
    case PIN15: LATCbits.LATC15 = Val; break;
    }
}

void CCpu_SetPPinDVal(int Pin, Bool Val)
{
    switch (Pin) {
    case PIN05: LATDbits.LATD5 = Val; break;
    case PIN06: LATDbits.LATD6 = Val; break;
    case PIN08: LATDbits.LATD8 = Val; break;
    }
}

void CCpu_SetPPinEVal(int Pin, Bool Val)
{
    switch (Pin) {
    case PIN12: LATEbits.LATE12 = Val; break;
    case PIN13: LATEbits.LATE13 = Val; break;
    case PIN14: LATEbits.LATE14 = Val; break;
    case PIN15: LATEbits.LATE15 = Val; break;
    }
}

void CCpu_SetPPinFVal(int Pin, Bool Val)
{
    switch (Pin) {
    case PIN00: LATFbits.LATF0 = Val; break;
    case PIN01: LATFbits.LATF1 = Val; break;
    }
}

// Get Ppi
// =======
Bool CCpu_GetPPinAVal(int Pin)
{
    Bool b = FALSE;
    switch (Pin) {
    case PIN00: b = PORTAbits.RA0;  break;
    case PIN01: b = PORTAbits.RA1;  break;
    case PIN04: b = PORTAbits.RA4;  break;
    case PIN07: b = PORTAbits.RA7;  break;
    case PIN08: b = PORTAbits.RA8;  break;
    case PIN09: b = PORTAbits.RA9;  break;
    case PIN10: b = PORTAbits.RA10; break;
    case PIN11: b = PORTAbits.RA11; break;
    case PIN12: b = PORTAbits.RA12; break;
    }
    return b;
}

Bool CCpu_GetPPinBVal(int Pin)
{
    Bool b = FALSE;
    switch (Pin) {
    case PIN00: b = PORTBbits.RB0;  break;
    case PIN01: b = PORTBbits.RB1;  break;
    case PIN02: b = PORTBbits.RB2;  break;
    case PIN03: b = PORTBbits.RB3;  break;
    case PIN04: b = PORTBbits.RB4;  break;
    case PIN08: b = PORTBbits.RB8;  break;
    case PIN14: b = PORTBbits.RB14; break;
    case PIN15: b = PORTBbits.RB15; break;
    case PIN09: b = PORTBbits.RB9;  break;
    case PIN10: b = PORTBbits.RB10; break;
    }
    return b;
}

Bool CCpu_GetPPinCVal(int Pin)
{
    Bool b = FALSE;
    switch (Pin) {
    case PIN00: b = PORTCbits.RC0;  break;
    case PIN01: b = PORTCbits.RC1;  break;
    case PIN02: b = PORTCbits.RC2;  break;
    case PIN03: b = PORTCbits.RC3;  break;
    case PIN06: b = PORTCbits.RC6;  break;
    case PIN07: b = PORTCbits.RC7;  break;
    case PIN08: b = PORTCbits.RC8;  break;
    case PIN09: b = PORTCbits.RC9;  break;
    case PIN11: b = PORTCbits.RC11; break;
    case PIN12: b = PORTCbits.RC12; break;
    case PIN13: b = PORTCbits.RC13; break;
    case PIN15: b = PORTCbits.RC15; break;
    }
    return b;
}

Bool CCpu_GetPPinDVal(int Pin)
{
    Bool b = FALSE;
    switch (Pin) {
    case PIN05: b = PORTDbits.RD5; break;
    case PIN06: b = PORTDbits.RD6; break;
    case PIN08: b = PORTDbits.RD8; break;
    }
    return b;
}


Bool CCpu_GetPPinEVal(int Pin)
{
    Bool b = FALSE;
    switch (Pin) {
    case PIN12: b = PORTEbits.RE12; break;
    case PIN13: b = PORTEbits.RE13; break;
    case PIN14: b = PORTEbits.RE14; break;
    case PIN15: b = PORTEbits.RE15; break;
    }
    return b;
}

Bool CCpu_GetPPinFVal(int Pin) {
    Bool b = FALSE;
    switch (Pin){
    case PIN00: b = PORTFbits.RF0 ; break;
    case PIN01: b = PORTFbits.RF1 ; break;
    }
    return b;
}

// Tgl Ppi
// =======
void CCpu_TglPPinAVal(int Pin)
{
   switch (Pin) {
   case PIN00: LATAbits.LATA0 = ~LATAbits.LATA0; break;
   case PIN01: LATAbits.LATA1 = ~LATAbits.LATA1; break;
   case PIN04: LATAbits.LATA4 = ~LATAbits.LATA4; break;
   case PIN07: LATAbits.LATA7 = ~LATAbits.LATA7; break;
   case PIN08: LATAbits.LATA8 = ~LATAbits.LATA8; break;
   case PIN09: LATAbits.LATA9 = ~LATAbits.LATA9; break;
   case PIN10: LATAbits.LATA10 = ~LATAbits.LATA10; break;
   case PIN11: LATAbits.LATA11 = ~LATAbits.LATA11; break;
   case PIN12: LATAbits.LATA12 = ~LATAbits.LATA12; break;
   }
}

void CCpu_TglPPinBVal(int Pin)
{
   switch (Pin) {
   case PIN00: LATBbits.LATB0 = ~LATBbits.LATB0; break;
   case PIN01: LATBbits.LATB1 = ~LATBbits.LATB1; break;
   case PIN02: LATBbits.LATB2 = ~LATBbits.LATB2; break;
   case PIN03: LATBbits.LATB3 = ~LATBbits.LATB3; break;
   case PIN04: LATBbits.LATB4 = ~LATBbits.LATB4; break;
   case PIN08: LATBbits.LATB8 = ~LATBbits.LATB8; break;
   case PIN14: LATBbits.LATB14 = ~LATBbits.LATB14; break;
   case PIN15: LATBbits.LATB15 = ~LATBbits.LATB15; break;
   case PIN09: LATBbits.LATB9 = ~LATBbits.LATB9; break;
   case PIN10: LATBbits.LATB10 = ~LATBbits.LATB10; break;
   }
}

void CCpu_TglPPinCVal(int Pin)
{
   switch (Pin) {
   case PIN00: LATCbits.LATC0 = ~LATCbits.LATC0; break;
   case PIN01: LATCbits.LATC1 = ~LATCbits.LATC1; break;
   case PIN02: LATCbits.LATC2 = ~LATCbits.LATC2; break;
   case PIN03: LATCbits.LATC3 = ~LATCbits.LATC3; break;
   case PIN06: LATCbits.LATC6 = ~LATCbits.LATC6; break;
   case PIN07: LATCbits.LATC7 = ~LATCbits.LATC7; break;
   case PIN08: LATCbits.LATC8 = ~LATCbits.LATC8; break;
   case PIN09: LATCbits.LATC9 = ~LATCbits.LATC9; break;
   case PIN11: LATCbits.LATC11 = ~LATCbits.LATC11; break;
   case PIN12: LATCbits.LATC12 = ~LATCbits.LATC12; break;
   case PIN13: LATCbits.LATC13 = ~LATCbits.LATC13; break;
   case PIN15: LATCbits.LATC15 = ~LATCbits.LATC15; break;
    }
}

void CCpu_TglPPinDVal(int Pin)
{
    switch (Pin) {
    case PIN05: LATDbits.LATD5 = ~LATDbits.LATD5; break;
    case PIN06: LATDbits.LATD6 = ~LATDbits.LATD6; break;
    case PIN08: LATDbits.LATD8 = ~LATDbits.LATD8; break;
    }
}


void CCpu_TglPPinEVal(int Pin)
{
   switch (Pin) {
   case PIN12: LATEbits.LATE12 = ~LATEbits.LATE12; break;
   case PIN13: LATEbits.LATE13 = ~LATEbits.LATE13; break;
   case PIN14: LATEbits.LATE14 = ~LATEbits.LATE14; break;
   case PIN15: LATEbits.LATE15 = ~LATEbits.LATE15; break;
    }
}

void CCpu_TglPPinFVal(int Pin)
{
   switch (Pin) {
   case PIN00: LATFbits.LATF0 = ~LATFbits.LATF0; break;
   case PIN01: LATFbits.LATF1 = ~LATFbits.LATF1; break;
   }
}

// ====================================================================
// System ISR Calls
// ====================================================================
// Tmr1 ISR
// ========
void _ISR_PSV _T1Interrupt()
{
    CSys_OnIsrTck();
    IFS0bits.T1IF = 0; //Clear Timer1 interrupt flag
}

// UART1 ISR
// =========
void _ISR_PSV _U1RXInterrupt(void)
{
    if ((!U1STAbits.FERR) && (!U1STAbits.OERR)) {
        Byte ch = U1RXREG;
        // Hanover C^ to System
        CSys_Rs232ChnOnIsrRxC(CPU_RS232_CHN1, ch);
    }
    else {
        if (U1STAbits.OERR)
            U1STAbits.OERR = 0;
    }
    IFS0bits.U1RXIF = 0;
}

void _ISR_PSV _U1TXInterrupt(void)
{
    // Hanover C^ to System
    CSys_Rs232ChnOnIsrTxEmpty(CPU_RS232_CHN1);
    IFS0bits.U1TXIF = 0;
}

// UART2 ISR
// =========
void _ISR_PSV _U2RXInterrupt(void)
{
    if ((!U2STAbits.FERR) && (!U2STAbits.OERR)) {
        Byte ch = U2RXREG;
        // Hanover C^ to System
        CSys_Rs232ChnOnIsrRxC(CPU_RS232_CHN2, ch);
    }
    else {
        if (U2STAbits.OERR)
            U2STAbits.OERR = 0;
    }
    IFS1bits.U2RXIF = 0;
}

void _ISR_PSV _U2TXInterrupt(void)
{
    // Hanover C^ to System
    CSys_Rs232ChnOnIsrTxEmpty(CPU_RS232_CHN2);
    IFS1bits.U2TXIF = 0;
}

// CAN ISR
// =======
BOOL _C1Interrupt_IsRxFullBufPending()
{
    int i = 0;
    for (i = ESYS_ECAN_FIFO_STBUF; i < ESYS_ECAN_N_BUF; i++) {
        if (CCpu_CanIsBufFull(i))
            return TRUE;
    }
    return FALSE;
}

void _C1Interrupt_RdBuf()
{
    int i = 0;
    int bufnum;
    for (i = 0; i < ESYS_ECAN_N_RXBUF; i++) {
        bufnum = CCpu_CanGetDatRxNxtFifoBuf();
        if (!CCpu_CanIsBufFull(bufnum))
            return;
        CSys_CanDevOnIsrRx(bufnum);
        CCpu_CanClrBufFullFlag(bufnum);
    }
}

void _C1Interrupt_RdBufOpt1(BOOL bChkRxFull)
{
    int i = 0;
    int bufnum;
    for (i = 0; i < ESYS_ECAN_N_RXBUF; i++) {
        if ((bChkRxFull) && (!_C1Interrupt_IsRxFullBufPending()))
            break;
        bufnum = CCpu_CanGetDatRxNxtFifoBuf();
        CSys_CanDevOnIsrRx(bufnum);
        CCpu_CanClrBufFullFlag(bufnum);
    }
}

void _C1Interrupt_RdBufX()
{
    int bufnum;
    while (TRUE) {
        if (!_C1Interrupt_IsRxFullBufPending())
            return;
        bufnum = CCpu_CanGetDatRxNxtFifoBuf();
        CSys_CanDevOnIsrRx(bufnum);
        CCpu_CanClrBufFullFlag(bufnum);
    }
}

void _ISR_NPSV _C1Interrupt(void)
{
    if(C1INTFbits.TBIF) {
        C1INTFbits.TBIF = 0;
        DbgVal1++;
        // Hanover C^ to System
        CSys_CanDevOnIsrTx();
    }
    if(C1INTFbits.RBIF) {
        _C1Interrupt_RdBuf();
        //_C1Interrupt_RdBuf(TRUE);
        C1INTFbits.RBIF = 0;
    }
    
    if (C1INTFbits.FIFOIF) {
        _C1Interrupt_RdBuf();
        //_C1Interrupt_RdBuf(FALSE);
        C1INTFbits.FIFOIF = 0;
    }
    
    if (C1INTFbits.RBOVIF) {// Receive buffer overflow
        C1RXOVF1 = 0;
        C1RXOVF2 = 0;
        // Hanover C^ to System
        CSys_CanDevOnIsrErr(CAN_ISR_RBOV);
        C1INTFbits.RBOVIF = 0;
    }

    if(C1INTFbits.IVRIF) {// Invalid message
        // Hanover C^ to System
        CSys_CanDevOnIsrErr(CAN_ISR_IVR);
        C1INTFbits.IVRIF = 0;
    }

    if(C1INTFbits.ERRIF) {// Error interrupt
        if (C1INTFbits.TXBO)
            C1INTFbits.TXBO = 0;
        if (C1INTFbits.TXBP)
            C1INTFbits.TXBP = 0;
        if (C1INTFbits.RXBP)
            C1INTFbits.RXBP = 0;
        if (C1INTFbits.TXWAR)
            C1INTFbits.TXWAR = 0;
        if (C1INTFbits.RXWAR)
            C1INTFbits.RXWAR = 0;
        if (C1INTFbits.EWARN)
            C1INTFbits.EWARN = 0;
        // Hanover C^ to System
        CSys_CanDevOnIsrErr(CAN_ISR_ERRI);
        C1INTFbits.ERRIF = 0;
    }

    IFS2bits.C1IF = 0;        // clear interrupt flag
}

// DMA ISR
// =======
void _ISR_NPSV _DMA0Interrupt(void)
{
    IFS0bits.DMA0IF = 0;          // Clear the DMA0 Interrupt Flag;
}

void _ISR_NPSV _DMA1Interrupt(void)
{
    IFS0bits.DMA1IF = 0;          // Clear the DMA1 Interrupt Flag;
}

void _ISR_NPSV _DMACError(void)
{
    //static unsigned int ErrorLocation;
    // Peripheral Write Collision Error Location
    if(DMAPWC & 0x0001)
    {
        TstFlag |= 0x80;
    // ErrorLocation = DMA0STA;
    }
    // DMA RAM Write Collision Error Location
    if(DMARQC & 0x0002)
    {
        TstFlag |= 0x40;
     //ErrorLocation = DMA1STA;
    }
    INTCON1bits.DMACERR = 0;
}

// SPI ISR
// =======

void _ISR_PSV _SPI1Interrupt(void)
{
    // Hanover C^ to System
    if (SPI1STATbits.SPITBF) {
        CSys_Spi1OnTxEmpty();
    }
    if (SPI1STATbits.SPIRBF) {
        Byte ch = SPI1BUF;
        CSys_Spi1OnRx(ch);
    }
    if (SPI1STATbits.SPIROV) {
        CSys_Spi1OnErr();
        SPI1STATbits.SPIROV = 0;
    }
    IFS0bits.SPI1IF = 0;
}

void _ISR_PSV _SPI2Interrupt(void)
{
    // Hanover C^ to System
    if (SPI2STATbits.SPITBF) {
        CSys_Spi2OnTxEmpty();
    }
    if (SPI2STATbits.SPIRBF) {
        Byte ch = SPI2BUF;
        CSys_Spi2OnRx(ch);
    }
    if (SPI2STATbits.SPIROV) {
        CSys_Spi2OnErr();
        SPI2STATbits.SPIROV = 0;
    }
    IFS2bits.SPI2IF = 0;
}

// I2C ISR
// =======
void _ISR_NPSV _MI2C1Interrupt(void)
{
    if (IFS1bits.MI2C1IF) {
        TstFlag |= 1;
        CSys_I2c1OnEvn();
        IFS1bits.MI2C1IF = 0;   //Clear the DMA0 Interrupt Flag;
    }
}

//*********************************************************************
// End
//*********************************************************************
