//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

int CSys_CanBusInit()
{
    CCpu_CanReset();
    // Step-1 = Set Config Mode
    // Step-2 = Setup Baudrate
    // Step-3 = Setup filter mask
    // Step-4 = Set Sid/Eid
    // Step-5 = Enable Filter
    // Step-6 = Configure DMA
    // Step-7 = Set Normal Mode
    CSys_CanBusSetOpModeConfig();
    // System Initialisation
    CCpu_CanBaudInit();
    // Test Mode
    // DMA-0 allocated for ECAN-Tx
    // DMA-1 allocated for ECAN-Rx
    // Buffer-0,1 for Tx
    // Buffer-2,3,4,5 for Rx
    // =================
    // Tx Initialisation
    // =================
    // DMA config
    CCpu_CanSelBufWin();
    CSys_CanBusDmaInit(ESYS_ECAN_DMA_TX, DMACON_DRN_RAM_TO_PER, DMAREQ_IRQ_ECAN1_TX, &C1TXD);
    CCpu_DmaSetIsrMsk(ESYS_ECAN_DMA_TX, TRUE);
    //
//    CSys_CanBusRxBufInit(ESYS_ECAN_TOT_BUF, ESYS_ECAN_TOT_BUF-1);
    CSys_CanBusRxBufInit(ESYS_ECAN_N_BUF, ESYS_ECAN_FIFO_STBUF);
    // Buf-0,1 Txbuf
    CSys_CanBusSetBufTrxMode(ESYS_ECAN_TX_BUFNUM0, CAN_BUF_TXENB, ESYS_ECAN_TX_BUFPRIO);
    CSys_CanBusSetBufTrxMode(ESYS_ECAN_TX_BUFNUM1, CAN_BUF_TXENB, ESYS_ECAN_TX_BUFPRIO);
    CCpu_CanSelBufWin();
    // =================
    // Rx Initialisation
    // =================
    CCpu_CanSelBufWin();
    CSys_CanBusDmaInit(ESYS_ECAN_DMA_RX, DMACON_DRN_PER_TO_RAM, DMAREQ_IRQ_ECAN1_RX, &C1RXD);
    CCpu_DmaSetIsrMsk(ESYS_ECAN_DMA_RX, TRUE);
    int i = 0;
    for (i = 0; i < ESYS_ECAN_N_RXBUF; i++)
        CSys_CanBusSetBufTrxMode((ESYS_ECAN_FIFO_STBUF+i), CAN_BUF_RXENB, 0);
    // Buf-2,3,4,5 Rxbuf
//    CSys_CanBusSetBufTrxMode(ESYS_ECAN_RX_BUFNUM0, CAN_BUF_RXENB, 0);  // Rx enable
//    CSys_CanBusSetBufTrxMode(ESYS_ECAN_RX_BUFNUM1, CAN_BUF_RXENB, 0);  // Rx enable
//    CSys_CanBusSetBufTrxMode(ESYS_ECAN_RX_BUFNUM2, CAN_BUF_RXENB, 0);  // Rx enable
//    CSys_CanBusSetBufTrxMode(ESYS_ECAN_RX_BUFNUM3, CAN_BUF_RXENB, 0);  // Rx enable
    // FltMskNum=0, Accept any (SID or EID),
//    CSys_CanBusInitFltMsk(ESYS_ECAN_FLTMSKNUM, FALSE, ESYS_ECAN_MSK_SID, ESYS_ECAN_MSK_EID);
    CSys_CanBusInitFltMsk(ESYS_ECAN_FLTMSKNUM, FALSE, ESYS_ECAN_MSK_SID, ESYS_ECAN_MSK_EID);
    // Fifo enable for receive buf
    CSys_CanBusAtchFltMskNumBufNum(ESYS_ECAN_FLT0, ESYS_ECAN_FLTMSKNUM, ESYS_ECAN_FLT_FIFO_ENB);
    // Config Flt-0,1,2,3 attached with FltMsknum-0 and Buf-2,3,4,5 resp
//    CSys_CanBusAtchFltMskNumBufNum(ESYS_ECAN_FLT0, ESYS_ECAN_FLTMSKNUM, ESYS_ECAN_RX_BUFNUM0);
//    CSys_CanBusAtchFltMskNumBufNum(ESYS_ECAN_FLT1, ESYS_ECAN_FLTMSKNUM, ESYS_ECAN_RX_BUFNUM1);
//    CSys_CanBusAtchFltMskNumBufNum(ESYS_ECAN_FLT2, ESYS_ECAN_FLTMSKNUM, ESYS_ECAN_RX_BUFNUM2);
//    CSys_CanBusAtchFltMskNumBufNum(ESYS_ECAN_FLT3, ESYS_ECAN_FLTMSKNUM, ESYS_ECAN_RX_BUFNUM3);
    // Default: FltNum=2, FltVal=1=>NodeId
    CSys_CanBusSetMsgId(ESYS_ECAN_FLT0, TRUE, ESYS_ECAN_DFL_SID, ESYS_ECAN_DFL_EID);
//    CSys_CanBusSetMsgId(ESYS_ECAN_FLT1, TRUE, ESYS_ECAN_DFL_SID, ESYS_ECAN_DFL_EID);
//    CSys_CanBusSetMsgId(ESYS_ECAN_FLT2, TRUE, ESYS_ECAN_DFL_SID, ESYS_ECAN_DFL_EID);
//    CSys_CanBusSetMsgId(ESYS_ECAN_FLT3, TRUE, ESYS_ECAN_DFL_SID, ESYS_ECAN_DFL_EID);
    //
    CCpu_CanSetIsr();
    // CSys_CanBusSetOpModeLpbk();
    // Set Normal Mode
    CSys_CanBusSetOpModeNorm();
    return OK;
}

