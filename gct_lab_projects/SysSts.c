//*********************************************************************
// Title        : PTaC-BI
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

void CSysSts_Reset(CSysSts* Self)
{
    Self->m_nErrCanBusOvf = 0;
    Self->m_nErrCanBusInvMsg = 0;
    Self->m_nErrCanBusIntErr = 0;
    Self->m_nErrCanBusTx = 0;
    Self->m_nErrCanBusRx = 0;
    //
    Self->m_nErrRs232   = 0;
    Self->m_nErrI2cBus  = 0;
    Self->m_nErrSpiBus  = 0;
    Self->m_nErrFileRd  = 0;
    Self->m_nErrFileWr  = 0;
    Self->m_nErrEepRd   = 0;
    Self->m_nErrEepWr   = 0;
    Self->m_nErrMbsProto= 0;
}

void CSysSts_SetSccTypeId(CSysSts* Self, uInt2 SccType, uInt2 SccId)
{
    Self->m_SccType = SccType;
    Self->m_SccId   = SccId;
}
void CSysSts_SetVer(CSysSts* Self, uInt2 VerMjr, uInt2 VerMnr)
{
    Self->m_VerMjr = VerMjr;
    Self->m_VerMnr = VerMnr;
}

void CSysSts_IncErrCnt(CSysSts* Self, int Type, int SubType)
{
    switch (Type) {
    case ERR_TYPE_ECAN  : CSysSts_IncErrCntCanBus(Self, SubType);   break;
    case ERR_TYPE_RS232 : Self->m_nErrRs232++;              break;
    case ERR_TYPE_I2C   : Self->m_nErrI2cBus++; break;
    case ERR_TYPE_SPI   : Self->m_nErrSpiBus++; break;
    case ERR_TYPE_FILE  : CSysSts_IncErrCntFile(Self, SubType);     break;
    case ERR_TYPE_PROTO : Self->m_nErrMbsProto++;                   break;
    }
}

void CSysSts_IncErrCntCanBus(CSysSts* Self, int SubType)
{
    switch (SubType) {
    case CAN_ISR_RBOV   : Self->m_nErrCanBusOvf++;      break;
    case CAN_ISR_IVR    : Self->m_nErrCanBusInvMsg++;   break;
    case CAN_ISR_ERRI   : Self->m_nErrCanBusIntErr++;   break;
    case CAN_TX_ERR     : Self->m_nErrCanBusTx++;       break;
    case CAN_RX_ERR     : Self->m_nErrCanBusRx++;       break;
    }
}

void CSysSts_IncErrCntFile(CSysSts* Self, int SubType)
{
    switch (SubType) {
    case ERR_FILE_RD : Self->m_nErrFileRd++;    break;
    case ERR_FILE_WR : Self->m_nErrFileRd++;    break;
    }
}

void CSysSts_SetErrVal(CSysSts* Self, int Type, int SubType, int ErrVal)
{
    switch (Type) {
    case ERR_TYPE_ECAN  : CSysSts_SetErrValCanBus(Self, SubType, ErrVal);   break;
    case ERR_TYPE_RS232 : Self->m_nErrRs232 = ErrVal;  break;
    case ERR_TYPE_I2C   : Self->m_nErrI2cBus = ErrVal; break;
    case ERR_TYPE_SPI   : Self->m_nErrSpiBus = ErrVal; break;
    case ERR_TYPE_FILE  : CSysSts_SetErrValFile(Self, SubType, ErrVal);     break;
    case ERR_TYPE_PROTO : Self->m_nErrMbsProto = ErrVal;                    break;
    }
}

void CSysSts_SetErrValCanBus(CSysSts* Self, int SubType, int ErrVal)
{
    switch (SubType) {
    case CAN_ISR_RBOV   : Self->m_nErrCanBusOvf = ErrVal;      break;
    case CAN_ISR_IVR    : Self->m_nErrCanBusInvMsg = ErrVal;   break;
    case CAN_ISR_ERRI   : Self->m_nErrCanBusIntErr = ErrVal;   break;
    case CAN_TX_ERR     : Self->m_nErrCanBusTx = ErrVal;       break;
    case CAN_RX_ERR     : Self->m_nErrCanBusRx = ErrVal;       break;
    }
}

void CSysSts_SetErrValFile(CSysSts* Self, int SubType, int ErrVal)
{
    switch (SubType) {
    case ERR_FILE_RD : Self->m_nErrFileRd = ErrVal;    break;
    case ERR_FILE_WR : Self->m_nErrFileRd = ErrVal;    break;
    }
}

// ====================================================================
// TDbgInfo
// ====================================================================

void DbgInfo_Init(TDbgInfo* Self)
{
    if (!Self)
        return;
    Self->m_BitSts = 0;
    Self->m_BufSz = 0;
    Self->m_pBuf = NULL;
}

void DbgInfo_SetBuf(TDbgInfo* Self, Byte* pBuf, int BufSz)
{
    if (!Self)
        return;
    Self->m_BitSts = 0;
    Self->m_BufSz = BufSz;
    Self->m_pBuf = pBuf;
}

void DbgInfo_Clr(TDbgInfo* Self)
{
    if (!Self)
        return;
    Self->m_BitSts = 0;
    Self->m_iBuf = 0;
    if (!Self->m_pBuf)
        return;
    memset(Self->m_pBuf, 0, Self->m_BufSz);
}

void DbgInfo_SetBit(TDbgInfo* Self, int Ndx)
{
    if (!Self)
        return;
    if (Ndx >= 16)
        return;
    uInt2 msk = 1 << Ndx;
    Self->m_BitSts |= msk;
}

void DbgInfo_ClrBit(TDbgInfo* Self, int Ndx)
{
    if (!Self)
        return;
    if (Ndx >= 16)
        return;
    uInt2 msk = ~(1 << Ndx);
    Self->m_BitSts &= msk;

}

Bool DbgInfo_IsBitSet(TDbgInfo* Self, int Ndx)
{
    if (!Self)
        return FALSE;
    if (Ndx >= 16)
        return FALSE;
    uInt2 msk = 1 << Ndx;
    return ((Self->m_BitSts & msk) ? TRUE : FALSE);
}

void DbgInfo_AddByte(TDbgInfo* Self, Byte Dat)
{
    if (!Self)
        return;
    if (Self->m_iBuf >= Self->m_BufSz)
        return;
    Self->m_pBuf[Self->m_iBuf] = Dat;
    Self->m_iBuf++;
}

//*********************************************************************
// End
//*********************************************************************
