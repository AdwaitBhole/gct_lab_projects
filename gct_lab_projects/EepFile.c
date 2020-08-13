//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

//=====================================================================
// CFileDef
//=====================================================================

void CFileDef_Init(TFileDef* Self, uInt2 Id, uInt2 MaxRec, uInt2 zRec, Byte* pSltMap)
{
    Self->m_Id          = Id;		// File Id
    Self->m_MaxRec 	= MaxRec;	// Max Rec
    Self->m_zRec 	= zRec;		// Sz Rec
    Self->m_bOpen 	= FALSE;
    Self->m_bNew 	= FALSE;
    Self->m_pSltMap     = pSltMap;
}

void CFileDef_InitAph(TFileDef* Self, uInt2 zAph, Byte* pAph)
{
    Self->m_zAph = zAph;			// Sz Aph
    Self->m_pAph = pAph;			// Ptr To Aph
}

void CFileDef_InitEepAdr(TFileDef* Self, uInt2 Adr)
{
    Self->m_EepAdr = Adr;		// Start Adr
}

Int2 CFileDef_GetRecCnt(TFileDef* Self)
{
    return Self->m_nRec;
}

Bool CFileDef_IsOpen(TFileDef* Self)
{
    return Self->m_bOpen;
}

Bool CFileDef_IsNew(TFileDef* Self)
{
    return Self->m_bNew;
}

//=====================================================================
// CFileFph
//=====================================================================
void CFileFph_Init(TFileFph* Self)
{
    memset(Self, 0, sizeof(TFileFph));
}

void CFileFph_Init2(TFileFph* Self, TFileDef* pFileDef)
{
    Self->m_Id              = pFileDef->m_Id;
    Self->m_MaxRec	    = pFileDef->m_MaxRec;
    Self->m_zRec	    = pFileDef->m_zRec;
    Self->m_zAph	    = pFileDef->m_zAph;
    Self->m_iFstRecSlt      = 0xFF;
}

Bool CFileFph_IsOk(TFileFph* Self, TFileDef* pFileDef)
{
    return ((Self->m_Id != pFileDef->m_Id) ||
    (Self->m_MaxRec != pFileDef->m_MaxRec) ||
    (Self->m_zRec != pFileDef->m_zRec)     ||
    (Self->m_zAph != pFileDef->m_zAph)
    ) ? FALSE : TRUE;
}

//=====================================================================
// CFile
//=====================================================================
#define		SZ_FILE_FPH		(sizeof(TFileFph))
#define		SZ_SLTHDR		(sizeof(Byte))
#define		SZ_SLT(zRec)		(zRec+SZ_SLTHDR)
#define		OFS_FILE_FPH	         0
#define		OFS_FILE_APH	        (0+SZ_FILE_FPH)

#define		SZ_FILE_APH(pFile)	(pFile->m_pDef->m_zAph)
#define		SZ_REC(pFile)        	(pFile->m_Fph.m_zRec)
#define 	OFS_SLT_0(pFile)	(SZ_FILE_FPH + SZ_FILE_APH(pFile))
#define 	OFS_SLT(pFile, i)	(OFS_SLT_0(pFile) + (i*SZ_SLT(pFile->m_Fph.m_zRec)))
#define		OFS_REC(pFile, i)	(OFS_SLT(pFile, i)+SZ_SLTHDR)

void CFile_Init(TFile* Self, TFileDef* SelfDef, CDevEep24AA512* pDev)
{
    Self->m_pDef = SelfDef;
    CFileFph_Init2(&Self->m_Fph, Self->m_pDef);
    Self->m_pDef->m_bOpen = FALSE;
    Self->m_pDef->m_bNew = FALSE;
    Self->m_pDev = pDev;
}

int CFile_Create(TFile* Self)
{
    Int2 i;
    Int1 sts;
    CFileFph_Init2(&Self->m_Fph, Self->m_pDef);
    //m_Fph.Init(m_pDef);
    sts = CFile_WrData(Self, OFS_FILE_FPH, (Byte *)&Self->m_Fph, SZ_FILE_FPH);
    if (sts != OK)
	return sts;
    // Mark all slot as Free slot
    for (i = 0; i < Self->m_Fph.m_MaxRec; i++) {
	sts = CFile_WrSltHdr(Self,i, SLT_FREE);
	Self->m_pDef->m_pSltMap[i] = SLT_FREE;
	if (sts != OK)
	    return ERR;
        }
    Self->m_pDef->m_bOpen = TRUE;
    Self->m_pDef->m_bNew = TRUE;
    return OK;
}

