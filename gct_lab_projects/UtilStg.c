//*********************************************************************
// Title        : PTaC-Lib
// Developed For: GGA, Mumbai
// Developed By : Gayatri ComtekLab, Mumbai, India
//*********************************************************************
#include "../include/CmnInc.h"

// ====================================================================
// CStg
// ====================================================================

void CStg_Init(CStg* Self, char* Ptr, Int2 Sz)
{
    if (!Self)
        return;
    Self->m_Buf = Ptr;
    Self->m_Sz = Sz;
    memset(Self->m_Buf, 0, Self->m_Sz);
}

Bool CStg_IsDef(CStg* Self)
{
    return ((Self->m_Buf != NULL) && (Self->m_Sz != 0)) ? TRUE : FALSE;
}

void CStg_SetStgWithLen(CStg* Self, char* pSrc, int Len)
{
    if (!CStg_IsDef(Self))
        return;
    if (Len >= Self->m_Sz)
        Len = Self->m_Sz-1;
    strncpy(Self->m_Buf, pSrc, Len);
}

void CStg_SetStg(CStg* Self, char* pSrc)
{
    if (!CStg_IsDef(Self))
        return;
    //
    int len = strlen(pSrc);
    CStg_SetStgWithLen(Self, pSrc, len);
}


int CStg_GetLen(CStg* Self)
{
    return strlen(Self->m_Buf);
}

Bool CStg_IsEmpty(CStg* Self)
{
    return (CStg_GetLen(Self) == 0) ? TRUE : FALSE;
    return OK;
}

int CStg_Empty(CStg* Self)
{
    char* p = CStg_GetPtr(Self);
    int len = CStg_GetSz(Self);
    memset(p, 0, len);
    return OK;
}

char CStg_GetAt(CStg* Self, int Ndx)
{
    int len = CStg_GetLen(Self);
    int sz = CStg_GetSz(Self);
    char* p = CStg_GetPtr(Self);
    if (!p)
	return 0;
    if ((Ndx >= sz) || (Ndx >= len))
	return p[0];
    return p[Ndx];
}

void CStg_SetAt(CStg* Self, int Ndx, char Ch)
{
    if ((Ndx >= CStg_GetSz(Self)) || (Ndx >= CStg_GetLen(Self)))
	return;
    char* p = CStg_GetPtr(Self);
    if (!p)
	return;
    p[Ndx] = Ch;
}

void CStg_Set(CStg* Self, char Ch, int Len)
{
    int sz = CStg_GetSz(Self);
    if (Len >= sz)
	    Len = sz -1;
    char* p = CStg_GetPtr(Self);
    memset(p, Ch, Len);
    p[Len] = 0;
}

int CStg_Mid(CStg* Self, int Ndx, int Stg)
{
    // return length of Stg
    if ((Ndx >= CStg_GetSz(Self)) || (Ndx >= CStg_GetLen(Self))) {
	CStg_SetAt(Self, 0, 0);
	    return 0;
    }
     Stg = CStg_GetAt(Self, Ndx);

    return CStg_GetLen(Self);
}

int CStg_Mid2(CStg* Self, int Ndx, int Cnt, CStg* Stg)
{
    // return length of Stg
    char* p = CStg_GetPtr(Self);
    char* pstg = CStg_GetPtr(Stg);
    if (!p || !pstg)
	return 0;
    Int1 len = CStg_GetLen(Self);
    if ((Ndx >= CStg_GetSz(Self)) || (Ndx >= CStg_GetSz(Stg))|| (Ndx >= len)) {
	CStg_SetAt(Self, 0, 0);
	return 0;
    }
    strncpy(pstg, &p[Ndx], Cnt);
    pstg[Cnt] = 0;
    return CStg_GetLen(Stg);
}

int CStg_Left(CStg* Self, int Cnt, CStg* Stg)
{
    // return length of Stg
    char* p = CStg_GetPtr(Self);
    char* pstg = CStg_GetPtr(Stg);
    if (!p || !pstg)
	return 0;
    int stgsz = CStg_GetSz(Stg);
    if (Cnt >= stgsz) 	// If Cnt > Stg, Trim it to stgSz
	Cnt = stgsz - 1;
    int len = CStg_GetLen(Self);
    if ((Cnt >= CStg_GetSz(Self)) || (Cnt >= len))		// If Cnt > sz or length trimit ot length
	Cnt = len;
    strncpy(pstg, p, Cnt);
    pstg[Cnt] = 0;
    return CStg_GetLen(Stg);
}

