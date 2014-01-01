#include "sphinx.h"
#include "sphinxutils.h"
#include "py_layer.h"
#include "pyiface.h"
#include "pycsft.h"

//------ Python Configure Block -------
PySphConfig::PySphConfig(CSphConfig& conf)
    :_conf(conf)
{

}

bool PySphConfig::hasSection(const char* sType, const char* sName)
{
    //if (this->)
    if(!_conf.Exists(sType))
        return false;
    if (!_conf[sType].Exists ( sName ) )
        return false;
    return true;
}

bool PySphConfig::addSection(const char* sType, const char* sName)
{
    if ( !_conf.Exists ( sType ) )
        _conf.Add ( CSphConfigType(), sType ); // FIXME! be paranoid, verify that it returned true

    if ( _conf[sType].Exists ( sName ) )
    {
        m_sError.SetSprintf("section '%s' (type='%s') already exists", sName, sType );
        return false;
    }
    _conf[sType].Add ( CSphConfigSection(), sName ); // FIXME! be paranoid, verify that it returned true
    return false;
}

bool PySphConfig::addKey(const char* sType, const char* sName, const char* sKey, char* sValue)
{
    //assert ( _conf.Exists ( sType ) );
    //assert ( _conf[sType].Exists ( sName ) );
    // verify again.
    if(!hasSection(sType, sName))
        return false;

    sValue = trim ( sValue );
    CSphConfigSection & tSec = _conf[sType][sName];
    if ( tSec(sKey) )
    {
        if ( tSec[sKey].m_bTag )
        {
            // override value or list with a new value
            SafeDelete ( tSec[sKey].m_pNext ); // only leave the first array element
            tSec[sKey] = sValue; // update its value
            tSec[sKey].m_bTag = false; // mark it as overridden

        } else
        {
            // chain to tail, to keep the order
            CSphVariant * pTail = &tSec[sKey];
            while ( pTail->m_pNext )
                pTail = pTail->m_pNext;
            pTail->m_pNext = new CSphVariant ( sValue );
        }

    } else
    {
        // just add
        tSec.Add ( sValue, sKey ); // FIXME! be paranoid, verify that it returned true
    }
    return true;
}


IConfProvider::IConfProvider()
{
}

IConfProvider::~IConfProvider()
{
}

const CSphString & IConfProvider::GetErrorMessage () const
{
    CSphString	m_sError = "ccc";
    return m_sError;
}

/*
 *
 */
int ConfProviderWrap::process(CSphConfig & hConf)
{
    PySphConfig pyconf(hConf);
    return py_iconfprovider_process(obj, pyconf);
}

//------ Python Data Source Block -------


//------ Python Tokenizer Block -------


//------ Python Cache Block -------


//------ Python Query Block -------

