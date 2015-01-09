// TDConcept.h: interface for the CTDConcept class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDCONCEPT_H)
#define TDCONCEPT_H

class CTDConcept;

typedef CTypedPtrArray<CPtrArray, CTDConcept*> CTDConceptPtrArray;
class CTDConcepts : public CTDConceptPtrArray
{
public:
    CTDConcepts();
    virtual ~CTDConcepts();
	CString toString();

    void cleanup();
};

class CTDPartition;
class CTDPartitions;
class CTDAttrib;
class CTDRecords;

class CTDConcept
{
public:
    CTDConcept(CTDAttrib* pAttrib);
    virtual ~CTDConcept();

    virtual bool isContinuous() = 0;
    virtual bool initHierarchy(LPCTSTR conceptStr, int depth, CTDIntArray& maxChildren) = 0;
    virtual bool divideConcept(double epsilon, int nClasses) = 0;
    virtual CString toString() = 0;
    
    bool addChildConcept(CTDConcept* pConceptNode);
    int getNumChildConcepts() const;
	CTDConcepts * getChildConcepts() {return &m_childConcepts;}
    CTDConcept* getChildConcept(int idx) const;
    CTDConcept* getParentConcept();
    CTDAttrib* getAttrib() { return m_pAttrib; };

    CTDPartitions* getRelatedPartitions();
	CTDPartitions* getTestRelatedPartitions();
	POSITION registerPartition(CTDPartition* pPartition);
	POSITION testRegisterPartition(CTDPartition* pPartition);
    void deregisterPartition(POSITION pos);
	void testDeregisterPartition(POSITION pos);

	bool computeScore(int nClasses);
	bool computeMaxHelper(const CTDIntArray& supSums, 
                                 const CTDIntArray& classSums,
								 CTDMDIntArray& supMatrix,
                                 float& infoGainDiff);
	bool computeInfoGainHelper(float entropy, 
                                      const CTDIntArray& supSums, 
                                      const CTDIntArray& classSums, 
                                      CTDMDIntArray& supMatrix,
                                      float& infoGainDiff);
	
    static float computeEntropy(CTDIntArray* pClassSums);
    static bool parseFirstConcept(CString& firstConcept, CString& restStr);
        
// attributes
    CString        m_conceptValue;          // Actual value in string format.
    int            m_depth;                 // Depth of this concept.
    int            m_childIdx;              // Child index in concept hierarchy.
    int            m_flattenIdx;            // Flattened index in concept hierarchy.
    float          m_infoGain;              // Information gain
	float		   m_max;					// Max
    bool           m_bCutCandidate;         // Can it be a cut candidate?
    POSITION       m_cutPos;                // Position of this concept in the cut.    
	
protected:
// operations
    bool initSplitMatrix(int nConcepts, int nClasses);
    virtual bool findOptimalSplitPoint(CTDRecords& recs, int nClasses, double epsilon) = 0;
    virtual bool makeChildConcepts() = 0;

// attributes
    CTDAttrib*			m_pAttrib;					// Pointer to this attribute.
    CTDConcept*			m_pParentConcept;
    CTDConcepts			m_childConcepts;
    CTDPartitions*		m_pRelatedPartitions;		// related leaf partitions.
	CTDPartitions*		m_pTestRelatedPartitions;   // related leaf partitions.
	

    CTDMDIntArray* m_pSplitSupMatrix;   // raw count of supports
    CTDIntArray    m_splitSupSums;      // sum of supports of each concept
    CTDIntArray    m_splitClassSums;    // sum of classes
};


class CTDDiscConcept : public CTDConcept
{
public:
    CTDDiscConcept(CTDAttrib* pAttrib);
    virtual ~CTDDiscConcept();

    virtual bool isContinuous() { return false; };
    virtual bool initHierarchy(LPCTSTR conceptStr, int depth, CTDIntArray& maxBranches);
	virtual bool divideConcept(double epsilon, int nClasses){ return true; } ;
    virtual CString toString();

    static bool parseConceptValue(LPCTSTR str, CString& conceptVal, CString& restStr);

protected:
	virtual bool findOptimalSplitPoint(CTDRecords& recs, int nClasses, double epsilon) {return true;};
	virtual bool makeChildConcepts() {return true;};

// attributes
    CTDConcept* m_pSplitConcept;            // pointer to the winner concept.
};


class CTDContConcept : public CTDConcept
{
public:
    CTDContConcept(CTDAttrib* pAttrib);
    virtual ~CTDContConcept();
    virtual bool isContinuous() { return true; };
    virtual bool initHierarchy(LPCTSTR conceptStr, int depth, CTDIntArray& maxBranches);
    virtual bool divideConcept(double epsilon, int nClasses);
    virtual CString toString();
    
    float getSplitPoint() { return m_splitPoint; };

    static bool makeRange(float lowerB, float upperB, CString& range);
    static bool parseConceptValue(LPCTSTR  str, 
                                  CString& conceptVal, 
                                  CString& restStr,
                                  float&   lowerBound,
                                  float&   upperBound);
    static bool parseLowerUpperBound(const CString& str, 
                                     float& lowerB, 
                                     float& upperB);

// attributes
    float m_lowerBound;     // inclusive
    float m_upperBound;     // exclusive

protected:
// operations
    virtual bool findOptimalSplitPoint(CTDRecords& recs, int nClasses, double epsilon);
    virtual bool makeChildConcepts();
    bool computeSplitEntropy(float& entropy);

// attributes
    float m_splitPoint;     // Split point of this concept.
};



//------------------------------------------------------------------------------------

class CTDRange;

typedef CTypedPtrArray<CPtrArray, CTDRange*> CTDRangePtrArray;
class CTDRanges : public CTDRangePtrArray
{
public:
    CTDRanges();
    virtual ~CTDRanges();

    void cleanup();
};


class CTDRange
{
public:
    CTDRange(float upperValue, float lowerValue);
    virtual ~CTDRange();

	// attributes
    float          m_upperValue;      
    float          m_lowerValue;      
};

#endif
