// TDDataMgr.cpp: implementation of the CTDDataMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDDATAMGR_H)
    #include "TDDataMgr.h"
#endif

#if !defined(TDPARTITION_H)
    #include "TDPartition.h"
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDDataMgr::CTDDataMgr(LPCTSTR rawDataFile, LPCTSTR transformedDataFile, LPCTSTR transformedTestFile, int nInputRecs, int nTraining) 
    : m_rawDataFile(rawDataFile), 
      m_transformedDataFile(transformedDataFile), 
      m_transformedTestFile(transformedTestFile), 
      m_nInputRecs(nInputRecs),
      m_nTraining(nTraining)
{
}

CTDDataMgr::~CTDDataMgr() 
{
    m_records.cleanup();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDDataMgr::initialize(CTDAttribMgr* pAttribMgr)
{
    if (!pAttribMgr) {
        ASSERT(false);
        return false;
    }
    m_pAttribMgr = pAttribMgr;
    return true;
}

//---------------------------------------------------------------------------
// Read records from raw data file.
//---------------------------------------------------------------------------
bool CTDDataMgr::readRecords()
{
    cout << _T("Reading records...") << endl;
    m_records.cleanup();
    CTDAttribs* pAttribs = m_pAttribMgr->getAttributes();
    try {
        CStdioFile rawFile;
        if (!rawFile.Open(m_rawDataFile, CFile::modeRead)) {
            cerr << _T("CTDDataMgr: Failed to open file ") << m_rawDataFile << endl;
            return false;
        }

        // Parse each line.
        int commentCharPos = -1;
        CString lineStr;
        while (rawFile.ReadString(lineStr)) {
            CBFStrHelper::trim(lineStr);
            if (lineStr.IsEmpty())
                continue;

            // Remove comments.
            commentCharPos = lineStr.Find(TD_CONHCHY_COMMENT);
            if (commentCharPos != -1) {
                lineStr = lineStr.Left(commentCharPos);
                CBFStrHelper::trim(lineStr);
                if (lineStr.IsEmpty())
                    continue;
            }

            // Remove period at the end of the line.
            if (lineStr[lineStr.GetLength() - 1] == TD_RAWDATA_TERMINATOR) {
                lineStr = lineStr.Left(lineStr.GetLength() - 1);
                CBFStrHelper::trim(lineStr);
                if (lineStr.IsEmpty())
                    continue;
            }
   
            int attribID = 0;
            CString valueStr;
            CTDAttrib* pAttrib = NULL;
            CTDValue* pNewValue = NULL;
            CTDRecord* pNewRecord = new CTDRecord();                        
            CBFStrParser strParser(lineStr, TD_RAWDATA_DELIMETER);
            while (strParser.getNext(valueStr)) {
                // Check unknown value
				CBFStrHelper::trim(valueStr);
				if (valueStr.IsEmpty()) {
                    cerr << _T("CTDDataMgr: Empty value string in record: ") << lineStr << endl;
                    ASSERT(false);
                    return false;
                }
                if (valueStr == TD_UNKNOWN_VALUE) {
                    // Discard this record.
                    delete pNewRecord;
                    pNewRecord = NULL;
                    break;
                }

                // Allocate a new value.
                pNewValue = NULL;
                pAttrib = pAttribs->GetAt(attribID);

                if (pAttrib->isContinuous())
                    pNewValue = new CTDNumericValue((float) StrToFloat(valueStr));
               	else
                    pNewValue = new CTDStringValue();

                if (!pNewValue) {
                    ASSERT(false);
                    return false;
                }

				
                // Match the value to the lowest concept.
                // Then build the bit value in case of categorical attribute.
                if (!pNewValue->buildBitValue(valueStr, pAttrib)) {
                    cerr << _T("CTDDataMgr: Failed to build bit value: ") << valueStr
                         << _T(" in attribute ") << pAttrib->m_attribName << endl;                    
                    ASSERT(false);
                    return false;
                }

                if (attribID == pAttribs->GetSize() - 1) {
                    // Class attribute
                    if (!pNewValue->initConceptToLevel1(pAttrib))
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

                ++attribID;
            }

            if (pNewRecord) {
				
				if (m_records.GetSize() >= m_nTraining)
					pNewRecord->setRecordID(m_testRecords.Add(pNewRecord));
				else
					pNewRecord->setRecordID(m_records.Add(pNewRecord));
            }

            // Read in the specified number of records. Then stop.
			if (m_nInputRecs >= 0 && (m_records.GetSize()+ m_testRecords.GetSize()) >= m_nInputRecs)
                break;
        }
        rawFile.Close();

        if (m_records.GetSize() == 0) {
            cerr << _T("CTDDataMgr: No records.") << endl;
            return false;
        }

		 if (m_testRecords.GetSize() == 0) {
            cerr << _T("CTDDataMgr: No test records.") << endl;
            return false;
        }
    }
    catch (CFileException&) {
        cerr << _T("Failed to read raw data file: ") << m_rawDataFile << endl;
        ASSERT(false);
        return false;
    }

#ifdef _DEBUG_PRT_INFO
    cout << _T("Number of Records = ")<< (m_records.GetSize()+ m_testRecords.GetSize())<< endl;
/*  
	for (int c = 0; c < m_records.GetSize(); ++c) {
		for (int i = 0; i < m_records.GetAt(1)->getNumValues(); ++i)
			cout << m_records.GetAt(c)->getValue(i)->toString(true)<< _T(",");
		cout << endl;
    }
*/
    cout << endl;
#endif

    cout << _T("Reading records succeeded.") << endl;
    return true;
}


//---------------------------------------------------------------------------
// Write records to transformed data file. This method is used to write the 
// raw data without missing values.
//---------------------------------------------------------------------------
bool CTDDataMgr::writeRecords(bool bRawValue)
{
    cout << _T("Writing records...") << endl;
    try {
        // Write data file.
        CStdioFile transDataFile;
        if (!transDataFile.Open(m_transformedDataFile, CFile::modeCreate | CFile::modeWrite)) {
            cerr << _T("CTDDataMgr: Failed to open file ") << m_transformedDataFile << endl;
            return false;
        }

        int i = 0;
        int nRecords = m_records.GetSize();
        if (m_nTraining > nRecords) {
            cerr << _T("CTDDataMgr: Number of training records must be <= number of records in rawdata.") << m_transformedDataFile << endl;
            ASSERT(false);
            return false;
        }
        for (i = 0; i < m_nTraining; ++i) {
            if (bRawValue)
                transDataFile.WriteString(m_records.GetAt(i)->toString(bRawValue) + _T("\n"));
            else
                transDataFile.WriteString(m_records.GetAt(i)->toString(bRawValue) + _T("\n"));
        }
        transDataFile.Close();

        // Write test file.
        CStdioFile transTestFile;
        if (!transTestFile.Open(m_transformedTestFile, CFile::modeCreate | CFile::modeWrite)) {
            cerr << _T("CTDDataMgr: Failed to open file ") << m_transformedTestFile << endl;
            return false;
        }

        for (i = m_nTraining; i < nRecords; ++i) {
            if (bRawValue)
                transTestFile.WriteString(m_records.GetAt(i)->toString(bRawValue) + _T("\n"));
            else
                transTestFile.WriteString(m_records.GetAt(i)->toString(bRawValue) + _T("\n"));
        }
        transTestFile.Close();
    }
    catch (CFileException&) {
        cerr << _T("Failed to write transformed data file: ") << m_transformedDataFile << endl;
        ASSERT(false);
        return false;
    }
    cout << m_records;
    cout << _T("Writing records succeeded.") << endl;
    return true;
}


//---------------------------------------------------------------------------
// Write records to transformed data file in the differential privacy format
//---------------------------------------------------------------------------
bool CTDDataMgr::writeDiffRecords(CTDPartitions* pLeafPartitions)
{
    cout << _T("Writing records...") << endl;
    try {
        
		// Write data file.
        CStdioFile transDataFile;
        if (!transDataFile.Open(m_transformedDataFile, CFile::modeCreate | CFile::modeWrite)) {
            cerr << _T("CTDDataMgr: Failed to open file ") << m_transformedDataFile << endl;
            return false;
        }
		CTDPartition* pChildPartition = NULL;
		for (POSITION childPos = pLeafPartitions->GetHeadPosition(); childPos != NULL;) {
			pChildPartition = pLeafPartitions->GetNext(childPos);

			if ( pChildPartition->getNumGenRecords() != pChildPartition->getNumClasses()) {
				cerr << _T("Number of generalized records is not same as the number of classes") << endl;
				ASSERT(false);
				return false;
			}
			CString str;
			for (int j = 0; j < pChildPartition->getNumClasses(); ++j){
				str = pChildPartition->getGenRecords()->GetAt(j)->toString(false) ;
				// print the generalized records according to the noisyCount..
				for (int i = 0; i < pChildPartition->m_classNoisySums[j]; ++i) 
					transDataFile.WriteString(str + _T("\n"));
			}
		}
		transDataFile.Close();

        // Write test file.
        CStdioFile transTestFile;
        if (!transTestFile.Open(m_transformedTestFile, CFile::modeCreate | CFile::modeWrite)) {
            cerr << _T("CTDDataMgr: Failed to open file ") << m_transformedTestFile << endl;
            return false;
        }
		int nRecords = m_testRecords.GetSize();
        for (int i = 0; i < nRecords; ++i) {
			transTestFile.WriteString(m_testRecords.GetAt(i)->toString(false) + _T("\n"));
        }
        transTestFile.Close();
    }
    catch (CFileException&) {
        cerr << _T("Failed to write transformed data file: ") << m_transformedDataFile << endl;
        ASSERT(false);
        return false;
    }
    cout << m_records;
    cout << _T("Writing records succeeded.") << endl;
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDDataMgr::addRecord(CTDRecord* pRecord)
{
    try {
        m_records.Add(pRecord);
        return true;
    }
    catch (CMemoryException&) {
        ASSERT(false);
        return false;
    }
}