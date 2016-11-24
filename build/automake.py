# -*- coding: UTF-8 -*-  

'''
*  自动搜索C/C++源文件生成Makefile
*  Copyright (c) 2016年 Luwei. All rights reserved.
*  Github: https://github.com/Luweimy
*
*  自动在指定目录(不指定则当前目录)下搜索，后缀名为'.automake'的文件作为参数文件，若不存在则使用默认参数
*  然后依据这些参数生成Makefile文件。
'''
__version__ = '1.0.3'
__author__  = 'Luwei <luweimy@126.com>'


import os
import copy
import json
import sys

## ------------------------------------------
## 要搜索的源码文件所在的根目录列表(相对于本文件的路径)
search_sources_dir_root_list   = ['./']
## 要搜索的源码文件的忽略路径
search_sources_ignore_dir_list = []
## 要搜索的源文件的扩展名类型列表
search_sources_extension_list  = ['c', 'cpp', 'cc', 'cxx', 'C']
## 要搜索的头文件文件的扩展名类型列表
search_headers_extension_list  = ['h', 'hpp']
## 生成的执行文件的名称
build_target_name 		= 'bin/app'
## 对象文件的存放路径
build_object_dir		= 'obj/'
## c源码编译器和编译选项
build_compiler_cc 		= 'gcc'
build_compiler_ccflags 	= '-g -Wall'
## c++源码编译器和编译选项
build_compiler_cxx 		= 'g++'
build_compiler_cxxflags = '-g -Wall -std=c++11'
## 链接参数
build_compiler_link		= 'g++'
build_compiler_linkflags= '-g -Wall -std=c++11'
## ------------------------------------------

class Utils(object):
	@classmethod
	def getFilenameReplaceExtension(cls, path, extension):
		basename = os.path.basename(path)
		if extension.startswith('.'):
			return basename.split('.')[0] + extension
		return basename.split('.')[0] + '.' + extension

	@classmethod
	def getFileExtension(cls, path):
		return path.split('.')[-1]

	@classmethod
	def isSourceTypeLangC(cls, path):
		extension = path.split('.')[-1]
		return extension == 'c' or extension == 'C'

	@classmethod
	def searchAutomakeConfig(cls, path):
		for name in os.listdir(path):
			if name.endswith('.automake'):
				return os.path.join(path, name)
		return None


class SourceCodeSearcher(object):
	def __init__(self, rootdirs, ignoredirs, sourceexts, headerexts):
		self.rootdirs   = rootdirs
		self.ignoredirs = ignoredirs
		self.sourceexts = sourceexts
		self.headerexts = headerexts

	def getAllDirectoryRecursion(self, rootdir):
		'''
		深度递归遍历，获得所有的根目录下的所有文件夹路径
		'''
		alldirs = []
		if not os.path.exists(rootdir):
			return alldirs 
		for name in os.listdir(rootdir):
			path = os.path.join(rootdir, name)
			if os.path.isdir(path):
				if path in self.ignoredirs:
					continue
				alldirs.extend(self.getAllDirectoryRecursion(path))
				alldirs.append(path)
		return alldirs

	def getFileListWithExtension(self, rootdir, extension):
		'''
		获取文件夹中指定后缀名的所有文件
		'''
		files = []
		for name in os.listdir(rootdir):
			path = os.path.join(rootdir, name)		
			if os.path.isfile(path):
				ext = Utils.getFileExtension(path)
				if ext == extension:
					files.append(path)
		return files

	def getAllDirectoryWithRootDirectoryList(self):
		'''
		获得所有的文件夹路径集合
		'''
		alldirs = copy.deepcopy(self.rootdirs)
		for path in self.rootdirs:
			alldirs.extend(self.getAllDirectoryRecursion(path))
		return alldirs

	def searchingSourceList(self):
		'''
		获得源文件的路径列表
		'''
		alldirs = self.getAllDirectoryWithRootDirectoryList()

		sources = []
		for path in alldirs:
			for extension in self.sourceexts:
				sources.extend(self.getFileListWithExtension(path, extension))

		return sources

	def searchingHeaderList(self):
		'''
		获得头文件的路径列表
		'''
		alldirs = self.getAllDirectoryWithRootDirectoryList()

		headers = []
		for path in alldirs:
			for extension in self.headerexts:
				headers.extend(self.getFileListWithExtension(path, extension))

		return headers

## config 

if len(sys.argv) > 1:
	path = sys.argv[1]
	if os.path.isdir(path):
		config_path = Utils.searchAutomakeConfig(path)
	else:
		config_path = path
