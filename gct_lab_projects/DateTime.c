//*********************************************************************
// Title        : PTaC
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************

#include "../include/CmnInc.h"

// ======================
// FilePvt Globals
// ======================
CTmrHnd	    _gTmrHnd;
CTckCntr    _gSysTck;

#define	    TMRHND	_gTmrHnd
#define	    SYSTCK	_gSysTck

// ====================================================================
// TTmr
// ====================================================================
void Tmr_Init(TTmr* Self, uInt1 Flag, Int2 Tc, void pCbk(void*), void* pObj)
{
    if (!Self)
	return;
    BitMsk8_SetMsk(&Self->m_Flag, Flag);
    Self->m_Tc	 = Tc;
    Self->m_Cnt	 = Tc;
    Self->m_pCbk = pCbk;
    Self->m_pObj = pObj;
}

void Tmr_Start( TTmr* Self)
{
    if (!Self) {
	return;
    }
    if (BitMsk8_IsSet(&Self->m_Flag, iMSK_TMR_INUSE))
	return;
    BitMsk8_Clr(&Self->m_Flag, iMSK_TMR_TMOUT);
    Self->m_Cnt = Self->m_Tc;
    (Self->m_Tc) ? Tmr_Enb(Self) : Tmr_Dsb(Self);
}

void Tmr_Stop(TTmr* Self)
{
    BitMsk8_Clr(&Self->m_Flag, iMSK_TMR_ENB);
}

void Tmr_Set(TTmr* Self, Int2 Tc)
{
    Self->m_Tc= Tc;
    Tmr_Start(Self);
}

Bool Tmr_IsCntAvl(TTmr* Self)
{
    return (Self->m_Cnt)? TRUE : FALSE;
}

void Tmr_Reload(TTmr* Self)
{
    Self->m_Cnt=Self->m_Tc;
}

Bool Tmr_IsAuto(TTmr* Self)
{
    return (BitMsk8_IsSet(&Self->m_Flag, iMSK_TMR_MODE)? FALSE :TRUE);
}

void Tmr_SetAuto(TTmr* Self)
{
    BitMsk8_Clr(&Self->m_Flag, iMSK_TMR_MODE);
}

void Tmr_SetMan(TTmr* Self)
{
    BitMsk8_SetVal(&Self->m_Flag, iMSK_TMR_MODE,TRUE);
}

void Tmr_CntUpd(TTmr* Self)
{
    if(Self->m_Cnt)
	Self->m_Cnt--;
}

Bool Tmr_IsAlloc(TTmr* Self)
{
    return BitMsk8_IsSet(&Self->m_Flag, iMSK_TMR_INUSE);
}

void Tmr_Alloc(TTmr* Self)
{
    BitMsk8_SetVal(&Self->m_Flag, iMSK_TMR_INUSE,TRUE);
}

void Tmr_Free(TTmr* Self)
{
    BitMsk8_Clr(&Self->m_Flag, iMSK_TMR_INUSE);
}

Bool Tmr_IsEnb(TTmr* Self)
{
    return BitMsk8_IsSet(&Self->m_Flag, iMSK_TMR_ENB);
}

void Tmr_Enb(TTmr* Self)
{
    BitMsk8_SetVal(&Self->m_Flag, iMSK_TMR_ENB,TRUE);
}

void Tmr_Dsb(TTmr* Self)
{
    BitMsk8_Clr(&Self->m_Flag, iMSK_TMR_ENB);
}

Bool Tmr_IsTmOut(TTmr* Self)
{
    return BitMsk8_IsSet(&Self->m_Flag, iMSK_TMR_TMOUT);
}

void Tmr_SetTmOut(TTmr* Self)
{
    BitMsk8_SetVal(&Self->m_Flag, iMSK_TMR_TMOUT,TRUE);
}

void Tmr_ClrTmOut(TTmr* Self)
{
    BitMsk8_Clr(&Self->m_Flag, iMSK_TMR_TMOUT);
}

Bool Tmr_IsCbk(TTmr* Self)
{
    return (BitMsk8_IsSet(&Self->m_Flag, (iMSK_TMR_TMOUT)&& Self->m_pCbk) ? TRUE :FALSE) ;
}

void Tmr_SetCbk(TTmr* Self, Int1 bCbk, void pCbk(void* pObj), void* pObj)
{
    (bCbk) ? BitMsk8_SetVal(&Self->m_Flag, iMSK_TMR_CBK,TRUE) :  BitMsk8_Clr(&Self->m_Flag, iMSK_TMR_CBK);
    Self->m_pCbk = pCbk;
    Self->m_pObj  = pObj;
}

