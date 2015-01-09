// TDVirtualID.cpp: implementation of the CTDVirtualID class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDVIRTUALID_H)
    #include "TDVirtualID.h"
#endif

//***************
// CTDVirtualID *
//***************

CTDVirtualID::CTDVirtualID(int defaultK)
    : m_pMinACNode(NULL), m_kAnonymity(defaultK), m_vTreeNodeRoot(NULL, -1)
{
}

CTDVirtualID::~CTDVirtualID() 
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDVirtualID::initVID(LPCTSTR vidStr, CTDAttribs* pAttribs)
{
    m_virtualAttribs.RemoveAll();
    ASSERT(pAttribs);

    // Take away the first and last characters.
    CString restStr = vidStr;
    CBFStrHelper::trim(restStr);
    int len = restStr.GetLength();
    if (len < 2 ||
        restStr[0] != TD_CONHCHY_OPENTAG || 
        restStr[len - 1] != TD_CONHCHY_CLOSETAG) {
        ASSERT(false);
        return false;
    }
    restStr = restStr.Mid(1, len - 2);
    CBFStrHelper::trim(restStr);
    
    // Retrieve the k value for this VID.
    CString kStr;
    if (!parseVIDAttrib(kStr, restStr)) {
        cerr << _T("CTDVirtualID: Failed to extract k from ") << restStr << endl;
        return false;
    }
    if (m_kAnonymity <= 0) {
        m_kAnonymity = StrToInt((LPCTSTR) kStr);
        if (m_kAnonymity <= 0) {
            cerr << _T("CTDVirtualID: Failed to extract k from ") << restStr << endl;
            ASSERT(false);
            return false;
        }
    }

    // Extract each Virtual Attribute.
    CString firstVIDAttrib;    
    CTDAttrib* pAttrib = NULL;
    int nAttribs = pAttribs->GetSize();
    bool bFound = false;
    while (!restStr.IsEmpty()) {
        if (!parseVIDAttrib(firstVIDAttrib, restStr)) {
            cerr << _T("CTDVirtualID: Failed to build VID from ") << restStr << endl;
            return false;
        }

        // Find the attribute.
        bFound = false;
        for (int i = 0; i < nAttribs; ++i) {
            pAttrib = pAttribs->GetAt(i);
            if (pAttrib->m_attribName.CompareNoCase(firstVIDAttrib) == 0) {
                // Found! 
                // Tag this attribute as Virtual Attribute.
                // Put this attribute into this VID.
                bFound = true;
                pAttrib->m_bVirtualAttrib = true;
                m_virtualAttribs.Add(pAttrib);
                break;
            }
        }

        if (!bFound) {
            cerr << _T("CTDVirtualID: Failed to find VID ") << firstVIDAttrib << endl;
            ASSERT(false);
            return false;
        }
    }

    if (m_virtualAttribs.GetSize() == 0) {
        ASSERT(false);
        return false;
    }
    return true;
}

//---------------------------------------------------------------------------
// If path cannot be found, return 0.
//---------------------------------------------------------------------------
int CTDVirtualID::getVPathCount(const CTDIntArray& vPath) 
{ 
    ASSERT(m_virtualAttribs.GetSize() == vPath.GetSize());
    CTDVirtualTreeNode* pNode = m_vTreeNodeRoot.getVPathNode(vPath);
    if (!pNode) {
        // Node cannot be found.
        return 0;
    }
    return pNode->getCount();
}

