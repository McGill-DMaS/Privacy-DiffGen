// BFStrHelper.h: interface for the String Helper classes
//
//////////////////////////////////////////////////////////////////////

#if !defined(BFSTRHELPER_H)
#define BFSTRHELPER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//--------------------------------------------------------------------
//--------------------------------------------------------------------
class CBFStrHelper
{
public:
    CBFStrHelper();
    virtual ~CBFStrHelper();

// operations    
    static void trim(CString& str);
};

#endif