int CSys_CanBusInit_X()
{
    // Step-1 = Set Config Mode
    // Step-2 = Setup Baudrate
    // Step-3 = Setup filter mask
    // Step-4 = Set Sid/Eid
    // Step-5 = Enable Filter
    // Step-6 = Configure DMA
    // Step-7 = Set Normal Mode
    CSys_CanBusSetOpModeConfig();
    // System Initialisation
    CCpu_CanBaudInit();
    // Test Mode
    // DMA-0 allocated for ECAN-Tx
    // DMA-1 allocated for ECAN-Rx
    // Buffer-0,1 for Tx
    // Buffer-2,3,4,5 for Rx
    // =================
    // Tx Initialisation
    // =================
    // DMA config
    CCpu_CanSelBufWin();
    CSys_CanBusDmaInit(ESYS_ECAN_DMA_TX, DMACON_DRN_RAM_TO_PER, DMAREQ_IRQ_ECAN1_TX, &C1TXD);
    CCpu_DmaSetIsrMsk(ESYS_ECAN_DMA_TX, TRUE);
    //
    CSys_CanBusRxBufInit(ESYS_ECAN_TOT_BUF, ESYS_ECAN_TOT_BUF-1);
    // Buf-0,1 Txbuf
    CSys_CanBusSetBufTrxMode(ESYS_ECAN_TX_BUFNUM0, CAN_BUF_TXENB, ESYS_ECAN_TX_BUFPRIO);
    CSys_CanBusSetBufTrxMode(ESYS_ECAN_TX_BUFNUM1, CAN_BUF_TXENB, ESYS_ECAN_TX_BUFPRIO);
    CCpu_CanSelBufWin();
    // =================
    // Rx Initialisation
    // =================
    CCpu_CanSelBufWin();
    CSys_CanBusDmaInit(ESYS_ECAN_DMA_RX, DMACON_DRN_PER_TO_RAM, DMAREQ_IRQ_ECAN1_RX, &C1RXD);
    CCpu_DmaSetIsrMsk(ESYS_ECAN_DMA_RX, TRUE);
    // Buf-2,3,4,5 Rxbuf
    CSys_CanBusSetBufTrxMode(ESYS_ECAN_RX_BUFNUM0, CAN_BUF_RXENB, 0);  // Rx enable
    CSys_CanBusSetBufTrxMode(ESYS_ECAN_RX_BUFNUM1, CAN_BUF_RXENB, 0);  // Rx enable
    CSys_CanBusSetBufTrxMode(ESYS_ECAN_RX_BUFNUM2, CAN_BUF_RXENB, 0);  // Rx enable
    CSys_CanBusSetBufTrxMode(ESYS_ECAN_RX_BUFNUM3, CAN_BUF_RXENB, 0);  // Rx enable
    // FltMskNum=0, Accept any (SID or EID),
    CSys_CanBusInitFltMsk(ESYS_ECAN_FLTMSKNUM, FALSE, ESYS_ECAN_MSK_SID, ESYS_ECAN_MSK_EID);
    // Config Flt-0,1,2,3 attached with FltMsknum-0 and Buf-2,3,4,5 resp
    CSys_CanBusAtchFltMskNumBufNum(ESYS_ECAN_FLT0, ESYS_ECAN_FLTMSKNUM, ESYS_ECAN_RX_BUFNUM0);
    CSys_CanBusAtchFltMskNumBufNum(ESYS_ECAN_FLT1, ESYS_ECAN_FLTMSKNUM, ESYS_ECAN_RX_BUFNUM1);
    CSys_CanBusAtchFltMskNumBufNum(ESYS_ECAN_FLT2, ESYS_ECAN_FLTMSKNUM, ESYS_ECAN_RX_BUFNUM2);
    CSys_CanBusAtchFltMskNumBufNum(ESYS_ECAN_FLT3, ESYS_ECAN_FLTMSKNUM, ESYS_ECAN_RX_BUFNUM3);
    // Default: FltNum=2, FltVal=1=>NodeId
    CSys_CanBusSetMsgId(ESYS_ECAN_FLT0, TRUE, ESYS_ECAN_DFL_SID, ESYS_ECAN_DFL_EID);
    CSys_CanBusSetMsgId(ESYS_ECAN_FLT1, TRUE, ESYS_ECAN_DFL_SID, ESYS_ECAN_DFL_EID);
    CSys_CanBusSetMsgId(ESYS_ECAN_FLT2, TRUE, ESYS_ECAN_DFL_SID, ESYS_ECAN_DFL_EID);
    CSys_CanBusSetMsgId(ESYS_ECAN_FLT3, TRUE, ESYS_ECAN_DFL_SID, ESYS_ECAN_DFL_EID);
    //
    CCpu_CanSetIsr();
    // CSys_CanBusSetOpModeLpbk();
    // Set Normal Mode
    CSys_CanBusSetOpModeNorm();
    return OK;
}

