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
class PySphConfig
{
    /* The Python Interface of CSphConfig
     *  - 不可以删除系统中已经存在的配置节
     *  - 可以对已经存在的配置节 添加 | 改写?
     *  - 可以创建新配置节
     *  - 不可以删除自己的配置节
     */
public:
    PySphConfig(CSphConfig& conf);

public:
    bool hasSection(const char* sType, const char* sName);
    bool addSection(const char* sType, const char* sName);
    bool addKey(const char* sType, const char* sName, const char* sKey, char* sValue);

    const CSphString &		GetErrorMessage () const	{ return m_sError; }
protected:
    //char			m_sError [ 1024 ];
    CSphString	m_sError;
private:
    CSphConfig& _conf;
};

class IConfProvider
{
public:
  IConfProvider();
  virtual ~IConfProvider();
  /*
   *
   */
  virtual int process(CSphConfig & hConf) = 0;
  virtual const CSphString & GetErrorMessage () const;
};

class ConfProviderWrap : public IConfProvider
{
public:
  PyObject *obj;
  // RunFct fct;

  ConfProviderWrap(PyObject *obj):obj(obj) {} //, RunFct fct);
  virtual ~ConfProviderWrap(){}

  virtual int process(CSphConfig & hConf);
};


//------ Python Data Source Block -------


//------ Python Tokenizer Block -------


//------ Python Cache Block -------


//------ Python Query Block -------

#endif // PYIFACECSFT_H