int CStg_Right(CStg* Self, int Cnt, CStg* Stg)
{
    int len = CStg_GetLen(Self);
    if (Cnt > len)
	Cnt = len;
    int ndx = len-Cnt;
    return CStg_Mid2(Self, ndx, Cnt, Stg);
}

void CStg_MakeUpper(CStg* Self)
{
    int i;
    int len = CStg_GetLen(Self);
    char* p = CStg_GetPtr(Self);
    for (i = 0; i < len; i++) {
	if (isalpha(p[i]))
	    p[i] &= (~0x20);
	}
}

void CStg_MakeLower(CStg* Self)
{
    int i;
    int len = CStg_GetLen(Self);
    char* p = CStg_GetPtr(Self);
    for (i = 0; i < len; i++) {
	if (isalpha(p[i]))
	    p[i] |= 0x20;
	}
}

void CStg_MakeReverse(CStg* Self)
{
    int i;
    int len = CStg_GetLen(Self);
    int j = len-1;
    char* p = CStg_GetPtr(Self);
    char ch;
    for (i = 0; i < len; i++) {
	if (i == j)
	    break;
	ch = p[i];
	p[i] = p[j];
	p[j] = ch;
	j--;
	}
}

int CStg_Replace(CStg* Self, char Old, char New)
{
    // returns cnt of char replaced
    int i;
    int cnt = 0;
    int len = CStg_GetLen(Self);
    char* p = CStg_GetPtr(Self);
    for (i = 0; i < len; i++) {
	if (p[i] == Old) {
	    p[i] = New;
	    cnt++;
	    }
	}
    return cnt;
}

int CStg_Remove(CStg* Self, char Ch)
{
    // returns cnt of char removed
    int i;
    int cnt = 0;
    int len = CStg_GetLen(Self);
    char* p = CStg_GetPtr(Self);
    for (i = 0; i < len; i++) {
	if (p[i] == Ch)
	    continue;
	p[cnt] = p[i];
	cnt++;
	}
    p[cnt] = 0;
    return cnt;
}

int CStg_Insert(CStg* Self, int Ndx, char Ch)
{
    // returns length of modified stg if success else return -1
    int len = CStg_GetLen(Self);
    CStg_GetSz(Self);
    if (len >= CStg_GetSz(Self)-1) // No room for inserting
        return -1;
    if (Ndx > len)
        return -1;

    char* p = CStg_GetPtr(Self);
    int i;
    for (i = len; i >= 0; i--) {
        if (i == Ndx) {
            p[i] = Ch;
            break;
        }
        p[i] = p[i-1];
    }
    p[len+1] = 0;
    return CStg_GetLen(Self);
}

Bool CStg_AddCh(CStg* Self, char Ch)
{
    // "Hello"
    // CStg_Add(Self, " World") => "Hello World"
    char* s1 = CStg_GetPtr(Self);
    int sz = CStg_GetSz(Self);
    int s1Len = CStg_GetLen(Self);
    int avlSpc = sz - s1Len -1;
    if (avlSpc > 0) {
        s1[s1Len] = Ch;
        s1[s1Len+1] = 0;
        return TRUE;
    }
    return FALSE;
}

int CStg_Add(CStg* Self, char* pSubStg)
{
    // "Hello"
    // CStg_Add(Self, " World") => "Hello World"
    char* s1 = CStg_GetPtr(Self);
    int sz = CStg_GetSz(Self);
    int s1Len = CStg_GetLen(Self);
    int avlSpc = sz - s1Len -1;

    int s2Len = strlen(pSubStg);
    if (s2Len >= avlSpc)
        s2Len = avlSpc;
    int i;
    for (i = 0; i < s2Len; i++) {
        s1[s1Len++] = pSubStg[i];
    }
    s1[s1Len] = 0;
    return s1Len;
}

int CStg_AddStg(CStg* Self, CStg* SubStg)
{
    char* p = CStg_GetPtr(SubStg);
    return CStg_Add(Self, p);
}