void CSys_CanBusSetOpModeConfig()
{
    CCpu_CanSetOpMode(CAN_OPMODE_CNF);
}

void CSys_CanBusSetOpModeNorm()
{
    CCpu_CanSetOpMode(CAN_OPMODE_NORM);
}

void CSys_CanBusSetOpModeLpbk()
{
    CCpu_CanSetOpMode(CAN_OPMODE_LPBK);
}

int CSys_CanBusRxBufInit(int nBuf, int FifoStAdr)
{// FifoStAdr => last bufnum disable FIFO
    if (!CCpu_CanIsOpMode(CAN_OPMODE_CNF))
        return ERR;
    int nbufcode = -1;
    switch (nBuf) {
        case 4:  nbufcode = CAN_4_BUF;     break;
        case 6:  nbufcode = CAN_6_BUF;     break;
        case 8:  nbufcode = CAN_8_BUF;     break;
        case 12: nbufcode = CAN_12_BUF;    break;
        case 16: nbufcode = CAN_16_BUF;    break;
        case 24: nbufcode = CAN_24_BUF;    break;
        case 32: nbufcode = CAN_32_BUF;    break;
        default: nbufcode = -1;             break;
    }
    if (nbufcode < 0)
        return ERR;
    CCpu_CanFifoCtrl(nbufcode, FifoStAdr);
    return OK;
}

