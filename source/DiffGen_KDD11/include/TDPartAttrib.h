// TDPartAttrib.h: interface for the CTDPartAttrib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDPARTATTRIB_H)
#define TDPARTATTRIB_H

#if !defined(TDATTRIBUTE_H)
    #include "TDAttribute.h"
#endif

class CTDPartAttrib  
{
public:
    CTDPartAttrib(CTDAttrib* pActualAttrib);
    virtual ~CTDPartAttrib();

// operations
    bool initSupportMatrix(CTDConcept* pCurrCon, int nClasses);
    
    CTDMDIntArray* getSupportMatrix() { return m_pSupportMatrix; };
    CTDIntArray* getSupportSums() { return &m_supportSums; };
    CTDIntArray* getClassSums() { return &m_classSums; };

    CTDAttrib* getActualAttrib() { return m_pActualAttrib; };

// attributes
    bool           m_bCandidate;
    POSITION       m_relatedPos;        // Position of the partition in the related list of concept.

protected:
    CTDAttrib*     m_pActualAttrib;    
    CTDMDIntArray* m_pSupportMatrix;    // raw count of supports
    CTDIntArray    m_supportSums;       // sum of supports of each concept
    CTDIntArray    m_classSums;         // sum of classes
};


typedef CTypedPtrList<CPtrList, CTDPartAttrib*> CTDPartAttribList;
class CTDPartAttribs : public CTDPartAttribList
{
public:
    CTDPartAttribs();
    virtual ~CTDPartAttribs();
};

#endif