int CFile_Open(TFile* Self, Bool bCreate)
{
    if (CFile_IsOpen(Self)) {	// File already open!
	return OK;
    }

    int sts = CFile_RdData(Self, OFS_FILE_FPH, (Byte*)&Self->m_Fph, SZ_FILE_FPH);
    if (sts != OK) {
	return sts;
    }

    if(!CFileFph_IsOk(&Self->m_Fph, Self->m_pDef)) {
	if (bCreate)
	    return CFile_Create(Self);
    //ERRHND.Upd(ERR_EEP_FPH);
    }

    CFile_OnOpen(Self);
    Self->m_pDef->m_bOpen = TRUE;
    return OK;
}

int CFile_OnOpen(TFile* Self)
{
    Int2 i;
    int sts;
    Byte slthdr, lstrec;
    if (Self->m_Fph.m_iFstRecSlt == SLT_FREE)
	return ERR;
    Self->m_pDef->m_nRec = 0;
    lstrec = 0;

    for (i = 0; i < Self->m_Fph.m_MaxRec; i++)
    {
        uInt2 ofs = OFS_SLT(Self, i);
        sts = CFile_RdData(Self, ofs, (Byte *)&slthdr, SZ_SLTHDR);	// Rd RecHdr
        if (sts != OK)
            return ERR;
        Self->m_pDef->m_pSltMap[i] = slthdr;							// Upd SltMap
        if (slthdr != SLT_FREE)
            Self->m_pDef->m_nRec++;
        if (slthdr == SLT_LAST)
            lstrec++;
    }
    if (lstrec != 1) {
        return ERR;
    }
    return OK;
}

int CFile_GetRecCnt(TFile* Self)
{
    return CFileDef_GetRecCnt(Self->m_pDef);
}

int CFile_RdRec(TFile* Self, Int2 iRec, Byte* pRec)
{
    if (!CFile_IsOpen(Self)) {	// File not open!
    //ERRHND.Upd(ERR_FILE_OPEN);
	return ERR;
    }
    Int2 islt = CFile_GetRecSlt(Self, iRec);
    if (islt < 0)
	return ERR;
    return CFile_RdData(Self, OFS_REC(Self, islt), pRec, Self->m_Fph.m_zRec);
}

int CFile_AddRec(TFile* Self, Byte* pRec)
{
    int sts;
    if (!CFile_IsOpen(Self)) {	// File not open!
    // ERRHND.Upd(ERR_FILE_OPEN);
	    return ERR;
    }
    // First Rec
    if (!Self->m_pDef->m_nRec)
	return CFile_AddFirstRec(Self,pRec);
    // Search Free Slot
    Int2 newslt = CFile_GetFreeSlt(Self);
    if (newslt < 0) {
    //ERRHND.Upd(ERR_EEP_MEMFULL);
	return ERR;
    }
    // Search Lst Slot
    Int2 lstslt = CFile_GetLstRecSlt(Self);
    sts = CFile_WrSltHdr(Self,lstslt, newslt);					// Lnk lstslt to newslt
    if (sts != OK)
	return ERR;
    Self->m_pDef->m_pSltMap[lstslt] = newslt;				// Upd Slot Map
    //
    sts = CFile_WrSltHdr(Self,newslt, SLT_LAST);				// treat newslt as lstslt
    if (sts != OK)
	return ERR;
    sts = CFile_WrData(Self, OFS_REC(Self, newslt), pRec, Self->m_Fph.m_zRec);	// Wr Rec
    if (sts != OK)
	return ERR;
    Self->m_pDef->m_pSltMap[newslt] = SLT_LAST;			// Upd Slot Map
    Self->m_pDef->m_nRec++;
    return CFile_WrData(Self, OFS_FILE_FPH, (Byte* )&Self->m_Fph, SZ_FILE_FPH);
    return OK;
}