int CSys_CanBusSetBufTrxMode(int BufNum, BOOL bTrxMode, int Prio)
{   // Buffer Transfer Mode
    // bTrxMode:1 => Tx Buffer
    // bTrxMode:0 => Rx Buffer
    if (!CCpu_CanIsOpMode(CAN_OPMODE_CNF))
        return ERR;
    CCpu_CanBufTrxMode(BufNum, bTrxMode, Prio);
    return OK;
}

int CSys_CanBusInitFltMsk(int FltMskNum, BOOL bMide, uInt2 SidMsk, uInt4 EidMsk)
{
    // Initialises FltMsk, BufPtr
    // bMide: Accept restricted Msg i.e. either SID or EID
    if (!CCpu_CanIsOpMode(CAN_OPMODE_CNF))
        return ERR;
//    uInt2 eidmsk_17_16 = (EidMsk >> 15) & 0x03;
    uInt2 eidmsk_17_16 = (EidMsk >> 16) & 0x03;
    uInt2 eidmsk_15_00 = EidMsk & 0xFFFF;
    CCpu_CanSetFltMskSidReg(FltMskNum, SidMsk, bMide, eidmsk_17_16);
    CCpu_CanSetFltMskEidReg(FltMskNum, eidmsk_15_00);
    return OK;
}

int CSys_CanBusAtchFltMskNumBufNum(int FltNum, int FltMskNum, int BufNum)
{
    if (!CCpu_CanIsOpMode(CAN_OPMODE_CNF))
        return ERR;
    // Select Maskt number
    CCpu_CanFltAtchMskNum(FltNum, FltMskNum);
    // Set Flt Sid
    CCpu_CanFltAtchBufPtr(FltNum, BufNum);
    return OK;
}

int CSys_CanBusSetMsgId(int FltNum, BOOL bExIde, uInt2 Sid, uInt4 Eid)
{
    if (!CCpu_CanIsOpMode(CAN_OPMODE_CNF))
        return ERR;
    uInt2 eid_17_16 = (Eid >> 16) & 0x03;
    uInt2 eid_15_00 = Eid & 0xFFFF;
    CCpu_CanSetFltSidReg(FltNum, Sid, bExIde, eid_17_16);
    CCpu_CanSetFltEidReg(FltNum, eid_15_00);
    // Enable Flt
    CCpu_CanEnbFlt(FltNum, TRUE);
    return OK;
}

void CSys_CanBusDmaInit(int DmaChn, int Drn, int IntReqId, volatile unsigned int* pPer)
{   // DMA Allocation
    CCpu_DmaReqSetTrxMode(DmaChn, DMAREQ_TXMODE_AUTO);
    CCpu_DmaConSetTrxSz(DmaChn, DMACON_TXSZ_WORD);
    CCpu_DmaConSetTrxDrn(DmaChn, Drn);
    CCpu_DmaConSetAdrMode(DmaChn, DMACON_AMODE_PER_IND);
    CCpu_DmaConSetOpMode(DmaChn, DMACON_CONTPINGPONGDSB);
    CCpu_DmaReqSetIrq(DmaChn, IntReqId);
    CCpu_DmaCntSetTrxCnt(DmaChn, 7);
    CCpu_DmaSetPerAdr(DmaChn, pPer);

    //unsigned int adrL = __builtin_dmaoffset(pSYS->m_CanBuf);
    //unsigned int adrH = __builtin_dmapage(pSYS->m_CanBuf);
    unsigned int adrL = __builtin_dmaoffset(gSys.m_CanBuf);
    unsigned int adrH = __builtin_dmapage(gSys.m_CanBuf);

    CCpu_DmaSetStartAdrA_(DmaChn, adrL, adrH);
    CCpu_DmaConEnb(DmaChn, TRUE);
}

