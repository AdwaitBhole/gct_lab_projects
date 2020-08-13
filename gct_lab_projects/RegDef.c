//*********************************************************************
// Title        : PTaC-BI
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

char RegDatFmtBuf[zREG_STGFMT_BUF];

CScnBuf gScnBuf;

// ====================================================================
// CRegDat
// ====================================================================

void CRegDat_Init(CRegDat* Self)
{
    if (!Self)
        return;

    Self->m_Dty = REG_TYPE_INV;
    Self->m_Sz  = 0;
    Self->m_nSmp= 0;

    int i = 0;
    for (i = 0; i < MAX_RAW_SMP; i++)
        Self->m_aSmp[i] = 0;
    Self->m_Val = 0;
    Self->m_pScnBuf = NULL;
}

void CRegDat_Def(CRegDat* Self, Int1 Dty, Int1 Sz, bool bScnBuf)
{
    if (!Self)
        return;

    Self->m_Dty = Dty;
    Self->m_Sz = Sz;
    Self->m_nSmp = 0;

    int i = 0;
    for (i = 0; i < MAX_RAW_SMP; i++)
        Self->m_aSmp[i] = 0;
    Self->m_Val = 0;

    int nbyte = 0;
    if (Self->m_Dty == REG_TYPE_BMP) {
        nbyte = 4;
//        nbyte = Self->m_Sz / 8;
//        if (Self->m_Sz % 8)
//            nbyte++;
    }
    else nbyte = Sz;

    if (bScnBuf)
         Self->m_pScnBuf = CScnBuf_Add(&gScnBuf, nbyte);
    else Self->m_pScnBuf = NULL;
}

uInt1 CRegDat_GetType(CRegDat* Self)
{
    if (!Self)
        return 0;
    return Self->m_Dty;
}

void CRegDat_SetType(CRegDat* Self, Int1 Dty)
{
    if (!Self)
        return;
    Self->m_Dty = Dty;
}

uInt1 CRegDat_GetSz(CRegDat* Self)
{
    if (!Self)
        return 0;
    return Self->m_Sz;
}

void CRegDat_SetSz(CRegDat* Self, Int1 Sz)
{
    if (!Self)
        return;
    Self->m_Sz = Sz;
}

Byte* CRegDat_GetDptr(CRegDat* Self)
{
    if (!Self)
        return NULL;
    Byte* p = (Byte*)&Self->m_Val;
    return p;
}

int CRegDat_SetDatByDptr(CRegDat* Self, Byte* pDat)
{
    if (!Self || !pDat)
        return ERR;
    int sts = OK;
    Int4 val;
    switch (Self->m_Dty) {
    case REG_TYPE_BIT   : val = *((Byte*)pDat);     break;
    case REG_TYPE_I1    : val = *((Byte*)pDat);     break;
    case REG_TYPE_I2    : val = *((Int2*)pDat);     break;
    case REG_TYPE_I4    : val = *((Int4*)pDat);     break;
    case REG_TYPE_BMP   : val = *((uInt4*)pDat);    break;
    default             : sts = ERR_INV_REGTYPE;    break;
    }

    if (sts == OK)
        sts = CRegDat_SetVal(Self, val);
    return sts;
}

int CRegDat_AddDatByDptr(CRegDat* Self, Byte* pDat)
{
    if (!Self || !pDat)
        return ERR;
    int sts = OK;
    Int4 val;
    switch (Self->m_Dty) {
    case REG_TYPE_BIT   : val = *((Byte*)pDat);     break;
    case REG_TYPE_I1    : val = *((Byte*)pDat);     break;
    case REG_TYPE_I2    : val = *((Int2*)pDat);     break;
    case REG_TYPE_I4    : val = *((Int4*)pDat);     break;
    case REG_TYPE_BMP   : val = *((uInt4*)pDat);    break;
    default             : sts = ERR_INV_REGTYPE;    break;
    }

    if (sts == OK)
        sts = CRegDat_AddVal(Self, val);
    return sts;
}

