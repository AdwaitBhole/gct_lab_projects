//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

void CSys_I2cChnInit(CI2cChn* Self, int Wtm)
{
    Self->m_bEvn = FALSE;
    CPollTmr_Init(&Self->m_WtmTmr, Wtm);
}

BOOL CSys_I2cChnIsEvn(CI2cChn* Self)
{
    CPollTmr_Start(&Self->m_WtmTmr);
    Bool flag = FALSE;
    while (true) {
        if (Self->m_bEvn) {
            flag = TRUE;
            Self->m_bEvn = FALSE;
            break;
        }
        if (CPollTmr_IsTmout(&Self->m_WtmTmr)) {
            Self->m_bEvn = FALSE;
            break;
        }
    }

    return flag;
}

BOOL CSys_I2cIsAck(CI2cChn* Self)
{
    CPollTmr_Start(&Self->m_WtmTmr);
    Bool flag = FALSE;
    while (true) {
        if (!CCpu_I2c1RdAck()) {
            flag = TRUE;
            break;
        }
        if (CPollTmr_IsTmout(&Self->m_WtmTmr)) 
            break;
    }
    return flag;
}

BOOL CSys_I2cIsTxBusy(CI2cChn* Self)
{
    CPollTmr_Start(&Self->m_WtmTmr);
    Bool flag = TRUE;
    while (true) {
        if (!CCpu_I2c1IsTxBusy()) {
            flag = FALSE;
            break;
        }
        if (CPollTmr_IsTmout(&Self->m_WtmTmr)) 
            break;
    }
    return flag;
}

BOOL CSys_I2cIsDatRdy(CI2cChn* Self)
{
    CPollTmr_Start(&Self->m_WtmTmr);
    Bool flag = FALSE;
    while (true) {
        if (CCpu_I2c1IsDatRdy()) {
            flag = TRUE;
            break;
        }
        if (CPollTmr_IsTmout(&Self->m_WtmTmr))
            break;
    }
    return flag;
}

int CSys_I2cStart(CI2cChn* Self)
{
    CCpu_I2c1Start();
    return CSys_I2cChnIsEvn(Self) ?  OK : ERR;
}

int CSys_I2cStop(CI2cChn* Self)
{
    CCpu_I2c1Stop();
    return CSys_I2cChnIsEvn(Self) ?  OK : ERR;
}

int CSys_I2cRestart(CI2cChn* Self)
{
    CCpu_I2c1Restart();
    if (!CSys_I2cChnIsEvn(Self)) {
        CSys_I2cStop(Self);
        return ERR;
    }
    return OK;
}

int CSys_I2cTxd(CI2cChn* Self, Byte Dat)
{
    if (CSys_I2cIsTxBusy(Self)) {
        return ERR;
    }

    CCpu_I2c1Txd(Dat);
    
    if (!CSys_I2cChnIsEvn(Self)) {
        CSys_I2cStop(Self);
        return ERR;
    }

    if (!CSys_I2cIsAck(Self)) {
        CSys_I2cStop(Self);
        return ERR;
    }
    return OK;
}

int CSys_I2cTxAdr(CI2cChn* Self, Byte Adr, Bool bRd)
{
    Adr &= 0xFF;
    if (bRd)
         Adr |= I2C_RD;
    else Adr &= I2C_WR;

    return CSys_I2cTxd(Self, Adr);
}

int CSys_I2cRxd(CI2cChn* Self, Byte* pDat, Bool bAck)
{
    CCpu_I2c1RxEnb();

    if (!CSys_I2cIsAck(Self)) {
        CSys_I2cStop(Self);
        return ERR;
    }
    //
    if (!CSys_I2cIsDatRdy(Self))
        return ERR;

    *pDat = CCpu_I2c1Rxd();
    CCpu_I2c1TxAck(bAck);
    return OK;
}

// ISR
void CSys_I2c1OnEvn()
{
    pI2C_CHN1->m_bEvn = TRUE;
}

//*********************************************************************
// End
//*********************************************************************
