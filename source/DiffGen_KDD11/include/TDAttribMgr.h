// TDAttribMgr.h: interface for the CTDAttribMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDATTRIBMGR_H)
#define TDATTRIBMGR_H

#if !defined(TDATTRIBUTE_H)
    #include "TDAttribute.h"
#endif

class CTDAttribMgr  
{
public:
    CTDAttribMgr(LPCTSTR attributesFile, LPCTSTR nameFile);
    virtual ~CTDAttribMgr();

// operations
    bool readAttributes();
    bool writeNameFile();

    CTDAttribs* getAttributes() { return &m_attributes; };
    CTDAttrib* getAttribute(int idx) { return m_attributes.GetAt(idx); };
    int getNumAttributes() const { return m_attributes.GetSize(); };

    CTDAttrib* getClassAttrib() { return m_attributes.GetAt(m_attributes.GetSize() - 1); };
    int getNumClasses() { return getClassAttrib()->getConceptRoot()->getNumChildConcepts(); };
	int getNumConAttribs() {return m_numConAttrib; };

    bool pickSpecializeConcept(CTDAttrib*& pSelectedAttrib, CTDConcept*& pSelectedConcept, double epsilon);
    bool computeScore();
    
protected:
// attributes
    CString    m_attributesFile;
    CString    m_nameFile;
    CTDAttribs m_attributes;
	int		   m_numConAttrib;
};

#endif
