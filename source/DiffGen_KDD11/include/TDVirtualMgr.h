// TDVirtualMgr.h: interface for the CTDVirtualMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDVIRTUALMGR_H)
#define TDVIRTUALMGR_H

#if !defined(TDVIRTUALID_H)
    #include "TDVirtualID.h"
#endif

#if !defined(TDATTRIBMGR_H)
    #include "TDAttribMgr.h"
#endif

class CTDVirtualMgr  
{
public:
    CTDVirtualMgr(LPCTSTR vidsFile, int defaultK);
    virtual ~CTDVirtualMgr();

// operations
    bool initialize(CTDAttribMgr* pAttribMgr);
    bool readVirtualIDs();
    CTDVirtualIDs* getVIDs() { return &m_vids; };
    
protected:
// attributes
    int           m_defaultK;
    CString       m_vidsFile;
    CTDVirtualIDs m_vids;
    CTDAttribMgr* m_pAttribMgr;
};

#endif
