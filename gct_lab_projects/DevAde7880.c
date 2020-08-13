//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

// ====================================================================
// CPwrPt
// ====================================================================

void CRmsPt_AttRegDefPtrVrms(CRmsPt* Self, CRegDef* pReg)
{
    if (!Self)
        return;
    Self->m_pRegVrms = pReg;
}

void CRmsPt_AttRegDefPtrIrms(CRmsPt* Self, CRegDef* pReg)
{
    if (!Self)
        return;
    Self->m_pRegIrms = pReg;
}

void CRmsPt_AttRegDefPtrPf(CRmsPt* Self, CRegDef* pReg)
{
    if (!Self)
        return;
    Self->m_pRegPf = pReg;
}

void CRmsPt_AttRegDefPtrPact(CRmsPt* Self, CRegDef* pReg)
{
    if (!Self)
        return;
    Self->m_pRegPact = pReg;
}

void CRmsPt_AttRegDefPtrPrct(CRmsPt* Self, CRegDef* pReg)
{
    if (!Self)
        return;
    Self->m_pRegPrct = pReg;
}

void CRmsPt_AttRegDefPtrPapr(CRmsPt* Self, CRegDef* pReg)
{
    if (!Self)
        return;
    Self->m_pRegPapr = pReg;
}

void CRmsPt_AttRegDefPtrFrq(CRmsPt* Self, CRegDef* pReg)
{
    if (!Self)
        return;
    Self->m_pRegFrq = pReg;
}

void CRmsPt_SetVrms(CRmsPt* Self, Int4 Val)
{
    if (!Self)
        return;
    CRegDef_CbkOnRawValUpd(Self->m_pRegVrms, Val);
}

void CRmsPt_SetIrms(CRmsPt* Self, Int4 Val)
{
    if (!Self)
        return;
    CRegDef_CbkOnRawValUpd(Self->m_pRegIrms, Val);
}

void CRmsPt_SetPf(CRmsPt* Self, Int2 Val)
{
    if (!Self)
        return;
    CRegDef_CbkOnRawValUpd(Self->m_pRegPf, Val);
}

void CRmsPt_SetPact(CRmsPt* Self, Int4 Val)
{
    if (!Self)
        return;
    CRegDef_CbkOnRawValUpd(Self->m_pRegPact, Val);
}

void CRmsPt_SetPrct(CRmsPt* Self, Int4 Val)
{
    if (!Self)
        return;
    CRegDef_CbkOnRawValUpd(Self->m_pRegPrct, Val);
}

void CRmsPt_SetPapr(CRmsPt* Self, Int4 Val)
{
    if (!Self)
        return;
    CRegDef_CbkOnRawValUpd(Self->m_pRegPapr, Val);
}

void CRmsPt_SetFrq(CRmsPt* Self, Int2 Val)
{
    if (!Self)
        return;
    CRegDef_CbkOnRawValUpd(Self->m_pRegFrq, Val);
}

Int4 CRmsPt_Vrms(CRmsPt* Self)
{
    if (!Self)
        return UNKNOWN;
    Int4 val = *((Int4*)CRegDef_GetDptr(Self->m_pRegVrms));
    return val;
}

Int4 CRmsPt_Irms(CRmsPt* Self)
{
    if (!Self)
        return UNKNOWN;
    Int4 val = *((Int4*)CRegDef_GetDptr(Self->m_pRegIrms));
    return val;
}

Int4 CRmsPt_Pf(CRmsPt* Self)
{
    if (!Self)
        return UNKNOWN;
    Int4 val = *((Int4*)CRegDef_GetDptr(Self->m_pRegPf));
    return val;
}

Int4 CRmsPt_Pact(CRmsPt* Self)
{
    if (!Self)
        return UNKNOWN;
    Int4 val = *((Int4*)CRegDef_GetDptr(Self->m_pRegPact));
    return val;
}

Int4 CRmsPt_Prct(CRmsPt* Self)
{
    if (!Self)
        return UNKNOWN;
    Int4 val = *((Int4*)CRegDef_GetDptr(Self->m_pRegPrct));
    return val;
}

Int4 CRmsPt_Papr(CRmsPt* Self)
{
    if (!Self)
        return UNKNOWN;
    Int4 val = *((Int4*)CRegDef_GetDptr(Self->m_pRegPapr));
    return val;
}

Int4 CRmsPt_Frq(CRmsPt* Self)
{
    if (!Self)
        return UNKNOWN;
    Int4 val = *((Int4*)CRegDef_GetDptr(Self->m_pRegFrq));
    return val;
}

// ====================================================================
// CPwrRec
// ====================================================================

void CRmsRecPtr_AttRegDefPtrVrms(CRmsRecPtr* Self, int iPh, CRegDef* pReg)
{
    if (iPh >= MAX_PH)
        return;

    if (!Self)
        return;

    CRmsPt_AttRegDefPtrVrms(&Self->m_aPwrPt[iPh], pReg);
}

