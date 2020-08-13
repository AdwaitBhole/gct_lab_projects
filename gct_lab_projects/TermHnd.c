//*********************************************************************
// Title        : PTaC-CMN
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

void CTermHnd_Init(CTermHnd* Self)
{
    Self->m_hCom = NULL;
    //
    Self->m_bEnb        = FALSE;
    Self->m_Stt         = TERM_STT_IDLE;
    Self->m_RxpOfs      = 0;
    Self->m_bRxpEnb     = FALSE;
    Self->m_bRxpBusy    = FALSE;
}

void CTermHnd_Setup(CTermHnd* Self, CCom* hCom,
                    int (*pCbkGetReg)(int, char*, CStg*),
                    int (*pCbkSetReg)(char*, CStg*))
{
    CTermHnd_Init(Self);
    CStg_Init(&Self->m_sCmd, Self->m_CmdBuf, zTERM_BUF);
    CStg_Init(&Self->m_sPrm, Self->m_PrmBuf, zTERM_BUF);
    CStg_Init(&Self->m_sRpy, Self->m_RpyBuf, zTERM_BUF);
    Self->m_hCom = hCom;
    Self->m_pCbkGetReg = pCbkGetReg;
    Self->m_pCbkSetReg = pCbkSetReg;
    CPollTmr_Init(&Self->m_RxpIchTmr, DFL_ICH_WTM);
}

int CTermHnd_Start(CTermHnd* Self)
{
    if (!Self->m_hCom)
        return ERR;
    Self->m_Stt  = TERM_STT_START;
    Self->m_bEnb = TRUE;
    return OK;
}

void CTermHnd_Mon(CTermHnd* Self)
{
    if (!Self->m_bEnb)
        return;
    //
    CTermHnd_RxpHnd(Self);
    //
    Bool loop = TRUE;
    while (loop) {
        loop = FALSE;
        switch (Self->m_Stt) {
        case TERM_STT_IDLE:
            loop = FALSE;
            break;
        case TERM_STT_START:
            CTermHnd_RxpEnb(Self);
            Self->m_Stt = TERM_STT_WT_RXP;
            break;
        case TERM_STT_WT_RXP:
            if (Self->m_bRxpBusy)
                break;
            if (Self->m_RxpSts == ERR) 
                 Self->m_Stt = TERM_STT_START;
            else Self->m_Stt = TERM_STT_PRC_RXP;
            break;
        case TERM_STT_PRC_RXP:
            CTermHnd_OnPrcRxp(Self);
            Self->m_Stt = TERM_STT_START;
            break;
        } // switch
    } // while
}

void CTermHnd_ResetRxpStt(CTermHnd* Self)
{
    Self->m_RxpStt  = TERM_RXPSTT_IDLE;
    Self->m_RxpSts  = UNKNOWN;
    Self->m_RxpOfs  = 0;
    Self->m_bRxpEnb = FALSE;
    Self->m_bRxpBusy= FALSE;
}

void CTermHnd_RxpEnb(CTermHnd* Self)
{
    CTermHnd_ResetRxpStt(Self);
    Self->m_bRxpBusy= TRUE;
    Self->m_RxpStt  = TERM_RXPSTT_WT_FIRST_CH;
    Self->m_bRxpEnb = TRUE;
    CPollTmr_Start(&Self->m_RxpIchTmr);
}

