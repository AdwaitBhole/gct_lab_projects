//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************

#include "../include/CmnInc.h"

// ====================================================================
// CCnbPkt
// ====================================================================

void CCnbPkt_Init(CCnbPkt* Self)
{
    Self->m_MstId    = 0;
    Self->m_ClnId    = 0xFF;
    Self->m_Dlen     = 0;
    Self->m_ReqCode  = 0;
    Self->m_RegId    = 0;
    Self->m_Flag     = 0;
    Self->m_Buflen   = 0;

    Self->m_bCont   = false;
    Self->m_bErr    = false;
    Self->m_iPktOfs = 0;

    memset(Self->m_aBuf, 0, sizeof(Self->m_aBuf));
    // status
    Self->m_bExtIdn = TRUE;
    Self->m_bRtrFrm = FALSE;
    Self->m_bErrFrm = FALSE;
}

void CCnbPkt_SetId(CCnbPkt* Self, uInt1 MstId, uInt2 ClnId)
{
    Self->m_MstId = MstId;
    Self->m_ClnId = ClnId;
}

uInt1 CCnbPkt_GetClnId(CCnbPkt* Self)
{
    return Self->m_ClnId;
}

void CCnbPkt_SetIdnFlag(CCnbPkt* Self, Bool bExtIdn, Bool bRtrFrm, Bool bErrFrm)
{
    Self->m_bExtIdn = bExtIdn;
    Self->m_bRtrFrm = bRtrFrm;
    Self->m_bErrFrm = bErrFrm;
}

void CCnbPkt_ArcIn(CCnbPkt* Self, CCanFrm* pFrm)
{// Prepare Pkt from CanFrame
    uInt4 idn = CCanFrm_GetIdn(pFrm);

    Self->m_MstId = (uInt1)((idn >> iBITOFS_SND_ID) & 0xFF);    // HostId
    Self->m_ClnId = (uInt1)((idn >> iBITOFS_RCV_ID) & 0xFF);    // MyId

    Byte tmp = idn & 0xFF;
    Self->m_bCont   = (tmp & MSK_PKT_CONT_IND) ? true : false;
    Self->m_bErr    = (tmp & MSK_PKT_ERR_IND) ? true : false;
    Self->m_iPktOfs = tmp & MSK_PKT_OFS;
    //
    Self->m_bExtIdn = CCanFrm_IsExtIdn(pFrm);
    Self->m_bRtrFrm = CCanFrm_IsRmt(pFrm);
    Self->m_bErrFrm = CCanFrm_IsErr(pFrm);
    //
    Self->m_Dlen    = CCanFrm_GetDlc(pFrm);
    Self->m_Buflen  = 0;
    Byte* p = CCanFrm_GetBufPtr(pFrm);
    Self->m_ReqCode = p[0];
    Self->m_RegId   = p[1];
    if (Self->m_ReqCode == SCC_REQ_SET) {
        Self->m_Flag = p[2];
        Self->m_Buflen = Self->m_Dlen -3;
        memcpy(Self->m_aBuf, &p[3], Self->m_Buflen);
    }
}

void CCnbPkt_ArcOut(CCnbPkt* Self, CCanFrm* pFrm)
{// Prepare CanFrame
    CCanFrm_SetOpt(pFrm, Self->m_bExtIdn, Self->m_bRtrFrm, Self->m_bErrFrm);

    uInt4 idn = Self->m_MstId;
    idn <<= iBITOFS_RCV_ID;
    idn &= 0xFF0000;
    idn |= (((uInt4)Self->m_ClnId << iBITOFS_SND_ID) & 0xFF00);
    uInt1 tmp = Self->m_iPktOfs;
    if (Self->m_bCont)
        tmp |= MSK_PKT_CONT_IND;
    if (Self->m_bErr)
        tmp |= MSK_PKT_ERR_IND;
    idn |= tmp;

    CCanFrm_SetIdn(pFrm, idn);
    //
    tmp = Self->m_ReqCode & 0x0F;
    Byte* p = CCanFrm_GetBufPtr(pFrm);
    if (Self->m_bErr) {
        p[0] = Self->m_ReqCode;
        p[1] = Self->m_Flag;
        Self->m_Buflen = 2;
    }
    else {
        if (tmp == SCC_REQ_GET_BUF) {
            memcpy(p, Self->m_aBuf, Self->m_Dlen);
        }
        else {
            p[0] = Self->m_ReqCode;
            p[1] = Self->m_RegId;
            p[2] = Self->m_Flag;
            if (Self->m_Buflen > 0)
                memcpy(&p[3], Self->m_aBuf, Self->m_Buflen);
        }
    }

    CCanFrm_SetDlc(pFrm, Self->m_Dlen);
}