void Tmr_CbkEnb(TTmr* Self)
{
    BitMsk8_SetVal(&Self->m_Flag, iMSK_TMR_CBK,TRUE);
}

void Tmr_CbkDsb(TTmr* Self)
{
    BitMsk8_Clr(&Self->m_Flag, iMSK_TMR_CBK);
}

void Tmr_RmvCbk(TTmr* Self)
{
    Self->m_pCbk = NULL;
}

void Tmr_OnCbk(TTmr* Self)
{
    Self->m_pCbk(Self->m_pObj);
}

// ====================================================================
// CTmrHnd
// ====================================================================

void CTmrHnd_Init()
{
    int i;
    for(i = 0; i <= MAX_TMR; i++)
	Tmr_Free(&TMRHND.m_aTmr[i]);
    TMRHND.m_nTmr = 0;
}

void CTmrHnd_InitTmr(int iTmr, uInt1 Flag, Int2 Tc, void pCbk(void*), void* pObj)
{
    Tmr_Init(&TMRHND.m_aTmr[iTmr], Flag, Tc, pCbk, pObj);
}

int CTmrHnd_Start(int iTmr)
{
    if (iTmr >= MAX_TMR)
	return ERR;
    Tmr_Start(&TMRHND.m_aTmr[iTmr]);
    return OK;
}

int CTmrHnd_StartTc(int iTmr, Int2 Tc)
{
    if (iTmr >= MAX_TMR)
	return ERR;
    TTmr* pTmr = &TMRHND.m_aTmr[iTmr];
    Tmr_Init(pTmr, MSK_TMR_MAN, Tc, NULL, NULL);
    Tmr_Start(pTmr);
    return OK;
}

int CTmrHnd_Stop(int iTmr)
{
    if (iTmr >= MAX_TMR)
	return ERR;
    Tmr_Stop(&TMRHND.m_aTmr[iTmr]);
    return OK;
}

void CTmrHnd_UpdOnTck()
{
    int i;
    for (i = 0; i < MAX_TMR; i++) {
	if(( Tmr_IsAlloc(&TMRHND.m_aTmr[i])) || (Tmr_IsEnb(&TMRHND.m_aTmr[i])))
	    continue;
	CTmrHnd_Upd(i);
    }
}

void CTmrHnd_Upd(int iTmr)
{
    Tmr_CntUpd(&TMRHND.m_aTmr[iTmr]);
    if(Tmr_IsCntAvl(&TMRHND.m_aTmr[iTmr]))
	return;
    if(Tmr_IsAuto(&TMRHND.m_aTmr[iTmr]))
	 Tmr_Reload(&TMRHND.m_aTmr[iTmr]);
    else Tmr_Dsb(&TMRHND.m_aTmr[iTmr]);
    // Timeout !
    Tmr_SetTmOut(&TMRHND.m_aTmr[iTmr]);
    if(Tmr_IsCbk(&TMRHND.m_aTmr[iTmr]))
	Tmr_OnCbk(&TMRHND.m_aTmr[iTmr]);
}

int CTmrHnd_Alloc()
{
    if (TMRHND.m_nTmr >= MAX_TMR)
	return -1;
    int i;
    for (i = 0 ; i < MAX_TMR; i++) {
	if(Tmr_IsAlloc(&TMRHND.m_aTmr[i]))
	    break;
    }
    Tmr_Alloc(&TMRHND.m_aTmr[i]);
    TMRHND.m_nTmr++;
    return i;
}

void CTmrHnd_Free(int iTmr)
{
    if (iTmr >= MAX_TMR)
	return;
    Tmr_Stop(&TMRHND.m_aTmr[iTmr]);
    Tmr_Free(&TMRHND.m_aTmr[iTmr]);
    TMRHND.m_nTmr--;
}

int CTmrHnd_Set(int iTmr, Int2 Tc)
{
    if (iTmr >= MAX_TMR)
	return ERR;
    Tmr_Set(&TMRHND.m_aTmr[iTmr],  Tc);
    return OK;
}

int CTmrHnd_SetCbk(int iTmr, void pCbk(void*), void* pObj)
{
    if (iTmr >= MAX_TMR)
	return ERR;
    Tmr_SetCbk(&TMRHND.m_aTmr[iTmr],TRUE, pCbk ,  pObj);
    return OK;
}

int CTmrHnd_IsTmout(int iTmr)
{
    if (iTmr >= MAX_TMR)
	return ERR;
    return Tmr_IsTmOut(&TMRHND.m_aTmr[iTmr]);
}