int CRegDat_GetRegDat(CRegDat* Self, CRegDat* pRegDat)
{
    if (!Self || !pRegDat)
        return ERR;
    pRegDat->m_Dty = Self->m_Dty;
    pRegDat->m_Sz = Self->m_Sz;
    int sts = OK;
    switch (Self->m_Dty) {
    case REG_TYPE_BIT   : pRegDat->m_Val = Self->m_Val;   break;
    case REG_TYPE_I1    : pRegDat->m_Val = Self->m_Val;   break;
    case REG_TYPE_I2    : pRegDat->m_Val = Self->m_Val;   break;
    case REG_TYPE_I4    : pRegDat->m_Val = Self->m_Val;   break;
    case REG_TYPE_BMP   : pRegDat->m_Val = Self->m_Val;   break;
    default             : sts = ERR_INV_REGTYPE;          break;
    }
    return sts;
}

int CRegDat_ArcOut(CRegDat* Self, Byte* pBuf, int* BufLen)
{// Fill RegVal to pBuf
    if (!Self || !pBuf || !BufLen)
        return ERR;

    int buflen = -1;
    Byte* p = (Byte*)&Self->m_Val;
    switch (Self->m_Dty) {
    case REG_TYPE_BIT   :
    case REG_TYPE_I1    : buflen = 1;   break;
    case REG_TYPE_I2    : buflen = 2;   break;
    case REG_TYPE_I4    : buflen = 4;   break;
    case REG_TYPE_BMP   : buflen = 5;   break;
    default             : buflen = -1;  break;
    }

    if (buflen <= 0)
        return ERR;

    *BufLen = buflen;

    if (Self->m_Dty == REG_TYPE_BMP) {
        *pBuf = Self->m_Sz;
        pBuf++;
        buflen--;
    }

    memcpy(pBuf, p, buflen);
    
    return OK;
}

int CRegDat_ArcIn(CRegDat* Self, Byte* pBuf, int BufLen)
{
    if (!Self || !pBuf)
        return ERR;

    Byte* p = (Byte*)&Self->m_Val;
    Bool bbmp = FALSE;
    int buflen;
    switch (Self->m_Dty) {
    case REG_TYPE_BIT   :
    case REG_TYPE_I1    : buflen = 1;   break;
    case REG_TYPE_I2    : buflen = 2;   break;
    case REG_TYPE_I4    : buflen = 4;   break;
    case REG_TYPE_BMP   :
        Self->m_Dty = pBuf[0];
        pBuf++;
        buflen = BufLen -1;
        bbmp = TRUE;
        break;
    default             :
        buflen = -1;
        break;
    }

    if (buflen <= 0)
        return ERR_INV_REGTYPE;
    if ((!bbmp) && (buflen != BufLen))
        return ERR;
    memcpy(p, pBuf, buflen);
    return OK;
}

int CRegDat_GetFmtRegDat(CRegDat* Self, char* pFmtRegVal)
{
    if (!Self || !pFmtRegVal)
        return ERR;

    switch (Self->m_Dty) {
    case REG_TYPE_BIT   : 
    case REG_TYPE_I1    : 
        sprintf(pFmtRegVal, "%02d;%02d;%02d", Self->m_Dty, Self->m_Sz, (Int1)Self->m_Val);
        break;
    case REG_TYPE_I2    : 
        sprintf(pFmtRegVal, "%02d;%02d;%d", Self->m_Dty, Self->m_Sz, (Int2)Self->m_Val);
        break;
    case REG_TYPE_I4    : 
        sprintf(pFmtRegVal, "%02d;%02d;%ld", Self->m_Dty, Self->m_Sz, Self->m_Val);
        break;
    case REG_TYPE_BMP   : 
        sprintf(pFmtRegVal, "%02d;%02d;%08lX", Self->m_Dty, Self->m_Sz, Self->m_Val);
        break;
    default             : 
        return ERR;
    }

    return OK;
}

int CRegDat_SetValByStg(CRegDat* Self, CStg* pVal)
{
    if (!Self || !pVal)
        return ERR;

    int sts = OK;
    char* p = CStg_GetPtr(pVal);
    if (!p)
        return ERR;
    switch (Self->m_Dty) {
    case REG_TYPE_BIT   : 
    case REG_TYPE_I1    : 
    case REG_TYPE_I2    : Self->m_Val = (Int2)atoi(p);    break;
    case REG_TYPE_I4    : Self->m_Val = (Int4)atol(p);    break;
    case REG_TYPE_BMP   : Self->m_Val = (Int4)atol(p);  break;
    default             : sts = ERR_INV_REGTYPE;         break;
    }
    return sts;
}

