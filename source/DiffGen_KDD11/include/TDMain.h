
#if !defined(TDMAIN_H)
#define TDMAIN_H


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"


void debugPrint(LPCTSTR str);
void printTime();
long get_runtime(void);
ostream& operator<<(ostream& os, const CTDIntArray& intAry);
float calEntropy(CTDIntArray* pArray);
void orderNumbers(float& a, float& b, float& c);
void swapNumbers(float& a, float& b);
double laplaceNoise(double epsilon);
int expoMech(double epsilon, CTDFloatArray* weights);
int expoMechSplit(double epsilon, CTDFloatArray* weights, CTDFloatArray* ranges);
#endif
