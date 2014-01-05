# -*- coding: UTF-8 -*-
import time

documents = [
	{
		'id':100,
		'oid': 100000000000L,
		'title': 'abc efg',
		'body': "I know this is a very basic question but I couldn't find any good conclusive answer to this.",
		'bigtag':[1,2, 100000000000]

	} ,
	{
		'id':101,
		'oid': 100000000001L,
		'title': 'Timestamp Python',
		'body': "Here is some sample output I ran on my computer, converting it to a string as well.",
		'bigtag':[2,3, 100000000001]
	} ,
	{
		'id':102,
		'oid': 100000000002L,
		'title': 'a-z',
		'body': """If the question is expressed by the title, 
			then you can get the timestamp as a string using the .now() or .utcnow() of the datetime.datetime""",
		'bigtag':[3, 100000000003]
	}
]

join_docs = [
	{
		'id': 100,
		'tag': 1,
		'tag2': 100000000001L
	},
	{
		'id': 100,
		'tag': 2,
		'tag2': 100000000002L
	},
	{
		'id': 101,
		'tag': 1,
		'tag2': 100000000001L
	}
]

class TestSource(object):
	"""测试使用的 Python 数据源"""
	def __init__(self):
		super(TestSource, self).__init__()
		self.abc=123
		self.idx = 0

		self.attr2id = {}
		self.field2id = {}

	"""
		配置 索引的 字段信息
	"""
	def setup(self, source_conf):
		"""
			在 Python Source2 中， 使用 Setup 方法定义索引， 通过调用 schema 的成员变量添加新函数；
			source_conf 是 python 的 dict 对象， 记录了由配置文件提供的 数据源信息；
			@source_conf 	Dictionary
		"""
		print 'pysource, setup called'
		return True
		#return False

	"""
		想索引 提供数据
		 buildhit has limit , no more than #define MAX_SOURCE_HITS	32768
	"""
	def feed(self, docinfo, hit_collector):
		"""
			向索引 送入数据 
			@docinfo 		扩展，填充文档的属性信息
			@hit_collector	Hit 采集，根据属性主动向索引推送 命中 （hit）

			@return  True  | None -> 继续采集文档信息
			@return  False | Has Exception -> 停止采集
		"""
		#print 'pysource, feed', docinfo, hit_collector
		#print dir(docinfo)
		#print dir(hit_collector)
		if self.idx >= len(documents):
			return False
		doc = documents[self.idx]
		if True:
			# fill docinfo.
			docinfo.setDocID(doc['id'])
			#print '---', time.time()
			#print self.attr2id, self.attr2id["bigtag"] , type(self.attr2id["bigtag"])
			docinfo.setAttrTimestamp( self.attr2id["create_at"], time.time() + doc['id']);
			docinfo.setAttrInt64(self.attr2id["oid"], doc['oid'])
			docinfo.setAttrMulti(self.attr2id["bigtag"], doc['bigtag']) #be care!
			docinfo.setAttrString(self.attr2id["title"], doc['title'])

			docinfo.setField(self.field2id["title"], doc['title'])
			docinfo.setField(self.field2id["body"], doc['body'])
			#docinfo.setField(100, doc['body'])
			#print doc
			pass

		self.idx += 1
		return True

	def feedJoinField(self, docinfo, hit_collector):
		# fieldname => the code knows which is the joint field. -> IterateJoinedHits
		print 'pysource, feedJoinField'
		pass

	# no hit_collector in mva , for dHits is reused in building mva values.
	def feedMultiValueAttribute(self, fieldname):
		print 'pysource, feedMultiValueAttribute' # return (docid, val)
		print fieldname, '------'
		pass

	def feedKillList(self):
		# 参数待定
		print 'pysource, feedKillList'
		pass

	"""
		与数据连接有关的接口
	"""
	def connect(self, schema):

		self.attr2id["create_at"] = schema.addAttribute("create_at", "timestamp", 0, False, False)
		#attr2id["tag"] = 
		schema.addAttribute("tag", "integer", 0, bJoin=True, bIsSet=True)
		schema.addAttribute("tag2", "long", 0, bJoin=True, bIsSet=True) # 测试 bigint MVA & multi join MVA
		
		self.attr2id["oid"] = schema.addAttribute("oid", "long")
		self.attr2id["title"] = schema.addAttribute("title", "string")
		self.attr2id["bigtag"] =  schema.addAttribute("bigtag", "long", bIsSet = True)


		self.field2id["title"] =  schema.addField("title")		
		self.field2id["body"] =  schema.addField("body")

		#print self.field2id
		schema.addField("comments", bJoin = True)	
		schema.addField("tasks", bJoin = True)	# 测试 multi join fields, useing @tasks ...

		schema.done() #notify system, all attribute & fields are add.

		#print schema, source_conf
		#print schema.fieldsCount(), schema.attributeCount()
		#print schema.fieldsInfo(1), schema.attributeInfo(1)
		# build attr -> id map
		if True:
			for i in range(0, schema.fieldsCount()):
				print i,  schema.fieldsInfo(i)
			for i in range(0, schema.attributeCount()):
				print i,  schema.attributeInfo(i)

		print 'pysource, Connect'
		return True

	def beforeIndex(self):
		print 'pysource, beforeIndex'
		pass

	def afterIndex(self, bNormalExit = True):
		print 'pysource, afterIndex', bNormalExit
		pass

	def indexFinished(self):
		print 'pysource, indexFinished'
		pass



# The end of file