int CRegDat_CpyVal(CRegDat* Self, CRegDat* pSrcDat)
{
    if (!Self || !pSrcDat)
        return ERR;

    Self->m_Val = pSrcDat->m_Val;
    return OK;
}

int CRegDat_AddVal(CRegDat* Self, Int4 Val)
{
    if (!Self)
        return ERR;
    if ((Self->m_Dty == REG_TYPE_BIT) || (Self->m_Dty == REG_TYPE_BMP)) {
        Self->m_Val = Val;
    }
    else {
        int i = 0;
        if (Self->m_nSmp < MAX_RAW_SMP-1) {
            Self->m_aSmp[Self->m_nSmp] = Val;
            Self->m_nSmp++;
        }
        else {
            for (i = 0; i < Self->m_nSmp-1; i++)
                Self->m_aSmp[i] = Self->m_aSmp[i+1];
            Self->m_aSmp[i] = Val;
        }
        uInt4 tot = 0;
        if (Self->m_nSmp > 0) {
            for (i = 0; i < Self->m_nSmp; i++)
                tot += Self->m_aSmp[i];
            Self->m_Val = tot / Self->m_nSmp;
        }
    }

    if (Self->m_pScnBuf) {
        switch (Self->m_Dty) {
        case REG_TYPE_BIT   : *((Int1*)Self->m_pScnBuf) = (Int1)Self->m_Val;    break;
        case REG_TYPE_I1    : *((Int1*)Self->m_pScnBuf) = (Int1)Self->m_Val;    break;
        case REG_TYPE_I2    : *((Int2*)Self->m_pScnBuf) = (Int2)Self->m_Val;    break;
        case REG_TYPE_I4    : *((Int4*)Self->m_pScnBuf) = (Int4)Self->m_Val;    break;
        case REG_TYPE_BMP   : *((Int4*)Self->m_pScnBuf) = (Int4)Self->m_Val;    break;
        }
    }

    return OK;
}

int CRegDat_SetVal(CRegDat* Self, Int4 Val)
{
    if (!Self)
        return ERR;
    Self->m_Val = Val;
    if (Self->m_pScnBuf) {
        switch (Self->m_Dty) {
        case REG_TYPE_BIT   : *((Int1*)Self->m_pScnBuf) = (Int1)Self->m_Val;    break;
        case REG_TYPE_I1    : *((Int1*)Self->m_pScnBuf) = (Int1)Self->m_Val;    break;
        case REG_TYPE_I2    : *((Int2*)Self->m_pScnBuf) = (Int2)Self->m_Val;    break;
        case REG_TYPE_I4    : *((Int4*)Self->m_pScnBuf) = (Int4)Self->m_Val;    break;
        case REG_TYPE_BMP   : *((Int4*)Self->m_pScnBuf) = (Int4)Self->m_Val;    break;
        }
    }

    return OK;
}


int CRegDat_SetValByBool(CRegDat* Self, bool Val)
{
    if (!Self)
        return ERR;
    Self->m_Val = Val;
    return OK;
}

int CRegDat_SetValByI1(CRegDat* Self, Int1 Val)
{
    if (!Self)
        return ERR;
    Self->m_Val = Val;
    return OK;
}

int CRegDat_SetValByI2(CRegDat* Self, Int2 Val)
{
    if (!Self)
        return ERR;
    Self->m_Val = Val;
    return OK;
}

int CRegDat_SetValByI4(CRegDat* Self, Int4 Val)
{
    if (!Self)
        return ERR;
    Self->m_Val = Val;
    return OK;
}

