//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"
// ====================================================================
// CDevMax11300
// ====================================================================
void CDevMax11300_Reset(CDevMax11300* Self)
{
    Self->m_pChn = NULL;
    Self->m_DevId = 0xFFFF;
    Self->m_bEnb = FALSE;
}

void CDevMax11300_Init(CDevMax11300* Self)
{
    CDevMax11300_Reset(Self);
}

void CDevMax11300_Setup(CDevMax11300* Self, CSpiChn* pChn)
{
    Self->m_pChn = pChn;
    // Harware initialisation
    CSys_SpiChnSetup(Self->m_pChn, SPI_MST_MODE, SPI_PROTO_MODE_0, SPI_FRQ_10MHz, SPI_DAT_BYTE, uCPORTB, PIN00, TC_10MS);
}

int CDevMax11300_Start(CDevMax11300* Self)
{
    if (!Self->m_pChn)
        return ERR;

    int sts = UNKNOWN;
    sts = CDevMax11300_Rd(Self, MAX11300_REG_DEVID, &Self->m_DevId);
    if (sts != OK)
        return ERR;

    sts = CDevMax11300_DevCtrlSet(Self);
    if (sts != OK)
        return ERR;

    Self->m_bEnb = TRUE;
    return OK;
}

int CDevMax11300_DevCtrlSet(CDevMax11300* Self)
{
    uInt2 dev = 0;
    dev |= DEVCTRL_ADC_CONT_SWEEP;  // Continuous sweep mode
    dev |= DEVCTRL_DAC_SEQUPD;      // DAC sequntial update
    dev |= DEVCTRL_ADC_400KSPS;     // ADC speed 400 ksps
    dev |= DEVCTRL_DAC_INTREF;      // DAC internal ref voltage
    dev |= DEVCTRL_THERMSHDN_ENB;   // Thermal shutdown enable
    dev |= DEVCTRL_TEMMON1_DSB;     // Temperature1 Mon disable
    dev |= DEVCTRL_TEMMON2_DSB;     // Temperature2 Mon disable
    dev |= DEVCTRL_TEMMON3_DSB;     // Temperature3 Mon disable
    dev |= DEVCTRL_TEMCNV_TM_DFL;   // Default Temperature cnv time
    dev |= DEVCTRL_TEM_RSCANCEL_DSB;// temperature rs compensation disable
    dev |= DEVCTRL_PWRMD_DFL;       // Default power mode
    dev |= DEVCTRL_SPI_BURST_DSB;   // burst mode disable

    return CDevMax11300_Wr(Self, MAX11300_REG_DEVCTRL, dev);
}

int CDevMax11300_SetPinHiImpd(CDevMax11300* Self, Byte RegId)
{
    if ((RegId < MAX11300_REG_CNF_PORT00) ||
        (RegId > MAX11300_REG_CNF_PORT19))
        return ERR;

    int sts = CDevMax11300_Wr(Self, RegId, 0);
    if (sts != OK)
        return sts;

    Sys_Nop(1000);

    return OK;
}

