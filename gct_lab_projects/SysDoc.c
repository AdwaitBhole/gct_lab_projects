//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

CCpu        gCpu;
CSys        gSys;
CSys*       pSYS  = &gSys;
CSysSts     gSysSts;
CSysSts*    pSysSts = &gSysSts;

CCom        gCom1;
CCom*       pCOM1 = &gCom1;
CCom        gCom2;
CCom*       pCOM2 = &gCom2;

CLogCtx     gCtx;


// Temp
uInt2 TstFlag;
uInt2 DbgVal1 = 0;
uInt2 DbgVal2 = 0;

TDbgInfo DbgInfo;
TDbgInfo* pDbgInfo = &DbgInfo;

// ====================================================================
// CSysDoc Utility
// ====================================================================

CCom* CSysDoc_GetCom(void* pChn)
{
    if (pCOM1->m_pRs232Chn == pChn)
         return pCOM1;
    else if (pCOM2->m_pRs232Chn == pChn)
         return pCOM2;
    else return NULL;
}

//*********************************************************************
// End
//*********************************************************************
