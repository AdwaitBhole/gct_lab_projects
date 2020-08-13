//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

// Code Section
// ============
void CRs232Chn_InitPrm(CRs232Chn* Self, int ChnId, uInt2 Baud, int Dbits, int Parity, int Sbits)
{
    Self->m_Id = ChnId;
    Self->m_ChnPrm.m_Baud = Baud;
    Self->m_ChnPrm.m_Dbits = Dbits;
    Self->m_ChnPrm.m_Parity = Parity;
    Self->m_ChnPrm.m_Sbits = Sbits;
}

void CRs232Chn_Pgm(CRs232Chn* Self)
{
    CCpu_Rs232ChnInit(Self->m_Id, Self->m_ChnPrm.m_Baud, Self->m_ChnPrm.m_Dbits,
                      Self->m_ChnPrm.m_Parity, Self->m_ChnPrm.m_Sbits);
}

void CRs232Chn_TxD(CRs232Chn* Self, CQue* pQue)
{
    CQue_Rd(pQue,(char*)&Self->m_Txd);
    CCpu_Rs232ChnTxD(Self->m_Id, Self->m_Txd);
}

void CRs232Chn_OnRxc(CRs232Chn* Self, Byte Ch)
{
    Self->m_Rxd = Ch;
    Self->m_RxLen = 1;
}

void CRs232Chn_GetRxc(CRs232Chn* Self, CQue* pQue)
{
    CQue_Wrt(pQue, Self->m_Rxd);
}

// UART Isr call from CPU
// =======================
void CSys_Rs232ChnOnIsrRxC(int Id, Byte Ch)
{
    CRs232Chn* pChn = NULL;
    switch (Id) {
        case SYS_RS232_CHN1: pChn = pRS232_CHN1;    break;
        case SYS_RS232_CHN2: pChn = pRS232_CHN2;    break;
    }
    if (pChn) {
        CRs232Chn_OnRxc(pChn, Ch);
        CCom_OnRx(pChn);
        // pChn->m_Rxd = Ch;
        // CCom_OnRx(SYS_CHN_TY_RS232, pChn->m_Id, &pChn->m_Rxd, 1);
        //CCom_OnRxC(SYS_CHN_TY_RS232, pChn);
    }
}

void CSys_Rs232ChnOnIsrTxEmpty(int Id)
{
    CRs232Chn* pChn = NULL;
    switch (Id) {
        case SYS_RS232_CHN1: pChn = pRS232_CHN1;    break;
        case SYS_RS232_CHN2: pChn = pRS232_CHN2;    break;
    }
    if (pChn)
        CCom_OnTxEmpty(pChn);
}

//*********************************************************************
// End
//*********************************************************************