int CTmrHnd_WaitForTmout(int iTmr)
{
    if (iTmr >= MAX_TMR)
	return ERR;
    while (!Tmr_IsTmOut(&TMRHND.m_aTmr[iTmr]));
    return OK;
}

int CTmrHnd_DelayS(Int2 Wtm)
{
    Int2 i;
    if (!Wtm)
	return OK;
    Wtm = Wtm * 1000;
    for ( i= 0; i < Wtm; i++) {
	if (CTmrHnd_DelayMs(1) != OK) {
	    return ERR;
	}
    }
    return OK;
}

int CTmrHnd_DelayMs(Int2 Wtm)
{    
    if (!Wtm)
	return OK;
    int tid = CTmrHnd_Alloc();
    if (tid < 0)
	return ERR;
    Wtm = Wtm * TC_1MS;
    CTmrHnd_InitTmr(tid ,(MSK_TMR_MAN), Wtm, NULL, NULL);
    CTmrHnd_Start(tid);
    CTmrHnd_WaitForTmout(tid);
    CTmrHnd_Free(tid);
    return OK;
}

// ====================================================================
// TTckCntr
// ====================================================================

void CTckCntr_Init(CTckCntr* Self)
{
    Self->m_nSec = 0;
    Self->m_nMsec = 0;
}

void CTckCntr_Set(CTckCntr* SelfDst, CTckCntr* SelfSrc)
{
    SelfDst->m_nSec = SelfSrc->m_nSec;
    SelfDst->m_nMsec = SelfSrc->m_nMsec;
}

void CTckCntr_OnTckIsr(CTckCntr* Self)
{
    // Upd in Isr Only
    uInt2 msec = Self->m_nMsec + SYS_TCK;
    if (msec >= 1000) {
	Self->m_nSec++;
	msec -= 1000;
    }
    Self->m_nMsec = msec;
}

void CTckCntr_Add(CTckCntr* Self, Int4 nMsec)
{
    Int4 msec = nMsec + Self->m_nMsec;
    Self->m_nMsec  =  msec % 1000;
    Self->m_nSec  += (msec / 1000);
}

int CTckCntr_Cmp(CTckCntr* Self1, CTckCntr* Self2)
{

    // Return: -1 : Self1  < Given
    //		0 : Self1 == Given
    //		1 : Self1  > Given

    if (Self1->m_nSec == Self2->m_nSec) {
	if (Self1->m_nMsec == Self2->m_nMsec)
	    return 0;
	return (Self1->m_nMsec < Self2->m_nMsec) ? -1 : 1;
    }
    return (Self1->m_nSec < Self2->m_nSec) ? -1 : 1;
}

// =======
// SysTck
// =======
void SysTck_Init()
{
    CTckCntr_Init(&SYSTCK);
}

void SysTck_OnTckIsr()
{
    CTckCntr_OnTckIsr(&SYSTCK);
}

// ====================================================================
// CPollTmr
// ====================================================================
void CPollTmr_Init(CPollTmr* Self, Int4 TmCnt)
{
    Self->m_bEnb = FALSE;
    Self->m_TmCnt = TmCnt;
}

void CPollTmr_Start(CPollTmr* Self)
{
    Self->m_bEnb = TRUE;
    CTckCntr_Set(&Self->m_FinTck, &SYSTCK);
    CTckCntr_Add(&Self->m_FinTck, Self->m_TmCnt);
}

void CPollTmr_Restart(CPollTmr* Self, Int4 TmCnt)
{
    if (!Self)
        return;
    CPollTmr_Init(Self, TmCnt);
    CPollTmr_Start(Self);
}

Bool CPollTmr_IsTmout(CPollTmr* Self)
{
    if (!Self->m_bEnb)
        return FALSE;
    int flag = CTckCntr_Cmp(&SYSTCK, &Self->m_FinTck);
//    int flag = TTckCntr_Cmp(&Self->m_FinTck, &SYSTCK);
    return (flag >= 0) ? TRUE : FALSE;
}

// ====================================================================
// Gen
// ====================================================================
void WaitSec(Int4 Sec)
{
    Int4 msec = TC_1SEC * Sec;
    WaitMsec(msec);
}

void WaitMsec(Int4 Msec)
{
    CPollTmr tmr;
    CPollTmr_Restart(&tmr, Msec);
    while(!CPollTmr_IsTmout(&tmr))
        ;
}


//*********************************************************************
// End
//*********************************************************************