void CTermHnd_RxpHnd(CTermHnd* Self)
{
    if (!Self->m_bRxpEnb) {
        CCom_RxBufEmpty(Self->m_hCom);
        return;
    }
    Bool loop = TRUE;
    Bool bRxc;
    Byte ch;
    while (loop) {
        bRxc = CCom_RxD(Self->m_hCom, &ch, 1);
        if (!bRxc) {
            if (CPollTmr_IsTmout(&Self->m_RxpIchTmr)) {
                //Self->m_RxpStt = TERM_RXPSTT_ERR;
            }
            break;
        }
        else CPollTmr_Start(&Self->m_RxpIchTmr);
        
        switch (Self->m_RxpStt) {
        case TERM_STT_IDLE:
            loop = FALSE;
            break;
        case TERM_RXPSTT_WT_FIRST_CH:
            if ((ch == TERM_EOP_CR) || (ch == TERM_EOP_NL))
                break;
            Self->m_RxpBuf[Self->m_RxpOfs++] = ch;
            Self->m_RxpStt = TERM_RXPSTT_WT_TRL;
            break;
        case TERM_RXPSTT_WT_TRL:
            if (Self->m_RxpOfs >= zTERM_BUF) {
                Self->m_RxpStt = TERM_RXPSTT_ERR;
            }
            else if ((ch == TERM_EOP_CR) || (ch == TERM_EOP_NL)) {
                Self->m_RxpStt = TERM_RXPSTT_OVR;
            }
            else {
                Self->m_RxpBuf[Self->m_RxpOfs++] = ch;
            }
            break;
        case TERM_RXPSTT_OVR:
            Self->m_bRxpBusy= FALSE;
            Self->m_bRxpEnb = FALSE;
            Self->m_RxpSts  = OK;
            Self->m_RxpStt  = TERM_RXPSTT_IDLE;
            break;
        case TERM_RXPSTT_ERR:
            Self->m_bRxpBusy= FALSE;
            Self->m_RxpOfs  = 0;
            Self->m_RxpSts  = ERR;
            Self->m_RxpStt  = TERM_RXPSTT_WT_FIRST_CH;
            break;
        }
    }
}

void CTermHnd_OnPrcRxp(CTermHnd* Self)
{
    int sts = CTermHnd_PrcRxp(Self);
    char* p = NULL;
    if (sts == OK) 
         p = "OK\r\n";
    else p = "ERROR\r\n";
    
    CStg_Empty(&Self->m_sRpy);
    CStg_SetStg(&Self->m_sRpy, p);
    CCom_TxStg(Self->m_hCom, &Self->m_sRpy);
}

int CTermHnd_PrcRxp(CTermHnd* Self)
{
    if (CTermHnd_ChkRxp(Self) != OK)
        return ERR;

    CStg_Empty(&Self->m_sCmd);
    CStg_SetStgWithLen(&Self->m_sCmd, (char*)Self->m_RxpBuf, Self->m_RxpOfs);
    int sts = UNKNOWN;
    if (CTermHnd_IsWrCmd(&Self->m_sCmd))
         sts = CTermHnd_PrcWrCmd(Self);
    else sts = CTermHnd_PrcRdCmd(Self);
    return sts;
}

int CTermHnd_ChkRxp(CTermHnd* Self)
{
    Byte* p = Self->m_RxpBuf;
    if (p[0] == '?')
        return OK;
    if (!isalpha(p[0]))
        return ERR;
    return OK;
}

Bool CTermHnd_IsWrCmd(CStg* pCmd)
{
    int ofs = CStg_Find(pCmd, '=');
    return (ofs > 0);
}

int CTermHnd_PrcRdCmd(CTermHnd* Self)
{
    if (!Self->m_pCbkGetReg)
        return ERR_INTERNAL;

    int type;
    //const char* pname = CStg_GetPtr(&Self->m_sCmd);
    char* pname = CStg_GetPtr(&Self->m_sCmd);
    Byte* p = Self->m_RxpBuf;
    if (p[0] == '?')  //Read all
        type = CMD_RD_ALL;
    else if (p[1] == '?')   // Read all reg match with first char
         type = CMD_RD_SPCFIC_ALL;
    else type = CMD_RD_SPCFIC;  // Read specific register
    
    return Self->m_pCbkGetReg(type, pname, &Self->m_sRpy);
}

int CTermHnd_PrcWrCmd(CTermHnd* Self)
{
    if (!Self->m_pCbkSetReg)
        return ERR;

    int ofs = CStg_Find(&Self->m_sCmd, '=');
    if (ofs <= 0)
        return ERR;
    int cnt = CStg_GetLen(&Self->m_sCmd);
    if (!cnt)
        return ERR;
    cnt = cnt - ofs - 1;
    CStg_Empty(&Self->m_sPrm);
    CStg_Right(&Self->m_sCmd, cnt, &Self->m_sPrm);
    CStg_SetAt(&Self->m_sCmd, ofs, 0);
    //const char* pname = CStg_GetPtr(&Self->m_sCmd);
    char* pname = CStg_GetPtr(&Self->m_sCmd);
    //
    return Self->m_pCbkSetReg(pname, &Self->m_sPrm);
}

void CTermHnd_SendStg(CTermHnd* Self, CStg* pStg)
{
    CCom_TxStg(Self->m_hCom, pStg);
}

//*********************************************************************
// End
//*********************************************************************
