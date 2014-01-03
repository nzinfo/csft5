#ifndef PYIFACECSFT_H
#define PYIFACECSFT_H

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
void setColumnAsMVA(CSphColumnInfo& info, bool bJoin);
void addFieldColumn(CSphSchema* pSchema, CSphColumnInfo& info);
int  getSchemaFieldCount(CSphSchema* pSchema);

uint32_t getCRC32(const char* data, size_t iLength);

uint32_t getConfigValues(const CSphConfigSection & hSource, const char* sKey, CSphStringList& value);

CSphSource * SpawnSourcePython ( const CSphConfigSection & hSource, const char * sSourceName);

//------ Python Tokenizer Block -------


//------ Python Cache Block -------


//------ Python Query Block -------

#endif // PYIFACECSFT_H
