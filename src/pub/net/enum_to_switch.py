#!/usr/bin/env python  
#-*-coding:utf-8-*-

#在python脚本中第一行必须是#!/usr/bin/env python，这样才可以向下以python的环境来解释下面的语句
#=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/wukun/work/enum_to_switch.py
#    Author       : Sunsolo(卓一航)
#    Email        : wukun@kunyan-inc.com
#    Date         : 2015-12-29 10:30
#    Description  : 
#=============================================================================

import sys
import re
import codecs
import time

COLOR_OUTPUT = {"FG_GREEN" : '\033[32;1m', "END" : '\033[0m'} # 输出颜色控制
SOURCE_PATH = "./error_comm.h"  #保存枚举变量的文件路径
TIME_SHOW = {                
    "YEAR"  : str(time.localtime().tm_year), # 控制时间显示
    "MONTH" : str(time.localtime().tm_mon),
    "DAY"   : str(time.localtime().tm_mday),
    "HOUR"  : str(time.localtime().tm_hour),
    "MIN"   : str(time.localtime().tm_min),
}

all_enum_map = {} # 存储格式为{ 枚举变量名 : { 枚举值名 : 注释含义} }

def gen_enum_to_dictionary(src_path):
  '''gen_enum_to_dictionary:

             args: src_path[保存枚举变量所在文件的路径]
             
             function: 将src_path里枚举变量解析成{ 枚举变量名 : { 枚举值名 : 注释含义} }的格式
  '''
  
  global all_enum_map
  temp = None 
  enum_name = ""
  is_enum_block = None
  
  fp = open(src_path, "r") 
  lines = fp.readlines()
  for line_char in lines:
      line_char.strip()
      if line_char.startswith("enum"):
          is_enum_block = 1
          enum_name = line_char.split(" ")
          temp = enum_name[1]
          location = temp.find("{")
          if location > 0:
            temp = temp[:location]
          all_enum_map[temp] = {}
          continue

      if line_char.startswith("}"):
          is_enum_block = None

      if len(line_char.split()) == 0 :
        continue

      if is_enum_block is not None:
          line_char.lstrip("\r\n\t ")
          enum_define = line_char.split()[0]
          line_char = line_char.decode("utf8")
          enum_note = line_char.split("//")[1].lstrip().rstrip()
          if enum_define is not None and temp is not None :
              enum_define.lstrip("\t")
              all_enum_map[temp][enum_define] = enum_note

def gen_dictionary_to_switch(cc_target_file):
  """gen_dictionary_to_switch:

           args:cc_target_file[保存最后转换的switch语句所在的目标cc文件]

           function:将{ 枚举变量名 : { 枚举值名 : 注释含义} }格式的变量映射为switch语句
  """
  
  fp = codecs.open(cc_target_file, "w","utf-8")
  fp.write('''/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : ''' + cc_target_file + '''
#    Author       : Sunsolo
#    Email        : wukun@kunyan-inc.com
#    Date         : ''' + TIME_SHOW["YEAR"] + "-" + TIME_SHOW["MONTH"] + "-" + TIME_SHOW["DAY"] +" " +  TIME_SHOW["HOUR"] + ":"+ TIME_SHOW["MIN"] + '''
#    Description  : The file is generated automatically by enum_to_switch.py.
#=============================================================================*/''')

  fp.write('''
\n#include "net/error_comm.h"
\nconst char* buddha_strerror(int error_code) {
  char* error_string = "";
  switch(error_code){\n
''')

  for key, value in all_enum_map["neterrorcode"].items():
    fp.write("    case {0}:".format(key))
    fp.write('\n\n        error_string = "' + value + '";')
    fp.write('\n        break')
    s = ';' + '\n\n'
    fp.write(s)

  fp.write('''    default:
        error_string = "errorcode not define";
        break;
    }
    return error_string;
}
''')

if __name__ == "__main__":

    executable_name = sys.argv[0]

    if len(sys.argv) != 2:      # 脚本的用法
      print COLOR_OUTPUT["FG_GREEN"] + "Usage: \n      " + executable_name + " error_comm.cc" + COLOR_OUTPUT["END"]
      exit(1)
    else:
      generate_file_name = sys.argv[1]  #所要生成的文件名


    gen_enum_to_dictionary(SOURCE_PATH)

    gen_dictionary_to_switch(generate_file_name)