int CSys_CanBusUpdMsgId(BOOL bExIde, uInt2 Sid, uInt4 Eid)
{
    CSys_CanBusSetOpModeConfig();
    //
    CSys_CanBusSetMsgId(ESYS_ECAN_FLT0, bExIde, Sid, Eid);
//    CSys_CanBusSetMsgId(ESYS_ECAN_FLT1, bExIde, Sid, Eid);
//    CSys_CanBusSetMsgId(ESYS_ECAN_FLT2, bExIde, Sid, Eid);
//    CSys_CanBusSetMsgId(ESYS_ECAN_FLT3, bExIde, Sid, Eid);
    //
    CSys_CanBusSetOpModeNorm();
    return OK;
}

// ====================================================================
// CCanFltMsk
// ====================================================================
void CCanFltMsk_Set(CCanFltMsk* Self, int iMsk, uInt4 Msk)
{
    if (iMsk >= MAX_N_MSK)
        return;
    Self->m_iMsk = iMsk;
    Self->m_Msk = Msk;
}

int CCanFltMsk_GetMskNum(CCanFltMsk* Self)
{
    return Self->m_iMsk;
}

uInt4 CCanFltMsk_GetMsk(CCanFltMsk* Self)
{
    return Self->m_Msk;
}

// ====================================================================
// CCanFltVal
// ====================================================================
void CCanFltVal_Set(CCanFltVal* Self, int iFlt, uInt4 m_Val)
{
    if (iFlt >= MAX_N_FLT)
        return;
    Self->m_iFlt = iFlt;
    Self->m_Val = m_Val;
}

int CCanFltVal_GetFltNum(CCanFltVal* Self)
{
    return Self->m_iFlt;
}

uInt4 CCanFltVal_GetVal(CCanFltVal* Self)
{
    return Self->m_Val;
}

// ====================================================================
// CCanFlt
// ====================================================================
void CCanFlt_SetFltMsk(CCanFlt* Self, int iMsk, uInt4 Msk)
{
    CCanFltMsk_Set(&Self->m_Msk, iMsk, Msk);
}

void CCanFlt_SetFltVal(CCanFlt* Self, int iFlt, uInt4 Val)
{
    CCanFltVal_Set(&Self->m_Val, iFlt, Val);
}

void CCanFlt_SetCtrl(CCanFlt* Self, Bool bCtrl, Bool bExtIdn)
{
    Self->m_bFrmCtrl = bCtrl;
    Self->m_bExtIdn  = bExtIdn;
}

void CCanFlt_UpdFltAtr(CCanFlt* Self, Bool bFrmType)
{
    CSys_CanBusSetOpModeConfig();
    int imsk = CCanFltMsk_GetMskNum(&Self->m_Msk);
    uInt4 v = CCanFltMsk_GetMsk(&Self->m_Msk);
    uInt2 sid = 0;
    uInt4 eid = 0;
    if (bFrmType) {
        eid = v & 0x03FFFF;
        sid = (uInt2)((v >> 18) & 0x07FF);
    }
    else {
        sid = v & 0x07FF;
        eid = ((v >> 11) & 0x03FFFF);
    }

    // ALERT
//    sid = 0x0000;
//    eid = 0x0000FF00;
    // ALERT
    CSys_CanBusInitFltMsk(imsk, Self->m_bFrmCtrl, sid, eid);

    int iflt = CCanFltVal_GetFltNum(&Self->m_Val);
    v = CCanFltVal_GetVal(&Self->m_Val);
    if (bFrmType) {
        eid = v & 0x03FFFF;
        sid = (uInt2)((v >> 18) & 0x07FF);
    }
    else {
        sid = v & 0x07FF;
        eid = ((v >> 11) & 0x03FFFF);
    }

//    sid = v & 0x07FF;
//    eid = v >> 11;
//    eid &= 0x03FFFF;

    // ALERT
//    sid = 0x0000;
//    eid = 0x00000200;
    // ALERT
    // Select Maskt number
    CCpu_CanFltAtchMskNum(iflt, imsk);
    CSys_CanBusSetMsgId(iflt, Self->m_bExtIdn, sid, eid);
    //
    CSys_CanBusSetOpModeNorm();
}

