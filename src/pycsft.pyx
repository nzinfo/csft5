# -*- coding: UTF-8 -*-

cimport pycsft
cimport cpython.ref as cpy_ref
import os
from libcpp cimport bool
from libc.stdint cimport uint32_t

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

"""
    Import C++ Interface
"""

## --- python conf ---

## --- python source ---

cdef extern from "sphinxstd.h":
    cdef cppclass CSphString:
        const char * cstr () const

cdef extern from "sphinx.h":

    cdef cppclass CSphColumnInfo:
        #CSphColumnInfo ( const char * sName=NULL, ESphAttr eType=SPH_ATTR_NONE )
        int     m_iIndex

    cdef cppclass CSphSchema:
        int	GetFieldIndex ( const char * sName ) const
        int	GetAttrIndex ( const char * sName ) const

        void	Reset ()
        void    ResetAttrs ()
        int	GetRowSize () const
        int	GetStaticSize () const
        int	GetDynamicSize () const
        int	GetAttrsCount () const

        const CSphColumnInfo &	GetAttr ( int iIndex ) const
        const CSphColumnInfo *	GetAttr ( const char * sName ) const
        void  AddAttr ( const CSphColumnInfo & tAttr, bool bDynamic )

    cdef cppclass CSphSource:
        pass

cdef extern from "sphinxutils.h":
    cdef cppclass CSphConfigSection:
        void IterateStart () const
        bool IterateStart ( const CSphString & tKey ) const
        bool IterateNext () const
        const CSphString & IterateGetKey () const

cdef extern from "pyiface.h":
    cdef cppclass CSphStringList:
        int GetLength () const
        void Reset ()
        const CSphString & operator [] ( int iIndex ) const

    cdef uint32_t getCRC32(const char* data, size_t iLength)
    uint32_t getConfigValues(const CSphConfigSection & hSource, const char* sKey, CSphStringList& value)

cdef extern from "pysource.h":
    cdef cppclass CSphSource_Python2:
        pass

## --- python tokenizer ---

## --- python cache ---

## --- python query ---

"""
    Define Python Wrap, for Python side.
"""

## --- python conf ---

## --- python source ---

## --- python tokenizer ---

## --- python cache ---

## --- python query ---

"""
    Define Python Wrap , for CPP side.
"""

## --- python conf ---

## --- python source ---

## --- python tokenizer ---

## --- python cache ---

## --- python query ---


"""
    Python Object's C API
"""

## --- python conf ---

## --- python source ---
# 处理配置文件的读取, 读取 配置到  key -> value; key-> valuelist.
cdef public int py_source_setup(void *ptr, const CSphConfigSection & hSource):
    cdef const char* key
    cdef CSphStringList values
    cdef uint32_t value_count

    conf_items = {}
    hSource.IterateStart()
    while hSource.IterateNext():
        values.Reset()
        key = hSource.IterateGetKey().cstr()
        value_count = getConfigValues(hSource, key, values)

        if value_count == 1:
            conf_items[key] = values[0].cstr()
            continue
        v = []
        for i in range(0, values.GetLength()):
            v.append( values[i].cstr() )
        conf_items[key] = v

    print conf_items
    pass

# - [Renamed] GetSchema -> buildSchema
#cdef public int py_source_build_schema(void *ptr, PySphConfig& hConf):
#    pass

# - Connected
cdef public int py_source_connected(void *ptr):
    pass

# - OnIndexFinished
cdef public int py_source_index_finished(void *ptr):
    pass

# - OnBeforeIndex
cdef public int py_source_before_index(void *ptr):
    pass

# - GetDocField
cdef public int py_source_get_join_field(void *ptr, const char* fieldname):
    pass

# - GetMVAValue
cdef public int py_source_get_join_mva(void *ptr, const char* fieldname):
    pass

# - NextDocument
cdef public int py_source_next(void *ptr):
    pass

# - OnAfterIndex
cdef public int py_source_after_index(void *ptr):
    pass

# - GetKillList
cdef public int py_source_get_kill_list(void *ptr):
    pass

# - [Removed] GetFieldOrder -> 在 buildSchema 统一处理
# - [Removed] BuildHits -> 有 TokenPolicy 模块处理

## --- python tokenizer ---

## --- python cache ---

## --- python query ---


"""
    Object creation function.
"""

## --- python conf ---

## --- python source ---
# pass source config infomation.
#cdef public api CSphSource * createPythonDataSourceObject ( const CSphConfigSection & hSource, const char * sSourceName ):

## --- python tokenizer ---

## --- python cache ---

## --- python query ---


#end of file
