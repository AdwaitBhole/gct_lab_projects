//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"
// ====================================================================
// CDevMax1400X
// ====================================================================
void CDevMax1400X_Reset(CDevMax1400X* Self)
{
    Self->m_pChn = NULL;
    Self->m_bEnb = FALSE;
    Self->m_nDev = 1;
    //
    CSys_GpioInit(DEC_A0_PORT, DEC_A0_PIN, PORT_DRN_OUT);
    CSys_GpioInit(DEC_A1_PORT, DEC_A1_PIN, PORT_DRN_OUT);
    CSys_GpioInit(DEC_A2_PORT, DEC_A2_PIN, PORT_DRN_OUT);
    CSys_GpioInit(DEC_A3_PORT, DEC_A3_PIN, PORT_DRN_OUT);    
}

void CDevMax1400X_Init(CDevMax1400X* Self)
{
    CDevMax1400X_Reset(Self);
}

void CDevMax1400X_Setup(CDevMax1400X* Self, CSpiChn* pChn, int nDev)
{
    CDevMax1400X_Init(Self);
    //
    Self->m_pChn = pChn;
    Self->m_nDev = nDev;
    // Hardware initialization
    CSys_SpiChnSetup(Self->m_pChn, SPI_MST_MODE, SPI_PROTO_MODE_0, SPI_FRQ_5MHz, SPI_DAT_BYTE, DEC_INH_PORT, DEC_INH_PIN, TC_10MS);
}

int CDevMax1400X_Start(CDevMax1400X* Self, int iDev)
{
    if (!Self->m_pChn)
        return ERR;
    
    if ((iDev < 0) || (iDev >= Self->m_nDev))
        return ERR;
    
    int sts = OK;

    uInt2 cfg = 0;
    sts = CDevMax1400X_Rd(Self, iDev, MAX1400X_CFG, &cfg);
    if (sts != OK)
        return ERR;
    
//    if (cfg != MAX1400X_DFL_CFG)
//        return ERR;

    CDevMax1400X_EnbWr(Self, iDev);
    cfg |= 0x020;
    CDevMax1400X_Wr(Self, iDev, MAX1400X_CFG, cfg);
    CDevMax1400X_Wr(Self, iDev, MAX1400X_CFGV, cfg);
    //
    CDevMax1400X_Wr(Self, iDev, MAX1400X_THU, 520);
    CDevMax1400X_Wr(Self, iDev, MAX1400X_THUV, 520);
    CDevMax1400X_Wr(Self, iDev, MAX1400X_THL, 515);
    CDevMax1400X_Wr(Self, iDev, MAX1400X_THLV, 515);
    //
    CDevMax1400X_DsbWr(Self, iDev);
    
    Self->m_bEnb = TRUE;
    return sts;
}

int CDevMax1400X_SetThrU(CDevMax1400X* Self, int iDev, int Val)
{
    int sts = CDevMax1400X_EnbWr(Self, iDev);
    if (sts != OK)
        return sts;

    Val &= 0x03FF;    
    sts = CDevMax1400X_Wr(Self, iDev, MAX1400X_THU, Val);
    CDevMax1400X_DsbWr(Self, iDev);
    return sts;
}

int CDevMax1400X_GetThrU(CDevMax1400X* Self, int iDev, uInt2* pVal)
{
    if (!Self)
        return ERR;
    
    return CDevMax1400X_Rd(Self, iDev, MAX1400X_THU, pVal);
}

int CDevMax1400X_SetThrL(CDevMax1400X* Self, int iDev, int Val)
{
    int sts = CDevMax1400X_EnbWr(Self, iDev);
    if (sts != OK)
        return sts;
    
    Val &= 0x03FF;    
    sts = CDevMax1400X_Wr(Self, iDev, MAX1400X_THL, Val);
    CDevMax1400X_DsbWr(Self, iDev);
    
    return sts;
}

int CDevMax1400X_GetThrL(CDevMax1400X* Self, int iDev, uInt2* pVal)
{
    if (!Self)
        return ERR;
    
    return CDevMax1400X_Rd(Self, iDev, MAX1400X_THL, pVal);
}

int CDevMax1400X_ScnADC(CDevMax1400X* Self, int iDev, uInt2* pVal)
{
    if (!Self)
        return ERR;
    
    return CDevMax1400X_Rd(Self, iDev, MAX1400X_ADC, pVal);
}

// Pvt:
// ====
int CDevMax1400X_EnbWr(CDevMax1400X* Self, int iDev)
{
    return CDevMax1400X_Wr(Self, iDev, MAX1400X_WEN, MAX1400X_WENB_VAL);
}

int CDevMax1400X_DsbWr(CDevMax1400X* Self, int iDev)
{
    return CDevMax1400X_Wr(Self, iDev, MAX1400X_WEN, MAX1400X_WDSB_VAL);
}

int CDevMax1400X_Wr(CDevMax1400X* Self, int Ndx, Byte RegId, uInt2 Dat)
{
    if (!Self->m_pChn)
        return ERR;

    int sts = ERR;
    
    Self->m_TxMsb = (Dat >> 8) & 0xFF;
    Self->m_TxLsb = Dat & 0xFF;
    Self->m_RegId = RegId;
    CDevMax1400X_Sel(Ndx);
    
    CSys_SpiChnCsLo(Self->m_pChn);
    sts = CDevMax1400X_SpiTrx(Self, FALSE);
    CSys_SpiChnCsHi(Self->m_pChn);
    
    return sts;
}

