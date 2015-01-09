// TDPartAttrib.cpp: implementation of the CTDPartAttrib class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDPARTATTRIB_H)
    #include "TDPartAttrib.h"
#endif

//****************
// CTDPartAttrib *
//****************

CTDPartAttrib::CTDPartAttrib(CTDAttrib* pActualAttrib)
    : m_pActualAttrib(pActualAttrib), 
      m_pSupportMatrix(NULL), 
      m_bCandidate(true),
      m_relatedPos(NULL)
{
}

CTDPartAttrib::~CTDPartAttrib() 
{
    delete m_pSupportMatrix;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDPartAttrib::initSupportMatrix(CTDConcept* pCurrCon, int nClasses)
{
    // Allocate the matrix
    int dims[] = {pCurrCon->getNumChildConcepts(), nClasses};
    m_pSupportMatrix = new CTDMDIntArray(sizeof(dims) / sizeof(int), dims);
    if (!m_pSupportMatrix) {
        ASSERT(false);
        return false;
    }

    // Allocate support sums
    m_supportSums.SetSize(pCurrCon->getNumChildConcepts());

    // Allocate class sums
    m_classSums.SetSize(nClasses);

    for (int i = 0; i < pCurrCon->getNumChildConcepts(); ++i) {
        for (int j = 0; j < nClasses; ++j) {
            (*m_pSupportMatrix)[i][j] = 0;
            m_classSums.SetAt(j, 0);
        }
        m_supportSums.SetAt(i, 0);
    }
    return true;
}



//*****************
// CTDPartAttribs *
//*****************
CTDPartAttribs::CTDPartAttribs()
{
}

CTDPartAttribs::~CTDPartAttribs()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