// ====================================================================
// CCanFrm
// ====================================================================
void CCanFrm_Init(CCanFrm* Self)
{
    Self->m_CanId = 0;
    Self->m_Dlc = 0;
    memset(Self->m_Dat, 0, SZ_CAN_DBUF);
}

void CCanFrm_SetIdn(CCanFrm* Self, uInt4 Idn)
{
    Self->m_CanId &= ~CAN_EFF_MASK;
    if (CCanFrm_IsExtIdn(Self)) 
         Idn &= CAN_EFF_MASK;
    else Idn &= CAN_SFF_MASK;

    Self->m_CanId |= Idn;
}

uInt4 CCanFrm_GetIdn(CCanFrm* Self)
{
    uInt4 idn = 0;
    if (CCanFrm_IsExtIdn(Self))
         idn = (Self->m_CanId & CAN_EFF_MASK);
    else idn = (Self->m_CanId & CAN_SFF_MASK);

    return idn;
}

uInt4 CCanFrm_GetExtIdn(CCanFrm* Self)
{
    uInt4 idn = CCanFrm_GetIdn(Self);
    idn >>= 11;
    idn &= 0x3FFFF;
    return idn;
}

uInt4 CCanFrm_GetStdIdn(CCanFrm* Self)
{
    uInt4 idn = CCanFrm_GetIdn(Self);
    idn &= 0x7FF;
    return idn;
}

void CCanFrm_SetOpt(CCanFrm* Self, Bool bExtIdn, Bool bRmtFrm, Bool bErrFrm)
{
    CCanFrm_SetOptExtIdn(Self, bExtIdn);
    CCanFrm_SetOptRmt(Self, bRmtFrm);
    CCanFrm_SetOptErr(Self, bErrFrm);
}

void CCanFrm_SetOptExtIdn(CCanFrm* Self, Bool bExtIdn)
{
    if (bExtIdn)
         Self->m_CanId |= CAN_EFF_FLAG;
    else Self->m_CanId &= ~CAN_EFF_FLAG;
}

Bool CCanFrm_IsExtIdn(CCanFrm* Self)
{
    return (Self->m_CanId & CAN_EFF_FLAG);
}

void CCanFrm_SetOptRmt(CCanFrm* Self, Bool bRmtFrm)
{
    if (bRmtFrm)
         Self->m_CanId |= CAN_RTR_FLAG;
    else Self->m_CanId &= ~CAN_RTR_FLAG;
}

Bool CCanFrm_IsRmt(CCanFrm* Self)
{
    return (Self->m_CanId & CAN_RTR_FLAG);
}

void CCanFrm_SetOptErr(CCanFrm* Self, Bool bErrFrm)
{
    if (bErrFrm)
         Self->m_CanId |= CAN_ERR_FLAG;
    else Self->m_CanId &= ~CAN_ERR_FLAG;
}

Bool CCanFrm_IsErr(CCanFrm* Self)
{
    return (Self->m_CanId & CAN_ERR_FLAG);
}

int CCanFrm_SetDat(CCanFrm* Self, Byte* pDat, int Dlen)
{
    if ((Dlen > 8) || (Dlen < 0))
        return ERR;
    memcpy(Self->m_Dat, pDat, Dlen);
    Self->m_Dlc = Dlen;
    return OK;
}

int CCanFrm_GetDat(CCanFrm* Self, Byte* pDat)
{
    if (Self->m_Dlc <= 0)
        return 0;
    memcpy(pDat, Self->m_Dat, Self->m_Dlc);
    return Self->m_Dlc;
}

