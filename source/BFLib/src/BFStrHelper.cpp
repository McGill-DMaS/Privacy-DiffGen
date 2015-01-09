//---------------------------------------------------------------------------
// File:
//      BFStrHelper.cpp BFStrHelper.hpp
//
// Module:
//      CBFStrHelper
//
// History:
//		Jan. 15, 2004		Created by Benjamin Fung
//---------------------------------------------------------------------------

#include "BFPch.h"

#if !defined(BFSTRHELPER_H)
	#include "BFStrHelper.h"
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
CBFStrHelper::CBFStrHelper()
{
}

CBFStrHelper::~CBFStrHelper()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void CBFStrHelper::trim(CString& str)
{
    str.TrimLeft();
    str.TrimRight();
}