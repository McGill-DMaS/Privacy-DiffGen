// TDVirtualID.h: interface for the CTDVirtualID class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDVIRTUALID_H)
#define TDVIRTUALID_H

#if !defined(TDATTRIBUTE_H)
    #include "TDAttribute.h"
#endif

#if !defined(TDVIRTUALTREE_H)
    #include "TDVirtualTree.h"
#endif

#if !defined(TDROLLBACKREC_H)
    #include "TDRollbackRec.h"
#endif

class CTDVirtualID
{
public:
    CTDVirtualID(int defaultK);
    virtual ~CTDVirtualID();

// operations
    bool initVID(LPCTSTR vidStr, CTDAttribs* pAttribs);
    static bool parseVIDAttrib(CString& firstAttrib, CString& restStr);

    CTDAttribs* getVirtualAttribs() { return &m_virtualAttribs; };

    int getVPathCount(const CTDIntArray& vPath);
    bool updateVPathCount(const CTDIntArray& vPath, int count, bool bRollback);

    bool updateMinACNode();
    bool searchMinAC(int& minAC);

    void initRollback() { m_rollbackRecs.cleanup(); };
    bool rollback();

    int m_kAnonymity;
    CTDVirtualTreeNode* m_pMinACNode;   // The node of the minimum anonymity count of this VID.

protected:
// attributes    
    CTDAttribs m_virtualAttribs;        // array of virtual attributes.
    CTDVirtualTreeNode m_vTreeNodeRoot; // Virtual tree root.
    CTDRollbackRecs m_rollbackRecs;     // Rollback records.
};


typedef CTypedPtrArray<CPtrArray, CTDVirtualID*> CTDVirtualIDArray;
class CTDVirtualIDs : public CTDVirtualIDArray
{
public:
    CTDVirtualIDs();
    virtual ~CTDVirtualIDs();
    void cleanup();

// operations
    bool initVIDs(LPCTSTR vidStr, CTDAttribs* pAttribs, int defaultK);
    static bool parseFirstVID(CString& firstVID, CString& restStr);
};

#endif
