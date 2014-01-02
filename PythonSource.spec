使用 Python 为 Sphinx 提供待索引的数据

API:

基于数据库的数据源基类，提供 Python Source 接口
    - [Renamed] GetSchema -> buildSchema
    - Connected
    - OnIndexFinished
    - OnBeforeIndex
    - GetFieldOrder
    - GetDocField
    - GetMVAValue
    - [Removed] BuildHits 
    - NextDocument
    - OnAfterIndex
    - GetKillList


较过去的改进:
	- 不再使用 GetSchema ，改为传入一个可定制 Schema 的对象
		- addField
		- listFields
		- getField
		- rmField
		$ 可以定制字段存储的位数

	- 去掉 BuildHits 接口 -> 此 接口 的任务改为 TokenPolicy 完成
	- 仍然提供 hitCollector, 用于在处理属性的过程中，提供 fake key-words

