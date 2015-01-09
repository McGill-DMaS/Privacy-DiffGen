// TDCut.h: interface for the CTDCut class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDCUT_H)
#define TDCUT_H

#if !defined(TDCONCEPT_H)
    #include "TDConcept.h"
#endif

typedef CTypedPtrList<CPtrList, CTDConcept*> CTDConceptPtrList;
class CTDCut : public CTDConceptPtrList
{
public:
    CTDCut();
    virtual ~CTDCut();

    bool specializeConcept(CTDConcept* pParent);
};

#endif
