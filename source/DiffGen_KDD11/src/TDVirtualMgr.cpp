// TDVirtualMgr.cpp: implementation of the CTDVirtualMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#if !defined(TDVIRTUALMGR_H)
    #include "TDVirtualMgr.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDVirtualMgr::CTDVirtualMgr(LPCTSTR vidsFile, int defaultK) 
    : m_vidsFile(vidsFile), m_defaultK(defaultK)
{
    //ASSERT(m_pAttribMgr);
}

CTDVirtualMgr::~CTDVirtualMgr() 
{
    m_vids.cleanup();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool CTDVirtualMgr::initialize(CTDAttribMgr* pAttribMgr)
{
    if (!pAttribMgr) {
        ASSERT(false);
        return false;
    }
    m_pAttribMgr = pAttribMgr;
    return true;
}

//---------------------------------------------------------------------------
// Read information from the VIDs file.
//---------------------------------------------------------------------------
bool CTDVirtualMgr::readVirtualIDs()
{
    cout << _T("Reading virtual identifiers...") << endl;
    m_vids.RemoveAll();
    try {
        CStdioFile vidFile;
        if (!vidFile.Open(m_vidsFile, CFile::modeRead)) {
            cerr << _T("CTDVirtualMgr: Failed to open file ") << m_vidsFile << endl;
            return false;
        }

        // Parse each line.        
        CString lineStr, attribName, attribType, vidValuesStr;
        int commentCharPos = -1, semiColonPos = -1;
        while (vidFile.ReadString(lineStr)) {
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
                cerr << _T("CTDVirtualMgr: Unknown line: ") << lineStr << endl;
                ASSERT(false);
                return false;
            }

            // Extract attribute name.
            attribName = lineStr.Left(semiColonPos);
            CBFStrHelper::trim(attribName);
            if (attribName.IsEmpty()) {
                cerr << _T("CTDVirtualMgr: Invalid attribute: ") << lineStr << endl;
                ASSERT(false);
                return false;
            }

            // Extract attribute type.
            attribType = lineStr.Mid(semiColonPos + 1);
            CBFStrHelper::trim(attribType);
            if (attribType.IsEmpty())
                attribType = TD_DISCRETE_ATTRIB;

            // Read the next line which contains the VID.
            if (!vidFile.ReadString(vidValuesStr)) {
                cerr << _T("CTDVirtualMgr: Invalid attribute: ") << attribName << endl;
                ASSERT(false);
                return false;
            }
            CBFStrHelper::trim(vidValuesStr);
            if (vidValuesStr.IsEmpty()) {
                cerr << _T("CTDVirtualMgr: Invalid attribute: ") << attribName << endl;
                ASSERT(false);
                return false;
            }

            // Remove comments.
            commentCharPos = vidValuesStr.Find(TD_CONHCHY_COMMENT);
            if (commentCharPos != -1) {
                vidValuesStr = vidValuesStr.Left(commentCharPos);
                CBFStrHelper::trim(vidValuesStr);
                if (vidValuesStr.IsEmpty()) {
                    cerr << _T("CTDVirtualMgr: Invalid attribute: ") << attribName << endl;
                    ASSERT(false);
                    return false;
                }
            }

            if (attribName.CompareNoCase(TD_VID_ATTRIB_NAME) == 0) {
                if (!m_vids.initVIDs(vidValuesStr, m_pAttribMgr->getAttributes(), m_defaultK)) {
                    cerr << _T("CTDVirtualMgr: Invalid VID: ") << vidValuesStr << endl;
                    return false;
                }
            }
        }
        vidFile.Close();
    }
    catch (CFileException&) {
        cerr << _T("Failed to read VIDs file: ") << m_vidsFile << endl;
        ASSERT(false);
        return false;
    }
    cout << _T("Reading virtual identifiers succeeded.") << endl;
    return true;
}
