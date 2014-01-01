# -*- coding: UTF-8 -*-

cimport pycsft
cimport cpython.ref as cpy_ref
import os
from libcpp cimport bool

"""
    定义
        Python 数据源
        Python 分词法
        Python 配置服务
        Python Cache 的 C++ <-> Python 的调用接口
"""


# Ref: http://stackoverflow.com/questions/1176136/convert-string-to-python-class-object
def __findPythonClass(sName):
    import importlib
    pos = sName.find('.')
    module_name = sName[:pos]
    cName = sName[pos+1:]
    try:
        m = importlib.import_module(module_name)
        c = getattr(m, cName)
        return c
    except ImportError, e:
        print e
        return None


# Cython creator API
# 设置类库的加载路径
cdef public api void __setPythonPath(const char* sPath):
    import sys
    sPaths = [x.lower() for x in sys.path]
    sPath_ = os.path.abspath(sPath)
    if sPath_ not in sPaths:
        sys.path.append(sPath_)
    #print __findPythonClass('flask.Flask')

# 根据类的名称 加载 Python 的 类型对象
cdef public api cpy_ref.PyObject* __getPythonClassByName(const char* class_name):
    import sys
    sName = class_name
    clsType = __findPythonClass(sName)
    if clsType:
        return ( <cpy_ref.PyObject*>clsType )
    else:
        return NULL

"""
    Import C++ Interface
"""

## --- python conf ---
cdef extern from "pyiface.h":
    cpdef cppclass PySphConfig:
        bool hasSection(const char* sType, const char* sName)
        bool addSection(const char* sType, const char* sName)
        bool addKey(const char* sType, const char* sName, const char* sKey, char* sValue)

    cdef cppclass IConfProvider:
        pass

    cdef cppclass ConfProviderWrap:
        #PyObject *obj
        ConfProviderWrap(cpy_ref.PyObject *obj)

## --- python source ---

## --- python tokenizer ---

## --- python cache ---

## --- python query ---

"""
    Define Python Wrap, for Python side.
    Wrap the python code interface
    -> after import c++ class, we needs build python wrap... silly.
"""

## --- python conf ---
cdef class PySphConfigWrap(object):
    cdef PySphConfig* conf_
    def __init__(self):
        self.conf_ = NULL

    cdef init_wrap(self, PySphConfig* conf):
        self.conf_ = conf

    # FIXME: should raise an exception.
    def hasSection(self, sType, sName):
        if self.conf_:
            return self.conf_.hasSection(sType, sName)
        return False

    def addSection(self, sType, sName):
        if self.conf_:
            return self.conf_.addSection(sType, sName)
        return False

    # if not section section, just return false.
    def addKey(self, sType, sName, sKey, sValue):
        if self.conf_:
            return self.conf_.addKey(sType, sName, sKey, sValue)
        return False

## --- python source ---

## --- python tokenizer ---

## --- python cache ---

## --- python query ---

"""
    Define Python Wrap , for CPP side.
"""

## --- python conf ---
cdef class PyConfProviderWrap:
    cdef ConfProviderWrap* _p
    cdef cpy_ref.PyObject* _pyconf
    def __init__(self, pyConfObj):
        self._pyconf = <cpy_ref.PyObject*>pyConfObj # the user customed config object.
        self._p = new ConfProviderWrap(<cpy_ref.PyObject*>self)

    cdef int process(self, PySphConfig & hConf):
        cdef int nRet = 0
        _pyconf = <object>self._pyconf
        hConfwrap = PySphConfigWrap(); hConfwrap.init_wrap(&hConf)
        nRet = _pyconf.process(hConfwrap)
        if nRet == None:
            return 0
        return int(nRet)

## --- python source ---

## --- python tokenizer ---

## --- python cache ---

## --- python query ---


"""
    Python Object's C API
"""

## --- python conf ---

cdef public int py_iconfprovider_process(void *ptr, PySphConfig& hConf):
    cdef PyConfProviderWrap self = <PyConfProviderWrap>(ptr)
    return self.process(hConf)

## --- python source ---

## --- python tokenizer ---

## --- python cache ---

## --- python query ---


"""
    Object creation function.
"""

## --- python conf ---

cdef public api IConfProvider* createPythonConfObject(const char* class_name):
    cdef PyConfProviderWrap pyconf
    sName = class_name
    clsType = __findPythonClass(sName)
    if clsType:
        obj = clsType()
        pyconf = PyConfProviderWrap(obj)
        return <IConfProvider*>(pyconf._p)
    else:
        return NULL # provider not found.

## --- python source ---

## --- python tokenizer ---

## --- python cache ---

## --- python query ---


#end of file
