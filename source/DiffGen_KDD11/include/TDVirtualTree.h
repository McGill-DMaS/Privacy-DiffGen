// TDVirtualTree.h: interface for the CTDVirtualTree class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDVIRTUALTREE_H)
#define TDVIRTUALTREE_H

class CTDRollbackRecs;
class CTDVirtualTreeNode;

typedef CTypedPtrList<CPtrList, CTDVirtualTreeNode*> CTDVirtualTreeNodeList;
class CTDVirtualTreeNodes : public CTDVirtualTreeNodeList
{
public:
    CTDVirtualTreeNodes();
    virtual ~CTDVirtualTreeNodes();
    void cleanup();   
};

class CTDVirtualTreeNode
{
public:
    CTDVirtualTreeNode(CTDVirtualTreeNode* pParentNode, int flattenConceptIdx);
    virtual ~CTDVirtualTreeNode();

    int getCount() { return m_count; };
    void updateCount(int c) { m_count += c; }; 

    bool addChildNode(CTDVirtualTreeNode* pChildNode);
    void removeChildNode(POSITION childPos);
      
    CTDVirtualTreeNode* getVPathNode(const CTDIntArray& vPath); 
    bool updateVPathCount(const CTDIntArray& vPath, int diff, CTDRollbackRecs* pRollbackLog);

    bool searchMinACNode(CTDVirtualTreeNode*& pMinNode);

protected:
    CTDVirtualTreeNode* getVPathNodeHelper(const CTDIntArray& vPath, int vIdx);
    bool updateVPathCountHelper(const CTDIntArray& vPath, int vIdx, int diff, CTDRollbackRecs* pRollbackLog);
    bool searchMinACNodeHelper(CTDVirtualTreeNode*& pMinNode);
    void deleteZeroBranch();

// attributes
    CTDVirtualTreeNode* m_pParentNode;
    POSITION m_vTreeChildPos;

    CTDVirtualTreeNodes m_childNodes;
    int m_flattenConceptIdx;
    int m_count;
};

#endif
