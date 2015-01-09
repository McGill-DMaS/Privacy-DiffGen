// TDAttribMgr.cpp: implementation of the CTDAttribMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDATTRIBMGR_H)
    #include "TDAttribMgr.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDAttribMgr::CTDAttribMgr(LPCTSTR attributesFile, LPCTSTR nameFile) 
    : m_attributesFile(attributesFile), m_nameFile(nameFile), m_numConAttrib(0)
{
}

CTDAttribMgr::~CTDAttribMgr() 
{
    m_attributes.cleanup();
}

//---------------------------------------------------------------------------
// Read information from the attribute hierachy file, and build the concept
// hierarchy.
//---------------------------------------------------------------------------
bool CTDAttribMgr::readAttributes()
{
    cout << _T("Reading attributes...") << endl;
	m_attributes.cleanup();
    try {
        CStdioFile attribFile;
        if (!attribFile.Open(m_attributesFile, CFile::modeRead)) {
            cerr << _T("CTDAttribMgr: Failed to open file ") << m_attributesFile << endl;
            return false;
        }

        // Parse each line.        
        CString lineStr, attribName, attribType, attribValuesStr;
        int commentCharPos = -1, semiColonPos = -1;
        bool bMaskTypeSuppress = false;
        CTDAttrib* pClassAttribute = NULL;
        while (attribFile.ReadString(lineStr)) {
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

            // Find semicolon.
            semiColonPos = lineStr.Find(TCHAR(':'));
            if (semiColonPos == -1) {
                cerr << _T("CTDAttribMgr: Unknown line: ") << lineStr << endl;
                ASSERT(false);
                return false;
            }

            // Extract attribute name.
            attribName = lineStr.Left(semiColonPos);
            CBFStrHelper::trim(attribName);
            if (attribName.IsEmpty()) {
                cerr << _T("CTDAttribMgr: Invalid attribute: ") << lineStr << endl;
                ASSERT(false);
                return false;
            }
            
            // Find semicolon.
            lineStr = lineStr.Mid(semiColonPos + 1);
            CBFStrHelper::trim(lineStr);
            semiColonPos = lineStr.Find(TCHAR(':'));
            if (semiColonPos == -1) {
                attribType = lineStr;
                bMaskTypeSuppress = false;
            }
            else {
                // Extract attribute type.
                attribType = lineStr.Left(semiColonPos);
                CBFStrHelper::trim(attribType);
                if (attribType.IsEmpty()) {
                    cerr << _T("CTDAttribMgr: Invalid attribute type: ") << lineStr << endl;
                    ASSERT(false);
                    return false;
                }

                // Extract mask type.
                lineStr = lineStr.Mid(semiColonPos + 1);
                CBFStrHelper::trim(lineStr);
                bMaskTypeSuppress = lineStr.CompareNoCase(TD_MASKTYPE_SUP) == 0;
            }
			
			// Count the number of continuous attributes.
			if (attribType.CompareNoCase(TD_CONTINUOUS_ATTRIB) == 0)
				++m_numConAttrib;
            
			if (bMaskTypeSuppress && attribType.CompareNoCase(TD_CONTINUOUS_ATTRIB) == 0) {
                cerr << _T("CTDAttribMgr: Continuous attribute cannot be suppression.") << endl;
                ASSERT(false);
                return false;
            }

            if (bMaskTypeSuppress && attribName.CompareNoCase(TD_CLASSES_ATTRIB_NAME) == 0) {
                cerr << _T("CTDAttribMgr: Classes cannot be suppression.") << endl;
                ASSERT(false);
                return false;
            }

            if (attribName.CompareNoCase(TD_CLASSES_ATTRIB_NAME) == 0 && 
                attribType.CompareNoCase(TD_CONTINUOUS_ATTRIB) == 0) {
                cerr << _T("CTDAttribMgr: Classes cannot be continuous.") << endl;
                ASSERT(false);
                return false;
            }

            // Read the next line which contains the hierarchy.
            if (!attribFile.ReadString(attribValuesStr)) {
                cerr << _T("CTDAttribMgr: Invalid attribute: ") << attribName << endl;
                ASSERT(false);
                return false;
            }

            CBFStrHelper::trim(attribValuesStr);
            if (attribValuesStr.IsEmpty()) {
                cerr << _T("CTDAttribMgr: Invalid attribute: ") << attribName << endl;
                ASSERT(false);
                return false;
            }

            // Remove comments.
            commentCharPos = attribValuesStr.Find(TD_CONHCHY_COMMENT);
            if (commentCharPos != -1) {
                attribValuesStr = attribValuesStr.Left(commentCharPos);
                CBFStrHelper::trim(attribValuesStr);
                if (attribValuesStr.IsEmpty()) {
                    cerr << _T("CTDAttribMgr: Invalid attribute: ") << attribName << endl;
                    ASSERT(false);
                    return false;
                }
            }

			if (attribName.CompareNoCase(TD_VID_ATTRIB_NAME) != 0) {
                // Create an attribute.
				ASSERT(attribType == TD_CONTINUOUS_ATTRIB || attribType == TD_DISCRETE_ATTRIB);
                CTDAttrib* pNewAttribute = NULL;
                if (attribType.CompareNoCase(TD_CONTINUOUS_ATTRIB) == 0)
                    pNewAttribute = new CTDContAttrib(attribName);
                else if (attribType.CompareNoCase(TD_DISCRETE_ATTRIB) == 0)
                    pNewAttribute = new CTDDiscAttrib(attribName, bMaskTypeSuppress);
				

                if (!pNewAttribute) {
                    ASSERT(false);
                    return false;
                }
                if (!pNewAttribute->initHierarchy(attribValuesStr)) {
                    cerr << _T("CTDAttribMgr: Failed to build hierarchy for ") << attribName << endl;
                    return false;
                }

                if (attribName.CompareNoCase(TD_CLASSES_ATTRIB_NAME) != 0) {
                    // Add the attribute to the attribute array.
                    pNewAttribute->m_attribIdx = m_attributes.Add(pNewAttribute);
					pNewAttribute->m_bVirtualAttrib = true;    
                }
                else {
                    ASSERT(!pClassAttribute);
                    pClassAttribute = pNewAttribute;
                }
            }
        }

        // Add class attribute to the end of the attribute array.
        if (!pClassAttribute) {
            cerr << _T("CTDAttribMgr: Missing classes.") << endl;
            ASSERT(false);
            return false;
        }
        pClassAttribute->m_attribIdx = m_attributes.Add(pClassAttribute);
        attribFile.Close();
    }
    catch (CFileException&) {
        cerr << _T("Failed to read attributes file: ") << m_attributesFile << endl;
        ASSERT(false);
        return false;
    }
    cout << m_attributes;
    cout << _T("Reading attributes succeeded.") << endl;
    return true;
}