void CRmsRecPtr_AttRegDefPtrIrms(CRmsRecPtr* Self, int iPh, CRegDef* pReg)
{
    if (iPh >= MAX_PH)
        return;

    if (!Self)
        return;

    CRmsPt_AttRegDefPtrIrms(&Self->m_aPwrPt[iPh], pReg);
}

void CRmsRecPtr_AttRegDefPtrPf(CRmsRecPtr* Self, int iPh, CRegDef* pReg)
{
    if (iPh >= MAX_PH)
        return;

    if (!Self)
        return;

    CRmsPt_AttRegDefPtrPf(&Self->m_aPwrPt[iPh], pReg);
}

void CRmsRecPtr_AttRegDefPtrPact(CRmsRecPtr* Self, int iPh, CRegDef* pReg)
{
    if (iPh >= MAX_PH)
        return;

    if (!Self)
        return;

    CRmsPt_AttRegDefPtrPact(&Self->m_aPwrPt[iPh], pReg);
}
void CRmsRecPtr_AttRegDefPtrPrct(CRmsRecPtr* Self, int iPh, CRegDef* pReg)
{
    if (iPh >= MAX_PH)
        return;

    if (!Self)
        return;

    CRmsPt_AttRegDefPtrPrct(&Self->m_aPwrPt[iPh], pReg);
}

void CRmsRecPtr_AttRegDefPtrPapr(CRmsRecPtr* Self, int iPh, CRegDef* pReg)
{
    if (iPh >= MAX_PH)
        return;

    if (!Self)
        return;

    CRmsPt_AttRegDefPtrPapr(&Self->m_aPwrPt[iPh], pReg);
}

void CRmsRecPtr_AttRegDefPtrFrq(CRmsRecPtr* Self, int iPh, CRegDef* pReg)
{
    if (iPh >= MAX_PH)
        return;

    if (!Self)
        return;

    CRmsPt_AttRegDefPtrFrq(&Self->m_aPwrPt[iPh], pReg);
}

void CRmsRecPtr_SetVrms(CRmsRecPtr* Self, int iPh, Int4 Val)
{
    if (iPh >= MAX_PH)
        return;
    CRmsPt_SetVrms(&Self->m_aPwrPt[iPh], Val);
}

void CRmsRecPtr_SetIrms(CRmsRecPtr* Self, int iPh, Int4 Val)
{
    if (iPh >= MAX_PH)
        return;
    CRmsPt_SetIrms(&Self->m_aPwrPt[iPh], Val);
}

void CRmsRecPtr_SetPf(CRmsRecPtr* Self, int iPh, Int2 Val)
{
    if (iPh >= MAX_PH)
        return;
    CRmsPt_SetPf(&Self->m_aPwrPt[iPh], Val);
}

void CRmsRecPtr_SetPact(CRmsRecPtr* Self, int iPh, Int4 Val)
{
     if (iPh >= MAX_PH)
        return;
    CRmsPt_SetPact(&Self->m_aPwrPt[iPh], Val);
}
void CRmsRecPtr_SetPrct(CRmsRecPtr* Self, int iPh, Int4 Val)
{
     if (iPh >= MAX_PH)
        return;
    CRmsPt_SetPrct(&Self->m_aPwrPt[iPh], Val);
}
void CRmsRecPtr_SetPapr(CRmsRecPtr* Self, int iPh, Int4 Val)
{
     if (iPh >= MAX_PH)
        return;
    CRmsPt_SetPapr(&Self->m_aPwrPt[iPh], Val);
}

void CRmsRecPtr_SetFrq(CRmsRecPtr* Self, int iPh, Int4 Val)
{
    if (iPh >= MAX_PH)
        return;
    CRmsPt_SetFrq(&Self->m_aPwrPt[iPh], Val);
}

Int4 CRmsRecPtr_Vrms(CRmsRecPtr* Self, int iPh)
{
    if (iPh >= MAX_PH)
        return UNKNOWN;
    return CRmsPt_Vrms(&Self->m_aPwrPt[iPh]);
}

Int4 CRmsRecPtr_Irms(CRmsRecPtr* Self, int iPh)
{
    if (iPh >= MAX_PH)
        return UNKNOWN;
    return CRmsPt_Irms(&Self->m_aPwrPt[iPh]);
}

Int4 CRmsRecPtr_Pf(CRmsRecPtr* Self, int iPh)
{
    if (iPh >= MAX_PH)
        return UNKNOWN;
    return CRmsPt_Pf(&Self->m_aPwrPt[iPh]);
}

Int4 CRmsRecPtr_Pact(CRmsRecPtr* Self, int iPh)
{
    if (iPh >= MAX_PH)
        return UNKNOWN;
    return CRmsPt_Pact(&Self->m_aPwrPt[iPh]);
}

Int4 CRmsRecPtr_Prct(CRmsRecPtr* Self, int iPh)
{
    if (iPh >= MAX_PH)
        return UNKNOWN;
    return CRmsPt_Prct(&Self->m_aPwrPt[iPh]);
}