/*
int CRegDat_Add(CRegDat* Self, CRegDat* pSrcDat)
{
    if (!Self || !pSrcDat)
        return ERR;
    int sts = OK;
    switch (pSrcDat->m_Dty) {
    case REG_TYPE_BIT   : Self->uVal.m_bVal  += pSrcDat->uVal.m_bVal;    break;
    case REG_TYPE_I1    : Self->uVal.m_i1Val += pSrcDat->uVal.m_i1Val;   break;
    case REG_TYPE_I2    : Self->uVal.m_i2Val += pSrcDat->uVal.m_i2Val;   break;
    case REG_TYPE_I4    : Self->uVal.m_i4Val += pSrcDat->uVal.m_i4Val;   break;
    case REG_TYPE_FL    : Self->uVal.m_fVal  += pSrcDat->uVal.m_fVal;    break;
    case REG_TYPE_BMP   : Self->uVal.m_Bmp   += pSrcDat->uVal.m_Bmp;     break;
    default             : sts = ERR_INV_REGTYPE;                        break;
    }
    return sts;
}

int CRegDat_DivByNum(CRegDat* Self, int Num)
{
    if (!Self)
        return ERR;
    if (Num <= 0)
        return ERR;
    int sts = OK;
    switch (Self->m_Dty) {
    case REG_TYPE_BIT   : Self->uVal.m_bVal  /= Num;   break;
    case REG_TYPE_I1    : Self->uVal.m_i1Val /= Num;   break;
    case REG_TYPE_I2    : Self->uVal.m_i2Val /= Num;   break;
    case REG_TYPE_I4    : Self->uVal.m_i4Val /= Num;   break;
    case REG_TYPE_FL    : Self->uVal.m_fVal  /= Num;   break;
    case REG_TYPE_BMP   : Self->uVal.m_Bmp   /= Num;   break;
    default             : sts = ERR_INV_REGTYPE;       break;
    }
    return sts;
}
*/
// ====================================================================
// CRegAna
// ====================================================================
/*
void CRegAna_Init(CRegAna* Self, Int1 Dty, Int1 Sz)
{
    if (!Self)
        return;
    Self->m_nSmp = 0;
    int i = 0;
    for (i = 0; i < MAX_ANA_SMP; i++) {
        CRegDat_Init(&Self->m_aDat[i], Dty, Sz);
    }
}

void CRegAna_Push(CRegAna* Self, CRegDat* pSrcDat)
{
    if (!Self)
        return;
    CRegDat* pDat;
    CRegDat* pDat1;
    int i;
    if (Self->m_nSmp < MAX_ANA_SMP) {
        pDat = &Self->m_aDat[Self->m_nSmp];
        Self->m_nSmp++;
    }
    else {
        Self->m_nSmp = MAX_ANA_SMP;
        for (i = (Self->m_nSmp-1); i >= 1; i--) {
            pDat = &Self->m_aDat[i];
            pDat1 = &Self->m_aDat[i-1];
            CRegDat_SetVal(pDat, pDat1);
        }
        pDat = &Self->m_aDat[0];
    }
    CRegDat_SetVal(pDat, pSrcDat);
}

void CRegAna_Upd(CRegAna* Self, CRegDat* pDat)
{
    if (!Self)
        return;
    CRegAna_Push(Self, pDat);
}

void CRegAna_AvgVal(CRegAna* Self, CRegDat* pDat)
{
    if (!Self)
        return;
    CRegDat dat;
    CRegDat_Init(&dat, pDat->m_Dty, pDat->m_Sz);

    int i = 0;
    for (i = 0; i < Self->m_nSmp; i++) {
        CRegDat_Add(&dat, &Self->m_aDat[i]);
    }
    CRegDat_DivByNum(&dat, Self->m_nSmp);
    CRegDat_SetVal(pDat, &dat);
}
*/
// ====================================================================
// CRegDef
// ====================================================================

void CRegDef_Init(CRegDef* Self)
{
    Self->m_Id = REG_ID_INV;
    Self->m_pName = NULL;
    Self->m_pDsc = NULL;
    Self->m_Flag = 0x00;
    CRegDat_Init(&Self->m_RegDat);
}

void CRegDef_Def(CRegDef* Self, uInt1 RegId, const char* pName,
                const char* pDsc, uInt1 Flag, Int1 Dty, Int1 Sz, bool bScnBuf)
{
    Self->m_Id      = RegId;
    Self->m_pName   = pName;
    Self->m_pDsc    = pDsc;
    Self->m_Flag    = Flag;

    CRegDat_Def(&Self->m_RegDat, Dty, Sz, bScnBuf);
}

