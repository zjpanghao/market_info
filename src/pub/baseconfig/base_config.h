/*******************************************************************************
 * Copyright 2016/05 - 2021/07 
 * file	  :
 * author : tianyiheng
 * date   : 2016-06-14
 * brief  :
 *******************************************************************************/

#ifndef __TYH_PUB_BASE_CONFIG_H__
#define __TYH_PUB_BASE_CONFIG_H__

#include <map>
#include <utility>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

namespace plugin_pub {

class ConfigValue {
public:
  ConfigValue();
  ~ConfigValue();

  void AddValue(std::string const & key, std::string const& val);
  std::string GetValue(std::string const & key);
protected:
  std::map<std::string, std::string> map_key_val_;

};

enum SpaceToken {
  ETOKEN_TAB = '\t',
  ETOKEN_BACKSPACE = ' ',
  ETOKEN_RETURN = '\r',
  ETOKEN_LINE = '\n'
};

class BaseConfig {
 public:
  BaseConfig();
  ~BaseConfig();
  
  bool LoadFile(std::string config_file);
  std::string GetParamString(std::string sec_name, std::string key);
  int GetParamInt(std::string sec_name, std::string key);

  template<class T>
  T GetParam(std::string sec_name, std::string key) {
	std::string val = "a";
	std::map<std::string, ConfigValue *>::iterator it = config_sec_.find(sec_name);
	if (it != config_sec_.end()) {
	  ConfigValue * conf_val_pt = it->second;
	  if (NULL != conf_val_pt) {
		val = conf_val_pt->GetValue(key);
	  }
	}
	std::stringstream ss;
	ss << val;
	T ret_val;
	ss >> ret_val;
	return ret_val;
  }

protected:
  void ParseFile(std::ifstream & in_file);
  std::string EraseSpaceToken(std::string const &s);
  bool IsSect(std::string const &s);
  std::string FetchSecName(std::string const & s);
  bool IsKeyValue(std::string const &s);
  void AddConfig(std::string sect, std::string key, std::string val);
  std::pair<std::string, std::string> FectchKeyValue(std::string const & s);
  void Clear();
protected:
  std::map<std::string, ConfigValue *> config_sec_;
};



}  // namespace plugin_pub

#endif  // __TYH_PUB_BASE_CONFIG_H__
