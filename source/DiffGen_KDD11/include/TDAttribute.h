// TDAttribute.h: interface for the CTDAttribute class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDATTRIBUTE_H)
#define TDATTRIBUTE_H

#if !defined(TDCONCEPT_H)
    #include "TDConcept.h"
#endif

#if !defined(TDCUT_H)
    #include "TDCut.h"
#endif

class CTDAttrib  
{
public:
    CTDAttrib(LPCTSTR attribName);
    virtual ~CTDAttrib();

// operations
    bool initCutToRoot();
    virtual bool isContinuous() = 0;
	virtual bool isMaskTypeSup() { return m_bMaskTypeSup; };
    virtual bool initHierarchy(LPCTSTR conStr) = 0;

    CTDConcept* getConceptRoot() { return m_pConceptRoot; };	
    bool flattenHierarchy();
    CTDConcepts* getFlattenConcepts() { return &m_flattenConcepts; };
   	CTDIntArray& getReqBits() { return m_reqBits; };
    bool calBits();

// attributes
    CString     m_attribName;       // Attribute name.
  	int         m_attribIdx;        // Attribute Index
    bool        m_bVirtualAttrib;   // Is it a virtual attribute?
    int         m_nOFlatConcepts;   // Number of concepts of original flatten concepts.
    CTDCut      m_cut;

protected:    
    bool        m_bMaskTypeSup;     // Mask type is suppression.
    CTDConcept* m_pConceptRoot;     // Root of concept hierarchy.
    CTDConcepts m_flattenConcepts;  
    CTDIntArray m_reqBits;          // Maximum required bits for each level
	
};


class CTDDiscAttrib : public CTDAttrib
{
public:
    CTDDiscAttrib(LPCTSTR attribName, bool bMaskTypeSup);
    virtual ~CTDDiscAttrib();
    virtual bool isContinuous() { return false; };
    virtual bool initHierarchy(LPCTSTR conceptStr); 
    
protected:
    bool reconstructHierarchy();
    bool reconstructHierarchyHelper(const CTDConcept* pThisConcept, CTDConcept* pNewRootConcept);
};


class CTDContAttrib : public CTDAttrib
{
public:
    CTDContAttrib(LPCTSTR attribName);
    virtual ~CTDContAttrib();
    virtual bool isContinuous() { return true; };
    virtual bool initHierarchy(LPCTSTR conceptStr);
};



typedef CTypedPtrArray<CPtrArray, CTDAttrib*> CTDAttribArray;
class CTDAttribs : public CTDAttribArray
{
public:
    CTDAttribs();
    virtual ~CTDAttribs();
    void cleanup();
    friend ostream& operator<<(ostream& os, const CTDAttribs& attribs);
};

#endif