Byte* CRegDef_GetDptr(CRegDef* Self)
{
    return CRegDat_GetDptr(&Self->m_RegDat);
}

Bool CRegDef_IsRO(CRegDef* Self)
{
    return (Self->m_Flag & REG_FLAG_RO) ? TRUE : FALSE;
}

int CRegDef_GetRegDef(CRegDef* Self, CRegDef* pRegDef)
{
    pRegDef->m_Id = Self->m_Id;
    pRegDef->m_pName   = Self->m_pName;
    pRegDef->m_pDsc    = Self->m_pDsc;
    pRegDef->m_Flag = Self->m_Flag;
    pRegDef->m_Sts = Self->m_Sts;
    return CRegDat_GetRegDat(&Self->m_RegDat, &pRegDef->m_RegDat);
}

int CRegDef_ArcOut(CRegDef* Self, Byte* pBuf, int* BufLen)
{
    return CRegDat_ArcOut(&Self->m_RegDat, pBuf, BufLen);
}

int CRegDef_ArcIn(CRegDef* Self, Byte* pBuf, int BufLen)
{
    return CRegDat_ArcIn(&Self->m_RegDat, pBuf, BufLen);
}

int CRegDef_GetFmtReg(CRegDef* Self, CStg* pFmtRegVal)
{
    int sts = CRegDat_GetFmtRegDat(&Self->m_RegDat, RegDatFmtBuf);
    if (sts != OK) 
        return sts;

    CStg_Fmt(pFmtRegVal, "%02d;%s;%s;%02X;%02d;%s\r\n",
            Self->m_Id, Self->m_pName, Self->m_pDsc,
            Self->m_Flag, Self->m_Sts, RegDatFmtBuf);
    return OK;
}

Bool CRegDef_IsNameMatch(CRegDef* Self, char* pName, int Flt)
{
    if (!pName)
        return FALSE;

    if (Flt == CMP_ALL) {
        if (strcmp(Self->m_pName, pName) == 0)
            return TRUE;
        return FALSE;
    }
    else {
        if (Self->m_pName[0] == pName[0])
            return TRUE;
        return FALSE;
    }
}

int CRegDef_SetValByStg(CRegDef* Self, CStg* pVal)
{
    if (CRegDef_IsRO(Self))
        return ERR;
    return CRegDat_SetValByStg(&Self->m_RegDat, pVal);
}

// cbks
void CRegDef_CbkOnRawBinValUpd(CRegDef* Self, bool Val)
{
    if (!Self)
        return;
    CRegDat_SetValByBool(&Self->m_RegDat, Val);
}

void CRegDef_CbkOnRawValUpd(CRegDef* Self, Int4 Val)
{
    if (!Self)
        return;
    CRegDat_AddVal(&Self->m_RegDat, Val);
}

// ====================================================================
// CScnBuf
// ====================================================================

void CScnBuf_Init(CScnBuf* Self)
{
    if (!Self)
        return;
    Self->m_nBytes = 0;
    memset(Self->m_aBuf, 0, zSCN_BUF);
}

Byte* CScnBuf_Add(CScnBuf* Self, int nByte)
{
    if (!Self)
        return NULL;
    int nbyte = Self->m_nBytes + nByte;
    if (nbyte > zSCN_BUF)
        return NULL;
    int ofs = Self->m_nBytes;
    Self->m_nBytes = nbyte;
    return &Self->m_aBuf[ofs];
}

CScnBuf* CScnBuf_GetPtr()
{
    return &gScnBuf;
}

