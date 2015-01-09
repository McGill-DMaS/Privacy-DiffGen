// TDMain.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#if !defined(TDMAIN_H)
    #include "TDMain.h"
#endif

#if !defined(TDCONTROLLER_H)
    #include "TDController.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only application object

CWinApp theApp;

//using namespace std;
using namespace System;


//---------------------------------------------------------------------------
// Command Arguments: C:\\Users\\...\\...\\exp\\adult FALSE 10 1 -1 30162
// Total line of code ~ 6K.
//---------------------------------------------------------------------------
bool parseArgs(int      nArgs, 
               TCHAR*   argv[], 
               CString& dataSetName,
               bool&    bRemoveUnknownOnly,
               int&		nSpecialization,
			   double&  pBudget,
               int&     nInputRecs,
               int&     nTraining)
{
    if (nArgs != 7 || !argv) {
        cout << _T("Usage: PrivacyTD <dataSetName> <bRemoveUnknownOnly> <nSpecialization> <privacyB> <nInputRecs> <nTraining>") << endl;
        return false;
    }

    dataSetName = argv[1];
	
	if (_tcsicmp(argv[2], _T("TRUE")) == 0)
	    bRemoveUnknownOnly = true;
	else if (_tcsicmp(argv[2], _T("FALSE")) == 0)
		bRemoveUnknownOnly = false;
	else
        return false;

	nSpecialization = StrToInt(argv[3]);
	pBudget = StrToFloat(argv[4]);
    nInputRecs = int(StrToFloat(argv[5]));
    nTraining = int(StrToFloat(argv[6]));
    return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void debugPrint(LPCTSTR str)
{
    cout << str << endl;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void printTime()
{
    time_t ltime;
    time(&ltime);
    cout << _T("Current System Time = ") << ltime << endl;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
long get_runtime(void)
{
  clock_t start;
  start = clock();
  return((long)((double)start*100.0/(double)CLOCKS_PER_SEC));
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
ostream& operator<<(ostream& os, const CTDIntArray& intAry)
{
#ifdef _DEBUG_PRT_INFO
    os << _T("[");
    for (int i = 0; i < intAry.GetSize(); ++i) {
        os << intAry.GetAt(i);
        if (i != intAry.GetSize() - 1)
            os << _T(" ");
    }
    os << _T("]");
#endif
    return os;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	
	// rand() initialization
	srand( (unsigned)time( NULL ) );

	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: change error code to suit your needs
		cerr << _T("Fatal Error: MFC initialization failed") << endl;
		nRetCode = 1;
	}
	else
	{
		// TODO: code your application's behavior here.
		CString strHello;
		strHello.LoadString(IDS_HELLO);
		


        CString dataSetName;
        int nTraining = 0, nInputRecs = 0, nSpecialization = 0;
		double pBudget = 0;
        bool bRemoveUnknownOnly = false;

        if (!parseArgs(argc, argv, dataSetName, bRemoveUnknownOnly, nSpecialization, pBudget, nInputRecs, nTraining)) {
		    cerr << _T("Input Error: invalid arguments") << endl;
		    return 1;
        }
        
        // Construct the filenames
        CString rawDataFile, attributesFile, nameFile, supFile, transformedDataFile, transformedTestFile, transformedSVMDataFile, transformedSVMTestFile;
        rawDataFile = dataSetName;
        rawDataFile += _T(".");
        rawDataFile += TD_RAWDATAFILE_EXT;
        attributesFile = dataSetName;
        attributesFile += _T(".");
        attributesFile += TD_ATTRBFILE_EXT;
        nameFile = dataSetName;
        nameFile += _T(".");
        nameFile += TD_NAMEFILE_EXT;
        transformedDataFile = dataSetName;
        transformedDataFile += _T(".");
        transformedDataFile += TD_TRANSFORM_DATAFILE_EXT;
        transformedTestFile = dataSetName;
        transformedTestFile += _T(".");
        transformedTestFile += TD_TRANSFORM_TESTFILE_EXT;

        CTDController controller(rawDataFile, 
                                 attributesFile,
                                 nameFile,
                                 transformedDataFile, 
                                 transformedTestFile,
								 nSpecialization,
								 pBudget,
                                 nInputRecs,
                                 nTraining);

        if (bRemoveUnknownOnly) {
            if (!controller.removeUnknowns()) {
                cerr << _T("Error occured.") << endl;
                return 1;
            }
        }
        else {
            if (!controller.runTopDown()) {
                cerr << _T("Error occured.") << endl;
                return 1;
            }
        }
	}

    cout << _T("Bye!") << endl;

/*	#ifdef _DEBUG
		getch();
	#endif
*/
	return nRetCode;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
float calEntropy(CTDIntArray* pArray)
{    
    int c = 0;
    int total = 0;
    int sz = pArray->GetSize();
    for (c = 0; c < sz; ++c)
        total += pArray->GetAt(c);
	
	float r = 0.0f;
    float entropy = 0.0f;
	
	// if there is no record for a concept, then the entropy is zero.
	if (total == 0)
		return entropy;

    for (c = 0; c < sz; ++c) {
        r = float(pArray->GetAt(c)) / total;
        if (r > 0.0f)
            entropy += (r * log2f(r)) * -1;
    }
    return entropy;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void orderNumbers(float& a, float& b, float& c)
{
    if (a > b)
        swapNumbers(a, b);
    if (a > c)
        swapNumbers(a, c);
    if (b > c)
        swapNumbers(b, c);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void swapNumbers(float& a, float& b)
{
    float temp = b;
    b = a;
    a = temp;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
double laplaceNoise(double epsilon)
{
	//srand(time(0)); 
	double uniform = (double)rand()/RAND_MAX- 0.5;
	return Math::Ceiling(1.0 / epsilon * Math::Sign(uniform) * Math::Log(1 - 2.0 * Math::Abs(uniform)));
}
//---------------------------------------------------------------------------
// For selecting an attribute from the candidates for specialization 
//---------------------------------------------------------------------------

int expoMech(double epsilon, CTDFloatArray* weights)

{
	int i = 0;
	int sz = weights->GetSize();

	if (sz == 0) {
		cerr << _T("expoMech: Array weights is empty.") << endl;
        ASSERT(false);
        return false;
    }

	double r = 0;
	r = rand();
	r = r/RAND_MAX; // r is a number between 0 and 1

	double sensitivity = 1;

	// maxWeights is needed for normalizing
	float maxWeights = 0;
	for (i = 0; i < sz; ++i ){
		if (weights->GetAt(i)> maxWeights)
			maxWeights = weights->GetAt(i);
	}

	// The sum of all weights.
	long double total = 0;
	for (i = 0; i < sz; ++i )
		total += exp(epsilon * (weights->GetAt(i)- maxWeights)/(2*sensitivity));  
	
	int index = 0;
	long double prob = 0;
	long double tProb = 0;
	double sum = 0;
	
	for (index = 0; index < sz; ++index )
	{
		prob = exp(epsilon * (weights->GetAt(index)- maxWeights)/(2*sensitivity));
		tProb += prob;
		sum = tProb/total;  
		if (r <= sum)
			return index;	
	}
	
	if (index == sz) {
		cerr << _T("expoMech: Out of array size") << endl;
        ASSERT(false);
    }

	return false;
}

//---------------------------------------------------------------------------
// For selecting a split point of a continuous attribute 
//---------------------------------------------------------------------------

int expoMechSplit(double epsilon, CTDFloatArray* weights,  CTDFloatArray* ranges)

{
	int i = 0;
	int sz = weights->GetSize();

	if (sz == 0) {
		cerr << _T("expoMech: Array weights is empty.") << endl;
        ASSERT(false);
        return false;
    }

	double r = 0;
	r = rand();
	r = r/RAND_MAX; // r is a number between 0 and 1

	double sensitivity = 1;

	float maxWeights = 0;
	for (i = 0; i < sz; ++i ){
		if (weights->GetAt(i)> maxWeights)
			maxWeights = weights->GetAt(i);
	}


	long double total = 0;
	for (i = 0; i < sz; ++i )
		total += exp(epsilon * (weights->GetAt(i)- maxWeights)/(2*sensitivity))* ranges->GetAt(i) ;

	int index = 0;
	long double prob = 0;
	long double tProb = 0;
	double sum = 0;
	for (index = 0; index < sz; ++index )
	{
		prob = exp(epsilon * (weights->GetAt(index)- maxWeights)/(2*sensitivity))* ranges->GetAt(index);
		tProb += prob;
		sum = tProb/total; 
		if (r <= sum)
			return index;	
	}

	if (index == sz) {
		cerr << _T("expoMech: Out of array size") << endl;
        ASSERT(false);
    }	

	return false;
}

