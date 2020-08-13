//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************

#include "../include/CmnInc.h"

// ====================================================================
// CCom
// ====================================================================

void CCom_Init(CCom* Self, int Id, CRs232Chn* pChn)
{
    Self->m_pRs232Chn = NULL;
    // Initialize channels
    CQue_Init(&Self->m_Txq, (Byte*)Self->m_TxqBuf, (Int2)zCOM_BUF);
    CQue_Init(&Self->m_Rxq, Self->m_RxqBuf, zCOM_BUF);
    //
    Self->m_Id      = Id;
    Self->m_pRs232Chn = pChn;
    Self->m_bTxEmpty = TRUE;
    Self->m_bOpen = FALSE;
    CCom_SetCbk(Self, NULL, NULL);
}

void CCom_SetCbk(CCom* Self, void pCbk(void*), void* pObj)
{
    Self->m_pCbk = pCbk;
    Self->m_pObj = pObj;
}

void CCom_BufEmpty(CCom* Self)
{
    CCom_TxBufEmpty(Self);
    CCom_RxBufEmpty(Self);
}

void CCom_TxBufEmpty(CCom* Self)
{
     CQue_Empty(&Self->m_Txq);
     Self->m_bTxEmpty = TRUE;
}

void CCom_RxBufEmpty(CCom* Self)
{
    CQue_Empty(&Self->m_Rxq);
}

Int2 CCom_TxStg(CCom* Self, CStg* pStg)
{
    CQue_Empty(&Self->m_Txq);
    int len = CStg_GetLen(pStg);
    Byte* p = (Byte*)CStg_GetPtr(pStg);
    CCom_TxDat(Self, p, len);
    while (!CCom_IsTxOvr(Self));
    return OK;
}

Int2 CCom_TxDat(CCom* Self, Byte* pBuf, Int2 Len)
{
    if (!Self->m_bTxEmpty)
        return 0;

    int cnt = CQue_WrtN(&Self->m_Txq, (char *)pBuf, Len);
    if (!cnt)
	return 0;

    CRs232Chn_TxD(Self->m_pRs232Chn, &Self->m_Txq);
    Self->m_bTxEmpty = FALSE;
    return cnt;
}

BOOL CCom_IsTxOvr(CCom* Self)
{
    return Self->m_bTxEmpty;
}

Bool CCom_IsTxcTmout(CCom* Self)
{
    return Self->m_bTxEmpty;
}

BOOL CCom_IsRxAvl(CCom* Self)
{
    return (!CQue_IsEmpty(&Self->m_Rxq));
}

CQue* CCom_GetRxq(CCom* Self)
{
    return &Self->m_Rxq;
}

CQue* CCom_GetTxq(CCom* Self)
{
    return &Self->m_Txq;
}

Int2 CCom_RxAvlCnt(CCom* Self)
{
    return CQue_GetCnt(&Self->m_Rxq);
}

BOOL CCom_IsRxDatAvl(CCom* Self, int Cnt)
{
    int rxcnt = CQue_GetCnt(&Self->m_Rxq);
    return (rxcnt >= Cnt) ? TRUE : FALSE;
}

BOOL CCom_RxD(CCom* Self, Byte* pBuf, Int2 Cnt)
{// Read availabel bytes (==Cnt)
    BOOL flag = FALSE;
    if (CCom_IsRxDatAvl(Self, Cnt)) {
        CQue_RdN(&Self->m_Rxq, (char*)pBuf, Cnt);
        flag = TRUE;
    }
    return flag;
}

Int2 CCom_RxDAll(CCom* Self, Byte* pBuf)
{
    if (CCom_IsRxAvl(Self))
	return CQue_RdAll(&Self->m_Rxq, (char*)pBuf);
    return 0;
}

Int2 CCom_RxD2(CCom* Self, Byte* pBuf, Int2 Cnt)
{
    if (CCom_IsRxAvl(Self))
	return CQue_RdN(&Self->m_Rxq, (char*)pBuf, Cnt);
    return 0;
}

void CCom_TxOnChn(CCom* Self)
{
    if (CQue_IsEmpty(&Self->m_Txq)) {
    	Self->m_bTxEmpty = TRUE;
	return;
    }
    CRs232Chn_TxD(Self->m_pRs232Chn, &Self->m_Txq);
    Self->m_bTxEmpty = FALSE;
}

// ISR Ifc
// =======

void CCom_OnRx(CRs232Chn* pChn)
{
    CCom* pCom = CSysDoc_GetCom(pChn);
    if (!pCom)
        return;
    CRs232Chn_GetRxc(pChn, &pCom->m_Rxq);
}

void CCom_OnTxEmpty(CRs232Chn* pChn)
{// Called by ISR on Tx Empty
    CCom* pCom = CSysDoc_GetCom(pChn);
    if (!pCom)
        return;
    pCom->m_bTxEmpty = FALSE;
    CCom_TxOnChn(pCom);
}

//*********************************************************************
// End
//*********************************************************************
