// TDRollbackRec.cpp: implementation of the CTDRollbackRec class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDROLLBACKREC_H)
    #include "TDRollbackRec.h"
#endif

//*****************
// CTDRollbackRec *
//*****************

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CTDRollbackRecs::cleanup() {
    int nRecs = GetSize();
    for (int i = 0; i < nRecs; ++i)
        delete GetAt(i);
    RemoveAll();
}
