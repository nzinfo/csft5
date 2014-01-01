#ifdef _WIN32
#pragma warning(disable:4996) 
#pragma warning(disable:4127) 
#endif

#include "sphinx.h"
#include "sphinxutils.h"

#include "py_layer.h"
#include "pycsft.h"

#if USE_PYTHON

#define LOC_CHECK(_hash,_key,_msg,_add) \
	if (!( _hash.Exists ( _key ) )) \
	{ \
	fprintf ( stdout, "ERROR: key '%s' not found " _msg "\n", _key, _add ); \
	return false; \
	}

//////////////////////////////////////////////////////////////////////////
// get array of strings
#define LOC_GETAS(_sec, _arg,_key) \
	for ( CSphVariant * pVal = _sec(_key); pVal; pVal = pVal->m_pNext ) \
	_arg.Add ( pVal->cstr() );

// Manual write config reciever -> I'd NOT wanna took time on sip's linking..
#define RET_PYNONE	{ Py_INCREF(Py_None); return Py_None; }

// init & deinit
bool	cftInitialize( const CSphConfigSection & hPython)
{
#if USE_PYTHON
    if (!Py_IsInitialized()) {
        Py_Initialize();
        //PyEval_InitThreads();

        if (!Py_IsInitialized()) {
            return false;
        }
        // init extension.
        initpycsft();
    }

    // init paths
    {

        CSphVector<CSphString>	m_dPyPaths;
        LOC_GETAS(hPython, m_dPyPaths, "path");
        ///XXX: append system pre-defined path here.
        ARRAY_FOREACH ( i, m_dPyPaths )
        {
           __setPythonPath( m_dPyPaths[i].cstr() );
        }
    }
    // check the demo[debug] object creation.
    {
        CSphString demoClassName = hPython.GetStr ( "__debug_object_class" );
    }
    return true;
#endif
}

void    cftShutdown()
{

#if USE_PYTHON
        //FIXME: avoid the debug warning.
        if (Py_IsInitialized()) {
                //to avoid crash in release mode.
                Py_Finalize();
        }
#endif

}

#endif //USE_PYTHON

// end of file.