Int4 CRmsRecPtr_Papr(CRmsRecPtr* Self, int iPh)
{
    if (iPh >= MAX_PH)
        return UNKNOWN;
    return CRmsPt_Papr(&Self->m_aPwrPt[iPh]);
}

Int4 CRmsRecPtr_Frq(CRmsRecPtr* Self, int iPh)
{
    if (iPh >= MAX_PH)
        return UNKNOWN;
    return CRmsPt_Frq(&Self->m_aPwrPt[iPh]);
}

// ====================================================================
// CDevAde7880
// ====================================================================
void CDevAde7880_Reset(CDevAde7880* Self)
{
    Self->m_pChn = NULL;
    Self->m_DevId = 0xFFFF;
    Self->m_bEnb = FALSE;
}

void CDevAde7880_Init(CDevAde7880* Self)
{
    CDevAde7880_Reset(Self);
}

void CDevAde7880_Setup(CDevAde7880* Self, CSpiChn* pChn, int RstPort, int RstPin)
{
    Self->m_pChn = pChn;
    // Harware initialisation
    CSys_SpiChnSetup(Self->m_pChn, SPI_MST_MODE, SPI_PROTO_MODE_3, SPI_FRQ_2p5MHz, SPI_DAT_BYTE, uCPORTF, PIN00, TC_10MS);
    //
    Self->m_RstPort = RstPort;
    Self->m_RstPin  = RstPin;
    CSys_GpioInit(RstPort, RstPin, PORT_DRN_OUT);
}

bool CDevAde7880_IsRstDone(CDevAde7880* Self)
{
    if (!Self->m_pChn)
        return ERR;
    int sts = CDevAde7880_Rd32bit(Self, REGID_STS1, &Self->m_Sts1);
    if (sts != OK)
        return false;

    return (Self->m_Sts1 & BITMSK_RST_DONE) ? true : false;
}

int CDevAde7880_WtRstDone(CDevAde7880* Self)
{
    if (!Self->m_pChn)
        return ERR;
    // return OK => restet done within 15 sec
    // return ERR otherwise
    int i, sts;
    sts = UNKNOWN;
    for (i = 0; i < 15; i++) {
        WaitSec(1);
        if (CDevAde7880_IsRstDone(Self)) {
            sts = OK;
            break;
        }
    }
    return sts;
}

int CDevAde7880_HardRst(CDevAde7880* Self)
{ //
    if (!Self->m_pChn)
        return ERR;
    //
    CSys_SpiChnCsHi(Self->m_pChn);
    //
    CDevAde7880_AplHwRst(Self);
    WaitSec(5);
    CDevAde7880_RmvHwRst(Self);
    int i, sts;
    // Enable SPI mode
    for (i = 0; i < 3; i++) {
        CDevAde7880_Wr8bit(Self, 0xEBFF, 0xFF);
    }
    //CSys_SpiChnCsHi(Self->m_pChn);
    // Lock communication mode as SPI
    sts = CDevAde7880_Wr8bit(Self, REGID_CONFIG2, 0x00);
    if (sts != OK)
        return ERR;
    // wait for
    sts = CDevAde7880_WtRstDone(Self);
    if (sts != OK) {
        CLogCtx_Err("Hardware ResetDone doesn't occur\r\n");
        return sts;
    }
    return OK;
}

int CDevAde7880_SoftRst(CDevAde7880* Self)
{ //
    if (!Self->m_pChn)
        return ERR;
    uInt2 cfg1 = 0;
    int sts = UNKNOWN;

    // clear sts msk
    CDevAde7880_Wr32bit(Self, REGID_STS0, STS0_MSK);
    CDevAde7880_Wr32bit(Self, REGID_STS1, STS1_MSK);

    sts = CDevAde7880_Rd16bit(Self, REGID_CONFIG1, (Int2*)&cfg1);
    if (sts != OK)
        return sts;

    cfg1 |= BITMSK_SOFT_RST;
    sts = CDevAde7880_Wr16bit(Self, REGID_CONFIG1, cfg1);
    if (sts != OK)
        return sts;

    WaitSec(5);

    sts = CDevAde7880_WtRstDone(Self);
    if (sts != OK) {
        CLogCtx_Err("Software ResetDone doesn't occur\r\n");
        return sts;
    }
    return OK;
}

int CDevAde7880_RstDspI4Reg(CDevAde7880* Self, int FstReg, int LstReg, uInt4 Val)
{
    if (!Self->m_pChn)
        return ERR;
    int i, sts;
    //uInt4 rdval;
    for (i = FstReg; i <= LstReg; i++) {
        sts = CDevAde7880_Wr32bit(Self, i, Val);
        if (sts != OK)
            break;
        /*
        sts = CDevAde7880_Rd32bit(Self, i, &rdval);
        if (sts != OK)
            break;
        if (rdval != Val) {
            CLogCtx_Err("Error: RegValue Mismatch \r\n");
            sts = ERR;
            break;
        }
        */
    }
    return sts;
}