Byte* CCanFrm_GetBufPtr(CCanFrm* Self)
{
    return Self->m_Dat;
}

int CCanFrm_GetDlc(CCanFrm* Self)
{
    return Self->m_Dlc;
}

void CCanFrm_SetDlc(CCanFrm* Self, int Dlc)
{
    Self->m_Dlc = Dlc;
}

void CCanFrm_Cpy(CCanFrm* Self, CCanFrm* pDstFrm)
{
    pDstFrm->m_CanId = Self->m_CanId;
    pDstFrm->m_Dlc = Self->m_Dlc;
    memcpy(pDstFrm->m_Dat, Self->m_Dat, SZ_CAN_DBUF);
}


void CCanFrm_Dbg(CCanFrm* Self)
{
    if (!Self)
        return;
    CLogCtx_Info("CanId [Hex] = %08lX\r\n", Self->m_CanId);
    CLogCtx_Info("Dlc  = %d\r\n", Self->m_Dlc);
    CLogCtx_WrBuf(Self->m_Dat, Self->m_Dlc);
}

// ====================================================================
// CCanChn
// ====================================================================
void CCanDev_Init(CCanDev* Self)
{
    CCanFrm_Init(&Self->m_RxFrm);
    Self->m_bFrmAvl = FALSE;
    Self->m_bFrmAck = TRUE;
    Self->m_bTxEmpty= TRUE;
    Self->m_FrmOvride   = 0;
    Self->m_nOvfErr     = 0;
    Self->m_nInvMsg     = 0;
    Self->m_nIntErr     = 0;
    Self->m_nTxErr      = 0;
    Self->m_nRxErr      = 0;
    Self->m_hCnb        = NULL;
    Self->m_pCbkOnRxFrm = NULL;
}

void CCanDev_SetRxFrmCbk(CCanDev* Self, CCnbHnd* hCnb, void (*pCbkOnRxFrm)(CCnbHnd* hCnb))
{
    if (!Self)
        return;
    Self->m_hCnb        = hCnb;
    Self->m_pCbkOnRxFrm = pCbkOnRxFrm;
}

Bool CCanDev_IsFrmAvl(CCanDev* Self)
{
    return Self->m_bFrmAvl;
}

void CCanDev_AckFrm(CCanDev* Self)
{
    Self->m_bFrmAck = TRUE;
    Self->m_bFrmAvl = FALSE;
}

CCanFrm* CCanDev_GetRxFrmPtr(CCanDev* Self)
{
    return &Self->m_RxFrm;
}

void CCanDev_GetRxFrm(CCanDev* Self, CCanFrm* pDstFrm)
{
    CCanFrm_Cpy(&Self->m_RxFrm, pDstFrm);
}

Bool CCanDev_IsTxEmpty(CCanDev* Self)
{
    return Self->m_bTxEmpty;
}

void CCanDev_OnRx(CCanDev* Self, int BufNum)
{
    short unsigned int* pSrcBuf = &pSYS->m_CanBuf[BufNum][0];
    bool bextidn = FALSE;
    bool brtr = FALSE;
    uInt4 idn = 0;
    // Check Identifier
    if (pSrcBuf[0] & 0x1)
        bextidn = TRUE;
    // std idn & bit 0~11 of extidn
    idn = (pSrcBuf[0] >> 2) & 0x07FF;
    if (bextidn) {
        // extract rtr flag
        brtr = (pSrcBuf[2] & CPU_CAN_FRM_RTR) ? TRUE: FALSE;
        // issue observed that in case of bexteidn it becomes lsb
        // extract idn
        volatile uInt4 tmp = pSrcBuf[1] & 0x0FFF;        
        uInt4 eid = ((tmp << 6) & 0x0003FFC0);
        //
        tmp = pSrcBuf[2] >> 10;
        tmp &= 0x0000003F;
        eid |= tmp;

        //eid |= ((pSrcBuf[2] >> 10) & 0x003F);
        tmp = idn << 18;
        tmp &= 0x1FFC0000;
        eid |= tmp;
        //eid |= (idn << 18);
        idn = eid;
    }
    else {
        // extract rtr flag
        brtr = (pSrcBuf[0] & CPU_CAN_FRM_SRR) ? TRUE: FALSE;
    }
    // Check Frame ack or not
    if (!Self->m_bFrmAck)
        Self->m_FrmOvride++;
    Self->m_bFrmAvl = TRUE;
    //
    CCanFrm_SetOpt(&Self->m_RxFrm, bextidn, brtr, FALSE);
    CCanFrm_SetIdn(&Self->m_RxFrm, idn);
    if (!brtr) {
        int dlc = pSrcBuf[2] & 0x0F;
        Byte* pdat = (Byte*)&pSrcBuf[3];
        CCanFrm_SetDat(&Self->m_RxFrm, pdat, dlc);
    }
    Self->m_iRxFlt    = ((pSrcBuf[7] >> 8) & 0x1F);

    // OnCbk ...
    if ((Self->m_hCnb) && (Self->m_pCbkOnRxFrm))
        Self->m_pCbkOnRxFrm(Self->m_hCnb);
}

