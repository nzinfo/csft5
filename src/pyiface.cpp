#include "pyiface.h"

#include "pysource.h"

//------ Python Configure Block -------


//------ Python Data Source Block -------

// get string
#define LOC_GETS(_arg,_key) \
    if ( hSource.Exists(_key) ) \
        _arg = hSource[_key];

// get array of strings
#define LOC_GETAS(_arg,_key) \
    for ( CSphVariant * pVal = hSource(_key); pVal; pVal = pVal->m_pNext ) \
        _arg.Add ( pVal->cstr() );

#define LOC_CHECK(_hash,_key,_msg,_add) \
    if (!( _hash.Exists ( _key ) )) \
    { \
        fprintf ( stdout, "ERROR: key '%s' not found " _msg "\n", _key, _add ); \
        return false; \
    }

void initColumnInfo(CSphColumnInfo& info, const char* sName, const char* sType){
    info.m_sName = sName;
    //info.m_eAttrType
}

void setColumnBitCount(int iBitCount){

}

uint32_t getCRC32(const char* data, size_t iLength)
{
    return sphCRC32((const BYTE*)data, iLength);
}

uint32_t getConfigValues(const CSphConfigSection & hSource, const char* sKey, CSphVector<CSphString>& values){
    // hSource reference as the following.. (hidden)
    int orig_length = values.GetLength();
    LOC_GETAS(values, sKey);
    return values.GetLength() - orig_length;
}

#if USE_PYTHON
CSphSource * SpawnSourcePython ( const CSphConfigSection & hSource, const char * sSourceName )
{
    assert ( hSource["type"]=="python" );

    //LOC_CHECK ( hSource, "name", "in source '%s'.", sSourceName ); -> should move to setup.

    CSphSource_Python2 * pPySource = new CSphSource_Python2 ( sSourceName );
    if ( !pPySource->Setup ( hSource ) ) {
        if(pPySource->GetErrorMessage().Length())
            fprintf ( stdout, "ERROR: %s\n", pPySource->GetErrorMessage().cstr());
        SafeDelete ( pPySource );
        return NULL;
    }

    return pPySource;
}
#endif

//------ Python Tokenizer Block -------


//------ Python Cache Block -------


//------ Python Query Block -------