int CDevMax11300_SetPinCnf(CDevMax11300* Self, int Pin, int Cnf)
{
    uInt2 cnfval = 0;
    if (Cnf == PINCNF_ADC) {
        cnfval |= PINCNF_MODE7;         // single ended
        cnfval |= PINCNF_VRNG_0_P10;    // 0 to 10V
        cnfval |= PINCNF_ADC_SMP16;     // Avg 16 smp
    }
    else {
        cnfval |= PINCNF_MODE5;         // DAC o/p
        cnfval |= PINCNF_VRNG_0_P10;    // 0 to 10V
        cnfval |= PINCNF_ADC_SMP2;      // Avg Smp 2 // ALERT
    }
    Byte regid;
    switch (Pin) {
    case PIN00: regid = MAX11300_REG_CNF_PORT00;  break;
    case PIN01: regid = MAX11300_REG_CNF_PORT01;  break;
    case PIN02: regid = MAX11300_REG_CNF_PORT02;  break;
    case PIN03: regid = MAX11300_REG_CNF_PORT03;  break;
    case PIN04: regid = MAX11300_REG_CNF_PORT04;  break;
    case PIN05: regid = MAX11300_REG_CNF_PORT05;  break;
    case PIN06: regid = MAX11300_REG_CNF_PORT06;  break;
    case PIN07: regid = MAX11300_REG_CNF_PORT07;  break;
    case PIN08: regid = MAX11300_REG_CNF_PORT08;  break;
    case PIN09: regid = MAX11300_REG_CNF_PORT09;  break;
    case PIN10: regid = MAX11300_REG_CNF_PORT10;  break;
    case PIN11: regid = MAX11300_REG_CNF_PORT11;  break;
    case PIN12: regid = MAX11300_REG_CNF_PORT12;  break;
    case PIN13: regid = MAX11300_REG_CNF_PORT13;  break;
    case PIN14: regid = MAX11300_REG_CNF_PORT14;  break;
    case PIN15: regid = MAX11300_REG_CNF_PORT15;  break;
    case PIN16: regid = MAX11300_REG_CNF_PORT16;  break;
    case PIN17: regid = MAX11300_REG_CNF_PORT17;  break;
    case PIN18: regid = MAX11300_REG_CNF_PORT18;  break;
    case PIN19: regid = MAX11300_REG_CNF_PORT19;  break;
    default   : regid = 0xFF;                     break;
    }
    if (regid == 0xFF)
        return ERR;

    int sts = CDevMax11300_SetPinHiImpd(Self, regid);
    if (sts != OK)
        return sts;
    
    return CDevMax11300_Wr(Self, regid, cnfval);
}

int CDevMax11300_UpdDac(CDevMax11300* Self, int Pin, uInt2 Dat)
{
    Byte regid;
    switch (Pin) {
    case PIN00: regid = MAX11300_REG_DACDAT_PORT00;     break;
    case PIN01: regid = MAX11300_REG_DACDAT_PORT01;     break;
    case PIN02: regid = MAX11300_REG_DACDAT_PORT02;     break;
    case PIN03: regid = MAX11300_REG_DACDAT_PORT03;     break;
    case PIN04: regid = MAX11300_REG_DACDAT_PORT04;     break;
    case PIN05: regid = MAX11300_REG_DACDAT_PORT05;     break;
    case PIN06: regid = MAX11300_REG_DACDAT_PORT06;     break;
    case PIN07: regid = MAX11300_REG_DACDAT_PORT07;     break;
    case PIN08: regid = MAX11300_REG_DACDAT_PORT08;     break;
    case PIN09: regid = MAX11300_REG_DACDAT_PORT09;     break;
    case PIN10: regid = MAX11300_REG_DACDAT_PORT10;     break;
    case PIN11: regid = MAX11300_REG_DACDAT_PORT11;     break;
    case PIN12: regid = MAX11300_REG_DACDAT_PORT12;     break;
    case PIN13: regid = MAX11300_REG_DACDAT_PORT13;     break;
    case PIN14: regid = MAX11300_REG_DACDAT_PORT14;     break;
    case PIN15: regid = MAX11300_REG_DACDAT_PORT15;     break;
    case PIN16: regid = MAX11300_REG_DACDAT_PORT16;     break;
    case PIN17: regid = MAX11300_REG_DACDAT_PORT17;     break;
    case PIN18: regid = MAX11300_REG_DACDAT_PORT18;     break;
    case PIN19: regid = MAX11300_REG_DACDAT_PORT19;     break;
    default   : regid = 0xFF;                           break;
    }
    if (regid == 0xFF)
        return ERR;
    return CDevMax11300_Wr(Self, regid, Dat);
}