//---------------------------------------------------------------------------
// If path does not exist, create it.
//---------------------------------------------------------------------------
bool CTDVirtualID::updateVPathCount(const CTDIntArray& vPath, int count, bool bRollback) 
{ 
    ASSERT(m_virtualAttribs.GetSize() == vPath.GetSize());
    if (bRollback)
        return m_vTreeNodeRoot.updateVPathCount(vPath, count, &m_rollbackRecs);
    else
        return m_vTreeNodeRoot.updateVPathCount(vPath, count, NULL);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDVirtualID::updateMinACNode()
{
    CTDVirtualTreeNode* pNode = NULL;
    if (!m_vTreeNodeRoot.searchMinACNode(pNode))
        return false;

    ASSERT(pNode);
    m_pMinACNode = pNode;
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDVirtualID::searchMinAC(int& minAC)
{
    minAC = 0;
    CTDVirtualTreeNode* pNode = NULL;
    if (!m_vTreeNodeRoot.searchMinACNode(pNode))
        return false;

    ASSERT(pNode);
    minAC = pNode->getCount();
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDVirtualID::rollback()
{
    CTDRollbackRec* pRec = NULL;
    int nRecs = m_rollbackRecs.GetSize();
    for (int i = 0; i < nRecs; ++i) {
        pRec = m_rollbackRecs.GetAt(i);
        pRec->m_pVTreeNode->updateCount(pRec->m_diff * -1);
    }
    m_rollbackRecs.cleanup();
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// static
bool CTDVirtualID::parseVIDAttrib(CString& firstAttrib, CString& restStr)
{
    firstAttrib.Empty();
    int len = restStr.GetLength();
    if (len < 2 ||
        restStr[0] != TD_CONHCHY_OPENTAG || 
        restStr[len - 1] != TD_CONHCHY_CLOSETAG) {
        ASSERT(false);
        return false;
    }

    int closeTagPos = restStr.Find(TD_CONHCHY_CLOSETAG);
    if (closeTagPos < 0) {
        ASSERT(false);
        return false;
    }

    // Extract the first element.
    firstAttrib = restStr.Mid(1, closeTagPos - 1);
    CBFStrHelper::trim(firstAttrib);
    if (firstAttrib.IsEmpty()) {
        ASSERT(false);
        return false;
    }

    // Update the rest of the string.
    restStr = restStr.Mid(closeTagPos + 1);
    CBFStrHelper::trim(restStr);
    return true;
}


//****************
// CTDVirtualIDs *
//****************
CTDVirtualIDs::CTDVirtualIDs()
{
}

CTDVirtualIDs::~CTDVirtualIDs()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CTDVirtualIDs::cleanup()
{
    int nVIDs = GetSize();
    for (int i = 0; i < nVIDs; ++i)
        delete GetAt(i);
    RemoveAll();
}

//---------------------------------------------------------------------------
// {{{attrib A} {attrib B}} {{attrib C} {attrib D} {attrib E}}}
//---------------------------------------------------------------------------
bool CTDVirtualIDs::initVIDs(LPCTSTR vidStr, CTDAttribs* pAttribs, int defaultK)
{
    ASSERT(pAttribs);

    // Take away the first and last characters.
    CString restStr = vidStr;
    int len = restStr.GetLength();
    if (len < 2 ||
        restStr[0] != TD_CONHCHY_OPENTAG || 
        restStr[len - 1] != TD_CONHCHY_CLOSETAG) {
        cerr << _T("CTDVirtualIDs: Invalid VID: ") << restStr << endl;
        ASSERT(false);
        return false;
    }
    restStr = restStr.Mid(1, len - 2);
    CBFStrHelper::trim(restStr);

    CString firstVID;
    while (!restStr.IsEmpty()) {
        // Retrieve the first VID
        if (!parseFirstVID(firstVID, restStr))
            return false;
        if (firstVID.IsEmpty()) {
            ASSERT(false);
            continue;
        }

        // Create a VID.
        CTDVirtualID* pNewVID = new CTDVirtualID(defaultK);
        if (!pNewVID) {
            ASSERT(false);
            return false;
        }

        if (!pNewVID->initVID(firstVID, pAttribs)) {
            cerr << _T("CTDVirtualIDs: Failed to init VID for ") << firstVID << endl;
            return false;
        }
        Add(pNewVID);
    }
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// static
bool CTDVirtualIDs::parseFirstVID(CString& firstVID, CString& restStr)
{
    firstVID.Empty();
    int len = restStr.GetLength();
    if (len < 2 ||
        restStr[0] != TD_CONHCHY_OPENTAG || 
        restStr[len - 1] != TD_CONHCHY_CLOSETAG) {
        ASSERT(false);
        return false;
    }

    // Find the index number of the closing tag of the first concept.
    int tagCount = 0;
    for (int i = 0; i < len; ++i) {
        if (restStr[i] == TD_CONHCHY_OPENTAG)
            ++tagCount;
        else if (restStr[i] == TD_CONHCHY_CLOSETAG) {
            --tagCount;
            ASSERT(tagCount >= 0);
            if (tagCount == 0) {
                // Closing tag of first concept found!
                firstVID = restStr.Left(i + 1);
                restStr = restStr.Mid(i + 1);
                CBFStrHelper::trim(restStr);
                return true;
            }
        }
    }
    ASSERT(false);
    return false;
}