int CDevMax1400X_Rd(CDevMax1400X* Self, int Ndx, Byte RegId, uInt2 *pDat)
{
    if (!Self->m_pChn)
        return ERR;
    int sts = ERR;
    uInt2 dat = 0;
    Self->m_TxMsb = 0;
    Self->m_TxLsb = 0;
    Self->m_RegId = RegId;
    CDevMax1400X_Sel(Ndx);
    // RegId Cycle: Transfer RegId first, data gets in next cycle
    CSys_SpiChnCsLo(Self->m_pChn);
    sts = CDevMax1400X_SpiTrx(Self, TRUE);
    CSys_SpiChnCsHi(Self->m_pChn);
    if (sts != OK)
        return sts;
    // Data Cycle
    Self->m_TxMsb = 0;
    Self->m_TxLsb = 0;
    Self->m_RegId = RegId;
    CSys_SpiChnCsLo(Self->m_pChn);
    sts = CDevMax1400X_SpiTrx(Self, TRUE);
    CSys_SpiChnCsHi(Self->m_pChn);
    if (sts != OK)
        return sts;
    //
    dat = (Self->m_RxMsb & 0xFF);
    dat <<= 8;
    dat &= 0x0300;
    dat |= Self->m_RxLsb;
    //
    *pDat = dat;
    return OK;
}

int CDevMax1400X_SpiTrx(CDevMax1400X* Self, Bool bRdTrx)
{
    int sts;
    uInt1 datH, datL, regid;
    datH = CDevMax1400X_RevByte(Self->m_TxLsb);
    datL = CDevMax1400X_RevByte(Self->m_TxMsb);
    datL &= 0xC0;
    regid = CDevMax1400X_RevByte(Self->m_RegId);
    regid >>= 3;
    regid &= 0x1F;
    datL |= regid;
    if (!bRdTrx) 
        datL |= MAX1400X_BMSK_CMD_WR;

    // Send MSB
    Self->m_pChn->m_bTxEmpty = TRUE;
    Self->m_pChn->m_Txd = datH;
    sts = CSys_SpiChnTrx(Self->m_pChn, bRdTrx);
    if (sts != OK)
        return sts;
    datH = Self->m_pChn->m_Rxd;
    // Send LSB
    Self->m_pChn->m_Txd = datL;
    sts = CSys_SpiChnTrx(Self->m_pChn, bRdTrx);
    if (sts != OK)
        return sts;
    datL = Self->m_pChn->m_Rxd;
    
    if (bRdTrx) {
        // LSB is received first -> datH is copied to RxLsb
        Self->m_RxLsb = CDevMax1400X_RevByte(datH);
        Self->m_RxMsb = CDevMax1400X_RevByte(datL);        
    }
    
    return OK;
}

Byte CDevMax1400X_RevByte(Byte Dat)
{
    int i = 0;
    Byte revByte = 0;
    Byte d = FALSE;
    for (i = 0; i < 8; i++) {
        d = (Dat >> i) & 0x01;
        revByte <<= 1;
        revByte |= d;
    }
    return revByte;
}

void CDevMax1400X_Sel(int Ndx)
{
    switch (Ndx) {
        case 0 : CDevMax1400X_SelPort(FALSE, FALSE, FALSE, FALSE);  break;
        case 1 : CDevMax1400X_SelPort(FALSE, FALSE, FALSE, TRUE);   break;
        case 2 : CDevMax1400X_SelPort(FALSE, FALSE, TRUE, FALSE);   break;
        case 3 : CDevMax1400X_SelPort(FALSE, FALSE, TRUE, TRUE);    break;
        case 4 : CDevMax1400X_SelPort(FALSE, TRUE, FALSE, FALSE);   break;
        case 5 : CDevMax1400X_SelPort(FALSE, TRUE, FALSE, TRUE);    break;
        case 6 : CDevMax1400X_SelPort(FALSE, TRUE, TRUE, FALSE);    break;
        case 7 : CDevMax1400X_SelPort(FALSE, TRUE, TRUE, TRUE);     break;
        case 8 : CDevMax1400X_SelPort(TRUE, FALSE, FALSE, FALSE);   break;
        case 9 : CDevMax1400X_SelPort(TRUE, FALSE, FALSE, TRUE);    break;
        case 10: CDevMax1400X_SelPort(TRUE, FALSE, TRUE, FALSE);    break;
        case 11: CDevMax1400X_SelPort(TRUE, FALSE, TRUE, TRUE);     break;
        case 12: CDevMax1400X_SelPort(TRUE, TRUE, FALSE, FALSE);    break;
        case 13: CDevMax1400X_SelPort(TRUE, TRUE, FALSE, TRUE);     break;
        case 14: CDevMax1400X_SelPort(TRUE, TRUE, TRUE, FALSE);     break;
        case 15: CDevMax1400X_SelPort(TRUE, TRUE, TRUE, TRUE);      break;
        default: break;
    }        
}

void CDevMax1400X_SelPort(Bool bVal3, Bool bVal2, Bool bVal1, Bool bVal0)
{
    CSys_GpioSetVal(DEC_A0_PORT, DEC_A0_PIN, bVal0);
    CSys_GpioSetVal(DEC_A1_PORT, DEC_A1_PIN, bVal1);
    CSys_GpioSetVal(DEC_A2_PORT, DEC_A2_PIN, bVal2);
    CSys_GpioSetVal(DEC_A3_PORT, DEC_A3_PIN, bVal3);
}
//*********************************************************************
// End
//*********************************************************************