int CDevAde7880_RstDspRamReg(CDevAde7880* Self)
{
    if (!Self->m_pChn)
        return ERR;
    int sts = UNKNOWN;
    int i;

    sts = CDevAde7880_RstDspI4Reg(Self, REGID_PHA_IGN, REGID_N_IGN, 0);
    if (sts == OK)
        sts = CDevAde7880_RstDspI4Reg(Self, REGID_DICOEFF, REGID_N_IOFS, 0);
    if (sts == OK)
        sts = CDevAde7880_RstDspI4Reg(Self, REGID_HRM_PGN, REGID_VLVL, 0);
    if (sts == OK)
        sts = CDevAde7880_RstDspI4Reg(Self, REGID_PHA_FUN_POFS, REGID_HRM_Y_VOFS, 0);

    if (sts == OK) {
        // Flush last register
        for (i = 0; i < 3; i++) {
            sts = CDevAde7880_Wr32bit(Self, REGID_HRM_Z_VOFS, 0);
            if (sts != OK)
                break;
        }
    }
    return OK;
}

int CDevAde7880_Reboot(CDevAde7880* Self)
{
    if (!Self->m_pChn)
        return ERR;
    int sts = UNKNOWN;

    CLogCtx_Info("Applying Hardware Reset ... \r\n");
    sts = CDevAde7880_HardRst(Self);
    if (sts != OK)
        return sts;
    CLogCtx_Info("Hardware Reset OK ! \r\n");
    CLogCtx_Info("Reading Device Id ... \r\n");
    // Read Device Id
    sts = CDevAde7880_Rd8bit(Self, REGID_DEVID, &Self->m_DevId);
    if (sts != OK) {
        CLogCtx_Err("Error: Reading Device Id \r\n");
        return sts;
    }
    CLogCtx_Info("Reading Device OK ! \r\n");

    CLogCtx_Info("Applying Software Reset ... \r\n");
    sts = CDevAde7880_SoftRst(Self);
    if (sts != OK)
        return sts;
    CLogCtx_Info("Software Reset OK ! \r\n");
    //
    CDevAde7880_RstDspRamReg(Self);

    // Enable interrupt Dready
    uInt4 msk0;
    sts = CDevAde7880_Rd32bit(Self, REGID_INT_MSK0, &msk0);
    if (sts != OK)
        return sts;
    msk0 |= BITMSK_INT_DRDY;
    sts = CDevAde7880_Wr32bit(Self, REGID_INT_MSK0, msk0);
    if (sts != OK) {
        CLogCtx_Err("Error: Enabling Drdy Interrupt !\r\n");
        return sts;
    }

    CDevAde7880_RstDspRamReg(Self);
    // Enable RAM WrProtect
    CLogCtx_Info("Locking RAM ... \r\n");
    sts = CDevAde7880_Wr8bit(Self, 0xE7FE, 0xAD);
    if (sts == OK)
        sts = CDevAde7880_Wr8bit(Self, 0xE7E3, 0x80);
    CLogCtx_Info("RAM Locked! \r\n");

    // Starts DSP
    CLogCtx_Info("Running DSP ... \r\n");
    sts = CDevAde7880_Wr16bit(Self, REGID_DSP_RUN, 0x0001);
    if (sts != OK) {
        CLogCtx_Err("Error: Running DSP \r\n");
        return sts;
    }    
    CLogCtx_Info("DSP Run OK ! \r\n");
    
    Self->m_bEnb = TRUE;
    return OK;
}

#define ADE_MAX_STARTUP_RETRY   10
#define MIN_VRMS    50
#define MIN_IRMS    50

int CDevAde7880_ChkChn(CDevAde7880* Self)
{
    int i, sts = UNKNOWN;
    for (i = 0; i < 10; i++) {
        WaitSec(1);
        if (CDevAde7880_IsDrdy(Self)) {
            sts = OK;
            break;
        }
    }

    if (sts != OK)
        return sts;
    Int4 va, vb, vc;
    Int4 ia, ib, ic;
    sts = CDevAde7880_RdVrms(Self, &va, &vb, &vc);
    if (sts == OK)
        sts = CDevAde7880_RdIrms(Self, &ia, &ib, &ic);
    CDevAde7880_ClrDrdy(Self);

    if (sts != OK)
        return sts;

    CLogCtx_Info("Ph-A=%ld, %ld \r\n", va, ia);
    CLogCtx_Info("Ph-B=%ld, %ld \r\n", vb, ib);
    CLogCtx_Info("Ph-C=%ld, %ld \r\n", vc, ic);
    //
    if (va < MIN_VRMS) {
        CLogCtx_Err("Ph-A Vrms=[ %ld ] Error \r\n", va);
        return ERR;
    }
    if (vb < MIN_VRMS) {
        CLogCtx_Err("Ph-B Vrms=[ %ld ] Error \r\n", vb);
        return ERR;
    }
    if (vc < MIN_VRMS) {
        CLogCtx_Err("Ph-C Vrms=[ %ld ] Error \r\n", vc);
        return ERR;
    }

    if (ia < MIN_VRMS) {
        CLogCtx_Err("Ph-A Irms=[ %ld ] Error \r\n", ia);
        return ERR;
    }
    if (ib < MIN_VRMS) {
        CLogCtx_Err("Ph-B Irms=[ %ld ] Error \r\n", ib);
        return ERR;
    }
    if (ic < MIN_VRMS) {
        CLogCtx_Err("Ph-C Irms=[ %ld ] Error \r\n", ic);
        return ERR;
    }

    return OK;
}

