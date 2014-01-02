# -*- coding: UTF-8 -*-

class TestSource(object):
	"""测试使用的 Python 数据源"""
	def __init__(self):
		super(TestSource, self).__init__()

	"""
		配置 索引的 字段信息
	"""
	def setup(self, schema, source_conf):
		"""
			在 Python Source2 中， 使用 Setup 方法定义索引， 通过调用 schema 的成员变量添加新函数；
			source_conf 是 python 的 dict 对象， 记录了由配置文件提供的 数据源信息；
			@schema 		PySchemaWrap 
			@source_conf 	Dictionary
		"""
		schema.addAttribute("create_at", "timestamp", 0, False, False)
		schema.addAttribute("tag", "integer", 0, False, True)
		print schema, source_conf
		return False

	"""
		想索引 提供数据
	"""
	def feed(self, docinfo, hit_collector):
		"""
			向索引 送入数据 
			@docinfo 		扩展，填充文档的属性信息
			@hit_collector	Hit 采集，根据属性主动向索引推送 命中 （hit）

			@return  True  | None -> 继续采集文档信息
			@return  False | Has Exception -> 停止采集
		"""

	def feedJoinField(self, fieldname, docinfo, hit_collector):
		pass

	def feedMultiValueAttribute(self, fieldname, docinfo, hit_collector):
		pass

	def feedKillList(self):
		# 参数待定
		pass

	"""
		与数据连接有关的接口
	"""
	def beforeConnect(self):
		pass

	def afterConnected(self):
		pass

	def beforeIndex(self):
		pass

	def afterIndex(self):
		pass

	def indexFinished(self):
		pass



# The end of file