int CStg_Delete(CStg* Self, int Ndx, int Cnt)
{
    // returns length of modified stg
    Cnt = 1;
    Int1 len = CStg_GetLen(Self);
    if (Ndx >= len)
	return len;
    int j = 0;
    char* p = CStg_GetPtr(Self);
    int i;
    for (i = 0; i < len; i++) {
	if (i < Ndx) {
	    j++;
	    continue;
	    }
	if (Cnt--)
	    continue;
	p[j] = p[i];
	j++;
	}
    p[j] = 0;
    return CStg_GetLen(Self);
}



void CStg_TrimLeft(CStg* Self, char Ch)
{
    int len = CStg_GetLen(Self);
    char* p = CStg_GetPtr(Self);
    int i;
    for (i = 0; i < len; i++) {
	if (p[i] != Ch)
	    break;
	}
    int cnt = 0;
    int j;
    for (j = i; j < len; j++)
	p[cnt++] = p[j];
    p[cnt] = 0;
}

void CStg_TrimRight(CStg* Self, char Ch)
{
    Int1 len;
    len= CStg_GetLen(Self);
    CStg_MakeReverse(Self);
    CStg_TrimLeft(Self, Ch);
    CStg_MakeReverse(Self);
}

int CStg_Find(CStg* Self, char Ch)
{
    return CStg_Find2(Self, Ch, 0);
}

int CStg_Find2(CStg* Self, char Ch, int Ndx)
{
    Int1 len = CStg_GetLen(Self);
    if (Ndx >= len)
	return -1;
    char* p = CStg_GetPtr(Self);
    char* p1 = strchr(&p[Ndx], Ch);
    if (!p1)
	return -1;
    return (p1-p);
}

int CStg_Find3(CStg* Self, char* pSrc)
{
    return CStg_Find2(Self, *pSrc, 0);
}

int CStg_Find4(CStg* Self, char* pSrc, int Ndx)
{
    int len = CStg_GetLen(Self);
    int srclen = strlen(pSrc);
    if ((!len) || (Ndx >= len) ||
	(!srclen) || (Ndx >= srclen))
	return -1;
    char* p = CStg_GetPtr(Self);
    int i;
    for (i = Ndx; i < len; i++) {
	if (p[i] == *pSrc) {
	    if (strncmp(&p[i], pSrc, srclen) == 0)
		return i;
	    }
	}
    return -1;
}

int CStg_ReverseFind(CStg* Self, char Ch)
{
    int len = CStg_GetLen(Self);
    if (!len)
	return -1;
    int j = len-1;
    char* p = CStg_GetPtr(Self);
    int i;
    for (i = 0; i < len; i++) {
	if (p[j] == Ch)
	    return j;
	j--;
	}
    return -1;
}

int CStg_FindOneOf(CStg* Self, char* pSrcSet)
{
    int len = CStg_GetLen(Self);
    int srclen = strlen(pSrcSet);
    if ((!len) || (!srclen))
	return -1;
    char* p = CStg_GetPtr(Self);
    int i;
    for (i = 0; i < len; i++) {
        int j;
	for (j = 0; j < srclen; j++)
	    if (p[i] == pSrcSet[j])
		return i;
	}
    return -1;
}

int CStg_Compare(CStg* Self, char* pSrc)
{
    char* p = CStg_GetPtr(Self);
    return strcmp(p, pSrc);
}

int CStg_CompareNoCase(CStg* Self, char* pSrc)
{
    int i;
    int srclen = strlen(pSrc);
    int len = CStg_GetLen(Self);
    if (len > srclen)
	return 1;
    if (len < srclen)
	return -1;
    char* p = CStg_GetPtr(Self);
    for (i = 0; i < len; i++) {
        if ((p[i] | 0x20) != (pSrc[i] | 0x20)) {
	    if (p[i] > pSrc[i])
		return 1;
		return -1;
	    }
	}
    return 0;
}

char* CStg_GetPtr(CStg* Self)
{
    return Self->m_Buf;
}

Int2  CStg_GetSz(CStg* Self)
{
    return Self->m_Sz;
}

void CStg_Fmt(CStg* Self, char* Fmt, ...)
{
    va_list args;
    va_start(args, Fmt);
    vsprintf(Self->m_Buf, Fmt, args);
    va_end(args);
}

//*********************************************************************
// End
//*********************************************************************
