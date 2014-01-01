# -*- coding: UTF-8 -*-
import os


class DemoPyConf(object):
	
	def process(self, conf):
		print conf.hasSection('index','xml')
		print conf.hasSection('searchd','searchd')
		self._build_demo_index(conf)
		return  0

	def _build_demo_index(self, conf):
		"""
			index xml
			{
			    source            = xml            #对应的source名称
			    path            = idx/xml #请修改为实际使用的绝对路径，例如：/usr/local/coreseek/var/...
			    docinfo            = extern
			    mlock            = 0
			    morphology        = none
			    min_word_len        = 1
			    html_strip                = 0
			    charset_type        = utf-8
			}
		"""
		#conf.addSection('index', 'xml2')
		props = {
			"source": "xml",
			"path": "idx/xml2",
			"charset_type": "utf-8"
		}
		for k, v in props.items():
			#print k, v
			conf.addKey('index', 'xml2', k, v)
# end of file.