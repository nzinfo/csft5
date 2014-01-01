# -*- coding: UTF-8 -*-

cimport pycsft
cimport cpython.ref as cpy_ref
import os

"""
    定义
        Python 数据源
        Python 分词法
        Python 配置服务
        Python Cache 的 C++ <-> Python 的调用接口
"""
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
cdef public api void __setPythonPath(const char* sPath):
    import sys
    sPaths = [x.lower() for x in sys.path]
    sPath_ = os.path.abspath(sPath)
    if sPath_ not in sPaths:
        sys.path.append(sPath_)
    #print __findPythonClass('flask.Flask')

cdef public api cpy_ref.PyObject* __getPythonClassByName(const char* class_name):
    import sys
    sName = class_name
    clsType = __findPythonClass(sName)
    if clsType:
        return ( <cpy_ref.PyObject*>clsType)
    else:
        return NULL

#end of file