int CDevAde7880_Start(CDevAde7880* Self)
{
    if (!Self->m_pChn)
        return ERR;
    int sts = UNKNOWN;
    int i = 0;
    for (i = 0; i < ADE_MAX_STARTUP_RETRY; i++) {
        sts = CDevAde7880_Reboot(Self);
        if (sts != OK)
            continue;
        break;  // ALERT
        WaitSec(20);
        // Reboot OK
        sts = CDevAde7880_ChkChn(Self);
        if (sts == OK)
            break;
        else {
            CLogCtx_Info("Restaring ADE\r\n");
        }
    }

    return sts;
}

void CDevAde7880_DgnGnReg(CDevAde7880* Self)
{
    Int4 ign, vgn;
    int sts;
    CLogCtx_Info("Gain Registers ...\r\n");
    // Read Gain-Registers
    sts = CDevAde7880_RdSE32bit(Self, REGID_PHA_IGN, &ign);
    if (sts == OK)
        sts = CDevAde7880_RdSE32bit(Self, REGID_PHA_VGN, &vgn);
    if (sts == OK)
        CLogCtx_Info("PhA: Ign[%08lX]; Vgn[%08lX]\r\n", ign, vgn);

    sts = CDevAde7880_RdSE32bit(Self, REGID_PHB_IGN, &ign);
    if (sts == OK)
        sts = CDevAde7880_RdSE32bit(Self, REGID_PHB_VGN, &vgn);
    if (sts == OK)
        CLogCtx_Info("PhB: Ign[%08lX]; Vgn[%08lX]\r\n", ign, vgn);

    sts = CDevAde7880_RdSE32bit(Self, REGID_PHC_IGN, &ign);
    if (sts == OK)
        sts = CDevAde7880_RdSE32bit(Self, REGID_PHC_VGN, &vgn);
    if (sts == OK)
        CLogCtx_Info("PhC: Ign[%08lX]; Vgn[%08lX]\r\n", ign, vgn);
}

void CDevAde7880_Dgn(CDevAde7880* Self)
{
    if (!Self)
        return;
    int sts = UNKNOWN;
    Int2 i2Val = 0;
    uInt2 uI2Val = 0;
    // Read Cofig Register
    sts = CDevAde7880_Rd16bit(Self, REGID_CONFIG1, &i2Val);
    if (sts == OK)
        CLogCtx_Info("Config1: %04X\r\n", i2Val);
    sts = CDevAde7880_Rd16bit(Self, REGID_CONFIG2, &i2Val);
    if (sts == OK)
        CLogCtx_Info("Config2: %04X\r\n", i2Val);
    sts = CDevAde7880_Rd8bit(Self, REGID_CONFIG3, &uI2Val);
    if (sts == OK)
        CLogCtx_Info("Config3: %04X\r\n", uI2Val);

    CDevAde7880_DgnGnReg(Self);
}

bool CDevAde7880_IsDrdy(CDevAde7880* Self)
{
    int sts = UNKNOWN;
    sts = CDevAde7880_Rd32bit(Self, REGID_STS0, &Self->m_Sts0);
    if (sts != OK)
        return false;
    return (Self->m_Sts0 & BITMSK_DRDY) ? true : false;
}

void CDevAde7880_ClrDrdy(CDevAde7880* Self)
{
    CDevAde7880_Wr32bit(Self, REGID_STS0, Self->m_Sts0);
}

int CDevAde7880_RdVrms(CDevAde7880* Self, Int4* pPhA, Int4* pPhB, Int4* pPhC)
{
    int sts = UNKNOWN;
    sts = CDevAde7880_RdZP24bit(Self, REGID_PHA_VRMS, pPhA);
    if (sts == OK)
        sts = CDevAde7880_RdZP24bit(Self, REGID_PHB_VRMS, pPhB);
    if (sts == OK)
        sts = CDevAde7880_RdZP24bit(Self, REGID_PHC_VRMS, pPhC);
    return sts;
}

int CDevAde7880_RdIrms(CDevAde7880* Self, Int4* pPhA, Int4* pPhB, Int4* pPhC)
{
    int sts = UNKNOWN;
    sts = CDevAde7880_RdZP24bit(Self, REGID_PHA_IRMS, pPhA);
    if (sts == OK)
        sts = CDevAde7880_RdZP24bit(Self, REGID_PHB_IRMS, pPhB);
    if (sts == OK)
        sts = CDevAde7880_RdZP24bit(Self, REGID_PHC_IRMS, pPhC);
    return sts;
}

