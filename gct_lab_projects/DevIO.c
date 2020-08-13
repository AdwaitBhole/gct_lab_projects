//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

// ====================================================================
// CBiChn
// ====================================================================
void CBiChn_Init(CBiChn* Self, Int1 Port, Int1 PpId)
{
    Self->m_Port = Port;
    Self->m_PpId = PpId;
    Self->m_pReg = NULL;
    //
    Self->m_nOnSmp = 0;
    Self->m_nSmp   = 0;
    //
    CSys_GpioInit(Port, PpId, PORT_DRN_INP);
}

void CBiChn_AttRegDefPtr(CBiChn* Self, CRegDef* pReg)
{
    Self->m_pReg = pReg;
}

void CBiChn_ScnA(CBiChn* Self)
{//negative logic
    Byte bVal = CSys_GpioGetVal(Self->m_Port, Self->m_PpId) ? FALSE : TRUE;
    if (bVal)
        Self->m_nOnSmp++;
    Self->m_nSmp++;
    //
    if (Self->m_nSmp >= MAX_BI_SMP) {
        if (Self->m_nOnSmp >= MIN_BI_ONSMP)
             bVal = TRUE;
        else bVal = FALSE;
        CRegDef_CbkOnRawBinValUpd(Self->m_pReg, (bool)bVal);
        Self->m_nOnSmp = 0;
        Self->m_nSmp = 0;
    }
}

void CBiChn_Scn(CBiChn* Self)
{
    if (Self->m_pReg) {
        // negative logic
        //*Self->m_pVal = CSys_GpioGetVal(Self->m_Port, Self->m_PpId) ? FALSE : TRUE;
        // positive logic
        bool bVal = CSys_GpioGetVal(Self->m_Port, Self->m_PpId) ? TRUE : FALSE;
        CRegDef_CbkOnRawBinValUpd(Self->m_pReg, (bool)bVal);
    }
}

Bool CBiChn_RdVal(CBiChn* Self)
{
    if (!Self)
        return FALSE;
    Byte bVal = *((Byte*)CRegDef_GetDptr(Self->m_pReg));

    return (bVal) ? TRUE : FALSE;
}

Bool CBiChn_IsON(CBiChn* Self)
{
    if (!Self)
        return FALSE;
    bool bval = CBiChn_RdVal(Self);
    return (bval != 0);
}

Bool CBiChn_IsOFF(CBiChn* Self)
{
    if (!Self)
        return FALSE;
    bool bval = CBiChn_RdVal(Self);
    return (bval == 0);
}

// ====================================================================
// CBiChnHnd
// ====================================================================
void CBiChnHnd_Reset(CBiChnHnd* Self)
{
    Self->m_bScnEnb = FALSE;
    Self->m_nChn = 0;
    int i = 0;
    for (i = 0; i < MAX_BI_CHN; i++)
        CBiChn_Init(&Self->m_aChn[i], 0, 0);
}

void CBiChnHnd_Start(CBiChnHnd* Self)
{
    if (!Self)
        return;
    Self->m_bScnEnb = TRUE;
}

int CBiChnHnd_GetChnCnt(CBiChnHnd* Self)
{
    return Self->m_nChn;
}

int CBiChnHnd_AddChn(CBiChnHnd* Self, int Port, int PpId)
{
    if (Self->m_nChn >= MAX_BI_CHN)
        return ERR;
    CBiChn_Init(&Self->m_aChn[Self->m_nChn], Port, PpId);
    Self->m_nChn++;
    return OK;
}

void CBiChnHnd_AttRegDefPtr(CBiChnHnd* Self, int iChn, CRegDef* pReg)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_BI_CHN))
        return;
    CBiChn_AttRegDefPtr(&Self->m_aChn[iChn], pReg);
}

int CBiChnHnd_InitChn(CBiChnHnd* Self, int iChn, int Port, int PpId)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_BI_CHN))
        return ERR;
    CBiChn_Init(&Self->m_aChn[iChn], Port, PpId);
    return OK;
}

void CBiChnHnd_EnbChnScn(CBiChnHnd* Self)
{
    Self->m_bScnEnb = TRUE;
}

void CBiChnHnd_DsbChnScn(CBiChnHnd* Self)
{
    Self->m_bScnEnb = FALSE;
}