//---------------------------------------------------------------------------
// Create a name file for C4.5.
//---------------------------------------------------------------------------
bool CTDAttribMgr::writeNameFile()
{
    cout << _T("Writing name file...") << endl;
    try {
        CStdioFile nameFile;
        if (!nameFile.Open(m_nameFile, CFile::modeCreate | CFile::modeWrite)) {
            cerr << _T("CTDAttribMgr: Failed to open file ") << m_nameFile << endl;
            return false;
        }

        // Write class.
		int c = 0;
        int nAttributes = getNumAttributes();
        CTDAttrib* pAttrib = m_attributes.GetAt(nAttributes - 1);
        CTDConcepts* pFlattenConcepts = pAttrib->getFlattenConcepts();
        for (c = 1; c < pFlattenConcepts->GetSize(); ++c) {
            nameFile.WriteString(pFlattenConcepts->GetAt(c)->m_conceptValue);
            if (c < pFlattenConcepts->GetSize() - 1)
                nameFile.WriteString(CString(TD_NAMEFILE_SEPARATOR) + _T(" "));
            else {
                nameFile.WriteString(CString(TD_NAMEFILE_TERMINATOR));
                nameFile.WriteString(_T("\n\n"));
            }
        }

        // Write attributes.
        for (int a = 0; a < nAttributes - 1; ++a) {
            pAttrib = m_attributes.GetAt(a);
            nameFile.WriteString(pAttrib->m_attribName + TD_NAMEFILE_ATTNAMESEP + _T(" "));

            pFlattenConcepts = pAttrib->getFlattenConcepts();
#ifdef _TD_TREAT_CONT_AS_CONT
            if (pAttrib->isContinuous()) {
#else
            if (pAttrib->isContinuous() && !pAttrib->m_bVirtualAttrib) {
#endif         
                nameFile.WriteString(TD_NAMEFILE_CONTINUOUS);
                nameFile.WriteString(_T("\n"));
            }
            else {
                int nConcepts = 0;
                if (pAttrib->isMaskTypeSup())
                    nConcepts = pAttrib->m_nOFlatConcepts;
                else
                    nConcepts = pFlattenConcepts->GetSize();                  

                for (c = 0; c < nConcepts; ++c) {
                    nameFile.WriteString(pFlattenConcepts->GetAt(c)->m_conceptValue);
                    if (c < nConcepts - 1)
                        nameFile.WriteString(CString(TD_NAMEFILE_SEPARATOR) + _T(" "));
                    else {
                        if (pFlattenConcepts->GetSize() == 1) {
                            // add a fake concept if there is only one concept.
                            nameFile.WriteString(CString(TD_NAMEFILE_SEPARATOR) + _T(" "));
                            nameFile.WriteString(CString(TD_NAMEFILE_FAKE_CONT_CONCEPT));
                        }
                        nameFile.WriteString(CString(TD_NAMEFILE_TERMINATOR));
                        nameFile.WriteString(_T("\n"));
                    }
                }
            }
        }
        nameFile.Close();
    }
    catch (CFileException&) {
        cerr << _T("Failed to write name file: ") << m_nameFile << endl;
        ASSERT(false);
        return false;
    }
    cout << _T("Writing name file succeeded.") << endl;
    return true;
}



//---------------------------------------------------------------------------
// Pick a concept for specialization.
// Return true if a concept is picked; otherwise false.
//---------------------------------------------------------------------------
bool CTDAttribMgr::pickSpecializeConcept(CTDAttrib*& pSelectedAttrib, CTDConcept*& pSelectedConcept, double epsilon)
{
    pSelectedAttrib = NULL;
    pSelectedConcept = NULL;
	CTDConcepts candidates;
	candidates.cleanup();
	CTDFloatArray weights;
	int idx = 0;

    // For each attribute.
    // Minimize the distortion, so specialize even if score = 0.
    float maxInfo = -1.0f;

    CTDAttrib* pAttrib = NULL;
    CTDConcept* pCutConcept = NULL;
    int nAttribs = m_attributes.GetSize() - 1;  // exclude class attribute
    for (int a = 0; a < nAttribs; ++a) {
        pAttrib = m_attributes.GetAt(a);

        if (!pAttrib->m_bVirtualAttrib)
            continue;
        
        // For each concept in the cut.
        for (POSITION pos = pAttrib->m_cut.GetHeadPosition(); pos != NULL; ++idx) {
            pCutConcept = pAttrib->m_cut.GetNext(pos);
            if (!pCutConcept->m_bCutCandidate)
                continue;
			
			candidates.Add(pCutConcept);
			
#if defined(_TD_SCORE_FUNTION_MAX) 
			weights.Add(pCutConcept->m_max);
#endif

#if defined(_TD_SCORE_FUNCTION_INFOGAIN) 
			weights.Add(pCutConcept->m_infoGain);
#endif

		}
    }
	
	// Use exponential mechanism to select the candidate
    idx = expoMech(epsilon, &weights);
	pSelectedConcept = candidates.GetAt(idx);
	pSelectedAttrib = pSelectedConcept->getAttrib();

    if (!pSelectedAttrib || !pSelectedConcept) {
        // No concept can be specialized.
        return false;
    }

#ifdef _DEBUG_PRT_INFO
	cout << endl;
    cout << endl;
    cout << _T("* * * * * [Selected splitting attribute index = ") << pSelectedAttrib->m_attribIdx << _T(", name = ") << pSelectedAttrib->m_attribName << _T("]") << endl;
    cout << _T("* * * * * [Selected splitting concept flatten index = ") << pSelectedConcept->m_flattenIdx << _T(", name = ") << pSelectedConcept->m_conceptValue << _T("]") << endl;
    cout << _T("* * * * * [Selected splitting concept's InfoGain = ") << pSelectedConcept->m_infoGain << _T("]") << endl;
	cout << _T("* * * * * [Selected splitting concept's Max = ") << pSelectedConcept->m_max << _T("]") << endl;
	cout << endl;
    cout << endl;


#endif
    
    // Update the cut.
    if (!pSelectedAttrib->m_cut.specializeConcept(pSelectedConcept)) {
        ASSERT(false);
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDAttribMgr::computeScore()
{    

    // For each attribute.    
    CTDAttrib* pAttrib = NULL;
    CTDConcept* pCutConcept = NULL;
    int nAttribs = m_attributes.GetSize() - 1;  // exclude class attribute
    for (int a = 0; a < nAttribs; ++a) {
        pAttrib = m_attributes.GetAt(a);

        // Mode: not include non virtual attribute. So, never compute its InfoGain.
        if (!pAttrib->m_bVirtualAttrib)
            continue;

        // For each concept in the cut.
        for (POSITION conPos = pAttrib->m_cut.GetHeadPosition(); conPos != NULL;) {
            pCutConcept = pAttrib->m_cut.GetNext(conPos);
            if (!pCutConcept->m_bCutCandidate)
                continue;
            if (!pCutConcept->computeScore(getNumClasses()))
                return false;
        }
    }
    return true;
}