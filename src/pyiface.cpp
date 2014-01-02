#include "pyiface.h"

#include "pysource.h"

//------ Python Configure Block -------


//------ Python Data Source Block -------
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

#define LOC_CHECK(_hash,_key,_msg,_add) \
    if (!( _hash.Exists ( _key ) )) \
    { \
        fprintf ( stdout, "ERROR: key '%s' not found " _msg "\n", _key, _add ); \
        return false; \
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
    }

    return pPySource;
}
#endif

//------ Python Tokenizer Block -------


//------ Python Cache Block -------


//------ Python Query Block -------

