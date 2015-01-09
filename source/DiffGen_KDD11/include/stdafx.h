// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DD768C9B_C9C4_4176_94F5_3DED05FBF9B0__INCLUDED_)
#define AFX_STDAFX_H__DD768C9B_C9C4_4176_94F5_3DED05FBF9B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <afxtempl.h>
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <iostream>
#include <conio.h>
#include <math.h>
#include <limits.h>
#include <float.h>

#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <ctime>
#include <cmath>
#include <assert.h>


using namespace std;



// TODO: reference additional headers your program requires here

// BFLib
#if !defined (TYPES_HPP)
    #include "types.hpp"
#endif

#if !defined (STRUTIL_HPP)
    #include "strutil.hpp"
#endif

#if !defined(BFSTRHELPER_H)
	#include "BFStrHelper.h"
#endif

#if !defined(BFSTRPSER_H)
	#include "BFStrPser.h"
#endif

#if !defined(BFMULTIDIMARY_H)
    #include "BFMultiDimAry.h"
#endif

#if !defined(BFMATH_H)
    #include "BFMath.h"
#endif

#if !defined(TDDEF_HPP)
    #include "TDDef.hpp"
#endif

#if !defined(TDMAIN_H)
    #include "TDMain.h"
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__DD768C9B_C9C4_4176_94F5_3DED05FBF9B0__INCLUDED_)
