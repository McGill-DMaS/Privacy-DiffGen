// TDCut.cpp: implementation of the CTDCut class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDCUT_H)
    #include "TDCut.h"
#endif

CTDCut::CTDCut()
{
}

CTDCut::~CTDCut() 
{
}

//---------------------------------------------------------------------------
// Specialize the parent concept in the cut.
//---------------------------------------------------------------------------
bool CTDCut::specializeConcept(CTDConcept* pParent)
{
    ASSERT(pParent);
    CTDConcept* pChildConcept = NULL;
    int nChild = pParent->getNumChildConcepts();
    if (nChild <= 0) {
        ASSERT(false);
        return false;
    }
    for (int c = 0; c < nChild; ++c) {
        pChildConcept = pParent->getChildConcept(c);
        pChildConcept->m_cutPos = AddTail(pChildConcept);
    }
    RemoveAt(pParent->m_cutPos);
    pParent->m_cutPos = NULL;
    return true;
}