uInt1 CCnbPkt_GetReqCode(CCnbPkt* Self)
{
    return Self->m_ReqCode;
}

uInt1 CCnbPkt_GetRegId(CCnbPkt* Self)
{
    return Self->m_RegId;
}

void CCnbPkt_SetReqCode(CCnbPkt* Self, uInt1 ReqCode)
{
    Self->m_ReqCode = ReqCode;
}

void CCnbPkt_SetRegId(CCnbPkt* Self, uInt1 RegId)
{
    Self->m_RegId = RegId;
}

void CCnbPkt_SetFlag(CCnbPkt* Self, uInt1 Flag)
{
    Self->m_Flag = Flag;
}

void CCnbPkt_SetErrFlag(CCnbPkt* Self, bool Flag)
{
    Self->m_bErr = Flag;
    Self->m_bCont = false;
    Self->m_iPktOfs = 0;
}

void CCnbPkt_SetDlen(CCnbPkt* Self, int Dlen)
{
    Self->m_Dlen = Dlen;
}

void CCnbPkt_SetBufLen(CCnbPkt* Self, int BufLen)
{
    Self->m_Buflen = BufLen;
}

int CCnbPkt_ArcInRegDef(CCnbPkt* Self, uInt1 ReqCode, CRegDef* pRegDef)
{
    Self->m_ReqCode = ReqCode;
    Self->m_RegId   = pRegDef->m_Id;

    int sts = CRegDat_ArcOut(&pRegDef->m_RegDat, Self->m_aBuf, &Self->m_Buflen);
    if (sts != OK)
        return sts;

    uInt1 flag;
    switch (pRegDef->m_RegDat.m_Dty)
    {
    case REG_TYPE_BIT   : flag = SCC_FLAG_DTY_BIT;  break;
    case REG_TYPE_I1    : flag = SCC_FLAG_DTY_I1;   break;
    case REG_TYPE_I2    : flag = SCC_FLAG_DTY_I2;   break;
    case REG_TYPE_I4    : flag = SCC_FLAG_DTY_I4;   break;
    case REG_TYPE_BMP   : flag = SCC_FLAG_DTY_BMP;  break;
    }

    Self->m_bErr = false;
    Self->m_bCont = false;
    Self->m_iPktOfs = 0;

    Self->m_Flag = flag; // To do some work
    Self->m_Dlen = Self->m_Buflen + 3;  // +3 -> ReqCode, RegId, Flag
    return OK;
}

int CCnbPkt_ArcOutRegDef(CCnbPkt* Self, CRegDef* pRegDef)
{
    pRegDef->m_Id = Self->m_RegId;
    uInt1 dtycode = Self->m_Flag & 0x07;
    Int1 dty = -1;
    switch (dtycode)
    {
    case SCC_FLAG_DTY_BIT   : dty = REG_TYPE_BIT;  break;
    case SCC_FLAG_DTY_I1    : dty = REG_TYPE_I1;   break;
    case SCC_FLAG_DTY_I2    : dty = REG_TYPE_I2;   break;
    case SCC_FLAG_DTY_I4    : dty = REG_TYPE_I4;   break;
    case SCC_FLAG_DTY_BMP   : dty = REG_TYPE_BMP;  break;
    }

    pRegDef->m_RegDat.m_Dty = dty;
    Self->m_bErr = false;
    Self->m_bCont = false;
    Self->m_iPktOfs = 0;

    return CRegDat_ArcIn(&pRegDef->m_RegDat, Self->m_aBuf, Self->m_Buflen);
}

int CCnbPkt_ArcOutScnBuf(CCnbPkt* Self, int BufNdx, Byte* pBuf, int BufLen, bool bCont)
{
    if (!Self)
        return ERR;
    if ((BufNdx < 0) || (BufLen <= 0) || (pBuf == NULL))
        return ERR;
    
    Self->m_bCont   = bCont;
    Self->m_iPktOfs = BufNdx;
    Self->m_bErr    = false;
    memcpy(Self->m_aBuf, pBuf, BufLen);
    Self->m_Dlen    = BufLen;
    return OK;
}

