// TDRecord.h: interface for the CTDRecord class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TDRECORD_H)
#define TDRECORD_H

#if !defined(TDVALUE_H)
    #include "TDValue.h"
#endif

class CTDRecord  
{
public:
    CTDRecord();
    virtual ~CTDRecord();

// operations
    void setRecordID(int recID) { m_recordID = recID; };
    int getRecordID() { return m_recordID; };

    bool addValue(CTDValue* pValue);
    int getNumValues() { return m_values.GetSize(); };
    CTDValue* getValue(int idx) { return m_values.GetAt(idx); };
	CString toString(bool bRawValue) const;
    friend ostream& operator<<(ostream& os, const CTDRecord& record);

protected:
// attributes
    CTDValues m_values;
    int m_recordID;
};

typedef CTypedPtrArray<CPtrArray, CTDRecord*> CTDRecordArray;
class CTDRecords : public CTDRecordArray
{
public:
    CTDRecords();
    virtual ~CTDRecords();
    void cleanup();
    bool sortByAttrib(int attribIdx);
    friend ostream& operator<<(ostream& os, const CTDRecords& records);

protected:
    float getNumericValue(int recIdx, int attribIdx);
    bool quickSort(int attribIdx, int left, int right);
    bool findPivot(int attribIdx, int left, int right, float& pivot);
    int partition(int attribIdx, int left, int right, float pivot);
    void swapRecord(int recIdxA, int recIdxB);
};

#endif
