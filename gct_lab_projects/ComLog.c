//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

// ====================================================================
// CLogCtx
// ====================================================================
void CLogCtx_Init()
{
    gCtx.m_bEnb     = TRUE;
    gCtx.m_Lvl      = LOG_LVL_TRACE;
    gCtx.m_pLogCom  = NULL;
}

void CLogCtx_Setup(CCom* pLogCom)
{
    gCtx.m_pLogCom = pLogCom;
    CStg_Init(&gCtx.m_Msg, gCtx.m_BufMsg, sizeof(gCtx.m_BufMsg));
}

void CLogCtx_Enb()
{
    gCtx.m_bEnb     = TRUE;
}

void CLogCtx_Dsb()
{
    gCtx.m_bEnb     = FALSE;
}

void CLogCtx_SetCtx(char* pCtx, BOOL bEnb, int Lvl)
{
    gCtx.m_bEnb = bEnb;
    gCtx.m_Lvl = Lvl;
    gCtx.m_pCtx = pCtx;
}

void CLogCtx_Ok(const char* Fmt, ...)
{
    if (!gCtx.m_bEnb)
        return;
    if (gCtx.m_Lvl == LOG_LVL_ERR)
        return;
    
    va_list args;
    va_start(args, Fmt);
    vsprintf(gCtx.m_BufTmp, Fmt, args);
    va_end(args);
    CLogCtx_Wr(gCtx.m_BufTmp, LOGMSG_OK, OK);
}

void CLogCtx_Info(const char* Fmt, ...)
{
    if (!gCtx.m_bEnb)
        return;
    if (gCtx.m_Lvl != LOG_LVL_TRACE)
        return;
    
    va_list args;
    va_start(args, Fmt);
    vsprintf(gCtx.m_BufTmp, Fmt, args);
    va_end(args);
    CLogCtx_Wr(gCtx.m_BufTmp, LOGMSG_INFO, OK);
}

void CLogCtx_Err(const char* Fmt, ...)
{
    if (!gCtx.m_bEnb)
        return;
    if (gCtx.m_Lvl == LOG_LVL_OK)
        return;
    va_list args;
    va_start(args, Fmt);
    vsprintf(gCtx.m_BufTmp, Fmt, args);
    va_end(args);
    CLogCtx_Wr(gCtx.m_BufTmp, LOGMSG_ERR, ERR);
}

void CLogCtx_Wr(char* pMsg, ELogMsgType MsgType, int Sts)
{
    if (!gCtx.m_pLogCom)
        return;
    CStg_Empty(&gCtx.m_Msg);
    CStg_Add(&gCtx.m_Msg, gCtx.m_pCtx);
    CStg_Add(&gCtx.m_Msg, " ; ");
    if (MsgType == LOGMSG_OK) {
        CStg_Add(&gCtx.m_Msg, "OK");
    }
    else if (MsgType == LOGMSG_INFO) {
        CStg_Add(&gCtx.m_Msg, "Info");
    }
    else if (MsgType == LOGMSG_ERR) {
        sprintf(gCtx.m_Buf, "Err(%d)", Sts);
        CStg_Add(&gCtx.m_Msg, (char*)&gCtx.m_Buf);
    }

    CStg_Add(&gCtx.m_Msg, " ; ");
    CStg_Add(&gCtx.m_Msg, pMsg);

    CCom_TxStg(gCtx.m_pLogCom, &gCtx.m_Msg);
}

void CLogCtx_WrStg(char* pMsg)
{
    if (!gCtx.m_bEnb)
        return;
    CStg_Empty(&gCtx.m_Msg);
    CStg_SetStg(&gCtx.m_Msg, pMsg);
    CCom_TxStg(gCtx.m_pLogCom, &gCtx.m_Msg);
}

void CLogCtx_WrBuf(Byte* pBuf, int Len)
{
    if (!gCtx.m_bEnb)
        return;
    int i = 0;
    CStg_Empty(&gCtx.m_Msg);
    
    sprintf(gCtx.m_Buf, "Len = %02d \r\n", Len);
    CStg_Add(&gCtx.m_Msg, gCtx.m_Buf);
    CCom_TxStg(gCtx.m_pLogCom, &gCtx.m_Msg);

    CStg_Empty(&gCtx.m_Msg);
    for (i = 0; i < Len; i++) {
        sprintf(gCtx.m_Buf, "[%02X] ", pBuf[i]);
        CStg_Add(&gCtx.m_Msg, gCtx.m_Buf);
    }

    CStg_Add(&gCtx.m_Msg, "\r\n");
    CCom_TxStg(gCtx.m_pLogCom, &gCtx.m_Msg);
}

// ====================================================================
// CDbg
// ====================================================================
void CDbg_CtrlInit()
{
    CSys_GpioInit(DBG_GPIO_PORT1, DBG_GPIO_PIN1, PORT_DRN_INP);
    CSys_GpioInit(DBG_GPIO_PORT2, DBG_GPIO_PIN2, PORT_DRN_INP);
}

BOOL CDbg_IsPin1Lo()
{
    return CSys_GpioIsLo(DBG_GPIO_PORT1, DBG_GPIO_PIN1);
}

BOOL CDbg_IsPin2Lo()
{
    return CSys_GpioIsLo(DBG_GPIO_PORT2, DBG_GPIO_PIN2);
}

//*********************************************************************
// End
//*********************************************************************
