# -*- coding: UTF-8 -*-

cimport pycsft
cimport cpython.ref as cpy_ref
from cpython.ref cimport Py_INCREF, Py_DECREF, Py_XDECREF
from cpython.exc cimport PyErr_Fetch, PyErr_Restore

import os
from libcpp cimport bool
from libc.stdint cimport uint32_t, uint64_t
import traceback

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
    ctypedef SphDocID_t
    ctypedef SphAttr_t

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

    void initColumnInfo(CSphColumnInfo& info, const char* sName, const char* sType)
    void setColumnBitCount(CSphColumnInfo& info, int iBitCount)
    void setColumnAsMVA(CSphColumnInfo& info, bool bJoin)
    void addFieldColumn(CSphSchema* pSchema, CSphColumnInfo& info)
    int  getSchemaFieldCount(CSphSchema* pSchema)

cdef extern from "pysource.h":
    cdef cppclass CSphSource_Python2:
        CSphSource_Python2 ( const char * sName, cpy_ref.PyObject* obj)

## --- python tokenizer ---

## --- python cache ---

## --- python query ---

"""
    Define Python Wrap, for Python side.
"""

## --- python conf ---

## --- python source ---
class InvalidAttributeType(Exception):
    pass

cdef class PySchemaWrap(object):
    """
        用于向 Python 端 提供操作 Schema 的接口
    """
    cdef CSphSchema* _schema
    cdef object _valid_attribute_type # python list
    cdef int iIndex

    def  __cinit__(self):
        self._schema = NULL
        self._valid_attribute_type = ["integer", "timestamp", "boolean", "float", "long", "string", "poly2d", "field", "json"]
        self.iIndex = 0

    cdef bind(self, CSphSchema* s):
        self._schema = s


    cpdef addAttribute(self, const char* sName, const char* sType, int iBitSize=0, bool bJoin=False, bool bIsSet=False):
        """
            向实际的 Schema 中增加 新字段
            @iBitSize <= 0, means standand size.

            - check sType
        """
        cdef CSphColumnInfo tCol

        if sType not in self._valid_attribute_type:
            raise InvalidAttributeType()
        if sType == str("field"):
            raise InvalidAttributeType() # used addField plz.

        initColumnInfo(tCol, sName, sType);
        tCol.m_iIndex = self.iIndex
        self.iIndex += 1
        if iBitSize:
            setColumnBitCount(tCol, iBitSize)
        # Patch on MVA
        if bIsSet:
            setColumnAsMVA(tCol, bJoin)

        self._schema.AddAttr(tCol, True)


    cpdef addField(self, const char* sName, bool bJoin=False):
        """
            向 Schema 添加全文检索字段
        """
        cdef CSphColumnInfo tCol

        initColumnInfo(tCol, sName, NULL);
        tCol.m_iIndex = self.iIndex
        self.iIndex += 1
        addFieldColumn(self._schema, tCol);


    cpdef int fieldsCount(self):
        return getSchemaFieldCount(self._schema)

    cpdef int attributeCount(self):
        return self._schema.GetAttrsCount()

    cpdef object fieldsInfo(self, int iIndex):
        return None

    cpdef object attributeInfo(self, int iIndex):
        return None

    cpdef int   getFieldIndex(self, const char* sKey):
        return self._schema.GetFieldIndex(sKey)

    cpdef int   getAttributeIndex(self, const char* sKey):
        return self._schema.GetAttrIndex(sKey)

cdef class PyDocInfo(object):
    """
        供 Python 程序 设置 待检索文档的属性 和 全文检索字段
    """
    cpdef setDocID(self, uint64_t id):
        pass

    cpdef uint64_t getDocID(self):
        return 0

    cpdef int setAttr(self, int iIndex, SphAttr_t uValue):
        return 0

    cpdef uint64_t getLastDocID(self): #FIXME: larger this when docid lager than 64bit.
        return 0

cdef class PyHitCollector(object):
    """
        为 Python 程序提供在索引建立阶段使用 的 Hit 采集接口, 可以手工设置 FieldIndex
    """
    cpdef uint64_t getPrevDocID(self):
        return 0

    cpdef uint64_t getDocID(self):
        return 0



cdef class PySourceWrap(object):
    """
        C++ -> Python 的桥; 额外提供
          - DocInfo 让 Python 修改 Document 的属性信息
          - HitCollector 让 Python 可以主动推送索引
    """
    cdef object _pysource

    def __init__(self, pysrc):
        self._pysource = pysrc

    cdef bindSource(self, CSphSource_Python2* pSource):
        """
            绑定 DocInfo & HitCollector 到指定的 DataSource,
        """

    cpdef int setup(self, schema, source_conf):
        self._pysource.setup(schema, source_conf)
        return 0

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
cdef public int py_source_setup(void *ptr, CSphSchema& Schema, const CSphConfigSection & hSource):
    cdef const char* key
    cdef CSphStringList values
    cdef uint32_t value_count

    cdef PySourceWrap self = <PySourceWrap>(ptr)

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

    pySchema = PySchemaWrap()
    pySchema.bind(&Schema)

    try:
        return self.setup(pySchema, conf_items)
    except Exception, ex:
        traceback.print_exc()
        return -1 # setup failured.

    # temp usage for crc32 key
    if False:
        keys = ["integer", "timestamp", "boolean", "float", "long", "string", "poly2d", "field", "json"]
        for k in keys:
            print k, getCRC32(k, len(k))
    #print conf_items

# - [Renamed] GetSchema -> buildSchema
#cdef public int py_source_build_schema(void *ptr, PySphConfig& hConf):
#    pass

# - Connected
cdef public int py_source_connected(void *ptr):
    cdef PySourceWrap self = <PySourceWrap>(ptr)

# - OnIndexFinished
cdef public int py_source_index_finished(void *ptr):
    cdef PySourceWrap self = <PySourceWrap>(ptr)

# - OnBeforeIndex
cdef public int py_source_before_index(void *ptr):
    cdef PySourceWrap self = <PySourceWrap>(ptr)

# - GetDocField
cdef public int py_source_get_join_field(void *ptr, const char* fieldname):
    cdef PySourceWrap self = <PySourceWrap>(ptr)

# - GetMVAValue
cdef public int py_source_get_join_mva(void *ptr, const char* fieldname):
    cdef PySourceWrap self = <PySourceWrap>(ptr)

# - NextDocument
cdef public int py_source_next(void *ptr):
    cdef PySourceWrap self = <PySourceWrap>(ptr)

# - OnAfterIndex
cdef public int py_source_after_index(void *ptr):
    cdef PySourceWrap self = <PySourceWrap>(ptr)

# - GetKillList
cdef public int py_source_get_kill_list(void *ptr):
    cdef PySourceWrap self = <PySourceWrap>(ptr)

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
cdef public api CSphSource * createPythonDataSourceObject ( const char* sName, const char * class_name ):
    cdef CSphSource_Python2* pySource

    sName = class_name
    clsType = __findPythonClass(sName)
    if clsType:
        # Do error report @user code.
        try:
            obj = clsType()
        except Exception, ex:
            traceback.print_exc()
            return NULL

        wrap = PySourceWrap(obj)
        #Py_INCREF(wrap) # pass pyobjct* to cpp code should addref ( @ the cpp code. )

        pySource = new CSphSource_Python2(sName, <cpy_ref.PyObject*>wrap)
        # FIXME: crash when new failure.
        wrap.bindSource(pySource);
        return <CSphSource*>pySource
    else:
        return NULL

## --- python tokenizer ---

## --- python cache ---

## --- python query ---


#end of file