void CDevAde7880_GetRms(CDevAde7880* Self, CRmsRecPtr* pRmsRec)
{
    Int4 tmpval = 0;
    Int2 i2val;
    //
    int sts = CDevAde7880_Rd32bit(Self, REGID_STS0, &Self->m_Sts0);
    if (sts != OK)
        return;
    if (!(Self->m_Sts0 & BITMSK_DRDY))
        return;
    //
    CDevAde7880_RdZP24bit(Self, REGID_PHA_VRMS, &tmpval);
    CRmsRecPtr_SetVrms(pRmsRec, 0, tmpval);
    CDevAde7880_RdZP24bit(Self, REGID_PHA_IRMS, &tmpval);
    CRmsRecPtr_SetIrms(pRmsRec, 0, tmpval);
    CDevAde7880_RdPf(Self, REGID_PHA_PF, &i2val);
    CRmsRecPtr_SetPf(pRmsRec, 0, i2val);
    CDevAde7880_Rd16bit(Self, REGID_PHA_FRQ, &i2val);
    CRmsRecPtr_SetFrq(pRmsRec, 0, i2val);
    CDevAde7880_RdSE32bit(Self, REGID_PHA_PACT, &tmpval);
    CRmsRecPtr_SetPact(pRmsRec, 0, tmpval);
    CDevAde7880_RdSE32bit(Self, REGID_PHA_PAPR, &tmpval);
    CRmsRecPtr_SetPapr(pRmsRec, 0, tmpval);
    //
    CDevAde7880_RdZP24bit(Self, REGID_PHB_VRMS, &tmpval);
    CRmsRecPtr_SetVrms(pRmsRec, 1, tmpval);
    CDevAde7880_RdZP24bit(Self, REGID_PHB_IRMS, &tmpval);
    CRmsRecPtr_SetIrms(pRmsRec, 1, tmpval);
    CDevAde7880_RdPf(Self, REGID_PHB_PF, &i2val);
    CRmsRecPtr_SetPf(pRmsRec, 1, i2val);
    CDevAde7880_Rd16bit(Self, REGID_PHB_FRQ, &i2val);
    CRmsRecPtr_SetFrq(pRmsRec, 1, i2val);
    CDevAde7880_RdSE32bit(Self, REGID_PHB_PACT, &tmpval);
    CRmsRecPtr_SetPact(pRmsRec, 1, tmpval);
    CDevAde7880_RdSE32bit(Self, REGID_PHB_PAPR, &tmpval);
    CRmsRecPtr_SetPapr(pRmsRec, 1, tmpval);
    //
    CDevAde7880_RdZP24bit(Self, REGID_PHC_VRMS, &tmpval);
    CRmsRecPtr_SetVrms(pRmsRec, 2, tmpval);
    CDevAde7880_RdZP24bit(Self, REGID_PHC_IRMS, &tmpval);
    CRmsRecPtr_SetIrms(pRmsRec, 2, tmpval);
    CDevAde7880_RdPf(Self, REGID_PHC_PF, &i2val);
    CRmsRecPtr_SetPf(pRmsRec, 2, i2val);
    CDevAde7880_Rd16bit(Self, REGID_PHC_FRQ, &i2val);
    CRmsRecPtr_SetFrq(pRmsRec, 2, i2val);
    CDevAde7880_RdSE32bit(Self, REGID_PHC_PACT, &tmpval);
    CRmsRecPtr_SetPact(pRmsRec, 2, tmpval);
    CDevAde7880_RdSE32bit(Self, REGID_PHC_PAPR, &tmpval);
    CRmsRecPtr_SetPapr(pRmsRec, 2, tmpval);
    // Clear status
    CDevAde7880_Wr32bit(Self, REGID_STS0, BITMSK_DRDY);
}

void CDevAde7880_AplHwRst(CDevAde7880* Self)
{
    CSys_GpioSetVal(Self->m_RstPort, Self->m_RstPin, FALSE);
}

void CDevAde7880_RmvHwRst(CDevAde7880* Self)
{
    CSys_GpioSetVal(Self->m_RstPort, Self->m_RstPin, TRUE);
}

int CDevAde7880_Wr8bit(CDevAde7880* Self, uInt2 RegId, uInt1 Dat)
{
    Self->m_RegId   = RegId;
    Self->m_TxVal   = Dat;
    return CDevAde7880_SpiTrx(Self, REG_BITSZ_08, FALSE);
}

int CDevAde7880_Wr16bit(CDevAde7880* Self, uInt2 RegId, uInt2 Dat)
{
    Self->m_RegId   = RegId;
    Self->m_TxVal   = Dat;
    return CDevAde7880_SpiTrx(Self, REG_BITSZ_16, FALSE);
}

int CDevAde7880_Wr24bit(CDevAde7880* Self, uInt2 RegId, uInt4 Dat)
{
    Self->m_RegId   = RegId;
    Self->m_TxVal   = Dat;
    return CDevAde7880_SpiTrx(Self, REG_BITSZ_24, FALSE);
}

