// TDPartition.cpp: implementation of the CTDPartition class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDPARTITION_H)
    #include "TDPartition.h"
#endif

//***************
// CTDPartition *
//***************

CTDPartition::CTDPartition(int partitionIdx, CTDAttribs* pAttribs)
    : m_partitionIdx(partitionIdx), m_leafPos(NULL)
{
    // Add each attribute
    int nAttribs = pAttribs->GetSize();
    for (int i = 0; i < nAttribs - 1; ++i)
        m_partAttribs.AddTail(new CTDPartAttrib(pAttribs->GetAt(i)));

    // Number of classes
    m_nClasses = pAttribs->GetAt(nAttribs - 1)->getConceptRoot()->getNumChildConcepts();
}

CTDPartition::~CTDPartition() 
{
    while (!m_partAttribs.IsEmpty())
        delete m_partAttribs.RemoveHead();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDPartition::initGenRecords( CTDAttribs* pAttribs)
{
	m_genRecords.cleanup();
	int nAttribs = pAttribs->GetSize();

	for (int classInd = 0; classInd < m_nClasses; ++classInd){

		CTDAttrib* pAttrib = NULL;
		CTDValue* pNewValue = NULL;
		CTDRecord* pNewRecord = new CTDRecord(); 
		
		for (int attribID = 0; attribID < nAttribs; ++attribID){
	
			pNewValue = NULL;
			pAttrib = pAttribs->GetAt(attribID);
        
			if (pAttrib->isContinuous())
				pNewValue = new CTDNumericValue(-1.0);
			else
	            pNewValue = new CTDStringValue();

			if (!pNewValue) {
				ASSERT(false);
  				return false;
			}
	
			if (attribID == pAttribs->GetSize() - 1) {
				// Class attribute
				if (!pNewValue->assignGenClassValue(pAttrib, classInd))
					return false;
			}
			else {
				// Ordinary attribute
				// Initialize the current concept to the root concept.                    
				if (!pNewValue->initConceptToRoot(pAttrib))
					return false;
			}

			// Add the value to the record.
			if (!pNewRecord->addValue(pNewValue))
				return false;

		}    

		if (pNewRecord){
			pNewRecord->setRecordID(m_genRecords.Add(pNewRecord));
		}

	}
	if (m_genRecords.GetSize() != m_nClasses) {
            cerr << _T("CTDPartition::initGenRecords: Number of generalized record is not current.") << endl;
            return false;
    }

	return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDPartition::genRecords(CTDPartition*  pParentPartition,
                              CTDAttrib*     pSplitAttrib, 
                              CTDConcept*    pSplitConcept,
							  CTDAttribs*    pAttribs,
							  int			 childInd)
{
	m_genRecords.cleanup();
	int nAttribs = pAttribs->GetSize();
	int splitIdx = pSplitAttrib->m_attribIdx;

	for (int classInd = 0; classInd < m_nClasses; ++classInd){

		CTDAttrib* pAttrib = NULL;
		CTDValue* pNewValue = NULL;
		CTDRecord* pNewRecord = new CTDRecord(); 
		
		for (int attribID = 0; attribID < nAttribs; ++attribID){
	
			pNewValue = NULL;
			pAttrib = pAttribs->GetAt(attribID);

			if (pAttrib->isContinuous())
				pNewValue = new CTDNumericValue(-1.0);
			else
	            pNewValue = new CTDStringValue();

			if (!pNewValue) {
				ASSERT(false);
  				return false;
			}

	
			if (attribID == pAttribs->GetSize() - 1) {
				// Class attribute
				if (!pNewValue->assignGenClassValue(pAttrib, classInd))
					return false;
			}
			else if(attribID == splitIdx) {
				//split attribute
				if (!pNewValue->setSplitCurConcept(pSplitConcept, childInd))
					return false;	
			}

			else 
			{	// Other attributes
				// Initialize the current concept to the root concept. 
				if (!pNewValue->setCurConcept(pParentPartition->getGenRecords()->GetAt(0)->getValue(attribID)->getCurrentConcept()))
						return false;	
			}

			// Add the value to the record.
			if (!pNewRecord->addValue(pNewValue))
				return false;

		}    

		if (pNewRecord){
			pNewRecord->setRecordID(m_genRecords.Add(pNewRecord));
		}

	}
	if (m_genRecords.GetSize() != m_nClasses) {
		cerr << _T("CTDPartition::initGenRecords: Number of generalized record is not current.") << endl;
        return false;
    }

	 return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDPartition::addRecord(CTDRecord* pRecord)
{
    try {
        m_partRecords.Add(pRecord);
        return true;
    }
    catch (CMemoryException&) {
        ASSERT(false);
        return false;
    }
}

//---------------------------------------------------------------------------
// Deregister this parition from the concept.
//---------------------------------------------------------------------------
bool CTDPartition::deregisterPartition()
{
	
	CTDRecord* pFirstRec = NULL;

	if ( getNumRecords()== 0){
	
		// Get the first record of the generalized records.
		pFirstRec = getGenRecords()->GetAt(0);
	}
	else {
		// Get the first record of the partition.
		pFirstRec = getRecord(0);
		if (!pFirstRec) {
			ASSERT(false);
			return false;
		}
	}

    int a = 0;
    CTDPartAttrib* pPartAttrib = NULL;
    CTDConcept* pCurrentConcept = NULL;
    for (POSITION pos = m_partAttribs.GetHeadPosition(); pos != NULL; ++a) {
       	// Find the current concept of this attribute.
        pPartAttrib = m_partAttribs.GetNext(pos);
        pCurrentConcept = pFirstRec->getValue(a)->getCurrentConcept();
        if (!pCurrentConcept) {
            ASSERT(false);
            return false;
        }
        pCurrentConcept->deregisterPartition(pPartAttrib->m_relatedPos);
        pPartAttrib->m_relatedPos = NULL; 
	}
    return true;
}

//---------------------------------------------------------------------------
// Deregister this parition from the concept.
//---------------------------------------------------------------------------
bool CTDPartition::testDeregisterPartition()
{
    // Get the first record of the partition.
    CTDRecord* pFirstRec = getRecord(0);
    if (!pFirstRec) {
        ASSERT(false);
        return false;
    }

    int a = 0;
    CTDPartAttrib* pPartAttrib = NULL;
    CTDConcept* pCurrentConcept = NULL;
    for (POSITION pos = m_partAttribs.GetHeadPosition(); pos != NULL; ++a) {
        pPartAttrib = m_partAttribs.GetNext(pos);
		pCurrentConcept = pFirstRec->getValue(a)->getCurrentConcept();
			if (!pCurrentConcept) {
				ASSERT(false);
				return false;
			}
			pCurrentConcept->testDeregisterPartition(pPartAttrib->m_relatedPos);
			pPartAttrib->m_relatedPos = NULL;
    }
    return true;
}

//---------------------------------------------------------------------------
// Register this parition from the concept.
//---------------------------------------------------------------------------
bool CTDPartition::registerPartition()
{
    
	CTDRecord* pFirstRec = NULL;

	if ( getNumRecords()== 0){
	
		// Get the first record of the generalized records.
		pFirstRec = getGenRecords()->GetAt(0);
	}
	else {
		// Get the first record of the partition.
		pFirstRec = getRecord(0);
		if (!pFirstRec) {
			ASSERT(false);
			return false;
		}
	}
	
	int a = 0;
    CTDPartAttrib* pPartAttrib = NULL;
    CTDConcept* pCurrentConcept = NULL;
    for (POSITION pos = m_partAttribs.GetHeadPosition(); pos != NULL; ++a) {
        // Find the current concept of this attribute.
        pPartAttrib = m_partAttribs.GetNext(pos);
		pCurrentConcept = pFirstRec->getValue(a)->getCurrentConcept();
		if (!pCurrentConcept) {
			ASSERT(false);
			return false;
		}	
		pPartAttrib->m_relatedPos = pCurrentConcept->registerPartition(this);
	}
    return true;
}

//---------------------------------------------------------------------------
// Register this parition from the concept for test data.
//---------------------------------------------------------------------------
bool CTDPartition::testRegisterPartition()
{
    // Get the first record of the partition.
    CTDRecord* pFirstRec = getRecord(0);
    if (!pFirstRec) {
        ASSERT(false);
        return false;
    }

    int a = 0;
    CTDPartAttrib* pPartAttrib = NULL;
    CTDConcept* pCurrentConcept = NULL;

    for (POSITION pos = m_partAttribs.GetHeadPosition(); pos != NULL; ++a) {
        // Find the current concept of this attribute.
        pPartAttrib = m_partAttribs.GetNext(pos);
		pCurrentConcept = pFirstRec->getValue(a)->getCurrentConcept();
		if (!pCurrentConcept) {
			ASSERT(false);
			return false;
		}	
			pPartAttrib->m_relatedPos = pCurrentConcept->testRegisterPartition(this);
	}
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDPartition::constructSupportMatrix(double epsilon)
{
    CTDRecord* pFirstRec = NULL;

	if ( getNumRecords()== 0){
	
		// Get the first record of the generalized records.
		pFirstRec = getGenRecords()->GetAt(0);
	}
	else {
		// Get the first record of the partition.
		pFirstRec = getRecord(0);
		if (!pFirstRec) {
			ASSERT(false);
			return false;
		}
	}

    int a = 0;
    CTDPartAttrib* pPartAttrib = NULL;
    CTDConcept* pCurrentConcept = NULL;
    for (POSITION pos = m_partAttribs.GetHeadPosition(); pos != NULL; ++a) {
        
		pPartAttrib = m_partAttribs.GetNext(pos);

		if (!pPartAttrib->m_bCandidate)   // pPartAttrib->m_bCandidate  is true by default.
            continue;

		// Find the current concept of this attribute.
        pCurrentConcept = pFirstRec->getValue(a)->getCurrentConcept();

        // Need to find the split point. We also turn it true when we read the configuration file. 
        if (pPartAttrib->getActualAttrib()->m_bVirtualAttrib) {
            if (!pCurrentConcept->divideConcept(epsilon, m_nClasses)) {
                ASSERT(false);
                return false;
            }
        }

        // If this attribute does not have child concepts, this cannot be candidate.
        if (pCurrentConcept->getNumChildConcepts() == 0) {
            pCurrentConcept->m_bCutCandidate = false;  // "true" by default.
            pPartAttrib->m_bCandidate = false;
            continue;
        }

        // Construct the support matrix.
        if (!pPartAttrib->initSupportMatrix(pCurrentConcept, m_nClasses)) {
            ASSERT(false);
            return false;
        }
    }

	// initializing the noisy class sum count
	m_classNoisySums.SetSize(m_nClasses);

    for (int j = 0; j < m_nClasses; ++j)
		m_classNoisySums.SetAt(j, 0);


    // Compute the support matrix
    CTDConcept* pClassConcept = NULL;
    CTDConcept* pLowerConcept = NULL;    
    CTDMDIntArray* pSupMatrix = NULL;
    CTDIntArray* pSupSums = NULL;
    CTDIntArray* pClassSums = NULL;
    CTDRecord* pRec = NULL;
    int nRecs = getNumRecords();
    int classIdx = m_partAttribs.GetCount();
    for (int r = 0; r < nRecs; ++r) {       
        pRec = getRecord(r);
        // Get the class concept.
        pClassConcept = pRec->getValue(classIdx)->getCurrentConcept();        

		++m_classNoisySums[pClassConcept->m_childIdx];
		
        // Compute support counts for each attribute
        int aIdx = 0;
        for (POSITION pos = m_partAttribs.GetHeadPosition(); pos != NULL; ++aIdx) {
            // The partition attribute.
            pPartAttrib = m_partAttribs.GetNext(pos);
			
			if (!pPartAttrib->m_bCandidate)
                continue;

            // Get the lower concept value
            pLowerConcept = pRec->getValue(aIdx)->getLowerConcept();
            if (!pLowerConcept) {
                cerr << _T("No more child concepts. This should not be a candidate.") << endl;
                ASSERT(false);
                return false;
            }

            // Construct the support matrix.
            pSupMatrix = pPartAttrib->getSupportMatrix();
            if (!pSupMatrix) {
                ASSERT(false);
                return false;
            }
            ++((*pSupMatrix)[pLowerConcept->m_childIdx][pClassConcept->m_childIdx]);

            // Compute the support sum of this matrix.
            pSupSums = pPartAttrib->getSupportSums();
            if (!pSupSums) {
                ASSERT(false);
                return false;
            }
            ++((*pSupSums)[pLowerConcept->m_childIdx]);

            // Compute the class sum of this matrix.
            pClassSums = pPartAttrib->getClassSums();
            if (!pClassSums) {
                ASSERT(false);
                return false;
            }
            ++((*pClassSums)[pClassConcept->m_childIdx]);        
        }
    }

    return true;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDPartition::addNoise(double epsilon)
{
	if (epsilon <= 0) {
		cerr << _T("Not enough budget.") << endl;
        ASSERT(false);
        return false;
    }
	
	for (int j = 0; j < m_nClasses; ++j){
		// Add noise
		m_classNoisySums[j] = m_classNoisySums[j] + (int)laplaceNoise(epsilon);
		
		// Make zero the negative counts
		if (m_classNoisySums[j] < 0)
			m_classNoisySums[j] = 0;
	}
	
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ostream& operator<<(ostream& os, const CTDPartition& partition)
{
#ifdef _DEBUG_PRT_INFO
    os << _T("--------------------------------------------------------------------------") << endl;
    os << _T("Partition #") << partition.m_partitionIdx << endl;
    os << partition.m_partRecords;
#endif
    return os;
}

//****************
// CTDPartitions *
//****************
CTDPartitions::CTDPartitions()
{
}

CTDPartitions::~CTDPartitions()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CTDPartitions::cleanup()
{
    while (!IsEmpty())
        delete RemoveHead();
}

//---------------------------------------------------------------------------
// Delete empty partitions.
//---------------------------------------------------------------------------
void CTDPartitions::deleteEmptyPartitions()
{    
    POSITION tempPos = NULL;
    CTDPartition* pPartition = NULL;
    for (POSITION pos = GetHeadPosition(); pos != NULL;) {
        tempPos = pos;
        pPartition = GetNext(pos);
        if (pPartition->getNumRecords() <= 0) {
            RemoveAt(tempPos);
            delete pPartition;
            pPartition = NULL;
        }
    }
}

//---------------------------------------------------------------------------
// Join all the records in all paritions.
//---------------------------------------------------------------------------
bool CTDPartitions::joinPartitions(CTDRecords& jointRecs)
{
    jointRecs.RemoveAll();
    for (POSITION pos = GetHeadPosition(); pos != NULL;) {
        jointRecs.Append(*(GetNext(pos)->getAllRecords()));
    }
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ostream& operator<<(ostream& os, const CTDPartitions& partitions)
{
#ifdef _DEBUG_PRT_INFO
    CTDPartition* pPartition = NULL;
    for (POSITION pos = partitions.GetHeadPosition(); pos != NULL;) {
        pPartition = partitions.GetNext(pos);
        os << *pPartition << endl;
    }
#endif
    return os;
}

