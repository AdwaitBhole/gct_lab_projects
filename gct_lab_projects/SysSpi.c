//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

// Code Section
// ============

void CSys_SpiDef(CSpiChn* Self, int ChnId)
{
    Self->m_ChnId = ChnId;
}

void CSys_SpiChnSetup(CSpiChn* Self, int Mode, int ProtoMode, int FrqCode, int DatSz, int PortCS, int PinCS, int Wtm)
{
    Self->m_Mode = Mode;
    Self->m_ProtoMode = ProtoMode;
    Self->m_FrqCode = FrqCode;
    if (Mode == SPI_MST_MODE) {
        Self->m_PortCS = PortCS;
        Self->m_PinCS = PinCS;
        CSys_GpioInit(PortCS, PinCS, PORT_DRN_OUT);
        CSys_GpioSetVal(PortCS, PinCS, TRUE);
    }
    CCpu_SpiReset(Self->m_ChnId);
    CCpu_SpiMode(Self->m_ChnId, Mode);
    CCpu_SpiDatSz(Self->m_ChnId, DatSz);
    CCpu_SpiSmpAt(Self->m_ChnId, SPI_SMP_MID);
    CCpu_SpiProtoMode(Self->m_ChnId, ProtoMode);
    CCpu_SpiSetFrq(Self->m_ChnId, FrqCode);

    Self->m_Rxd = 0;
    Self->m_Txd = 0;
    Self->m_bRxAvl = FALSE;
    Self->m_bTxEmpty = TRUE;
    
    CCpu_SpiEnb(Self->m_ChnId);
    CCpu_SpiSetIsrMsk(Self->m_ChnId);
    
    CPollTmr_Init(&Self->m_WtmTmr, Wtm);
}

void CSys_SpiChnCsLo(CSpiChn* Self)
{
    CSys_GpioSetVal(Self->m_PortCS, Self->m_PinCS, FALSE);
}

void CSys_SpiChnCsHi(CSpiChn* Self)
{
    CSys_GpioSetVal(Self->m_PortCS, Self->m_PinCS, TRUE);
}

void CSys_SpiChnInit(CSpiChn* Self, int Wtm)
{
    Self->m_Rxd = 0;
    Self->m_Txd = 0;
    Self->m_bRxAvl = FALSE;
    Self->m_bTxEmpty = TRUE;
    CPollTmr_Init(&Self->m_WtmTmr, Wtm);
}

int CSys_SpiChnTrx(CSpiChn* Self, BOOL bRdDat)
{
    int sts = CSys_WtTxEmpty(Self);
    if (sts != OK)
        return ERR;
    CSys_SpiChnClrRxAvl(Self);
    //
    CCpu_SpiTxd(Self->m_ChnId, Self->m_Txd);
    
    // Wait for SPI tx buf empty
    sts = CSys_WtTxEmpty(Self);
    if (sts != OK)
        return ERR;
    if (bRdDat) 
        sts = CSys_WtRxAvl(Self);
    return sts;
}
/*
int CSys_SpiChnTrxA(CSpiChn* Self, BOOL bRdDat)
{
    CSys_SpiChnClrRxAvl(Self);
    //
    CCpu_SpiTxd(Self->m_ChnId, Self->m_Txd);
    while (SPI1STATbits.SPITBF);

    if (bRdDat)
        while (!SPI1STATbits.SPIRBF);
    return OK;
    // Wait for SPI tx buf empty
//    sts = CSys_WtTxEmpty(Self);
//    if (sts != OK)
//        return ERR;
//    if (bRdDat)
//        sts = CSys_WtRxAvl(Self);
//    return sts;
}
*/
int CSys_WtTxEmpty(CSpiChn* Self)
{
    int sts;
    CPollTmr_Start(&Self->m_WtmTmr);
    while (true) {
        if (Self->m_bTxEmpty) {
            sts = OK;
            break;
        }        
        if (CPollTmr_IsTmout((&Self->m_WtmTmr))) {
            sts = ERR;
            break;
        }
    }
    return sts;
}

int CSys_WtRxAvl(CSpiChn* Self)
{
    int sts;
    CPollTmr_Start(&Self->m_WtmTmr);
    while (true) {
        if (Self->m_bRxAvl) {
            sts = OK;
            break;
        }
        if (CPollTmr_IsTmout((&Self->m_WtmTmr))) {
            sts = ERR;
            break;
        }
    }
    return sts;
}

void CSys_SpiChnClrTxEmpty(CSpiChn* Self)
{
    Self->m_bTxEmpty = FALSE;
}

void CSys_SpiChnClrRxAvl(CSpiChn* Self)
{
    Self->m_bRxAvl = FALSE;
}

// ISR
// ===
void CSys_Spi1OnTxEmpty()
{
    pSPI_CHN1->m_bTxEmpty = TRUE;
}

void CSys_Spi1OnRx(Byte Ch)
{
    pSPI_CHN1->m_Rxd = Ch;
    pSPI_CHN1->m_bRxAvl = TRUE;
}

void CSys_Spi1OnErr()
{
    CSysSts_IncErrCnt(pSysSts, ERR_TYPE_SPI, 0);
}

void CSys_Spi2OnTxEmpty()
{
    pSPI_CHN2->m_bTxEmpty = TRUE;
}

void CSys_Spi2OnRx(Byte Ch)
{
    pSPI_CHN2->m_Rxd = Ch;
    pSPI_CHN2->m_bRxAvl = TRUE;
}

void CSys_Spi2OnErr()
{
    CSysSts_IncErrCnt(pSysSts, ERR_TYPE_SPI, 0);
}
//*********************************************************************
// End
//*********************************************************************