int CFile_EdtRec(TFile* Self, Int2 iRec, Byte* pRec)
{
    if (!CFile_IsOpen(Self)) {	// File not open!
    //ERRHND.Upd(ERR_FILE_OPEN);
	return ERR;
	}
    Int2 islt = CFile_GetRecSlt(Self,iRec);
    if (islt < 0)
	return ERR;
    return CFile_WrData(Self, OFS_REC(Self, islt), pRec, Self->m_Fph.m_zRec);		// Upd freeslt as lstslt
}

int CFile_DelRec(TFile* Self, Int2 iRec)
{
    Int2 nxtslt, curslt;
    Int1 sts;
    if (!CFile_IsOpen(Self)) {	// File not open!
    //ERRHND.Upd(ERR_FILE_OPEN);
	return ERR;
	}
    curslt = CFile_GetRecSlt(Self,iRec);
    if (curslt < 0)
	return ERR;
    // Is Fst Rec
    if (curslt == Self->m_Fph.m_iFstRecSlt) {
	nxtslt = Self->m_pDef->m_pSltMap[curslt];
	Self->m_Fph.m_iFstRecSlt = nxtslt;
	return CFile_FreeSlt(Self,curslt);
	}
    Int2 prvslt = CFile_GetPrvSlt(Self, curslt);
    // Is Lst Rec
    if (curslt == SLT_LAST)
	return CFile_DelRecAtLstSlt(Self, curslt, prvslt);
    // Is intermediate rec
    if (prvslt < 0)
	return ERR;
    nxtslt = Self->m_pDef->m_pSltMap[curslt];
    sts = CFile_WrSltHdr(Self, prvslt, nxtslt);				// Lnk PrvSlt to Nxtslt
    if (sts != OK)
	return ERR;
    Self->m_pDef->m_pSltMap[prvslt] = nxtslt;			// Upd SltMap
    return CFile_FreeSlt(Self, curslt);
    return OK;
}

int CFile_RdFld(TFile* Self, Int2 iRec, Int2 FldOfs, Int2 FldLen, Byte* pFld)
{
    uInt2 ofs;
    if (!CFile_IsOpen(Self)) {	// File not open!
    //ERRHND.Upd(ERR_FILE_OPEN);
	return ERR;
	}
    if (CFile_GetRecOfs(Self, iRec, &ofs) != OK)
	return ERR;
    ofs += FldOfs;
    return CFile_RdData(Self, ofs, pFld, FldLen);
}

int CFile_WrFld(TFile* Self, Int2 iRec, Int2 FldOfs, Int2 FldLen, Byte* pFld)
{
    uInt2 ofs;
    if (!CFile_IsOpen(Self)) {	// File not open!
    //ERRHND.Upd(ERR_FILE_OPEN);
	return ERR;
	}
    if (CFile_GetRecOfs(Self, iRec, &ofs) != OK)
	return ERR;
    ofs += FldOfs;
    return CFile_WrData(Self, ofs, pFld, FldLen);
}

int CFile_WrAph(TFile* Self)
{
    if (!CFile_IsOpen(Self)) {	// File not open!
    //	ERRHND.Upd(ERR_FILE_OPEN);
	return ERR;
    }
    return CFile_WrData(Self, OFS_FILE_APH, Self->m_pDef->m_pAph, Self->m_pDef->m_zAph);
}

int CFile_RdAph(TFile* Self)
{
    if (!CFile_IsOpen(Self)) {	// File not open!
    //	ERRHND.Upd(ERR_FILE_OPEN);
	return ERR;
	}
    return CFile_RdData(Self, OFS_FILE_APH, Self->m_pDef->m_pAph, Self->m_pDef->m_zAph);
}

Bool CFile_IsOpen(TFile* Self)
{
    return Self->m_pDef->m_bOpen;
}

int CFile_AddFirstRec(TFile* Self,Byte* pRec)
{
    Self->m_Fph.m_iFstRecSlt = 0;
    Int1 sts = CFile_WrSltHdr(Self,0, SLT_LAST);
    if (sts != OK)
	return ERR;
    sts = CFile_WrData(Self, OFS_REC(Self, 0), pRec, Self->m_Fph.m_zRec);
    if (sts != OK)
	return ERR;
    Self->m_pDef->m_pSltMap[0] = SLT_LAST;					// Upd SltMap
    Self->m_pDef->m_nRec++;
    return CFile_WrData(Self, OFS_FILE_FPH, (Byte* )&Self->m_Fph, SZ_FILE_FPH);
}

