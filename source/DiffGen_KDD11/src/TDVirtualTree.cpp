// TDVirtualTree.cpp: implementation of the CTDVirtualTree class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDVIRTUALTREE_H)
    #include "TDVirtualTree.h"
#endif

#if !defined(TDROLLBACKREC_H)
    #include "TDRollbackRec.h"
#endif

//**********************
// CTDVirtualTreeNodes *
//**********************
CTDVirtualTreeNodes::CTDVirtualTreeNodes()
{
}

CTDVirtualTreeNodes::~CTDVirtualTreeNodes()
{
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CTDVirtualTreeNodes::cleanup()
{
    while (!IsEmpty())
        delete RemoveHead();
}

//*********************
// CTDVirtualTreeNode *
//*********************

CTDVirtualTreeNode::CTDVirtualTreeNode(CTDVirtualTreeNode* pParentNode, int flattenConceptIdx)
    : m_pParentNode(pParentNode), m_vTreeChildPos(NULL), m_count(0), m_flattenConceptIdx(flattenConceptIdx)
{
}

CTDVirtualTreeNode::~CTDVirtualTreeNode() 
{
    m_childNodes.cleanup();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDVirtualTreeNode::addChildNode(CTDVirtualTreeNode* pChildNode)
{
    try {
        m_childNodes.AddTail(pChildNode);
        return true;
    }
    catch (CMemoryException&) {
        ASSERT(false);
        return false;
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CTDVirtualTreeNode::removeChildNode(POSITION childPos)
{
    CTDVirtualTreeNode* pChildNode = m_childNodes.GetAt(childPos);
    m_childNodes.RemoveAt(childPos);
    delete pChildNode;
    pChildNode = NULL;

    if (m_childNodes.GetCount() == 0 && m_pParentNode) {
        m_pParentNode->removeChildNode(m_vTreeChildPos);
    }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CTDVirtualTreeNode::deleteZeroBranch()
{
    if (m_count != 0)
        return;

    if (!m_pParentNode)
        return;
    
    m_pParentNode->removeChildNode(m_vTreeChildPos);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CTDVirtualTreeNode* CTDVirtualTreeNode::getVPathNode(const CTDIntArray& vPath)
{
    return getVPathNodeHelper(vPath, 0);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
CTDVirtualTreeNode* CTDVirtualTreeNode::getVPathNodeHelper(const CTDIntArray& vPath, int vIdx)
{        
    if (vIdx == vPath.GetSize()) {
        // This is the leaf.        
        ASSERT(vPath.GetAt(vIdx - 1) == m_flattenConceptIdx);
        ASSERT(m_childNodes.GetCount() == 0);
        return this;
    }
    
    CTDVirtualTreeNode* pChildNode = NULL;
    for (POSITION pos = m_childNodes.GetHeadPosition(); pos != NULL;) {
        pChildNode = m_childNodes.GetNext(pos);
        if (pChildNode->m_flattenConceptIdx == vPath.GetAt(vIdx)) {
            // Matched index found.
            return pChildNode->getVPathNodeHelper(vPath, vIdx + 1);
        }
    }

    // Cannot find a match.
    return NULL;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDVirtualTreeNode::updateVPathCount(const CTDIntArray& vPath, int diff, CTDRollbackRecs* pRollbackLog)
{
#ifdef _DEBUG_PRT_INFO
    cout << vPath << _T("\tDiff=") << diff;
#endif
    return updateVPathCountHelper(vPath, 0, diff, pRollbackLog);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDVirtualTreeNode::updateVPathCountHelper(const CTDIntArray& vPath, int vIdx, int diff, CTDRollbackRecs* pRollbackLog)
{    
    if (vIdx == vPath.GetSize()) {
        // This is the leaf.        
        ASSERT(vPath.GetAt(vIdx - 1) == m_flattenConceptIdx);
        ASSERT(m_childNodes.GetCount() == 0);
        m_count += diff;
#ifdef _DEBUG_PRT_INFO
        cout << _T("\t\tUpdated_Count=") << m_count << _T("\tRollback=") << (pRollbackLog ? _T("Yes") : _T("No")) << endl;
#endif
        //deleteZeroBranch();
        if (pRollbackLog) {
            pRollbackLog->Add(new CTDRollbackRec(this, diff));
        }
        return true;
    }

    int targetConceptIdx = vPath.GetAt(vIdx);    
    CTDVirtualTreeNode* pChildNode = NULL;
    for (POSITION pos = m_childNodes.GetHeadPosition(); pos != NULL;) {
        pChildNode = m_childNodes.GetNext(pos);
        if (pChildNode->m_flattenConceptIdx == targetConceptIdx) {
            // Matched index found.
            return pChildNode->updateVPathCountHelper(vPath, vIdx + 1, diff, pRollbackLog);
        }              
    }

    // Cannot find a match. Create one.
    pChildNode = new CTDVirtualTreeNode(this, targetConceptIdx);
    if (!pChildNode) {
        ASSERT(false);
        return false;
    }
    pChildNode->m_vTreeChildPos = m_childNodes.AddTail(pChildNode);
    return pChildNode->updateVPathCountHelper(vPath, vIdx + 1, diff, pRollbackLog);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDVirtualTreeNode::searchMinACNode(CTDVirtualTreeNode*& pMinNode)
{
    pMinNode = NULL;
    return searchMinACNodeHelper(pMinNode);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDVirtualTreeNode::searchMinACNodeHelper(CTDVirtualTreeNode*& pMinNode)
{
    if (m_childNodes.GetCount() == 0) {
        ASSERT(m_pParentNode);
        //ASSERT(m_count > 0);  Enable this if we call deleteZeroBranch();
        if (m_count == 0)
            return true;

        if (!pMinNode) {
            pMinNode = this;
            return true;
        }

        // This is the leaf.        
        if (m_count < pMinNode->m_count) {
            // A smaller node is found.
            pMinNode = this;
            return true;
        }

        // Not the minimum node;
        return true;
    }

    for (POSITION pos = m_childNodes.GetHeadPosition(); pos != NULL;) {
        if (!m_childNodes.GetNext(pos)->searchMinACNodeHelper(pMinNode)) {
            ASSERT(false);
            return false;
        }
    }
    return true;
}