int CCanDev_Wr(CCanDev* Self, CCanFrm* pFrm)
{// TODO: implement to take bufnum from system itself
    uInt2 word0, word1, word2;
    word1 = word2 = 0;
    uInt4 idn = CCanFrm_GetIdn(pFrm);
    bool brtr = CCanFrm_IsRmt(pFrm);
    if (CCanFrm_IsExtIdn(pFrm)) {
        word0 = idn >> 18;
        word0 <<= 2;
        word0 |= CPU_CAN_FRM_EXT;
        word0 |= CPU_CAN_FRM_SRR;

        uInt4 eid = idn & 0x03FFFF;
        word1 = eid >> 6;
        word2 = (eid & 0x3F) << 10;
        if (brtr)
            word2 |= CPU_CAN_FRM_RTR;
    }
    else {
        word0 = idn & 0x07FF;
        word0 <<= 2;
        if (brtr)
            word0 |= CPU_CAN_FRM_SRR;
    }

    int BufNum = 0;
    short unsigned int* pSrcBuf = &pSYS->m_CanBuf[BufNum][0];
    if (!brtr) {
        Byte* pdat = (Byte*)&pSrcBuf[3];
        int dlc = CCanFrm_GetDat(pFrm, pdat);
        if (dlc <= 0)
            return ERR;
        word2 |= (dlc & 0x0F);
    }
    pSrcBuf[0] = word0;
    pSrcBuf[1] = word1;
    pSrcBuf[2] = word2;
    //
    Self->m_nTxErr = 0;
    //
    Self->m_bTxEmpty = FALSE;
    CCpu_CanBufTxReq(BufNum, TRUE);
    return OK;
}

// =======
// Isr Ifc
// =======
void CSys_CanDevOnIsrTx()
{
    if (!pCANDEV)
        return;
    pCANDEV->m_bTxEmpty = TRUE;
}

void CSys_CanDevOnIsrRx(int BufNum)
{
    if (!pCANDEV)
        return;
    CCanDev_OnRx(pCANDEV, BufNum);
}

void CSys_CanDevOnIsrErr(int Type)
{
    if (!pCANDEV)
        return;
    // upd status rec
    CSysSts_IncErrCnt(pSysSts, ERR_TYPE_ECAN, Type);
    CSysSts_IncErrCnt(pSysSts, ERR_TYPE_ECAN, Type);
    int txerr = CCpu_CanGetTxErrCnt();
    int rxerr = CCpu_CanGetTxErrCnt();
    CSysSts_SetErrVal(pSysSts, ERR_TYPE_ECAN, CAN_TX_ERR, txerr);
    CSysSts_SetErrVal(pSysSts, ERR_TYPE_ECAN, CAN_RX_ERR, rxerr);
}

//*********************************************************************
// End
//*********************************************************************
