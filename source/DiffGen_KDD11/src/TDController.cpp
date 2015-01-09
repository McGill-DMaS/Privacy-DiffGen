// TDController.cpp: implementation of the TDController class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDCONTROLLER_H)
    #include "TDController.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDController::CTDController(LPCTSTR rawDataFile, 
                             LPCTSTR attributesFile,
                             LPCTSTR nameFile,
                             LPCTSTR transformedDataFile, 
                             LPCTSTR transformedTestFile, 
                             int nSpecialization,
							 double pBudget,
                             int  nInputRecs,
                             int  nTraining)
    : m_attribMgr(attributesFile, nameFile), 
      m_dataMgr(rawDataFile, transformedDataFile, transformedTestFile, nInputRecs, nTraining),
	  m_partitioner(nSpecialization, pBudget)
{
    if (!m_dataMgr.initialize(&m_attribMgr))
        ASSERT(false);
    if (!m_partitioner.initialize(&m_attribMgr, &m_dataMgr))
        ASSERT(false);
    if (!m_evalMgr.initialize(&m_attribMgr, &m_partitioner))
        ASSERT(false);
}

CTDController::~CTDController()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDController::runTopDown()
{	
    cout << _T("***************************************") << endl;
    cout << _T("* Privacy Data Transformer - Top Down *") << endl;
    cout << _T("***************************************") << endl;

	printTime();
	// Read the configuration file for the taxonomy trees of the atrributes
	if (!m_attribMgr.readAttributes())
        return false;

	
    printTime();
	// Load the data from the file
    if (!m_dataMgr.readRecords())
        return false;
    

	printTime();
	// Anonymize the whole data set
    if (!m_partitioner.transformData())
        return false;
   
	// Add noise to the "training" partitions
	if (!m_partitioner.addNoise())
        return false;
    
	printTime();
	// Print the "test" partitions 
 	if (!m_dataMgr.writeDiffRecords(m_partitioner.getLeafPartitions()))
		return false;


    printTime();
	// Write the configuration file the C4.5 classifier
    if (!m_attribMgr.writeNameFile())
        return false;


	printTime();

    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDController::removeUnknowns()
{	
    cout << _T("****************************************") << endl;
    cout << _T("* Removing Records With Unknown Values *") << endl;
    cout << _T("****************************************") << endl;
    
    if (!m_attribMgr.readAttributes())
        return false;

    if (!m_dataMgr.readRecords())
        return false;

    if (!m_dataMgr.writeRecords(true))
        return false;

    return true;
}