void CScnBuf_Prn()
{
    CLogCtx_Info("ScnBuf nByte=%d\r\n", gScnBuf.m_nBytes);
    int nPkt = gScnBuf.m_nBytes / 8;
    int i = 0;
    int ofs = 0;
    Byte* p;
    for (i = 0; i < nPkt; i++) {
        p = &gScnBuf.m_aBuf[ofs];
        CLogCtx_Info("%02x %02x %02x %02x %02x %02x %02x %02x\r\n",
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
        ofs += 8;
    }
}

// ====================================================================
// CRegTbl
// ====================================================================
void CRegTbl_Init(CRegTbl* Self)
{
    Self->m_nReg = 0;
    int i = 0;
    for (i = 0; i < MAX_REG; i++)
        CRegDef_Init(&Self->m_aReg[i]);
    CScnBuf_Init(&gScnBuf);
}

int CRegTbl_AddReg(CRegTbl* Self, uInt1 RegId, const char* pName,
                   const char* pDsc, uInt1 Flag, Int1 Dty, Int1 Sz, bool bScnBuf)
{
    if (RegId == REG_ID_INV)
        return ERR;
    if (Self->m_nReg >= MAX_REG)
        return ERR;
    CRegDef* pReg = &Self->m_aReg[Self->m_nReg];
    CRegDef_Def(pReg, RegId, pName, pDsc, Flag, Dty, Sz, bScnBuf);
    Self->m_nReg++;
    return OK;
}

int CRegTbl_GetCnt(CRegTbl* Self)
{
    return Self->m_nReg;
}

CRegDef* CRegTbl_GetRegById(CRegTbl* Self, int RegId)
{
    if (RegId == REG_ID_INV)
        return NULL;
    CRegDef* pRegDef = NULL;
    int i;
    for (i = 0; i < Self->m_nReg; i++) {
        pRegDef = &Self->m_aReg[i];
        if (!pRegDef)
            continue;
        if (pRegDef->m_Id == RegId)
            return pRegDef;
    }

    return NULL;
}

int CRegTbl_GetRegDef(CRegTbl* Self, int RegId, CRegDef* pRegDef)
{
    CRegDef* pDef = CRegTbl_GetRegById(Self, RegId);
    if (!pDef)
        return ERR_INV_REG;
    return CRegDef_GetRegDef(pDef, pRegDef);
}

Byte* CRegTbl_GetDptr(CRegTbl* Self, uInt1 RegId)
{
    CRegDef* pRegDef = CRegTbl_GetRegById(Self, RegId);
    if (!pRegDef)
        return NULL;
    return CRegDef_GetDptr(pRegDef);
}

int CRegTbl_GetFmtRegByNdx(CRegTbl* Self, int RegNdx, CStg* pFmtRegVal)
{
    if ((RegNdx < 0) || (RegNdx >= Self->m_nReg))
        return ERR;
    return CRegDef_GetFmtReg(&Self->m_aReg[RegNdx], pFmtRegVal);
}

int CRegTbl_GetFmtRegByNameFmOfs(CRegTbl* Self, char* pName, int Flt,
                                    int RegOfs, CStg* pFmtRegVal)
{
    if ((Self->m_nReg <= 0) || (RegOfs >= Self->m_nReg))
        return ERR;

    if (RegOfs < 0)
        RegOfs = 0;
    int i, sts;
    CRegDef* pRegDef;
    for (i = RegOfs; i < Self->m_nReg; i++) {
        pRegDef = &Self->m_aReg[i];
        if (!CRegDef_IsNameMatch(pRegDef, pName, Flt))
            continue;
        sts = CRegDef_GetFmtReg(pRegDef, pFmtRegVal);
        if (sts != OK)
            continue;
        return i;
    }

    return -1;
}

int CRegTbl_GetFmtRegByName(CRegTbl* Self, char* pName,
                            int Flt, CStg* pFmtRegVal)
{
    int sts = CRegTbl_GetFmtRegByNameFmOfs(Self, pName, Flt, 0, pFmtRegVal);
    return (sts >= 0) ? OK : ERR;
}

CRegDef* CRegTbl_GetRegDefByName(CRegTbl* Self, char* pName)
{
    if (Self->m_nReg <= 0)
        return NULL;
    int i;
    CRegDef* pRegDef;
    for (i = 0; i < Self->m_nReg; i++) {
        pRegDef = &Self->m_aReg[i];
        if (!CRegDef_IsNameMatch(pRegDef, pName, CMP_ALL))
            continue;
        return pRegDef;
    }

    return NULL;
}

//*********************************************************************
// End
//*********************************************************************

