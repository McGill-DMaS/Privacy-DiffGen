// TDPartitioner.h: interface for the CTDPartitioner class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDPARTITIONER_H)
#define TDPARTITIONER_H

#if !defined(TDATTRIBMGR_H)
    #include "TDAttribMgr.h"
#endif

#if !defined(TDDATAMGR_H)
    #include "TDDataMgr.h"
#endif

#if !defined(TDPARTITION_H)
    #include "TDPartition.h"
#endif

class CTDPartitioner  
{
public:
    CTDPartitioner(int nSpecialization,	double privacyB);
    virtual ~CTDPartitioner();

// operations
    bool initialize(CTDAttribMgr* pAttribMgr,
                    CTDDataMgr* pDataMgr);
    bool transformData();
	bool addNoise();
    CTDPartitions* getLeafPartitions() { return &m_leafPartitions; };


protected:
    CTDPartition* initRootPartition();
	CTDPartition* initTestRootPartition();
	bool initializeBudget();
    bool splitPartitions(CTDAttrib* pSplitAttrib, CTDConcept* pSplitConcept);
	bool splitTestPartitions(CTDAttrib* pSplitAttrib, CTDConcept* pSplitConcept);
    bool distributeRecords(CTDPartition*  pParentPartition, 
                           CTDAttrib*     pSplitAttrib, 
                           CTDConcept*    pSplitConcept, 
                           CTDPartitions& childPartitions);
	bool testDistributeRecords(CTDPartition*  pParentPartition, 
                           CTDAttrib*     pSplitAttrib, 
                           CTDConcept*    pSplitConcept, 
                           CTDPartitions& childPartitions);
 

// attributes
    CTDAttribMgr*  m_pAttribMgr;
    CTDDataMgr*    m_pDataMgr;
    CTDPartitions  m_leafPartitions;
	CTDPartitions  m_testLeafPartitions;
	int m_nSpecialization;
	double m_pBudget;
	double m_workingBudget; 
};

#endif
