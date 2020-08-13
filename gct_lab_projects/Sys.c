//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"
// ============
// Code Section
// ============
void CSys_Init(int Mode)
{
    gSys.m_SysMode = Mode;
    //
    CCpu_InitOsc();
    //
    CCpu_ClrIsrMsk();
    //
    CSys_InitPph();
    //
    SysTck_Init();
    //
    CTmrHnd_Init();
    CSys_LedInit();
    CDbg_CtrlInit();
    //
    // Initialize COM
    CCanDev_Init(pCANDEV);
    //
    CCom_Init(pCOM1, SYS_RS232_CHN1, pRS232_CHN1);
    CCom_Init(pCOM2, SYS_RS232_CHN2, pRS232_CHN2);
    //
    CSys_CanBusInit();
    //
    //
    CRs232Chn_InitPrm(pRS232_CHN1, SYS_RS232_CHN1, BAUD_9600, DBIT_8, PAR_NONE, SBIT_1);
    CRs232Chn_Pgm(pRS232_CHN1);
    //
//    CRs232Chn_InitPrm(pRS232_CHN2, SYS_RS232_CHN2, BAUD_9600, DBIT_8, PAR_NONE, SBIT_1);
//    CRs232Chn_Pgm(pRS232_CHN2);
    //
    CCpu_I2c1Init(I2C_FRQ_400KHz);
    CSys_I2cChnInit(pI2C_CHN1, TC_10MS);
    //
    CLogCtx_Init();
    CLogCtx_Setup(pCOM1);
    //
    CSys_SpiDef(pSPI_CHN1, CPU_SPI_CHN1);
    CSys_SpiDef(pSPI_CHN2, CPU_SPI_CHN2);
    //
    CPollTmr_Init(pSTS_TCK, STS_TCK);
    CPollTmr_Start(pSTS_TCK);
}

void CSys_InitPph()
{
    PPSUnLock;
    // Configure UART
    PPSOutput(OUT_FN_PPS_U1TX, OUT_PIN_PPS_RP118);      // UART1_TXD
    PPSInput(IN_FN_PPS_U1RX, IN_PIN_PPS_RPI119);        // UART1_RXD
    // Configure UART PORT2
//    PPSOutput(OUT_FN_PPS_U2TX, OUT_PIN_PPS_RP120);      // UART2_TXD
//    PPSInput(IN_FN_PPS_U2RX, IN_PIN_PPS_RPI121);        // UART2_RXD
    // Configure CAN
    PPSOutput(OUT_FN_PPS_C1TX, OUT_PIN_PPS_RP39);      // ECAN_TXD
    PPSInput(IN_FN_PPS_C1RX, IN_PIN_PPS_RPI58);        // ECAN_RXD
    // Configure as SPI2
    PPSInput(IN_FN_PPS_SDI2, IN_PIN_PPS_RP43);        // SPI2 MISO
    PPSOutput(OUT_FN_PPS_SDO2, OUT_PIN_PPS_RP56);      // SPI2 MOSI
    PPSOutput(OUT_FN_PPS_SCK2, OUT_PIN_PPS_RP55);      // SPI2 SCK
    
    //
    PPSLock;
}

void CSys_UpdIsr()
{
    CCpu_UpdIsr();
}

void CSys_CanChnSetSrvId(uInt1 m_SrvId)
{
//    CCanChn_SetMyNodeId(pCANBUS_CHN1, m_SrvId);
    CSys_CanBusUpdMsgId(TRUE, m_SrvId, 0);
}

void CSys_OnTckMon()
{
    if (CPollTmr_IsTmout(pSTS_TCK)) {
        CSys_LedTgl(STS_LED);
        CPollTmr_Start(pSTS_TCK);
    }
}

// Tmr1 Isr from CPU
// =================
void CSys_InitTmr1Isr()
{
    CCpu_InitTmr1Isr();
}

void CSys_OnIsrTck()
{
    SysTck_OnTckIsr();
    CTmrHnd_UpdOnTck();
}
//
// ====================================================================
// System Misc
// ====================================================================
void Sys_Nop(int Cnt)
{
    if (Cnt <= 0)
        return;
    int i = 0;
    for (i = 0; i < Cnt; i++);
}
//*********************************************************************
// End
//*********************************************************************
