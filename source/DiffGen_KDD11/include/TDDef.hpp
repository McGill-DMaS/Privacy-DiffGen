// TDDef.hpp: define the constants/types shared among all classes
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDDEF_HPP)
#define TDDEF_HPP

// pre-processor
#ifdef _DEBUG
    #define _DEBUG_PRT_INFO
#endif
//#define _DEBUG_PRT_INFO

#define DEBUGPrint _tprintf				// print in console
//#define DEBUGPrint TRACE				// print in debug window
//#define _TD_MANUAL_CONTHRCHY
//#define _TD_TREAT_CONT_AS_CONT		// Treat continuous attributes as continuous attributes in C4.5

#define _TD_SCORE_FUNCTION_INFOGAIN
//#define _TD_SCORE_FUNTION_MAX     


#define P_ASSERT(p) if (!p) { ASSERT(false); return false; }

// Common types
typedef CArray<int, int>      CTDIntArray;
typedef CArray<float, float>  CTDFloatArray;
typedef CArray<bool, bool>    CTDBoolArray;
typedef CBFMultiDimArray<int> CTDMDIntArray;


// Constants
#define TD_RAWDATAFILE_EXT                  _T("rawdata")
#define TD_ATTRBFILE_EXT                    _T("hchy")
#define TD_NAMEFILE_EXT                     _T("names")
#define TD_TRANSFORM_DATAFILE_EXT           _T("data")
#define TD_TRANSFORM_TESTFILE_EXT           _T("test")

#define TD_VID_ATTRIB_NAME                  _T("vid")
#define TD_CLASSES_ATTRIB_NAME              _T("classes")
#define TD_DISCRETE_ATTRIB                  _T("discrete")
#define TD_CONTINUOUS_ATTRIB                _T("continuous")
#define TD_MASKTYPE_GEN                     _T("generalization")
#define TD_MASKTYPE_SUP                     _T("suppression")

#define TD_TRANSACTION_ITEM_PRESENT         _T("1")

#define TD_CONHCHY_OPENTAG                  TCHAR('{')
#define TD_CONHCHY_CLOSETAG                 TCHAR('}')
#define TD_CONHCHY_DASHSYM                  TCHAR('-')
#define TD_CONHCHY_COMMENT                  TCHAR('|')
#define TD_RAWDATA_DELIMETER                TCHAR(',')
#define TD_SETVALUE_DELIMETER               TCHAR('>')
#define TD_RAWDATA_TERMINATOR               TCHAR('.')
#define TD_UNKNOWN_VALUE                    TCHAR('?')

#define TD_NAMEFILE_ATTNAMESEP              TCHAR(':')
#define TD_NAMEFILE_SEPARATOR               TCHAR(',')
#define TD_NAMEFILE_TERMINATOR              TCHAR('.')
#define TD_NAMEFILE_CONTINUOUS              _T("continuous")
#define TD_NAMEFILE_FAKE_CONT_CONCEPT       _T("fake")

#define TD_CONTVALUE_NUMDEC                 2

#endif