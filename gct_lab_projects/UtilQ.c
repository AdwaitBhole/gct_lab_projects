//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************

#include "../include/CmnInc.h"

// ====================================================================
// CQue
// ====================================================================
void  CQue_Init(CQue* Self, Byte* pBuf, Int2 Len)
{
    Self->m_Len  = Len;
    Self->m_pBuf = pBuf;
    Self->m_zBuf = pBuf + Len;
    Self->m_iWr = Self->m_iRd = 0;
}

void CQue_Empty(CQue* Self)
{
    Self->m_iWr = Self->m_iRd = 0;
}

Bool CQue_IsWrSpace(CQue* Self)
{
    uInt2 ndx = Self->m_iWr + 1;
    if (ndx >= Self->m_Len)
	ndx = 0;
    return (ndx != Self->m_iRd) ? TRUE : FALSE;
}

Bool CQue_IsFull(CQue* Self)
{
    return (CQue_IsWrSpace(Self)) ? FALSE : TRUE;
}

Bool CQue_IsEmpty(CQue* Self)
{
    return (Self->m_iWr == Self->m_iRd) ? TRUE : FALSE;
}

uInt2 CQue_GetCnt(CQue* Self)
{
    if (CQue_IsEmpty(Self))
	return 0;
    uInt2 cnt=0;
    uInt2 ird = Self->m_iRd;
    uInt2 iwr = Self->m_iWr;
    while (1)
	{
	    if (ird == iwr)
		break;
	    cnt++;
	    ird++;
	    if (ird >= Self->m_Len)
		ird = 0;
	}
	return cnt;
}

Int1 CQue_UpdCnt(CQue* Self, Int2 Cnt)
{
    //	if (Cnt >= m_Cnt+m_Len)
    //	return ERR;
    //	m_Cnt = Cnt;
    return OK;
}

uInt2 CQue_Rd(CQue* Self, char* pCh)
{
    return CQue_RdN(Self,pCh, 1);
}

uInt2 CQue_RdN(CQue* Self, char* pBuf, Int2 Len)
{
    uInt2 cnt=0;
    while (Len > 0)
    {
	if (Self->m_iRd == Self->m_iWr)
	    break;
	pBuf[cnt] = Self->m_pBuf[Self->m_iRd];
        cnt++;
        Self->m_iRd++;
	if (Self->m_iRd >= Self->m_Len)
	    Self->m_iRd = 0;
	Len--;
    }
    return cnt;
}

uInt2 CQue_RdAll(CQue* Self, char* pBuf)
{
    uInt2 cnt=0;
    while (true)
	{
	    if (Self->m_iRd == Self->m_iWr)
		    break;
	    pBuf[cnt++] = Self->m_pBuf[Self->m_iRd++];
	    if (Self->m_iRd >= Self->m_Len)
		Self->m_iRd = 0;
	    if (cnt >= Self->m_Len)
		break;
	}
	return cnt;
}

uInt2 CQue_Wrt(CQue* Self, char Ch)
{
    return CQue_WrtN(Self, &Ch, 1);
}

uInt2 CQue_WrtN(CQue* Self, char* pBuf, Int2 Len)
{
    uInt2 cnt=0;
    if (!Len)
	return 0;
    while (Len > 0)
    {
        if (!CQue_IsWrSpace(Self))
            break;
        Self->m_pBuf[Self->m_iWr] = pBuf[cnt];
        cnt++;
        Self->m_iWr++;
        if (Self->m_iWr >= Self->m_Len)
            Self->m_iWr = 0;
        Len--;
    }
    return cnt;
}

//*********************************************************************
// End
//*********************************************************************

