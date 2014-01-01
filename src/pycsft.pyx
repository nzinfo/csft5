# -*- coding: UTF-8 -*-

cimport pycsft
cimport cpython.ref as cpy_ref
import os
import sys

"""
    定义
        Python 数据源
        Python 分词法
        Python 配置服务
        Python Cache 的 C++ <-> Python 的调用接口
"""

# Cython creator API
cdef public api void __setPythonPath(const char* sPath):
    sPaths = [x.lower() for x in sys.path]
    sPath_ = os.path.abspath(sPath)
    if sPath_ not in sPaths:
        sys.path.append(sPath_)

#end of file