int CFile_DelRecAtLstSlt(TFile* Self, Int2 SltId, Int2 PrvSlt)
{
    if (PrvSlt >= 0) {
	Int1 sts = CFile_WrSltHdr(Self, PrvSlt, SLT_LAST);				// treat Prvslt as lstslt
    if (sts != OK)
	return ERR;
    Self->m_pDef->m_pSltMap[PrvSlt] = SLT_LAST;				// Upd SltMap
	}
    return CFile_FreeSlt(Self, SltId);
}

int CFile_FreeSlt(TFile* Self,uInt1 SltId)
{
    Int1 sts = CFile_WrSltHdr(Self, SltId, SLT_FREE);
    if (sts != OK)
	return ERR;
    Self->m_pDef->m_pSltMap[SltId] = SLT_FREE;						// Upd SltMap
    Self->m_pDef->m_nRec--;											// Upd SltMap
    return CFile_WrData(Self, OFS_FILE_FPH, (Byte* )&Self->m_Fph, SZ_FILE_FPH);
}

Int2 CFile_GetFreeSlt(TFile* Self)
{
    Int2 i;
    for (i = 0; i < Self->m_Fph.m_MaxRec; i++) {
	if (Self->m_pDef->m_pSltMap[i] == SLT_FREE)
	    return i;
	}
    return -1;
}

Int2 CFile_GetRecSlt(TFile* Self, Int2 iRec)
{
    Int2 islt = Self->m_Fph.m_iFstRecSlt;
    if (!iRec) 	// Fst Rec
	return islt;
    Int2 i = 0;
    while (islt != SLT_LAST) {
	if (i == iRec)
	    return islt;
	i++;
	if ((i >= Self->m_Fph.m_MaxRec) || (islt == SLT_FREE))
	    break;
	islt = Self->m_pDef->m_pSltMap[islt];
	}
//	ERRHND.Upd(ERR_EEP_REQREC);
    return -1;
}

Int2 CFile_GetLstRecSlt(TFile* Self)
{
    Int2 i ;
    for ( i = 0; i < Self->m_Fph.m_MaxRec; i++) {
	if (Self->m_pDef->m_pSltMap[i] == SLT_LAST)
	    return i;
	}
	return -1;
}

Int2 CFile_GetPrvSlt(TFile* Self, uInt1 SltId)
{
    Int2 curslt, prvslt, i;
    prvslt = Self->m_Fph.m_iFstRecSlt;
    i = 0;
    while (prvslt != SLT_LAST) {
	curslt = Self->m_pDef->m_pSltMap[prvslt];
	if (curslt == SltId)
	    return prvslt;
	prvslt = curslt;
	i++;
	if ((i >= Self->m_Fph.m_MaxRec) || (curslt == SLT_FREE))
	    break;
	}
    //	ERRHND.Upd(ERR_EEP_REQSLT);
    return -1;
}

int CFile_GetRecOfs(TFile* Self, Int2 iRec, uInt2* pOfs)
{
    Int2 slt = CFile_GetRecSlt(Self, iRec);
    if (slt < 0)
	return ERR;
    *pOfs = OFS_REC(Self, iRec);
    return OK;
}

int CFile_WrSltHdr(TFile* Self, uInt2 iSlt, Byte Data)
{
    return CFile_WrData(Self, OFS_SLT(Self, iSlt), &Data, SZ_SLTHDR);
}

int CFile_RdData(TFile* Self, uInt2 Ofs, Byte* pData, uInt2 Len)
{
    Ofs += Self->m_pDef->m_EepAdr;    
    return CDevEep24AA512_Rd(Self->m_pDev, Ofs, pData, Len);
}

int CFile_WrData(TFile* Self, uInt2 Ofs, Byte* pData, uInt2 Len)
{
    Ofs += Self->m_pDef->m_EepAdr;
    return CDevEep24AA512_Wr(Self->m_pDev, Ofs, pData, Len);
}

//*********************************************************************
// End
//*********************************************************************
