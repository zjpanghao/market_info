/*******************************************************************************
 * Copyright 2016/05 - 2021/07 
 * file	  :
 * author : tianyiheng
 * date   : 2016-06-14
 * brief  :
 *******************************************************************************/
#include <iostream>
#include "base_config.h"
using namespace plugin_pub;

int main(int argc, char* argv[]) {
  char ch = ' ';
  char ch2 = '	';

  BaseConfig my_config;
  if (!my_config.LoadFile("./test_config.txt")) {
    std::cout << "加载配置文件失败" << std::endl;
  }
  std::string val1 = my_config.GetParamString("section1", "key3");
  int val2 = my_config.GetParamInt("section1", "key1");

  int val3 = my_config.GetParam<int>("section1", "key2");
  std::string val4 = my_config.GetParam<std::string>("section1", "key3");

  int val5 = my_config.GetParam<int>("section2", "key2");
  std::string val6 = my_config.GetParam<std::string>("section2", "key3");
  std::cout << "val6:" << val6 << std::endl;
  std::cout << "val5:" << val5 << std::endl;
  std::cout << "val4:" << val4 << std::endl;
  std::cout << "val3:" << val3 << std::endl;
  std::cout << "val2:" << val2 << std::endl;
  return 0;
}