void CBiChnHnd_ScnChn(CBiChnHnd* Self)
{
    if (!Self->m_bScnEnb)
        return;
    //
    int i = 0;
    CBiChn* pChn;

    for (i = 0; i < Self->m_nChn; i++) {
        pChn = &Self->m_aChn[i];
        CBiChn_Scn(pChn);
    }
}

int CBiChnHnd_RdChn(CBiChnHnd* Self, int iChn, Bool* pVal)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_BI_CHN))
        return ERR;
    *pVal = CBiChn_RdVal(&Self->m_aChn[iChn]);
    return OK;
}

uInt4 CBiChnHnd_GetBmp(CBiChnHnd* Self)
{
    uInt4 bmp = 0;

    int i;
    for (i = 0; i < Self->m_nChn; i++) {
        if (CBiChn_IsON(&Self->m_aChn[i]))
            bmp |= ((uInt4)1 << i);
    }
    
    return bmp;
}

// ====================================================================
// CBoChn
// ====================================================================
void CBoChn_Init(CBoChn* Self, Int1 Port, Int1 PpId)
{
    Self->m_Port    = Port;
    Self->m_PpId    = PpId;
    Self->m_pReg = NULL;
    //
    CSys_GpioSetVal(Self->m_Port, Self->m_PpId, BO_OFF);
    CSys_GpioInit(Port, PpId, PORT_DRN_OUT);
}

void CBoChn_AttRegDefPtr(CBoChn* Self, CRegDef* pReg)
{
    Self->m_pReg = pReg;
}

Bool CBoChn_RdVal(CBoChn* Self)
{
    if (!Self)
        return FALSE;
    bool bval = *((Byte*)CRegDef_GetDptr(Self->m_pReg));
    return (bval) ? TRUE : FALSE;
}

Bool CBoChn_IsON(CBoChn* Self)
{
    if (!Self)
        return FALSE;
    bool bval = CBoChn_RdVal(Self);
    return (bval != 0);
}

Bool CBoChn_IsOFF(CBoChn* Self)
{
    if (!Self)
        return FALSE;
    bool bval = CBoChn_RdVal(Self);
    return (bval == 0);
}

void CBoChn_Upd(CBoChn* Self, Bool bVal)
{
    if (!Self)
        return;
    Bool b = bVal ? BO_ON : BO_OFF;
    CSys_GpioSetVal(Self->m_Port, Self->m_PpId, b);
    CRegDat_SetValByBool(&Self->m_pReg->m_RegDat, bVal);
}
/////////////////////////////////////////////////////////

// ====================================================================
// CBoChnHnd
// ====================================================================
void CBoChnHnd_Reset(CBoChnHnd* Self)
{
    Self->m_nChn = 0;
    int i = 0;
    for (i = 0; i < MAX_BO_CHN; i++)
        CBoChn_Init(&Self->m_aChn[i], 0, 0);
}

int CBoChnHnd_GetChnCnt(CBoChnHnd* Self)
{
    return Self->m_nChn;
}

int CBoChnHnd_AddChn(CBoChnHnd* Self, int Port, int PpId)
{
    if (Self->m_nChn >= MAX_BO_CHN)
        return ERR;
    CBoChn_Init(&Self->m_aChn[Self->m_nChn], Port, PpId);
    Self->m_nChn++;
    return OK;
}

void CBoChnHnd_AttRegDefPtr(CBoChnHnd* Self, int iChn, CRegDef* pReg)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_BO_CHN))
        return;
    CBoChn_AttRegDefPtr(&Self->m_aChn[iChn], pReg);
}

int CBoChnHnd_InitChn(CBoChnHnd* Self, int iChn, int Port, int PpId)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_BO_CHN))
        return ERR;
    CBoChn_Init(&Self->m_aChn[iChn], Port, PpId);
    return OK;
}

int CBoChnHnd_RdChn(CBoChnHnd* Self, int iChn, Bool* pVal)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_BO_CHN))
        return ERR;
    *pVal = CBoChn_RdVal(&Self->m_aChn[iChn]);
    return OK;
}

CBoChn* CBoChnHnd_GetChnByNdx(CBoChnHnd* Self, int iChn)
{
    if (iChn >= MAX_BO_CHN)
        return NULL;
    return &Self->m_aChn[iChn];
}