// ====================================================================
// CCnbHnd
// ====================================================================
void CCnbHnd_Init(CCnbHnd* Self, CCanDev* hChn, uInt1 NodeId,
     int (*pCbkGetReg)(int, CRegDef*), int (*pCbkSetReg)(int, CRegDef*), int (*pCbkGetBuf)(CScnBuf**))
{
    if (!Self)
        return;
    if (!hChn)
        return;
    Self->m_bEnb    = FALSE;
    Self->m_Stt     = CNB_STT_IDLE;
    //
    CCnbPkt_Init(&Self->m_TxPkt);
    CCnbPkt_Init(&Self->m_RxPkt);
    //
    Self->m_hChn        = hChn;
    CCanDev_SetRxFrmCbk(Self->m_hChn, Self, CCnbHnd_OnCbkRxFrm);
    //
    Self->m_NodeId      = NodeId;
    Self->m_pCbkGetReg  = pCbkGetReg;
    Self->m_pCbkSetReg  = pCbkSetReg;
    Self->m_pCbkGetBuf  = pCbkGetBuf;
}

int CCnbHnd_Start(CCnbHnd* Self)
{
    if (!Self->m_hChn)
        return ERR;
    Self->m_bEnb    = TRUE;
    Self->m_Stt     = CNB_STT_RXPWT;
    //
    CPollTmr_Init(&Self->m_TxWtTmr, PKT_TX_WTM);
    //
    return OK;
}

void CCnbHnd_Stop(CCnbHnd* Self)
{
    if (!Self->m_hChn)
        return;

    Self->m_bEnb    = FALSE;
    Self->m_Stt     = CNB_STT_IDLE;
}

// Cbk
void CCnbHnd_OnCbkRxFrm(CCnbHnd* Self)
{
    if (!Self)
        return;

    int sts = CCnbHnd_GetRxp(Self);
    if (sts != OK)
        return;

    sts = CCnbHnd_PrcRxp(Self);
//    if (sts != OK)
//        CCnbHnd_PrepErrPkt(Self, sts);

//    CCnbHnd_SendPkt(Self);
}

void CCnbHnd_Mon(CCnbHnd* Self)
{
    if (!Self->m_bEnb)
        return;
    int sts = UNKNOWN;
    Bool loop = TRUE;
    while (loop) {
        loop = FALSE;
        switch (Self->m_Stt) {
        case CNB_STT_IDLE:
            break;

        case CNB_STT_RXPWT:
            if (!CCnbHnd_IsPktAvl(Self))
                break;
            sts = CCnbHnd_GetRxp(Self);
            if (sts != OK)
                break;
            loop = TRUE;
            Self->m_Stt = CNB_STT_RXPPRC;
            break;
            
        case CNB_STT_RXPPRC:
            sts = CCnbHnd_PrcRxp(Self);
            if (sts != OK) 
                CCnbHnd_PrepErrPkt(Self, sts);           
            CPollTmr_Start(&Self->m_TxWtTmr);
            loop = TRUE;
            Self->m_Stt = CNB_STT_TXPWT;
            break;
            
        case CNB_STT_TXPWT:
            if (CCnbHnd_TxOvr(Self)) {
                Self->m_Stt = CNB_STT_OVR;
                break;
            }

            if (CPollTmr_IsTmout(&Self->m_TxWtTmr)) {
                Self->m_nTxTmout++;
                loop = TRUE;
                Self->m_Stt = CNB_STT_OVR;
            }
            break;

        case CNB_STT_OVR:
            Self->m_Stt = CNB_STT_RXPWT;
            break;
        }
    }
}

Bool CCnbHnd_IsPktAvl(CCnbHnd* Self)
{
    return CCanDev_IsFrmAvl(Self->m_hChn);
}

Bool CCnbHnd_TxOvr(CCnbHnd* Self)
{
    return CCanDev_IsTxEmpty(Self->m_hChn);
}

int CCnbHnd_GetRxp(CCnbHnd* Self)
{
    CCanFrm* pFrm = CCanDev_GetRxFrmPtr(Self->m_hChn);
    if (!pFrm)
        return ERR_INTERNAL;
    CCnbPkt_ArcIn(&Self->m_RxPkt, pFrm);
    CCanDev_AckFrm(Self->m_hChn);

    uInt1 clnid = CCnbPkt_GetClnId(&Self->m_RxPkt);
    if (clnid != Self->m_NodeId)
        return ERR;
    return OK;
}