else:
	config_path = Utils.searchAutomakeConfig('.')

config = {}
if config_path and os.path.exists(config_path):
	config_file = open(config_path)
	config = json.loads(config_file.read())
	config_file.close()

if 'search_sources_dir_root_list' in config:
	search_sources_dir_root_list = config['search_sources_dir_root_list']
if 'search_sources_ignore_dir_list' in config:
	search_sources_ignore_dir_list = config['search_sources_ignore_dir_list']
if 'search_sources_extension_list' in config:
	search_sources_extension_list = config['search_sources_extension_list']
if 'search_headers_extension_list' in config:
	search_headers_extension_list = config['search_headers_extension_list']

if 'build_target_name' in config:
	build_target_name = config['build_target_name']
if 'build_object_dir' in config:
	build_object_dir = config['build_object_dir']

if 'build_compiler_cc' in config:
	build_compiler_cc = config['build_compiler_cc']
if 'build_compiler_ccflags' in config:
	build_compiler_ccflags = config['build_compiler_ccflags']
if 'build_compiler_cxx' in config:
	build_compiler_cxx = config['build_compiler_cxx']
if 'build_compiler_cxxflags' in config:
	build_compiler_cxxflags = config['build_compiler_cxxflags']

if 'build_compiler_link' in config:
	build_compiler_link = config['build_compiler_link']
if 'build_compiler_linkflags' in config:
	build_compiler_linkflags = config['build_compiler_linkflags']

curdir = '.'
if config_path:
	curdir = os.path.dirname(config_path)
elif len(sys.argv) > 1:
	curdir = path = sys.argv[1]

os.chdir(curdir)

print 'curdir ->', curdir
print 'config ->', config_path
print 'source ->', search_sources_dir_root_list
print 'ignore ->', search_sources_ignore_dir_list

## build makefile

sources_searcher = SourceCodeSearcher(search_sources_dir_root_list, 
										search_sources_ignore_dir_list, 
										search_sources_extension_list, 
										search_headers_extension_list)
source_list = sources_searcher.searchingSourceList()

objects_strings = ''
for path in source_list:
	source_object = '$(OBJECT_DIR)' + Utils.getFilenameReplaceExtension(path, 'o')
	objects_strings += '\\\n\t' + source_object 

if not build_object_dir.endswith('/'):
	build_object_dir += '/' if len(build_object_dir) > 0 else './'

command_compile   = '{compiler} {cflags} -c -o {object} {source}\n'

makefile_strings  = 'TARGET     = ' + build_target_name + '\n'
makefile_strings += 'TARGET_DIR = ' + '$(dir ${TARGET})' + '\n'
makefile_strings += 'OBJECT_DIR = ' + build_object_dir + '\n'
makefile_strings += 'LINK       = ' + build_compiler_link + '\n'
makefile_strings += 'CXX        = ' + build_compiler_cxx + '\n'
makefile_strings += 'CC         = ' + build_compiler_cc + '\n'
makefile_strings += 'CCFLAGS    = ' + build_compiler_ccflags + '\n'
makefile_strings += 'CXXFLAGS   = ' + build_compiler_cxxflags + '\n'
makefile_strings += 'LINKFLAGS  = ' + build_compiler_linkflags + '\n'
makefile_strings += 'OBJECT     = ' + objects_strings + '\n\n'

makefile_strings += '$(shell test -d $(OBJECT_DIR) || mkdir -p $(OBJECT_DIR))\n\n'
makefile_strings += '$(TARGET):$(OBJECT)\n\t' \
					'test -d $(TARGET_DIR) || mkdir -p $(TARGET_DIR)\n\t' \
					'$(LINK) $(LINKFLAGS) -o $(TARGET) $(OBJECT)\n\n'

for source_path in source_list:
	print '> ' + source_path
	source_object_name = '$(OBJECT_DIR)' + Utils.getFilenameReplaceExtension(source_path, 'o')
	makefile_strings += source_object_name + ':' + source_path + '\n\t'
	build_compiler = '$(CXX)'
	build_compiler_flags = '$(CXXFLAGS)'
	if Utils.isSourceTypeLangC(source_path):
		build_compiler = '$(CC)'
		build_compiler_flags = '$(CCFLAGS)'
	makefile_strings += command_compile.format(	compiler=build_compiler, 
												cflags=build_compiler_flags, 
												object=source_object_name,
												source=source_path )
makefile_strings += '\nclean:\n\trm $(OBJECT)\n'

makefile_name = './Makefile'
makefile_file = file(makefile_name, 'w+')
makefile_file.write(makefile_strings)
makefile_file.close()