int CBoChnHnd_Upd(CBoChnHnd* Self, int iChn, Bool bVal)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_BO_CHN))
        return ERR;
    CBoChn_Upd(&Self->m_aChn[iChn], bVal);
    return OK;
}


///////////////////////////////////////////////////////



uInt4 CBoChnHnd_GetBmp(CBoChnHnd* Self)
{
    uInt4 bmp = 0;

    int i;
    for (i = 0; i < Self->m_nChn; i++) {
        if (CBoChn_IsON(&Self->m_aChn[i]))
            bmp |= ((uInt4)1 << i);
    }

    return bmp;
}
// ====================================================================
// CAiChn
// ====================================================================
void CAiChn_Init(CAiChn* Self, Int1 Id)
{
    Self->m_Id = Id;
    Self->m_pReg = NULL;
}

uInt2 CAiChn_RdVal(CAiChn* Self)
{
    if (!Self)
        return 0;

    uInt2 val = *((Int2*)CRegDef_GetDptr(Self->m_pReg));

    return val;
}

void CAiChn_AttRegDefPtr(CAiChn* Self, CRegDef* pReg)
{
    Self->m_pReg = pReg;
}

// ====================================================================
// CAiChnHnd
// ====================================================================
void CAiChnHnd_Reset(CAiChnHnd* Self)
{
    Self->m_bScnEnb = FALSE;
    Self->m_iChn = 0;
    Self->m_nChn = 0;
    Self->m_pDev = NULL;
    int i = 0;
    for (i = 0; i < MAX_AI_CHN; i++)
        CAiChn_Init(&Self->m_aChn[i], 0);
}

void CAiChnHnd_Setup(CAiChnHnd* Self, CDevMax11300* pDev)
{
    CAiChnHnd_Reset(Self);
    Self->m_pDev = pDev;
}

int CAiChnHnd_Start(CAiChnHnd* Self)
{
    if (!Self->m_pDev) 
        return ERR;
    Self->m_bScnEnb = TRUE;
    return OK;
}

int CAiChnHnd_GetChnCnt(CAiChnHnd* Self)
{
    return Self->m_nChn;
}

int CAiChnHnd_AddChn(CAiChnHnd* Self, int Pin)
{
    if (Self->m_nChn >= MAX_AI_CHN)
        return ERR;
    CAiChn_Init(&Self->m_aChn[Self->m_nChn], Pin);
    if (CDevMax11300_SetPinCnf(Self->m_pDev, Pin, PINCNF_ADC) != OK)
        return ERR;
    Self->m_nChn++;
    return OK;
}

void CAiChnHnd_AttRegDefPtr(CAiChnHnd* Self, int iChn, CRegDef* pReg)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_AI_CHN))
        return;
    CAiChn_AttRegDefPtr(&Self->m_aChn[iChn], pReg);
}

int CAiChnHnd_InitChn(CAiChnHnd* Self, int iChn, int Pin)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_AI_CHN))
        return ERR;
    CAiChn_Init(&Self->m_aChn[iChn], Pin);
    if (CDevMax11300_SetPinCnf(Self->m_pDev, Pin, PINCNF_ADC) != OK)
        return ERR;
    return OK;
}

void CAiChnHnd_EnbChnScn(CAiChnHnd* Self)
{
    Self->m_bScnEnb = TRUE;
}

void CAiChnHnd_DsbChnScn(CAiChnHnd* Self)
{
    Self->m_bScnEnb = FALSE;
}

void CAiChnHnd_ScnChn(CAiChnHnd* Self)
{
    if (!Self->m_bScnEnb)
        return;
    //
    if (Self->m_iChn >= Self->m_nChn)
        Self->m_iChn = 0;
    CAiChn* pchn = &Self->m_aChn[Self->m_iChn++];
    uInt2 rawDat = 0;
    CDevMax11300_RdAdc(Self->m_pDev, pchn->m_Id, &rawDat);
    //
    CRegDef_CbkOnRawValUpd(pchn->m_pReg, rawDat);
}