int CCnbHnd_PrcRxp(CCnbHnd* Self)
{
    int sts = UNKNOWN;
    uInt1 reqCode = CCnbPkt_GetReqCode(&Self->m_RxPkt);    
    switch (reqCode) {
    case SCC_REQ_GET:
        sts = CCnbHnd_PrcGetReq(Self);
        break;
    case SCC_REQ_SET:
        sts = CCnbHnd_PrcSetReq(Self);
        break;
    case SCC_REQ_GET_BUF:
        sts = CCnbHnd_PrcGetBufReq(Self);
        break;
    default:
        sts = ERR_INV_REQ;
        break;
    }
    return sts;
}

int CCnbHnd_PrcGetReq(CCnbHnd* Self)
{
    if (!Self->m_pCbkGetReg)
        return REQSTS_INTERNAL_ERR;
    //
    uInt1 regId = CCnbPkt_GetRegId(&Self->m_RxPkt);
    int sts = Self->m_pCbkGetReg(regId, &Self->m_RegDef);
    if (sts != OK)
        return sts;
    // Prep Sts Code
    uInt1 reqCode = CCnbPkt_GetReqCode(&Self->m_RxPkt);
    reqCode |= SCC_RPY_MSK;
    sts = CCnbPkt_ArcInRegDef(&Self->m_TxPkt, reqCode, &Self->m_RegDef);
    if (sts != OK)
        return sts;

    CCnbHnd_SendPkt(Self);
    return OK;
}

int CCnbHnd_PrcSetReq(CCnbHnd* Self)
{
    if (!Self->m_pCbkSetReg)
        return REQSTS_INTERNAL_ERR;
    //
    int sts = CCnbPkt_ArcOutRegDef(&Self->m_RxPkt, &Self->m_RegDef);
    if (sts != OK)
        return sts;    

    uInt1 regId = CCnbPkt_GetRegId(&Self->m_RxPkt);
    sts = Self->m_pCbkSetReg(regId, &Self->m_RegDef);
    if (sts != OK)
        return sts;
    //
    uInt1 reqCode = CCnbPkt_GetReqCode(&Self->m_RxPkt);
    reqCode |= SCC_RPY_MSK;
    CCnbPkt_SetReqCode(&Self->m_TxPkt, reqCode);
    CCnbPkt_SetRegId(&Self->m_TxPkt, regId);
    CCnbPkt_SetFlag(&Self->m_TxPkt, REQSTS_OK);
    CCnbPkt_SetDlen(&Self->m_TxPkt, 3);
    CCnbPkt_SetBufLen(&Self->m_TxPkt, 0);
    //
    CCnbHnd_SendPkt(Self);
    return OK;
}

int CCnbHnd_PrcGetBufReq(CCnbHnd* Self)
{
    if (!Self->m_pCbkGetBuf)
        return REQSTS_INTERNAL_ERR;
    //
    int sts = Self->m_pCbkGetBuf(&Self->m_pScnBuf);
    if (sts != OK)
        return sts;
    if (!Self->m_pScnBuf)
        return ERR_INTERNAL;

    uInt1 reqCode = CCnbPkt_GetReqCode(&Self->m_RxPkt);
    reqCode |= SCC_RPY_MSK;
    CCnbPkt_SetReqCode(&Self->m_TxPkt, reqCode);

    int nbytes = Self->m_pScnBuf->m_nBytes;
    if (nbytes <= 0)
        return ERR_BUF_UNDEF;

    int nPkt = nbytes / SZ_CAN_DBUF;
    int rmnbyte = nbytes % SZ_CAN_DBUF;

    CScnBuf* pScnBuf = Self->m_pScnBuf;

    int i = 0;
    int ofs = 0;
    bool bCont = false;

    for (i = 0; i < nPkt; i++) {
        if ((rmnbyte == 0) && (i == (nPkt-1)))
             bCont = false;
        else bCont = true;

        CCnbHnd_WaitForBufTx(Self);
        CCnbPkt_ArcOutScnBuf(&Self->m_TxPkt, i, &pScnBuf->m_aBuf[ofs], SZ_CAN_DBUF, bCont);
        CCnbHnd_SendPkt(Self);
        ofs += SZ_CAN_DBUF;
    }

    if (rmnbyte > 0) {
        bCont = false;
        CCnbHnd_WaitForBufTx(Self);
        CCnbPkt_ArcOutScnBuf(&Self->m_TxPkt, i, &pScnBuf->m_aBuf[ofs], rmnbyte, false);
        CCnbHnd_SendPkt(Self);
    }

    return OK;
}