int CDevMax11300_RdAdc(CDevMax11300* Self, int Pin, uInt2* pDat)
{
    Byte regid;
    switch (Pin) {
    case PIN00: regid = MAX11300_REG_ADCDAT_PORT00;     break;
    case PIN01: regid = MAX11300_REG_ADCDAT_PORT01;     break;
    case PIN02: regid = MAX11300_REG_ADCDAT_PORT02;     break;
    case PIN03: regid = MAX11300_REG_ADCDAT_PORT03;     break;
    case PIN04: regid = MAX11300_REG_ADCDAT_PORT04;     break;
    case PIN05: regid = MAX11300_REG_ADCDAT_PORT05;     break;
    case PIN06: regid = MAX11300_REG_ADCDAT_PORT06;     break;
    case PIN07: regid = MAX11300_REG_ADCDAT_PORT07;     break;
    case PIN08: regid = MAX11300_REG_ADCDAT_PORT08;     break;
    case PIN09: regid = MAX11300_REG_ADCDAT_PORT09;     break;
    case PIN10: regid = MAX11300_REG_ADCDAT_PORT10;     break;
    case PIN11: regid = MAX11300_REG_ADCDAT_PORT11;     break;
    case PIN12: regid = MAX11300_REG_ADCDAT_PORT12;     break;
    case PIN13: regid = MAX11300_REG_ADCDAT_PORT13;     break;
    case PIN14: regid = MAX11300_REG_ADCDAT_PORT14;     break;
    case PIN15: regid = MAX11300_REG_ADCDAT_PORT15;     break;
    case PIN16: regid = MAX11300_REG_ADCDAT_PORT16;     break;
    case PIN17: regid = MAX11300_REG_ADCDAT_PORT17;     break;
    case PIN18: regid = MAX11300_REG_ADCDAT_PORT18;     break;
    case PIN19: regid = MAX11300_REG_ADCDAT_PORT19;     break;
    default   : regid = 0xFF;                           break;
    }

    if (regid == 0xFF)
        return ERR;
    return CDevMax11300_Rd(Self, regid, pDat);
}

int CDevMax11300_Wr(CDevMax11300* Self, Byte RegId, uInt2 Dat)
{
    Self->m_RegId = RegId;
    Self->m_RegMsb = (Dat >> 8) & 0xFF;
    Self->m_RegLsb = Dat & 0xFF;
    return CDevMax11300_SpiTrx(Self, FALSE);
}

int CDevMax11300_Rd(CDevMax11300* Self, Byte RegId, uInt2 *pDat)
{
    Self->m_RegId   = RegId;
    Self->m_RegMsb  = 0xFF;
    Self->m_RegLsb  = 0xFF;
    int sts = CDevMax11300_SpiTrx(Self, TRUE);
    if (sts == OK)
        *pDat = (Self->m_RegMsb << 8) | Self->m_RegLsb;
    return sts;
}

int CDevMax11300_SpiTrx(CDevMax11300* Self, Bool bRdTrx)
{
    CSys_SpiChnCsLo(Self->m_pChn);
    int sts = CDevMax11300_SpiTrxStart(Self, bRdTrx);
    CSys_SpiChnCsHi(Self->m_pChn);
    return sts;
}

int CDevMax11300_SpiTrxStart(CDevMax11300* Self, Bool bRdTrx)
{
    int sts = UNKNOWN;
    Byte id = (Self->m_RegId << 1);
    if (bRdTrx) 
         id |= 0x01;
    else id &= 0xFE;
    // Send Id
    Self->m_pChn->m_bTxEmpty = TRUE;
    Self->m_pChn->m_Txd = id;
    sts = CSys_SpiChnTrx(Self->m_pChn, FALSE);
    if (sts != OK)
        return sts;
    // Send MSB
    Self->m_pChn->m_Txd = Self->m_RegMsb;
    sts = CSys_SpiChnTrx(Self->m_pChn, bRdTrx);
    if (sts != OK)
        return sts;
    Self->m_RegMsb = Self->m_pChn->m_Rxd;
    // Send LSB
    Self->m_pChn->m_Txd = Self->m_RegLsb;
    sts = CSys_SpiChnTrx(Self->m_pChn, bRdTrx);
    if (sts != OK)
        return sts;
    Self->m_RegLsb = Self->m_pChn->m_Rxd;
    return OK;
}

//*********************************************************************
// End
//*********************************************************************
