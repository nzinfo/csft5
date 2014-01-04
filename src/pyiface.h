#ifndef PYIFACECSFT_H
#define PYIFACECSFT_H

#include <vector>

#include "sphinx.h"
#include "sphinxutils.h"

/*
 * 在 C++ 和 Python 层 中间, 提供一个适配层
 * C++ Object member function                           Cython Side
 *  -> Pass Cython instance as a parameter.
 *  -> Cython API with C-level function proto-type
 *                                                      -> Recast To Cython Class Object
 *                                                      -> Call object's method.
 *  -> The return value.
 */

//------ Python Configure Block -------


//------ Python Data Source Block -------
typedef CSphVector<CSphString> CSphStringList;

void initColumnInfo(CSphColumnInfo& info, const char* sName, const char* sType);
void setColumnBitCount(CSphColumnInfo& info, int iBitCount);
int  getColumnBitCount(CSphColumnInfo& info);
void setColumnAsMVA(CSphColumnInfo& info, bool bJoin);
int addFieldColumn(CSphSchema* pSchema, CSphColumnInfo& info);
int  getSchemaFieldCount(CSphSchema* pSchema);
CSphColumnInfo* getSchemaField(CSphSchema* pSchema, int iIndex);

uint32_t getCRC32(const char* data, size_t iLength);

uint32_t getConfigValues(const CSphConfigSection & hSource, const char* sKey, CSphStringList& value);

CSphSource * SpawnSourcePython ( const CSphConfigSection & hSource, const char * sSourceName);


class PySphMatch
{
public:
    PySphMatch():_m(NULL),_s(NULL) {}
    /*
     *  虽然接口上支持 CSphSource, 实际只能传入 CSphSource_Python2;
     */
    void bind(CSphSource* s, CSphMatch* m) { _s = s; _m = m; }
public:
    inline void setDocID(uint64_t id) {     _m->m_iDocID = (SphDocID_t)id;   }
    inline uint64_t getDocID() {    return _m->m_iDocID;    }
    void setAttr ( int iIndex, SphAttr_t uValue ) ;
    void setAttrFloat ( int iIndex, float fValue );

    //void pushMva( int iIndex, std::vector<DWORD>& values);
    int pushMva( int iIndex, std::vector<int64_t>& values, bool bMva64);

protected:
private:
    CSphMatch* _m;
    CSphSource* _s;
};

//------ Python Tokenizer Block -------


//------ Python Cache Block -------


//------ Python Query Block -------

#endif // PYIFACECSFT_H
