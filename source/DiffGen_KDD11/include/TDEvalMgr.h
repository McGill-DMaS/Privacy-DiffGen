#if !defined(TDEVALMGR_H)
#define TDEVALMGR_H

#if !defined(TDATTRIBMGR_H)
    #include "TDAttribMgr.h"
#endif

#if !defined(TDPARTITIONER_H)
    #include "TDPartitioner.h"
#endif

class CTDEvalMgr
{
public:
    CTDEvalMgr();
    virtual ~CTDEvalMgr();

// operations
    bool initialize(CTDAttribMgr* pAttribMgr, CTDPartitioner* pPartitioner);
    bool countNumDistortions(int& catDistortion, float& contDistortion);
    bool countNumDiscern(int& catDiscern);
    bool calPrecision(float& precision);

protected:
// attributes
    CTDAttribMgr* m_pAttribMgr;
    CTDPartitioner* m_pPartitioner;
};

#endif