bool CCnbHnd_WaitForBufTx(CCnbHnd* Self)
{
    CPollTmr_Start(&Self->m_TxWtTmr);
    bool flag = false;
    while (true) {
        if (CCnbHnd_TxOvr(Self)) {
            flag = true;
            break;
        }
        if (CPollTmr_IsTmout(&Self->m_TxWtTmr)) {
            Self->m_nTxTmout++;
            flag = false;
            break;
        }
    }
    return flag;
}

void CCnbHnd_PrepErrPkt(CCnbHnd* Self, int Sts)
{
    uInt1 stsCode;
    switch (Sts) {
    case OK             : stsCode = REQSTS_OK;              break;
    case ERR_INV_REQ    : stsCode = REQSTS_CODE_UNDEF;      break;
    case ERR_INV_REG    : stsCode = REQSTS_REG_UNDEF;       break;
    case ERR_BUF_UNDEF  : stsCode = REQSTS_BUF_UNDEF;       break;
    case ERR_INV_REGTYPE: stsCode = REQSTS_REG_TYPE_ERR;    break;
    case ERR_INTERNAL   : stsCode = REQSTS_INTERNAL_ERR;    break;
    default             : stsCode = REQSTS_INTERNAL_ERR;    break;
    }

    uInt1 tmp = CCnbPkt_GetReqCode(&Self->m_RxPkt);
    tmp |= SCC_RPY_ERR;
    CCnbPkt_SetReqCode(&Self->m_TxPkt, tmp);
    tmp = CCnbPkt_GetReqCode(&Self->m_RxPkt);
    CCnbPkt_SetErrFlag(&Self->m_TxPkt, true);
//    CCnbPkt_SetRegId(&Self->m_TxPkt, tmp);
    CCnbPkt_SetFlag(&Self->m_TxPkt, stsCode);
    CCnbPkt_SetDlen(&Self->m_TxPkt, 2);
    CCnbPkt_SetBufLen(&Self->m_TxPkt, 0);
    CCnbHnd_SendPkt(Self);
}

void CCnbHnd_SendPkt(CCnbHnd* Self)
{
    // Prepare Hdr
    CCnbPkt_SetId(&Self->m_TxPkt, Self->m_RxPkt.m_MstId, Self->m_NodeId);
    CCnbPkt_SetIdnFlag(&Self->m_TxPkt, Self->m_RxPkt.m_bExtIdn,
                        Self->m_RxPkt.m_bRtrFrm, Self->m_RxPkt.m_bErrFrm);
    CCnbPkt_ArcOut(&Self->m_TxPkt, &Self->m_TxFrm);
    CCanDev_Wr(Self->m_hChn, &Self->m_TxFrm);
}

void CCnbHnd_UpdNode(CCnbHnd* Self, uInt1 NodeId)
{
    CCnbHnd_Stop(Self);
//    Self->m_bEnb = FALSE;
    //
    CCanFlt* pCanFlt = &Self->m_CanFlt;
    CCanFltMsk_Set(&pCanFlt->m_Msk, 0, CANFLT_MSK_CLN_ID);
    // CCanFltMsk_Set(&pCanFlt->m_Msk, 0, 0);
    uInt4 fltval = NodeId;
    fltval <<= iBITOFS_RCV_ID;
    fltval &= CANFLT_MSK_CLN_ID;
    CCanFltVal_Set(&pCanFlt->m_Val, 0, fltval);
    //
//    CCanFlt_UpdFltAtr(pCanFlt);
    CCanFlt_UpdFltAtr(pCanFlt, TRUE);
    //
    pCanFlt->m_bExtIdn  = TRUE;
    pCanFlt->m_bFrmCtrl = TRUE;
    
    Self->m_NodeId = NodeId;

    CCnbHnd_Start(Self);
    //Self->m_bEnb = TRUE;
}
//*********************************************************************
// End
//*********************************************************************

