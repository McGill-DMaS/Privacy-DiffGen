// TDAttribute.cpp: implementation of the CTDAttrib class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDATTRIBUTE_H)
    #include "TDAttribute.h"
#endif

//************
// CTDAttrib *
//************

CTDAttrib::CTDAttrib(LPCTSTR attribName)
    : m_attribName(attribName), m_pConceptRoot(NULL), m_attribIdx(-1), m_nOFlatConcepts(-1), m_bVirtualAttrib(false), m_bMaskTypeSup(false)
{
}

CTDAttrib::~CTDAttrib() 
{
    delete m_pConceptRoot;
    m_pConceptRoot = NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDAttrib::initCutToRoot()
{
    m_cut.RemoveAll();
    if (!m_pConceptRoot) {
        ASSERT(false);
        return false;
    }
    m_pConceptRoot->m_cutPos = m_cut.AddTail(m_pConceptRoot);
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDAttrib::flattenHierarchy()
{
    m_flattenConcepts.RemoveAll();

    CPtrList attList;
    attList.AddTail(m_pConceptRoot);

    CTDConcept* pConcept = NULL;
    while (!attList.IsEmpty()) {
        pConcept = (CTDConcept*) attList.RemoveHead();
        if (!pConcept) {
            ASSERT(false);
            return false;
        }                
        pConcept->m_flattenIdx = m_flattenConcepts.Add(pConcept);
        
        int nChildren = pConcept->getNumChildConcepts();
        for (int i = 0; i < nChildren; ++i) {
            attList.AddTail(pConcept->getChildConcept(i));
        }
    }
    m_nOFlatConcepts = m_flattenConcepts.GetSize();
    return true;
}


//---------------------------------------------------------------------------
// Calculate the number of required bits.
//---------------------------------------------------------------------------
bool CTDAttrib::calBits()
{
    for (int i = 0; i < m_reqBits.GetSize(); ++i)
        m_reqBits[i] = (int) ceil(log2(m_reqBits[i])); 
    return true;
}

//****************
// CTDDiscAttrib *
//****************

CTDDiscAttrib::CTDDiscAttrib(LPCTSTR attribName, bool bMaskTypeSup)
    : CTDAttrib(attribName)
{
    m_bMaskTypeSup = bMaskTypeSup;
}

CTDDiscAttrib::~CTDDiscAttrib() 
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDDiscAttrib::initHierarchy(LPCTSTR conceptStr)
{
    ASSERT(!m_pConceptRoot);
    m_pConceptRoot = new CTDDiscConcept(this);
    if (!m_pConceptRoot)
        return false;
    if (!m_pConceptRoot->initHierarchy(conceptStr, 0, m_reqBits))
        return false;
    if (m_bMaskTypeSup) {
        // Suppression. Recontruct hierarchy.
        if (!reconstructHierarchy())
            return false;
    }
    if (!flattenHierarchy())
        return false;
    if (!calBits())
        return false;
    if (!initCutToRoot())
        return false;
#ifdef _DEBUG_PRT_INFO
    cout << _T("Reconstructed attribute: ") << m_attribName << endl;
    for (int c = 0; c < m_flattenConcepts.GetSize(); ++c) {
        cout << m_flattenConcepts.GetAt(c)->m_conceptValue << _T(" ");
    }
    cout << endl;
#endif
    return true;
}

//---------------------------------------------------------------------------
// Reconstruct the hierarchy for categorical attributes.
// For protected attribute, the hierachy should be flat, e.g.,
//        Any
//       / | \
//      A  B  C
//---------------------------------------------------------------------------
bool CTDDiscAttrib::reconstructHierarchy()
{
    CTDConcept* pNewRootConcept = new CTDDiscConcept(m_pConceptRoot->getAttrib());
    pNewRootConcept->m_conceptValue = m_pConceptRoot->m_conceptValue;
    if (!reconstructHierarchyHelper(m_pConceptRoot, pNewRootConcept))
        return false;
    
    // Delete the old tree.
    delete m_pConceptRoot;
    m_pConceptRoot = pNewRootConcept;

    // Update the number of branches accordingly.
    m_reqBits[0] = pNewRootConcept->getNumChildConcepts();
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDDiscAttrib::reconstructHierarchyHelper(const CTDConcept* pThisConcept, CTDConcept* pNewRootConcept)
{
    if (pThisConcept->getNumChildConcepts() == 0) {
        // This is a leaf.
        CTDDiscConcept* pNewLeaf = new CTDDiscConcept(this);
        pNewLeaf->m_conceptValue = pThisConcept->m_conceptValue;
        pNewLeaf->m_depth = 1;
        if (!pNewRootConcept->addChildConcept(pNewLeaf)) {            
            delete pNewLeaf;
            pNewLeaf = NULL;
            ASSERT(false);
            return false;
        }
        return true;
    }

    for (int i = 0; i < pThisConcept->getNumChildConcepts(); ++i) {
        if (!reconstructHierarchyHelper(pThisConcept->getChildConcept(i), pNewRootConcept))
            return false;
    }
    return true;
}

//****************
// CTDContAttrib *
//****************

CTDContAttrib::CTDContAttrib(LPCTSTR attribName) 
    : CTDAttrib(attribName)
{
}

CTDContAttrib::~CTDContAttrib() 
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDContAttrib::initHierarchy(LPCTSTR conceptStr)
{
    ASSERT(!m_pConceptRoot);
    m_pConceptRoot = new CTDContConcept(this);
    if (!m_pConceptRoot)
        return false;
    if (!m_pConceptRoot->initHierarchy(conceptStr, 0, m_reqBits))
        return false;
    if (!flattenHierarchy())
        return false;
    return initCutToRoot();
}



//*************
// CTDAttribs *
//*************
CTDAttribs::CTDAttribs()
{
}

CTDAttribs::~CTDAttribs()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CTDAttribs::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ostream& operator<<(ostream& os, const CTDAttribs& attribs)
{
#ifdef _DEBUG_PRT_INFO
    CTDAttrib* pAttrib = NULL;
    for (int a = 0; a < attribs.GetSize(); ++a) {
        pAttrib = attribs.GetAt(a);
        os << _T("[") << pAttrib->m_attribIdx << _T("]\t") << pAttrib->m_attribName << endl;
    }
#endif
    return os;
}