int CDevAde7880_Wr32bit(CDevAde7880* Self, uInt2 RegId, uInt4 Dat)
{
    Self->m_RegId   = RegId;
    Self->m_TxVal   = Dat;
    return CDevAde7880_SpiTrx(Self, REG_BITSZ_32, FALSE);
}

int CDevAde7880_RdPf(CDevAde7880* Self, uInt2 RegId, Int2* pDat)
{
    int sts = UNKNOWN;
    uInt2  ui2val = 0;
    sts = CDevAde7880_Rd16bitUnSign(Self, RegId, &ui2val);
    if (sts != OK)
        return sts;
    Int2 i2val = ui2val & 0x7FFF;
    if (ui2val & 0x8000) {
        // perform 2's complement
        i2val = (~i2val & 0x7FFF) + 1;
        i2val = i2val * (-1);
    }

    *pDat = i2val;
    return OK;
}

int CDevAde7880_RdSE32bit(CDevAde7880* Self, uInt2 RegId, Int4* pDat)
{// 24-bit signed
    int sts = UNKNOWN;
    Int4  i4val = 0;
    uInt4 u4val=0;
    sts = CDevAde7880_Rd32bit(Self, RegId, &u4val);
    i4val = u4val & 0x00FFFFFF;
    if (u4val & 0x80000000) {
        // perform 2's complement
        i4val = (~i4val & 0x007FFFFF) + 1;
        i4val = i4val * (-1);
    }
    *pDat = i4val;
    return sts;
}

int CDevAde7880_RdZP24bit(CDevAde7880* Self, uInt2 RegId, Int4* pDat)
{// 24-bit signed
    int sts = UNKNOWN;
    Int4  i4val = 0;
    uInt4 u4val=0;
    sts = CDevAde7880_Rd32bit(Self, RegId, &u4val);
    i4val = u4val & 0x007FFFFF;
    if (u4val & 0x00800000) {
        // perform 2's complement
        i4val = (~i4val & 0x007FFFFF) + 1;
        i4val = i4val * (-1);
    }
    *pDat = i4val;
    return sts;
}

int CDevAde7880_RdZP24bitX(CDevAde7880* Self, uInt2 RegId, Int4* pDat)
{// 24-bit signed
    int sts = UNKNOWN;
    Int4  i4val = 0;
    uInt4 u4val=0;
    sts = CDevAde7880_Rd32bit(Self, RegId, &u4val);
    i4val = u4val & 0x00FFFFFF;
    if (u4val & 0x00800000) 
        i4val = i4val * (-1);
    *pDat = i4val;
    return sts;
}

int CDevAde7880_Rd32bit(CDevAde7880* Self, uInt2 RegId, uInt4* pDat)
{
    Self->m_RegId   = RegId;
    int sts = CDevAde7880_SpiTrx(Self, REG_BITSZ_32, TRUE);
    if (sts == OK)
        *pDat = (uInt4)Self->m_RxVal;
    return sts;
}

int CDevAde7880_Rd24bit(CDevAde7880* Self, uInt2 RegId, uInt4* pDat)
{
    Self->m_RegId   = RegId;
    int sts = CDevAde7880_SpiTrx(Self, REG_BITSZ_24, TRUE);
    if (sts == OK)
        *pDat = (uInt4)Self->m_RxVal;
    return sts;
}

int CDevAde7880_Rd16bit(CDevAde7880* Self, uInt2 RegId, Int2* pDat)
{
    Self->m_RegId   = RegId;
    int sts = CDevAde7880_SpiTrx(Self, REG_BITSZ_16, TRUE);
    if (sts == OK)
        *pDat = (Int2)Self->m_RxVal;
    return sts;
}

int CDevAde7880_Rd16bitUnSign(CDevAde7880* Self, uInt2 RegId, uInt2* pDat)
{
    Self->m_RegId   = RegId;
    int sts = CDevAde7880_SpiTrx(Self, REG_BITSZ_16, TRUE);
    if (sts == OK)
        *pDat = (uInt2)Self->m_RxVal;
    return sts;
}

int CDevAde7880_Rd8bit(CDevAde7880* Self, uInt2 RegId, uInt2* pDat)
{
    Self->m_RegId   = RegId;
    int sts = CDevAde7880_SpiTrx(Self, REG_BITSZ_08, TRUE);
    if (sts == OK)
        *pDat = (uInt2)Self->m_RxVal;
    return sts;
}

int CDevAde7880_SpiTrx8bit(CDevAde7880* Self, BOOL bRd);
int CDevAde7880_SpiTrx16bit(CDevAde7880* Self, BOOL bRd);
int CDevAde7880_SpiTrx32bit(CDevAde7880* Self, BOOL bRd);
int CDevAde7880_SpiTrxTrg(CSpiChn* pChn, Byte WrD, Byte* pRdD, BOOL bRd);

