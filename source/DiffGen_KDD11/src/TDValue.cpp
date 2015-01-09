// TDValue.cpp: implementation of the CTDValue class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDVALUE_H)
    #include "TDValue.h"
#endif

//***********
// CTDValue *
//***********

CTDValue::CTDValue()
    : m_pCurrConcept(NULL)
{

}

CTDValue::~CTDValue() 
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDValue::initConceptToRoot(CTDAttrib* pAttrib)
{
    ASSERT(pAttrib);
    
	m_pCurrConcept = pAttrib->getConceptRoot(); 
	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDValue::initConceptToLevel1(CTDAttrib* pAttrib) 
{ 
    initConceptToRoot(pAttrib); 
    if (!lowerCurrentConcept()) {
        ASSERT(false);
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDValue::setSplitCurConcept(CTDConcept* pSplitConcept, int childInd)
{
    ASSERT(pSplitConcept);
	m_pCurrConcept = pSplitConcept->getChildConcept(childInd); 

	return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDValue::setCurConcept(CTDConcept* pCurConcept)
{
    ASSERT(pCurConcept);
	m_pCurrConcept = pCurConcept; 
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDValue::assignGenClassValue(CTDAttrib* pAttrib,  int classInd) 
{ 
	m_pCurrConcept = pAttrib->getConceptRoot()->getChildConcept(classInd); 
    if (!assignRawConcept(pAttrib, classInd)) {
        ASSERT(false);
        return false;
    }
    return true;
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDValue::lowerCurrentConcept() 
{ 
    m_pCurrConcept = getLowerConcept(); 
    if (!m_pCurrConcept) {
        ASSERT(false);
        cerr << _T("CTDStringValue: Failed to lower concept value: ") << m_pCurrConcept->m_conceptValue << endl;
        return false;
    }
    return true;
}

//*****************
// CTDStringValue *
//*****************

//---------------------------------------------------------------------------
// Get the next lower concept.
//---------------------------------------------------------------------------
CTDConcept* CTDStringValue::getLowerConcept()
{   
    if (m_pCurrConcept->getAttrib()->isMaskTypeSup())
        return getLowerConceptSupMode();
    else
        return getLowerConceptGenMode(m_pCurrConcept);
}

//---------------------------------------------------------------------------
// For SUPPRESSION mode.
// Get the next lower concept.
//---------------------------------------------------------------------------
CTDConcept* CTDStringValue::getLowerConceptSupMode()
{   
    // Attempt to find a matched concept in child concepts.
    // If not found, find in grand child concepts but return the child concept.
    // It is still efficient since search at most two levels.
    CTDConcept* pChildConcept = NULL;
    CTDConcept* pGrandChildConcept = NULL;
    for (int c = 0; c < m_pCurrConcept->getNumChildConcepts(); ++c) {
        pChildConcept = m_pCurrConcept->getChildConcept(c);
        if (pChildConcept->m_flattenIdx == m_pRawConcept->m_flattenIdx)
            return pChildConcept;

        for (int g = 0; g < pChildConcept->getNumChildConcepts(); ++g) {
            pGrandChildConcept = pChildConcept->getChildConcept(g);
            if (pGrandChildConcept->m_flattenIdx == m_pRawConcept->m_flattenIdx)
                return pChildConcept;
        }
    }
    ASSERT(false);
    cerr << _T("CTDStringValue: Failed to get lower concept: ") << m_pCurrConcept->m_conceptValue << endl;
    return NULL;
}

//---------------------------------------------------------------------------
// For GENERALIZATION mode.
// Get the next lower concept of the given concept.
// Bit format: <depth n>...<depth 2><depth 1>
//---------------------------------------------------------------------------
CTDConcept* CTDStringValue::getLowerConceptGenMode(CTDConcept* pThisConcept)
{   
    CTDIntArray& reqBits = pThisConcept->getAttrib()->getReqBits();

    // Shift bits to the next child idx.
    UINT nextChildIdx = m_bitValue;
    int d = 0;
    for (d = 0; d < pThisConcept->m_depth; ++d)
        nextChildIdx >>= reqBits[d];
    
    // The next couple of bits is for this level.
    // Form the mask for the last n bits, e.g., 00000000 00000000 00000000 00000111
    UINT mask = 0xFFFFFFFF;
    mask >>= (sizeof(UINT) * CHAR_BIT - reqBits[d]);

    // Find the next child index.
    nextChildIdx &= mask;
    return pThisConcept->getChildConcept((int) nextChildIdx);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CTDConcept* CTDStringValue::getRawConcept()
{
    /*
    CTDConcept* pRawConcept = m_pCurrConcept;
    while (pRawConcept->getNumChildConcepts() > 0)
        pRawConcept = getLowerConcept(pRawConcept);
    return pRawConcept;
    */
    return m_pRawConcept;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDStringValue::assignRawConcept(CTDAttrib* pAttrib, int classInd)
{
    m_pRawConcept = pAttrib->getConceptRoot()->getChildConcept(classInd);
	
	return true;
}

//---------------------------------------------------------------------------
// Bit format: <depth n>...<depth 2><depth 1>
//---------------------------------------------------------------------------
bool CTDStringValue::buildBitValue(const CString& rawVal, CTDAttrib* pAttrib)
{
    CTDConcepts conceptPath;
    if (!buildConceptPath(rawVal, pAttrib->getFlattenConcepts(), conceptPath)) {
        ASSERT(false);
        return false;
    }
    
    // The first item is the raw concept.
    m_pRawConcept = conceptPath.GetAt(0);

    // Build from top-down and shift bits.
    // First form depth 1, then depth 2...
    m_bitValue = 0;
    CTDIntArray& reqBits = pAttrib->getReqBits();
    int chIdx = 0, rIdx = 0, nShiftBits = 0;
    for (int c = conceptPath.GetUpperBound() - 1; c >= 0; --c) {
        chIdx = conceptPath.GetAt(c)->m_childIdx;
        chIdx <<= nShiftBits;
        m_bitValue |= chIdx;
        nShiftBits += reqBits[rIdx];
        ++rIdx;
    }
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CString CTDStringValue::toString(bool bRawValue)
{
    if (bRawValue)
        return getRawConcept()->m_conceptValue;
    else
        return m_pCurrConcept->toString();
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// static
bool CTDStringValue::buildConceptPath(const CString& rawVal, CTDConcepts* pFlatten, CTDConcepts& conceptPath)
{
    conceptPath.RemoveAll();
    CTDConcept* pConcept = NULL;
    int nFlattenConcepts = pFlatten->GetSize();
    for (int i = nFlattenConcepts - 1; i >= 0; --i) {
        pConcept = pFlatten->GetAt(i);
        if (!pConcept) {
            ASSERT(false);
            return false;
        }

        if (rawVal.CompareNoCase(pConcept->m_conceptValue) == 0) {
            // Go up to its parent.
            while (pConcept) {
                conceptPath.Add(pConcept);
                pConcept = pConcept->getParentConcept();
            }
            return true;
        }
    }
    cerr << _T("CTDStringValue: Failed to match concept path: ") << rawVal << endl;
    return false;
}

//******************
// CTDNumericValue *
//******************

//---------------------------------------------------------------------------
// Get the next lower concept.
//---------------------------------------------------------------------------
CTDConcept* CTDNumericValue::getLowerConcept()
{   
    CTDContConcept* pConcept = NULL;
    for (int c = 0; c < m_pCurrConcept->getNumChildConcepts(); ++c) {
        pConcept = static_cast<CTDContConcept*> (m_pCurrConcept->getChildConcept(c));
        if (m_numValue >= pConcept->m_lowerBound && m_numValue < pConcept->m_upperBound)
            return pConcept;
    }
    ASSERT(false);
    return NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CString CTDNumericValue::toString(bool bRawValue)
{
    if (bRawValue) {
        LPTSTR tempStr = FloatToStr(m_numValue, TD_CONTVALUE_NUMDEC);
        CString str = tempStr;
        delete [] tempStr;
        tempStr = NULL;
        return str;
    }
    else
        return m_pCurrConcept->toString();
}


//************
// CTDValues *
//************
CTDValues::CTDValues()
{
}

CTDValues::~CTDValues()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CTDValues::cleanup()
{
    for (int i = 0; i < GetSize(); ++i)
        delete GetAt(i);

    RemoveAll();
}