void CAiChnHnd_ScnAllChn(CAiChnHnd* Self)
{
    if (!Self->m_bScnEnb)
        return;
    //
    int i = 0;
    for (i = 0; i < Self->m_nChn; i++) {
        CAiChn* pchn = &Self->m_aChn[i];
        uInt2 rawDat = 0;
        CDevMax11300_RdAdc(Self->m_pDev, pchn->m_Id, &rawDat);
        //
        CRegDef_CbkOnRawValUpd(pchn->m_pReg, rawDat);
    }
}

int CAiChnHnd_Rd(CAiChnHnd* Self, int iChn, uInt2* pVal)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_AI_CHN))
        return ERR;
    *pVal = CAiChn_RdVal(&Self->m_aChn[iChn]);
    return OK;
}

// ====================================================================
// CAoChn
// ====================================================================
void CAoChn_Init(CAoChn* Self, Int1 Id)
{
    Self->m_Id = Id;
    Self->m_pReg = NULL;
}

void CAoChn_AttRegDefPtr(CAoChn* Self, CRegDef* pReg)
{
    Self->m_pReg = pReg;
}

uInt2 CAoChn_RdVal(CAoChn* Self)
{
    if (!Self)
        return 0;

    uInt2 val = *((Int2*)CRegDef_GetDptr(Self->m_pReg));

    return val;
}

int CAoChn_Upd(CAoChn* Self, uInt2 Val)
{
    if (!Self)
        return 0;
    CRegDat_SetValByI2(&Self->m_pReg->m_RegDat, (Int2)Val);
    return Val;
}

// ====================================================================
// CAoChnHnd
// ====================================================================
void CAoChnHnd_Reset(CAoChnHnd* Self)
{
    Self->m_bScnEnb = FALSE;
    Self->m_iChn = 0;
    Self->m_nChn = 0;
    Self->m_pDev = NULL;
    int i = 0;
    for (i = 0; i < MAX_AO_CHN; i++)
        CAoChn_Init(&Self->m_aChn[i], 0);
}

void CAoChnHnd_Setup(CAoChnHnd* Self, CDevMax11300* pDev)
{
    CAoChnHnd_Reset(Self);
    Self->m_pDev = pDev;
}

int CAoChnHnd_Start(CAoChnHnd* Self)
{
    if (!Self->m_pDev)
        return ERR;
    Self->m_bScnEnb = TRUE;
    return OK;
}

int CAoChnHnd_GetChnCnt(CAoChnHnd* Self)
{
    return Self->m_nChn;
}

int CAoChnHnd_AddChn(CAoChnHnd* Self, int Pin)
{
    if (Self->m_nChn >= MAX_AO_CHN)
        return ERR;
    CAoChn_Init(&Self->m_aChn[Self->m_nChn], Pin);
    if (CDevMax11300_SetPinCnf(Self->m_pDev, Pin, PINCNF_DAC) != OK)
        return ERR;
    Self->m_nChn++;
    return OK;
}

void CAoChnHnd_AttRegDefPtr(CAoChnHnd* Self, int iChn, CRegDef* pReg)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_AO_CHN))
        return;
    CAoChn_AttRegDefPtr(&Self->m_aChn[iChn], pReg);
}

int CAoChnHnd_InitChn(CAoChnHnd* Self, int iChn, int Pin)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_AO_CHN))
        return ERR;
    CAoChn_Init(&Self->m_aChn[iChn], Pin);
    if (CDevMax11300_SetPinCnf(Self->m_pDev, Pin, PINCNF_DAC) != OK)
        return ERR;
    return OK;
}

int CAoChnHnd_Rd(CAoChnHnd* Self, int iChn, uInt2* pVal)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_AO_CHN))
        return ERR;
    *pVal = CAoChn_RdVal(&Self->m_aChn[iChn]);
    return OK;
}

int CAoChnHnd_Upd(CAoChnHnd* Self, int iChn, uInt2 Val)
{
    if ((iChn >= Self->m_nChn) || (iChn >= MAX_AO_CHN))
        return ERR;
    CAoChn* pchn = &Self->m_aChn[iChn];
    if (!pchn)
        return ERR;
    int sts = CDevMax11300_UpdDac(Self->m_pDev, pchn->m_Id, Val);
    if (sts == OK)
        CAoChn_Upd(pchn, Val);
    return sts;
}

//*********************************************************************
// End
//*********************************************************************