int CDevAde7880_SpiTrx(CDevAde7880* Self, int BitSz, BOOL bRd)
{
    CSys_SpiChnCsLo(Self->m_pChn);
    int sts = UNKNOWN;
    switch (BitSz) {
        case REG_BITSZ_08:
            sts = CDevAde7880_SpiTrx8bit(Self, bRd);
            break;
        case REG_BITSZ_16:
            sts = CDevAde7880_SpiTrx16bit(Self, bRd);
            break;
        case REG_BITSZ_32:
            sts = CDevAde7880_SpiTrx32bit(Self, bRd);
            break;
        default:
            sts = ERR;
            break;
    }

    CSys_SpiChnCsHi(Self->m_pChn);
    return sts;
}

int CDevAde7880_SpiTrx8bit(CDevAde7880* Self, BOOL bRd)
{
    Byte cmd, regh, regl;
    if (bRd)
         cmd = 0x01;
    else cmd = 0x00;

    int sts = UNKNOWN;
    regh = INT2_HIGH(Self->m_RegId);
    regl = INT2_LOW(Self->m_RegId);

    Byte rd, wr;
//    Self->m_pChn->m_bTxEmpty = true;
    // Send cmd
    sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, cmd, &rd, false);
    if (sts != OK)
        return sts;
    // Send reg id msb
    sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, regh, &rd, false);
    if (sts != OK)
        return sts;
    // Send reg id lsb
    sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, regl, &rd, false);
    if (sts != OK)
        return sts;

    if (bRd)
         wr = 0xFF;
    else wr = (Byte)(Self->m_TxVal & 0xFF);
    rd = 0;
    sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, wr, &rd, bRd);
    if (sts != OK)
        return sts;
    if (bRd)
        Self->m_RxVal = rd;
    return OK;
}

int CDevAde7880_SpiTrx16bit(CDevAde7880* Self, BOOL bRd)
{
    Byte cmd, regh, regl;
    if (bRd)
         cmd = 0x01;
    else cmd = 0x00;

    int sts = UNKNOWN;
    regh = INT2_HIGH(Self->m_RegId);
    regl = INT2_LOW(Self->m_RegId);

    Byte rd, wr;
//    Self->m_pChn->m_bTxEmpty = true;
    // Send cmd
    sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, cmd, &rd, false);
    if (sts != OK)
        return sts;
    // Send reg id msb
    sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, regh, &rd, false);
    if (sts != OK)
        return sts;
    // Send reg id lsb
    sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, regl, &rd, false);
    if (sts != OK)
        return sts;

    int bitshift, i = 0;
    bitshift = 16;

    uInt4 bval = 0;
    uInt4 rval = 0;

    for (i = 0; i < 2; i++) {
        bitshift -= 8;
        if (bRd)
             wr = 0xFF;
        else wr = (Byte)((Self->m_TxVal >> bitshift) & 0xFF);
        rd = 0;
        sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, wr, &rd, bRd);
        if (sts != OK)
            return sts;
        bval = rd;
        bval <<= bitshift;
        rval |= bval;
    }
    Self->m_RxVal = rval;
    return OK;
}

int CDevAde7880_SpiTrx32bit(CDevAde7880* Self, BOOL bRd)
{
    Byte cmd, regh, regl;
    if (bRd)
         cmd = 0x01;
    else cmd = 0x00;

    int sts = UNKNOWN;
    regh = INT2_HIGH(Self->m_RegId);
    regl = INT2_LOW(Self->m_RegId);

    Byte rd, wr;
//    Self->m_pChn->m_bTxEmpty = true;
    // Send cmd
    sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, cmd, &rd, false);
    if (sts != OK)
        return sts;
    // Send reg id msb
    sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, regh, &rd, false);
    if (sts != OK)
        return sts;
    // Send reg id lsb
    sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, regl, &rd, false);
    if (sts != OK)
        return sts;

    int bitshift, i = 0;
    bitshift = 32;

    uInt4 bval = 0;
    uInt4 rval = 0;

    for (i = 0; i < 4; i++) {
        bitshift -= 8;
        if (bRd)
             wr = 0xFF;
        else wr = (Byte)((Self->m_TxVal >> bitshift) & 0xFF);
        rd = 0;
        sts = CDevAde7880_SpiTrxTrg(Self->m_pChn, wr, &rd, bRd);
        if (sts != OK)
            return sts;
        if (bRd) {
            bval = rd;
            bval <<= bitshift;
            rval |= bval;
        }
    }
    Self->m_RxVal = rval;
    return OK;
}

int CDevAde7880_SpiTrxTrg(CSpiChn* pChn, Byte WrD, Byte* pRdD, BOOL bRd)
{
    if (!pChn)
        return ERR;
    pChn->m_bTxEmpty = true;
    pChn->m_Txd = WrD;
    int sts = CSys_SpiChnTrx(pChn, bRd);
    if (sts != OK)
        return sts;
    if (bRd)
        *pRdD = pChn->m_Rxd;
    return OK;
}

//*********************************************************************
// End
//*********************************************************************
