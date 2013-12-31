#ifdef _WIN32
#pragma warning(disable:4996) 
#pragma warning(disable:4127) 
#endif

#include "sphinx.h"
#include "sphinxutils.h"

#include "py_layer.h"

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
#if USE_PYTHON

int init_python_layer_helpers()
{
    int nRet = 0;
    nRet = PyRun_SimpleString("import sys\nimport os\n");
    if(nRet) return nRet;
    //helper function to append path to env.
    nRet = PyRun_SimpleString("\n\
def __coreseek_set_python_path(sPath):\n\
    sPaths = [x.lower() for x in sys.path]\n\
    sPath = os.path.abspath(sPath)\n\
    if sPath not in sPaths:\n\
        sys.path.append(sPath)\n\
    #print sPaths\n\
\n");
    if(nRet) return nRet;
    return nRet;
}

#endif

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
    }
    init_python_layer_helpers();
    // FIXME: change to cython version.
    //init paths
    PyObject * main_module = NULL;
    {

        CSphVector<CSphString>	m_dPyPaths;
        LOC_GETAS(hPython, m_dPyPaths, "path");
        ///XXX: append system pre-defined path here.
        {
            main_module = PyImport_AddModule("__main__");  //+1
            //init paths
            PyObject* pFunc = PyObject_GetAttrString(main_module, "__coreseek_set_python_path");

            if(pFunc && PyCallable_Check(pFunc)){
                ARRAY_FOREACH ( i, m_dPyPaths )
                {
                    PyObject* pArgsKey  = Py_BuildValue("(s)",m_dPyPaths[i].cstr() );
                    PyObject* pResult = PyEval_CallObject(pFunc, pArgsKey);
                    Py_XDECREF(pArgsKey);
                    Py_XDECREF(pResult);
                }
            } // end if
            if (pFunc)
                Py_XDECREF(pFunc);
            //Py_XDECREF(main_module); //no needs to decrease refer to __main__ module, else will got a crash!
        }
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


