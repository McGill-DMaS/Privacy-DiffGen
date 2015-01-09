// TDPartition.h: interface for the CTDPartition class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDPARTITION_H)
#define TDPARTITION_H

#if !defined(TDRECORD_H)
    #include "TDRecord.h"
#endif

#if !defined(TDPARTATTRIB_H)
    #include "TDPartAttrib.h"
#endif


class CTDPartition  
{
public:
    CTDPartition(int partitionIdx, CTDAttribs* pAttribs);
    virtual ~CTDPartition();

// operations
    bool initGenRecords(CTDAttribs* pAttribs);
	bool genRecords(CTDPartition*  pParentPartition,
                    CTDAttrib*     pSplitAttrib, 
                    CTDConcept*    pSplitConcept,
					CTDAttribs*    pAttribs,
					int			 childInd);

	int getPartitionIdx() { return m_partitionIdx; };
    CTDPartAttribs* getPartAttribs() { return &m_partAttribs; };
	
    bool addRecord(CTDRecord* pRecord);
    int getNumRecords() { return m_partRecords.GetSize(); };
	int getNumGenRecords() {return m_genRecords.GetSize(); };	
	int getNumClasses() { return m_nClasses; };
    CTDRecord* getRecord(int idx) { return m_partRecords.GetAt(idx); };
    CTDRecords* getAllRecords() { return &m_partRecords; };
	CTDRecords* getGenRecords() { return & m_genRecords; };

    
    bool deregisterPartition();
	bool testDeregisterPartition();
    bool registerPartition();
	bool testRegisterPartition();
    bool constructSupportMatrix(double epsilon);
	bool addNoise(double epsilon);
    friend ostream& operator<<(ostream& os, const CTDPartition& partition);

	CTDIntArray m_classNoisySums;   // sum of classes	
	POSITION m_leafPos;             // Position in the leaf list.
	CTDRecords m_genRecords;	    // Generalized record.

protected:
// attributes
    int m_partitionIdx;
   	CTDPartAttribs m_partAttribs;   // Pointers to attributes of this partition.
    CTDRecords m_partRecords;       // Pointers to records of this partition.    
    int m_nClasses;                 // Number of classes.
	
};


typedef CTypedPtrList<CPtrList, CTDPartition*> CTDPartitionList;
class CTDPartitions : public CTDPartitionList
{
public:
    CTDPartitions();
    virtual ~CTDPartitions();
    void cleanup();

    void deleteEmptyPartitions();
    bool joinPartitions(CTDRecords& jointRecs);

    friend ostream& operator<<(ostream& os, const CTDPartitions& partitions);
};

#endif
