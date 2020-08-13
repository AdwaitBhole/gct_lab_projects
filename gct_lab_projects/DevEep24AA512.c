//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

void CDevEep24AA512_Reset(CDevEep24AA512* Self)
{
    Self->m_bEnb = FALSE;
    Self->m_Id = 0;
    Self->m_pChn = NULL;
}

void CDevEep24AA512_Setup(CDevEep24AA512* Self, Byte Id, CI2cChn* pI2cChn, int Wtm)
{
    Self->m_Id = Id;
    Self->m_pChn = pI2cChn;
    if (Self->m_pChn)
        Self->m_bEnb = TRUE;
    CPollTmr_Init(&Self->m_WtmTmr, Wtm);
}

int CDevEep24AA512_WrMode(CDevEep24AA512* Self, uInt2 Adr)
{
    int sts = CSys_I2cStart(Self->m_pChn);
    if (sts != OK) {
        CLogCtx_WrStg("I2cStart Evn\r\n");
        return sts;
    }
    sts = CSys_I2cTxAdr(Self->m_pChn, Self->m_Id, FALSE);
    if (sts != OK) {
        CLogCtx_WrStg("Error EepAdr Write\r\n");
        return sts;
    }
    Byte h,l;
    h = INT2_HIGH(Adr);
    l = INT2_LOW(Adr);
    sts = CSys_I2cTxd(Self->m_pChn, h);
    if (sts != OK) {
        CLogCtx_WrStg("Error Eep Wr MSB\r\n");
        return sts;
    }
    sts = CSys_I2cTxd(Self->m_pChn, l);
    if (sts != OK) {
        CLogCtx_WrStg("Error Eep Wr LSB\r\n");
    }
    return sts;
}

int CDevEep24AA512_RdMode(CDevEep24AA512* Self, uInt2 Adr)
{
    int sts = CDevEep24AA512_WrMode(Self, Adr);		// Called to Set Address Pointer
    if (sts != OK)
        return sts;
    sts = CSys_I2cStart(Self->m_pChn);
    if (sts != OK)
        return sts;
    return CSys_I2cTxAdr(Self->m_pChn, Self->m_Id, TRUE);
}

int CDevEep24AA512_Wr(CDevEep24AA512* Self, uInt2 Adr, Byte *pDat, Int2 Len)
{
    if (!Self->m_bEnb)
        return ERR;
    int sts = CDevEep24AA512_WrMode(Self, Adr);
    if (sts != OK)
        return sts;
    Byte pgofs, ch;
    pgofs = Adr & 0x007F;
    while (Len) {
	ch = *pDat;
        sts = CSys_I2cTxd(Self->m_pChn, ch);
        if (sts != OK)
            return sts;
	//m_pI2c->Wrt(ch);
	pDat++;
	Adr++;
	pgofs++;
	Len--;
	if ((pgofs != 0x80) && (Len != 0))
	    continue;
        sts = CSys_I2cStop(Self->m_pChn);
        if (sts != OK)
            return sts;
        CDevEep24AA512_Wait(Self);
	if (Len != 0) {
            pgofs = 0;
            sts = CDevEep24AA512_WrMode(Self, Adr);		// Called to Set Address Pointer
            if (sts != OK)
                return sts;
	}
    }
    return OK;
}

int CDevEep24AA512_Rd(CDevEep24AA512* Self, uInt2 Adr, Byte *pDat, Int2 Len)
{
    if (!Self) {
        CLogCtx_WrStg("Self Error \r\n");
    }

    if (!Self->m_bEnb)
        return ERR;

    int sts = CDevEep24AA512_RdMode(Self, Adr);
    if (sts != OK) {
        CLogCtx_WrStg("Read Mode Error\r\n");
        return sts;
    }
    //	if (!IsRngOk(Adr, Len))
    //	return ERR;
    Byte   flag;
    Byte pgofs, ch;
    pgofs = Adr & 0x007F;
    while (Len) {
    // flag = 1 (NAK) : Last Byte or Page-End
    // flag = 0 (ACK) : Inc. Address in a Page
	flag = ((Len == 1) || (pgofs == 0x7F)) ? 1 : 0;
        sts = CSys_I2cRxd(Self->m_pChn, &ch, flag);
	if (sts != OK)
	    return ERR;
	*pDat = ch;
	pDat++;
	Adr++;
	pgofs++;
	Len--;
	if (!flag)
	    continue;
        sts = CSys_I2cStop(Self->m_pChn);
        if (sts != OK)
            return sts;
	if (Len != 0) {
	    pgofs = 0;
            sts = CDevEep24AA512_RdMode(Self, Adr);
            if (sts != OK)
                return sts;
	}
    }
    return OK;
}

void CDevEep24AA512_Wait(CDevEep24AA512* Self)
{
    CPollTmr_Start(&Self->m_WtmTmr);
    while (!CPollTmr_IsTmout(&Self->m_WtmTmr));
}

//*********************************************************************
// End
//*********************************************************************
